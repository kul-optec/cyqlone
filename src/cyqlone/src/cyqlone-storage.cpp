#include <cyqlone/cyqlone-storage.hpp>
#include <cyqlone/reduce.hpp>
#include <batmat/assume.hpp>
#include <batmat/config.hpp>
#include <guanaqo/blas/hl-blas-interface.hpp>
#include <limits>
#include <numeric>

namespace CYQLONE_NS(cyqlone) {

// TODO: make member function to reuse Ju0 and ny_0?
template <class T>
auto CyqloneStorage<T>::reconstruct_ineq_multipliers(std::span<const value_type> y_compressed) const
    -> std::vector<value_type> {
    std::vector<value_type> y(static_cast<size_t>(N_horiz * ny + ny_N));
    reconstruct_ineq_multipliers(y_compressed, y);
    return y;
}

template <class T>
void CyqloneStorage<T>::reconstruct_ineq_multipliers(std::span<const value_type> y_compressed,
                                                     std::span<value_type> y) const {
    BATMAT_ASSERT(static_cast<index_t>(y.size()) == N_horiz * ny + ny_N);
    BATMAT_ASSERT(static_cast<index_t>(y_compressed.size()) == (N_horiz - 1) * ny + ny_0 + ny_N);
    for (index_t r = 0, j = 0; r < ny; ++r)
        if (Ju0[r])
            y[r] = y_compressed[j++];
        else
            y[r] = 0;
    std::ranges::copy(y_compressed.subspan(ny_0), y.begin() + ny);
    // TODO: copy_reverse to allow performing everything in-place
}

template <class T>
auto CyqloneStorage<T>::reconstruct_solution(const LinearOCPStorage &ocp,
                                             std::span<const value_type> ux_compressed,
                                             std::span<const value_type> y_compressed,
                                             std::span<const value_type> λ_compressed) const
    -> Solution {
    Solution sol;
    sol.solution.resize(static_cast<size_t>(N_horiz * (nu + nx) + nx));
    sol.inequality_multipliers.resize(static_cast<size_t>(N_horiz * ny + ny_N));
    sol.equality_multipliers.resize(static_cast<size_t>(N_horiz * nx + nx));
    reconstruct_ineq_multipliers(y_compressed, sol.inequality_multipliers);
    std::ranges::copy_n(ocp.b(0).data, nx, sol.solution.begin()); // x0
    std::ranges::copy(ux_compressed, sol.solution.begin() + nx);
    std::ranges::copy(λ_compressed, sol.equality_multipliers.begin() + nx);
    using MatrixView = guanaqo::MatrixView<value_type, index_t>;
    MatrixView λ0{{.data = sol.equality_multipliers.data(), .rows = nx, .cols = 1}},
        λ1{{.data = sol.equality_multipliers.data() + nx, .rows = nx, .cols = 1}},
        x0{{.data = sol.solution.data(), .rows = nx, .cols = 1}},
        u0{{.data = sol.solution.data() + nx, .rows = nu, .cols = 1}};
    λ0 = ocp.q(0);
    guanaqo::blas::xsymv_L(value_type{1}, ocp.Q(0), x0, value_type{1}, λ0);
    guanaqo::blas::xgemv_T(value_type{1}, ocp.S(0), u0, value_type{1}, λ0);
    guanaqo::blas::xgemv_T(value_type{1}, ocp.A(0), λ1, value_type{1}, λ0);
    return sol;
}

template <class T>
auto CyqloneStorage<T>::compute_kkt_error(const LinearOCPStorage &ocp, const Solution &sol)
    -> KKTError {
    using std::clamp;
    const auto [N, nx, nu, ny, ny_N] = ocp.dim;
    std::vector<value_type> dual_residual_sto(sol.solution.size());
    auto dual_residual = guanaqo::MatrixView<value_type, index_t>::as_column(dual_residual_sto);
    auto x             = guanaqo::MatrixView<const value_type, index_t>::as_column(sol.solution),
         y = guanaqo::MatrixView<const value_type, index_t>::as_column(sol.inequality_multipliers),
         λ = guanaqo::MatrixView<const value_type, index_t>::as_column(sol.equality_multipliers);

    // Stationarity
    dual_residual = ocp.qr();
    for (index_t j = 0; j < N; ++j) {
        auto xuj = x.middle_rows(j * (nx + nu), nx + nu), λj = λ.middle_rows(j * nx, nx),
             λj_next = λ.middle_rows((j + 1) * nx, nx), yj = y.middle_rows(j * ny, ny);
        auto rj = dual_residual.middle_rows(j * (nx + nu), nx + nu);
        guanaqo::blas::xsymv_L(value_type{1}, ocp.H(j), xuj, value_type{1}, rj);
        guanaqo::blas::xgemv_T(value_type{1}, ocp.AB(j), λj_next, value_type{1}, rj);
        guanaqo::blas::xgemv_T(value_type{1}, ocp.CD(j), yj, value_type{1}, rj);
        for (index_t i = 0; i < nx; ++i)
            rj(i, 0) -= λj(i, 0);
    }
    auto xN = x.bottom_rows(nx), λN = λ.bottom_rows(nx), yN = y.bottom_rows(ny_N);
    auto rN = dual_residual.bottom_rows(nx);
    guanaqo::blas::xsymv_L(value_type{1}, ocp.Q(N), xN, value_type{1}, rN);
    guanaqo::blas::xgemv_T(value_type{1}, ocp.C(N), yN, value_type{1}, rN);
    for (index_t i = 0; i < nx; ++i)
        rN(i, 0) -= λN(i, 0);
    const auto norms = cyqlone::norms<value_type>{};
    auto stat_norm   = std::accumulate(dual_residual.data, dual_residual.data + dual_residual.rows,
                                       norms.zero(), norms);

    // Inequality constraints
    auto ineq_res_sto = std::vector<value_type>(sol.inequality_multipliers.size());
    auto ineq_res     = guanaqo::MatrixView<value_type, index_t>::as_column(ineq_res_sto);
    auto compl_norm   = norms.zero();
    using std::fmax;
    using std::fmin;
    using std::sqrt;
    const auto eps = std::numeric_limits<value_type>::epsilon(), big = 1 / (eps * eps);
    for (index_t j = 0; j < N; ++j) {
        auto xuj = x.middle_rows(j * (nx + nu), nx + nu), yj = y.middle_rows(j * ny, ny);
        auto lbj = ocp.b_min(j), ubj = ocp.b_max(j);
        auto cj = ineq_res.middle_rows(j * ny, ny);
        guanaqo::blas::xgemv_N(value_type{1}, ocp.CD(j), xuj, value_type{-1}, cj);
        for (index_t i = 0; i < ny; ++i) {
            compl_norm =
                norms(compl_norm, yj(i, 0) > 0 ? yj(i, 0) * (cj(i, 0) - fmin(ubj(i, 0), +big))
                                               : yj(i, 0) * (cj(i, 0) - fmax(lbj(i, 0), -big)));
            cj(i, 0) -= clamp(cj(i, 0), lbj(i, 0), ubj(i, 0));
        }
    }
    auto cN = ineq_res.bottom_rows(ny_N);
    guanaqo::blas::xgemv_N(value_type{1}, ocp.C(N), xN, value_type{-1}, cN);
    auto lbN = ocp.b_min().bottom_rows(ny_N), ubN = ocp.b_max().bottom_rows(ny_N);
    for (index_t i = 0; i < ny_N; ++i) {
        compl_norm =
            norms(compl_norm, yN(i, 0) > 0 ? yN(i, 0) * (cN(i, 0) - fmin(ubN(i, 0), +big))
                                           : yN(i, 0) * (cN(i, 0) - fmax(lbN(i, 0), -big)));
        cN(i, 0) -= clamp(cN(i, 0), lbN(i, 0), ubN(i, 0));
    }
    auto ineq_norm =
        std::accumulate(ineq_res.data, ineq_res.data + ineq_res.rows, norms.zero(), norms);
    // Complementarity: lb - Cx <= 0    and    Cx - ub <= 0

    // Equality constraints
    auto eq_res_sto = std::vector<value_type>(sol.equality_multipliers.size());
    auto eq_res     = guanaqo::MatrixView<value_type, index_t>::as_column(eq_res_sto);
    eq_res          = ocp.b();
    for (index_t j = 0; j <= N; ++j) {
        auto xj = x.middle_rows(j * (nx + nu), nx);
        auto cj = eq_res.middle_rows(j * nx, nx);
        for (index_t i = 0; i < nx; ++i)
            cj(i, 0) -= xj(i, 0);
        if (j > 0) {
            auto xu_prev = x.middle_rows((j - 1) * (nx + nu), nx + nu);
            guanaqo::blas::xgemv_N(value_type{1}, ocp.AB(j - 1), xu_prev, value_type{1}, cj);
        }
    }
    auto eq_norm = std::accumulate(eq_res.data, eq_res.data + eq_res.rows, norms.zero(), norms);

    return {
        .stationarity        = stat_norm.norminf(),
        .inequality_residual = ineq_norm.norminf(),
        .equality_residual   = eq_norm.norminf(),
        .complementarity     = compl_norm.norminf(),
    };
}

template <class T>
auto CyqloneStorage<T>::compute_kkt_error(const LinearOCPStorage &ocp,
                                          std::span<const value_type> ux_compressed,
                                          std::span<const value_type> y_compressed,
                                          std::span<const value_type> λ_compressed) const
    -> KKTError {
    return compute_kkt_error(ocp,
                             reconstruct_solution(ocp, ux_compressed, y_compressed, λ_compressed));
}

template <class T>
index_t CyqloneStorage<T>::count_constr_0(const LinearOCPStorage &ocp, std::vector<bool> &Ju0) {
    const auto [N, nx, nu, ny, ny_N] = ocp.dim;
    const auto nJu0                  = static_cast<index_t>(Ju0.size());
    BATMAT_ASSUME(nJu0 == ny);
    for (index_t c = 0; c < nu; ++c)
        for (index_t r = 0; r < ny; ++r)
            Ju0[r] = Ju0[r] || ocp.D(0)(r, c) != 0;
    return static_cast<index_t>(std::ranges::count(Ju0, true));
}

template <class T>
void CyqloneStorage<T>::update(const LinearOCPStorage &ocp) {
    const auto ny_0_ = count_constr_0(ocp, Ju0);
    BATMAT_ASSERT(ny_0_ <= ny_0);
    update_impl(ocp);
}

template <class T>
void CyqloneStorage<T>::update_impl(const LinearOCPStorage &ocp) {
    const auto N = N_horiz;
    // H₀ = [ R₀ 0 ]
    //      [ 0  Qₙ]
    data_H(0).top_left(nu, nu)     = ocp.R(0);
    data_H(0).bottom_right(nx, nx) = ocp.Q(N);
    data_H(0).bottom_left(nx, nu).set_constant(0);
    data_H(0).top_right(nu, nx).set_constant(0);
    // F₀ = [ B₀ 0 ]
    data_F(0).left_cols(nu) = ocp.B(0);
    data_F(0).right_cols(nx).set_constant(0);
    // G₀ = [ D₀ 0 ]  ny_0
    //      [ 0  Cₙ]  ny_N
    data_G(0).bottom_left(ny_N, nu).set_constant(0);
    data_G(0).top_right(ny_0, nx).set_constant(0);
    index_t j = 0;
    for (index_t r = 0; r < ny; ++r) {
        if (Ju0[r]) {
            BATMAT_ASSUME(j < ny_0);
            data_G0N(0).block(j, 0, 1, nu) = ocp.D(0).middle_rows(r, 1);
            value_type t                   = 0;
            for (index_t c = 0; c < nx; ++c) // lb - C₀ x₀
                t += ocp.C(0)(r, c) * ocp.b(0)(c, 0);
            data_lb0N(0, j, 0) = ocp.b_min(0)(r, 0) - t;
            data_ub0N(0, j, 0) = ocp.b_max(0)(r, 0) - t;
            indices_G0[j]      = r;
            ++j;
        }
    }
    data_G0N(0).block(j, 0, ny_0 - j, nu).set_constant(0);
    data_G0N(0).bottom_right(ny_N, nx) = ocp.C(N);
    data_lb0N(0).bottom_rows(ny_N)     = ocp.b_min().bottom_rows(ny_N);
    data_ub0N(0).bottom_rows(ny_N)     = ocp.b_max().bottom_rows(ny_N);
    // c̃₀ = c₀ + A₀ x₀      (b_eq = [x₀, c₀, ... cₙ₋₁])
    data_c(0) = ocp.b(1);
    for (index_t r = 0; r < nx; ++r)
        for (index_t c = 0; c < nx; ++c)
            data_c(0, r, 0) += ocp.A(0)(r, c) * ocp.b(0)(c, 0);
    // r̃₀ = r₀ + S₀ x₀
    data_rq(0).bottom_rows(nx) = ocp.q(N);
    data_rq(0).top_rows(nu)    = ocp.r(0);
    for (index_t r = 0; r < nu; ++r)
        for (index_t c = 0; c < nx; ++c)
            data_rq(0, r, 0) += ocp.S_trans(0)(c, r) * ocp.b(0)(c, 0);
    for (index_t i = 1; i < N; ++i) {
        data_H(i).top_left(nu, nu)     = ocp.R(i);
        data_H(i).bottom_left(nx, nu)  = ocp.S_trans(i);
        data_H(i).top_right(nu, nx)    = ocp.S(i);
        data_H(i).bottom_right(nx, nx) = ocp.Q(i);
        data_F(i).left_cols(nu)        = ocp.B(i);
        data_F(i).right_cols(nx)       = ocp.A(i);
        data_G(i - 1).left_cols(nu)    = ocp.D(i);
        data_G(i - 1).right_cols(nx)   = ocp.C(i);
        data_lb(i - 1)                 = ocp.b_min(i);
        data_ub(i - 1)                 = ocp.b_max(i);
        data_c(i)                      = ocp.b(i + 1);
        data_rq(i).bottom_rows(nx)     = ocp.q(i);
        data_rq(i).top_rows(nu)        = ocp.r(i);
    }
}

template <class T>
CyqloneStorage<T> CyqloneStorage<T>::build(const LinearOCPStorage &ocp, index_t ny_0) {
    const auto [N, nx, nu, ny, ny_N] = ocp.dim;
    // Count the number of input constraints in the first stage
    std::vector<bool> Ju0(ny);
    const auto ny_0_ = count_constr_0(ocp, Ju0);
    if (ny_0 < 0)
        ny_0 = ny_0_;
    else
        BATMAT_ASSERT(ny_0_ <= ny_0);
    CyqloneStorage<T> res{.N_horiz = N,
                          .nx      = nx,
                          .nu      = nu,
                          .ny      = ny,
                          .ny_0    = ny_0,
                          .ny_N    = ny_N,
                          .Ju0     = std::move(Ju0)};
    res.update_impl(ocp);
    return res;
}

template struct CyqloneStorage<double>;
#if BATMAT_WITH_SINGLE
template struct CyqloneStorage<float>;
#endif

} // namespace CYQLONE_NS(cyqlone)
