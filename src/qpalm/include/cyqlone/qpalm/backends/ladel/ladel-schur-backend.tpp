#pragma once

#include <guanaqo/eigen/span.hpp>
#include <guanaqo/eigen/view.hpp>
#include <guanaqo/linalg/sparsity-conversions.hpp>
#include <guanaqo/print.hpp>
#include <guanaqo/timed-cpu.hpp>
#include <cyqlone/qpalm/backends/ladel-schur-backend.hpp>

#include "ladel-sparse.hpp"
#include <ladel.h>

#include <Eigen/Core>

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <map>
#include <ranges>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace cyqlone::qpalm {

namespace {

auto check_dims(QPData &qp) {
    auto len = [](const auto &x) { return static_cast<index_t>(std::size(x)); };
    const auto n = num_rows(qp.Q_sparsity), m = num_rows(qp.A_sparsity);
    if (num_cols(qp.Q_sparsity) != n)
        throw std::invalid_argument("Invalid number of columns Q");
    if (num_cols(qp.A_sparsity) != n)
        throw std::invalid_argument("Invalid number of columns A");
    if (get_nnz(qp.Q_sparsity) != len(qp.Q))
        throw std::invalid_argument("Invalid number of elements Q");
    if (get_nnz(qp.A_sparsity) != len(qp.A))
        throw std::invalid_argument("Invalid number of elements A");
    if (len(qp.q) != n)
        throw std::invalid_argument("Invalid number of elements q");
    if (len(qp.b_min) != m)
        throw std::invalid_argument("Invalid number of elements b_min");
    if (len(qp.b_max) != m)
        throw std::invalid_argument("Invalid number of elements b_max");
    return std::tuple{n, m};
}

void mat_vec(const ladel_sparse_matrix &A, std::span<const real_t> x,
             std::span<real_t> y) {
    assert(x.data() != y.data());
    assert(static_cast<ladel_int>(x.size()) == A.ncol);
    assert(static_cast<ladel_int>(y.size()) == A.nrow);
    bool unsymm  = A.symmetry == UNSYMMETRIC;
    auto mat_vec = (unsymm ? ::ladel_matvec : ::ladel_symmetric_matvec);
    mat_vec(&A, x.data(), y.data(), TRUE);
}

void mat_tpose_vec(const ladel_sparse_matrix &A, std::span<const real_t> x,
                   std::span<real_t> y) {
    assert(x.data() != y.data());
    assert(static_cast<ladel_int>(x.size()) == A.nrow);
    assert(static_cast<ladel_int>(y.size()) == A.ncol);
    bool unsymm    = A.symmetry == UNSYMMETRIC;
    auto t_mat_vec = (unsymm ? ::ladel_tpose_matvec : ::ladel_symmetric_matvec);
    t_mat_vec(&A, x.data(), y.data(), TRUE);
}

template <class T>
inline const auto null_vec = Eigen::Map<const Eigen::VectorX<T>>{nullptr, 0};

} // namespace

struct LadelSchurBackend {
    using storage_t     = Eigen::VectorX<ladel_double>;
    using int_storage_t = std::vector<ladel_int>;

    // clang-format off
    struct active_set_t      : int_storage_t { using int_storage_t::int_storage_t; };
    struct var_vec_t         : storage_t     { using storage_t::storage_t; };
    struct ineq_constr_vec_t : storage_t     { using storage_t::storage_t; };
    struct eq_constr_vec_t {};
    // clang-format on

    LadelSchurBackendSettings settings;
    bool reset_factorization = true;
    index_t num_updates      = 0;

    /// Converts input sparsity to CSC.
    struct Converter;

    // QP data

    /// Number of variables.
    ladel_int n;
    /// Number of constraints.
    ladel_int m;
    /// General constraints bounds.
    Eigen::Map<const storage_t> b_min = null_vec<ladel_double>,
                                b_max = null_vec<ladel_double>;
    /// Linear term.
    Eigen::Map<const storage_t> q = null_vec<ladel_double>;
    /// Constant term.
    ladel_double c;
    /// CSC sparsity pattern of Hessian of the cost.
    std::unique_ptr<Converter> sparsity_Q;
    /// Values of the Hessian of the cost (upper triangular part only).
    std::span<const ladel_double> values_Q;
    /// LADEL view of the Hessian of the cost.
    ladel_sparse_matrix Q;
    /// CSC sparsity pattern of Jacobian of the constraints.
    std::unique_ptr<Converter> sparsity_A;
    /// Values of the Jacobian of the constraints.
    std::span<const ladel_double> values_A;
    /// LADEL view of the Jacobian of the constraints.
    ladel_sparse_matrix A;

    // Workspace

    /// Workspace for the right-hand side of the KKT system.
    storage_t rhs;
    /// Workspace for the residual of the system.
    storage_t res;
    /// Maps factorization indices to constraint indices.
    int_storage_t constr_idx;

    /// Cached matrix Aᵀ√Σ.
    ladel_sparse_matrix_ptr AᵀsqrtΣ;

    /// Workspace of size m.
    storage_t work_m;
    /// Index sets of entering, leaving, and active constraints.
    int_storage_t entering, leaving, active_idx;
    /// LADEL workspace of size `max(n, m)`.
    ladel_work_ptr ladel_work_nm;

    LadelSchurBackend(QPData data, const LadelSchurBackendSettings &settings);

    [[nodiscard]] index_t num_var() const { return n; }
    [[nodiscard]] index_t num_eq_constr() const { return 0; }
    [[nodiscard]] index_t num_ineq_constr() const { return m; }

    [[nodiscard]] var_vec_t var_vec() const {
        return var_vec_t(storage_t::Zero(num_var()));
    }
    [[nodiscard]] eq_constr_vec_t eq_constr_vec() const { return {}; }
    [[nodiscard]] ineq_constr_vec_t ineq_constr_vec() const {
        return ineq_constr_vec_t(storage_t::Zero(num_ineq_constr()));
    }
    [[nodiscard]] active_set_t active_set() const {
        active_set_t j(0);
        j.reserve(num_ineq_constr());
        return j;
    }

    template <class... Js>
    void initialize_active_set(Js &...js) const {
        ([this](active_set_t &j) { j = active_set(); }(js), ...);
    }
    template <class... Xs>
    void initialize_var_vec(Xs &...xs) const {
        ([this](var_vec_t &x) { x = var_vec(); }(xs), ...);
    }
    template <class... Ys>
    void initialize_ineq_constr_vec(Ys &...ys) const {
        ([this](ineq_constr_vec_t &y) { y = ineq_constr_vec(); }(ys), ...);
    }
    template <class... Λs>
    void initialize_eq_constr_vec(Λs &...λs) const {
        ([this](eq_constr_vec_t &λ) { λ = eq_constr_vec(); }(λs), ...);
    }

    void initial_variables(var_vec_t &x) const {
        set_constant(x, 0); // TODO
    }
    void initial_multipliers_eq(eq_constr_vec_t &) const {}
    void initial_multipliers_ineq(ineq_constr_vec_t &y) const {
        set_constant(y, 0);
    }

    // e = Ax - clamp(Ax, b_min, b_max)
    void ineq_constr_resid(const ineq_constr_vec_t &Ax,
                           ineq_constr_vec_t &e) const {
        e = Ax - Ax.cwiseMax(b_min).cwiseMin(b_max);
    }

    // e = Ax - clamp(Ax, b_min, b_max)
    ineq_constr_vec_t ineq_constr_resid(const ineq_constr_vec_t &Ax) const {
        auto e = ineq_constr_vec();
        ineq_constr_resid(Ax, e);
        return e;
    }

    real_t ineq_constr_viol(const ineq_constr_vec_t &Ax) const {
        auto e = Ax - Ax.cwiseMax(b_min).cwiseMin(b_max);
        return e.lpNorm<Eigen::Infinity>();
    }

    real_t ineq_constr_resid_al(const ineq_constr_vec_t &y,
                                const ineq_constr_vec_t &ŷ,
                                const ineq_constr_vec_t &Σ,
                                ineq_constr_vec_t &e) {
        e = (ŷ - y).cwiseQuotient(Σ);
        return e.lpNorm<Eigen::Infinity>();
    }

    void eq_constr_resid(const var_vec_t &, eq_constr_vec_t &) {}
    eq_constr_vec_t eq_constr_resid(const var_vec_t &) { return {}; }

    void mat_vec_MT(const eq_constr_vec_t &, var_vec_t &Mᵀλ) {
        set_constant(Mᵀλ, 0);
    }
    var_vec_t mat_vec_MT(const eq_constr_vec_t &λ) {
        auto Mᵀλ = var_vec();
        mat_vec_MT(λ, Mᵀλ);
        return Mᵀλ;
    }

    real_t unscaled_eq_constr_viol(const eq_constr_vec_t &) const { return 0; }

    void mat_vec_AT(const ineq_constr_vec_t &y, var_vec_t &Aᵀy) {
        mat_tpose_vec(A, guanaqo::as_span(y), guanaqo::as_span(Aᵀy));
    }

    void mat_vec_A(const var_vec_t &x, ineq_constr_vec_t &Ax) {
        mat_vec(A, guanaqo::as_span(x), guanaqo::as_span(Ax));
    }
    ineq_constr_vec_t mat_vec_A(const var_vec_t &x) {
        auto Ax = ineq_constr_vec();
        mat_vec_A(x, Ax);
        return Ax;
    }

    void grad_f(const var_vec_t &x, var_vec_t &grad_f) {
        mat_vec(Q, guanaqo::as_span(x), guanaqo::as_span(grad_f));
        grad_f += q;
    }
    void grad_f_regularized(real_t S, const var_vec_t &x,
                            const var_vec_t &x_reg, var_vec_t &grad_f) {
        using std::isfinite;
        this->grad_f(x, grad_f);
        if (isfinite(S))
            grad_f += (1 / S) * (x - x_reg);
    }
    void grad_f_remove_regularization(real_t S, const var_vec_t &x,
                                      const var_vec_t &x_reg,
                                      var_vec_t &grad_f) {
        using std::isfinite;
        if (isfinite(S))
            grad_f -= (1 / S) * (x - x_reg);
    }
    real_t f_grad_f(const var_vec_t &x, var_vec_t &grad_f) {
        mat_vec(Q, guanaqo::as_span(x), guanaqo::as_span(grad_f));
        real_t f = 0.5 * x.dot(grad_f) + x.dot(q) + c;
        grad_f += q;
        return f;
    }
    std::tuple<real_t, var_vec_t> f_grad_f(const var_vec_t &x) {
        auto grad_f = var_vec();
        real_t f    = f_grad_f(x, grad_f);
        return {f, std::move(grad_f)};
    }

    void update_regularization_changed(real_t S_new, real_t S_old) {
        if (S_new != S_old)
            reset_factorization = true;
    }

    real_t boost_regularization(real_t S, real_t S_boost) {
        update_regularization_changed(S_boost, S);
        return S_boost;
    }

    void update_penalty_changed(const ineq_constr_vec_t &Σ,
                                std::span<const ladel_double> Σ_update_factors,
                                std::span<const index_t> constr_changed) {
        std::ignore = Σ;
        std::ignore = Σ_update_factors;
        if (!constr_changed.empty())
            reset_factorization = true;
    }

    template <class T, class U>
    static void xaxpy(real_t a, const T &x, U &y) {
        y += a * x;
    }

    template <class T, class U>
    static void xcopy(const T &x, U &y) {
        std::ranges::copy(x, y.begin());
    }

    template <class T, class U>
    static void set_constant(T &x, const U &y) {
        x.setConstant(y);
    }

    [[nodiscard]] static real_t dot(const var_vec_t &a, const var_vec_t &b) {
        return a.dot(b);
    }

    [[nodiscard]] static real_t norm_inf(const ineq_constr_vec_t &x) {
        return x.lpNorm<Eigen::Infinity>();
    }

    [[nodiscard]] static real_t norm_inf(const eq_constr_vec_t &) { return 0; }

    [[nodiscard]] static real_t norm_squared(const ineq_constr_vec_t &x) {
        return x.squaredNorm();
    }
    [[nodiscard]] static real_t norm_squared(const var_vec_t &x) {
        return x.squaredNorm();
    }
    static void scale(real_t s, var_vec_t &x) { x *= s; }
    static void scale(real_t, eq_constr_vec_t &) {}
    static void scale(real_t s, ineq_constr_vec_t &x) { x *= s; }

    ineq_constr_vec_t Ax_min() const { return b_min; } // TODO: why copy?
    ineq_constr_vec_t Ax_max() const { return b_max; } // TODO: why copy?

    index_t calc_ŷ_Aᵀŷ(const ineq_constr_vec_t &Ax, const ineq_constr_vec_t &Σ,
                       const ineq_constr_vec_t &y, ineq_constr_vec_t &ŷ,
                       var_vec_t &Aᵀŷ, active_set_t &J) {

        auto ζ = Ax + y.cwiseQuotient(Σ);
        auto z = ζ.cwiseMax(b_min).cwiseMin(b_max);
        ŷ      = y + Σ.cwiseProduct(Ax - z);
        J.clear();
        for (index_t i = 0; i < static_cast<index_t>(m); ++i)
            if (z == b_min || z == b_max)
                J.push_back(i);
        return static_cast<index_t>(J.size());
    }

    real_t unscaled_aug_lagr_norm(const var_vec_t &grad_f, const var_vec_t &Mᵀλ,
                                  const var_vec_t &Aᵀŷ) {
        auto grad_al = grad_f + Mᵀλ + Aᵀŷ;
        return grad_al.lpNorm<Eigen::Infinity>();
    }

    void unscale_variables(const var_vec_t &in, std::span<real_t> out) const {
        assert(in.size() == out.size());
        std::ranges::copy(in, out.begin());
    }
    void unscale_ineq_constr(const ineq_constr_vec_t &in,
                             std::span<real_t> out) const {
        assert(in.size() == out.size());
        std::ranges::copy(in, out.begin());
    }
    void unscale_eq_constr(const eq_constr_vec_t &,
                           std::span<real_t> out) const {
        assert(0 == out.size());
    }

    bool active_set_change(real_t, const ineq_constr_vec_t &Σ,
                           const active_set_t &J, const active_set_t &J_old) {
        bool same_active_set =
            std::ranges::size(J) == std::ranges::size(J_old) &&
            std::ranges::equal(J, J_old);
        if (same_active_set)
            return false;
        if (reset_factorization)
            return true;
        // bool do_reset_fac    = ++num_updates > settings.max_update_count;
        // index_t num_changing = 0;
        // using std::views::zip;
        // for (auto [Jni, Joi] : zip(J, J_old))
        //     num_changing += index_t(Jni != Joi);
        // do_reset_fac |= 8 * num_changing >= num_ineq_constr();
        // if (do_reset_fac) {
        //     reset_factorization = true;
        //     return;
        // }
        // // std::cout << "                                     -- Fact update\n";
        // ocp.updowndate(Σ.storage, J_old.storage, J.storage);
        return true;
    }

    void solve([[maybe_unused]] const var_vec_t &x, const var_vec_t &grad,
               const var_vec_t &Mᵀλ, const var_vec_t &Aᵀŷ,
               const eq_constr_vec_t &Mxb, real_t S, const ineq_constr_vec_t &Σ,
               const active_set_t &J, //
               var_vec_t &d, var_vec_t &ξ, ineq_constr_vec_t &Ad,
               eq_constr_vec_t &Δλ, var_vec_t &MᵀΔλ) {
        using std::isfinite;
        using std::views::zip;
        if (std::exchange(reset_factorization, false)) {
            // FACTOR
        }
        // SOLVE
    }

    std::map<std::string, koqkatoo::DefaultTimings> clear_timings() {
        return {}; // TODO
    }
};

namespace linalg = guanaqo::linalg;

using CSCConverter =
    linalg::sparsity::SparsityConverter<linalg::Sparsity, sparse_csc_t>;
struct LadelSchurBackend::Converter : CSCConverter {
    using CSCConverter::CSCConverter;
};

LadelSchurBackend::LadelSchurBackend(QPData data,
                                     const LadelSchurBackendSettings &settings)
    : settings{settings} {
    std::tie(n, m) = check_dims(data);

    // Check the symmetry of the Hessian
    auto check_symmetry = []<class T>(T &sp) {
        if constexpr (std::is_same_v<T, linalg::sparsity::Dense>)
            if (sp.symmetry == linalg::sparsity::Symmetry::Unsymmetric)
                sp.symmetry = linalg::sparsity::Symmetry::Upper;
        if (sp.symmetry != linalg::sparsity::Symmetry::Upper)
            throw std::invalid_argument("Matrix Q should store its upper "
                                        "triangular part only (use "
                                        "sparsity::Symmetry::Upper)");
    };
    std::visit(check_symmetry, data.Q_sparsity.value);

    // Convert input matrices to CSC matrices with the correct index types
    sparsity_Q = std::make_unique<Converter>(data.Q_sparsity);
    values_Q   = sparsity_Q->convert_values_copy(data.Q);
    this->Q    = sparse_to_ladel_view(*sparsity_Q, values_Q);

    sparsity_A = std::make_unique<Converter>(data.A_sparsity);
    values_A   = sparsity_A->convert_values_copy(data.A);
    this->A    = sparse_to_ladel_view(*sparsity_A, values_A);

    // Allocate storage
    m = this->A.nrow;
    n = this->A.ncol;
    constr_idx.resize(m);
    rhs.resize(n);
    res.resize(n);
    work_m.resize(m);
    entering.reserve(m);
    leaving.reserve(m);
    active_idx.reserve(m);
    ladel_work_nm     = ladel_workspace_create(std::max(n, m));
    factorization.sym = ladel_symbolics_create(n);
    previous.active   = std::make_unique<bool[]>(m);
    AᵀsqrtΣ.reset();
    this->S                   = S;
    reset_newton              = true;
    factorization.initialized = false;
}

template <class Abi>
unique_OCPBackend<Abi>::~unique_OCPBackend() = default;

inline unique_LadelSchurBackend::unique_LadelSchurBackend(
    std::unique_ptr<LadelSchurBackend> &&o) noexcept
    : std::unique_ptr<LadelSchurBackend>{std::move(o)} {}

template <class Abi>
unique_OCPBackend<Abi>
make_qpalm_ocp_backend(const ocp::LinearOCPStorage &ocp_, OCPData data,
                       const LadelSchurBackendSettings &settings) {
    return {std::make_unique<OCPBackend<Abi>>(ocp_, data, settings)};
}

} // namespace cyqlone::qpalm
