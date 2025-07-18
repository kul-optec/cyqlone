#include <cyqlone/qpalm/example-problems/conversion.hpp>
#include <guanaqo/blas/hl-blas-interface.hpp>
#include <algorithm>
#include <numeric>

namespace cyqlone::qpalm {

std::vector<real_t> reference_to_gradient(const LinearOCPStorage &ocp,
                                          std::span<const real_t> ref) {
    auto [N, nx, nu, ny, ny_N] = ocp.dim;
    std::vector<real_t> grad(N * (nx + nu) + nx);
    auto n_ref = static_cast<index_t>(ref.size());
    if (n_ref == N * (nx + nu) + nx) {
        auto H0 = ocp.H(0), HN = ocp.H(N);
        guanaqo::blas::xgemv_batch_strided(CblasColMajor, CblasNoTrans, nx + nu, nx + nu,
                                           real_t{-1}, H0.data, H0.outer_stride,
                                           H0.outer_stride * H0.cols, ref.data(), index_t{1},
                                           nx + nu, real_t{0}, grad.data(), index_t{1}, nx + nu, N);
        index_t off_N = N * (nx + nu);
        guanaqo::blas::xgemv(CblasColMajor, CblasNoTrans, nx, nx, real_t{-1}, HN.data,
                             HN.outer_stride, &ref[off_N], index_t{1}, real_t{0}, &grad[off_N],
                             index_t{1});
    } else if (n_ref == 2 * nx + nu) {
        auto H0 = ocp.H(0), HN = ocp.H(N);
        guanaqo::blas::xgemv_batch_strided(
            CblasColMajor, CblasNoTrans, nx + nu, nx + nu, real_t{-1}, H0.data, H0.outer_stride,
            H0.outer_stride * H0.cols, ref.data(), index_t{1}, index_t{0}, real_t{0}, grad.data(),
            index_t{1}, nx + nu, N);
        index_t off_N = N * (nx + nu);
        guanaqo::blas::xgemv(CblasColMajor, CblasNoTrans, nx, nx, real_t{-1}, HN.data,
                             HN.outer_stride, &ref[nx + nu], index_t{1}, real_t{0}, &grad[off_N],
                             index_t{1});
    }
    return grad;
}

LinearOCPSparseQP LinearOCPSparseQP::build(const LinearOCPStorage &ocp) {
    using guanaqo::linalg::sparsity::Symmetry;
    LinearOCPSparseQP qp;
    auto [N, nx, nu, ny, ny_N] = ocp.dim;
    // Q matrix
    qp.n          = N * (nx + nu) + nx;
    index_t nnz_Q = N * (nx + nu) * (nx + nu + 1) / 2 + nx * (nx + 1) / 2;
    {
        qp.Q_outer_ptr.reserve(qp.n + 1);
        qp.Q_inner_idx.reserve(nnz_Q);
        qp.Q_values.reserve(nnz_Q);
        index_t r_off = 0;
        for (index_t i = 0; i < N; ++i) {
            auto Hi = ocp.H(i);
            for (index_t c = 0; c < nx + nu; ++c) {
                auto nnz = static_cast<index_t>(qp.Q_inner_idx.size());
                qp.Q_outer_ptr.push_back(nnz);
                for (index_t r = c; r < nx + nu; ++r) {
                    qp.Q_inner_idx.push_back(r_off + r);
                    qp.Q_values.push_back(Hi(r, c));
                }
            }
            r_off += nx + nu;
        }
        auto HN = ocp.H(N);
        for (index_t c = 0; c < nx; ++c) {
            auto nnz = static_cast<index_t>(qp.Q_inner_idx.size());
            qp.Q_outer_ptr.push_back(nnz);
            for (index_t r = c; r < nx; ++r) {
                qp.Q_inner_idx.push_back(r_off + r);
                qp.Q_values.push_back(HN(r, c));
            }
        }
        qp.Q_outer_ptr.push_back(nnz_Q);
        assert(r_off + nx == qp.n);
        assert(qp.Q_outer_ptr.size() == static_cast<size_t>(qp.n + 1));
        assert(qp.Q_inner_idx.size() == static_cast<size_t>(nnz_Q));
        assert(qp.Q_values.size() == static_cast<size_t>(nnz_Q));
        qp.Q_sparsity = {
            .rows      = qp.n,
            .cols      = qp.n,
            .symmetry  = Symmetry::Lower,
            .inner_idx = qp.Q_inner_idx,
            .outer_ptr = qp.Q_outer_ptr,
            .order     = decltype(Q_sparsity)::SortedRows,
        };
    }
    // A matrix
    qp.m_eq            = (N + 1) * nx;
    qp.m_ineq          = N * ny + ny_N;
    index_t m          = qp.m_eq + qp.m_ineq;
    index_t nnz_A_eq   = qp.m_eq + N * nx * (nx + nu);
    index_t nnz_A_ineq = N * ny * (nx + nu) + ny_N * nx;
    index_t nnz_A      = nnz_A_eq + nnz_A_ineq;
    {
        qp.A_outer_ptr.reserve(qp.n + 1);
        qp.A_inner_idx.reserve(nnz_A);
        qp.A_values.reserve(nnz_A);
        index_t r_off_eq = 0, r_off_ineq = qp.m_eq;
        for (index_t i = 0; i < N; ++i) {
            auto ABi = ocp.AB(i), CDi = ocp.CD(i);
            for (index_t c = 0; c < nx + nu; ++c) {
                auto nnz = static_cast<index_t>(qp.A_inner_idx.size());
                qp.A_outer_ptr.push_back(nnz);
                if (c < nx) {
                    qp.A_inner_idx.push_back(r_off_eq + c);
                    qp.A_values.push_back(1);
                }
                for (index_t r = 0; r < nx; ++r) {
                    qp.A_inner_idx.push_back(r_off_eq + nx + r);
                    qp.A_values.push_back(-ABi(r, c));
                }
                for (index_t r = 0; r < ny; ++r) {
                    qp.A_inner_idx.push_back(r_off_ineq + r);
                    qp.A_values.push_back(CDi(r, c));
                }
            }
            r_off_eq += nx;
            r_off_ineq += ny;
        }
        auto CDi = ocp.CD(N);
        for (index_t c = 0; c < nx; ++c) {
            auto nnz = static_cast<index_t>(qp.A_inner_idx.size());
            qp.A_outer_ptr.push_back(nnz);
            qp.A_inner_idx.push_back(r_off_eq + c);
            qp.A_values.push_back(1);
            for (index_t r = 0; r < ny_N; ++r) {
                qp.A_inner_idx.push_back(r_off_ineq + r);
                qp.A_values.push_back(CDi(r, c));
            }
        }
        qp.A_outer_ptr.push_back(nnz_A);
        assert(r_off_eq + nx == qp.m_eq);
        assert(r_off_ineq + ny_N == m);
        assert(qp.A_outer_ptr.size() == static_cast<size_t>(qp.n + 1));
        assert(qp.A_inner_idx.size() == static_cast<size_t>(nnz_A));
        assert(qp.A_values.size() == static_cast<size_t>(nnz_A));
        qp.A_sparsity = {.rows      = m,
                         .cols      = qp.n,
                         .symmetry  = Symmetry::Unsymmetric,
                         .inner_idx = qp.A_inner_idx,
                         .outer_ptr = qp.A_outer_ptr,
                         .order     = decltype(A_sparsity)::SortedRows};
    }
    return qp;
}

auto LinearOCPSparseQP::build_kkt(real_t S, std::span<const real_t> Σ,
                                  std::span<const bool> J) const -> KKTMatrix {
    KKTMatrix K;
    K.outer_ptr.reserve(n + m_ineq + m_eq + 1);
    const auto nnz_max = Q_sparsity.nnz() + A_sparsity.nnz() + m_ineq;
    K.inner_idx.reserve(nnz_max);
    K.values.reserve(nnz_max);
    assert(static_cast<index_t>(J.size()) == m_ineq);
    assert(static_cast<index_t>(Σ.size()) == m_ineq);
    std::vector<index_t> constr_indices(m_ineq + 1);
    std::inclusive_scan(J.begin(), J.end(), constr_indices.begin() + 1, std::plus{}, index_t{0});
    const index_t m_ineq_active = constr_indices.back();
    real_t inv_S                = 1 / S;
    for (index_t c = 0; c < n; ++c) {
        K.outer_ptr.push_back(static_cast<index_t>(K.inner_idx.size()));
        // Q
        index_t Q_ptr = Q_outer_ptr[c], A_eq_ptr = A_outer_ptr[c];
        const index_t Q_end = Q_outer_ptr[c + 1], A_end = A_outer_ptr[c + 1];
        while (Q_ptr < Q_end) {
            K.inner_idx.push_back(Q_inner_idx[Q_ptr]);
            K.values.push_back(Q_values[Q_ptr]);
            if (K.inner_idx.back() == c)
                K.values.back() += inv_S;
            ++Q_ptr;
        }
        // First A_ineq
        auto A_ineq_it =
            std::lower_bound(A_inner_idx.begin() + A_eq_ptr, A_inner_idx.begin() + A_end, m_eq);
        auto A_ineq_ptr     = static_cast<index_t>(A_ineq_it - A_inner_idx.begin());
        const auto A_eq_end = A_ineq_ptr;
        while (A_ineq_ptr < A_end) {
            const index_t r = A_inner_idx[A_ineq_ptr] - m_eq;
            if (J[r]) {
                K.inner_idx.push_back(n + constr_indices[r]);
                K.values.push_back(A_values[A_ineq_ptr]);
            }
            ++A_ineq_ptr;
        }
        // Then A_eq
        while (A_eq_ptr < A_eq_end) {
            K.inner_idx.push_back(n + m_ineq_active + A_inner_idx[A_eq_ptr]);
            K.values.push_back(A_values[A_eq_ptr]);
            ++A_eq_ptr;
        }
    }
    // Diagonal block Σ⁻¹
    for (index_t r = 0; r < m_ineq; ++r) {
        if (J[r]) {
            K.outer_ptr.push_back(static_cast<index_t>(K.inner_idx.size()));
            K.inner_idx.push_back(n + constr_indices[r]);
            K.values.push_back(-1 / Σ[r]);
        }
    }
    std::fill_n(std::back_inserter(K.outer_ptr), m_eq + 1,
                static_cast<index_t>(K.inner_idx.size()));
    using guanaqo::linalg::sparsity::Symmetry;
    K.sparsity = {.rows      = n + m_ineq_active + m_eq,
                  .cols      = n + m_ineq_active + m_eq,
                  .symmetry  = Symmetry::Lower,
                  .inner_idx = K.inner_idx,
                  .outer_ptr = K.outer_ptr,
                  .order     = decltype(K.sparsity)::SortedRows};
    return K;
}

} // namespace cyqlone::qpalm
