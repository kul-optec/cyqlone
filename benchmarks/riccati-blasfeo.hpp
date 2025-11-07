#pragma once

#include <cyqlone/config.hpp>
#include <batmat/assume.hpp>
#include <batmat/config.hpp>
#include <hyhound/ocp/riccati.hpp>
#include <blasfeo.h>
#include <algorithm>
#include <new>
#include <utility>
#include <vector>

namespace CYQLONE_NS(cyqlone::blasfeo) {

using batmat::index_t;
using batmat::real_t;

using hyhound::ocp::vw;

template <class T>
using MatrixView = guanaqo::MatrixView<T, index_t>;

template <class T>
struct blasfeo_types;
template <>
struct blasfeo_types<double> {
    using xmat = blasfeo_dmat;
    using xvec = blasfeo_dvec;
};
template <>
struct blasfeo_types<float> {
    using xmat = blasfeo_smat;
    using xvec = blasfeo_svec;
};
template <class T>
using blasfeo_xmat = blasfeo_types<T>::xmat;
template <class T>
using blasfeo_xvec = blasfeo_types<T>::xvec;

inline void blasfeo_allocate_xmat(int m, int n, blasfeo_dmat *sA) {
    ::blasfeo_allocate_dmat(m, n, sA);
}
inline void blasfeo_allocate_xmat(int m, int n, blasfeo_smat *sA) {
    ::blasfeo_allocate_smat(m, n, sA);
}
inline void blasfeo_pack_xmat(int m, int n, const double *A, int ldA, blasfeo_dmat *sB, int bi,
                              int bj) {
    ::blasfeo_pack_dmat(m, n, const_cast<double *>(A), ldA, sB, bi, bj);
}
inline void blasfeo_pack_xmat(int m, int n, const float *A, int ldA, blasfeo_smat *sB, int bi,
                              int bj) {
    ::blasfeo_pack_smat(m, n, const_cast<float *>(A), ldA, sB, bi, bj);
}
inline void blasfeo_unpack_xmat(int m, int n, const blasfeo_dmat *sA, int ai, int aj, double *B,
                                int ldB) {
    ::blasfeo_unpack_dmat(m, n, const_cast<blasfeo_dmat *>(sA), ai, aj, B, ldB);
}
inline void blasfeo_unpack_xmat(int m, int n, const blasfeo_smat *sA, int ai, int aj, float *B,
                                int ldB) {
    ::blasfeo_unpack_smat(m, n, const_cast<blasfeo_smat *>(sA), ai, aj, B, ldB);
}
inline void blasfeo_pack_tran_xmat(int m, int n, const double *A, int ldA, blasfeo_dmat *sB, int bi,
                                   int bj) {
    ::blasfeo_pack_tran_dmat(m, n, const_cast<double *>(A), ldA, sB, bi, bj);
}
inline void blasfeo_pack_tran_xmat(int m, int n, const float *A, int ldA, blasfeo_smat *sB, int bi,
                                   int bj) {
    ::blasfeo_pack_tran_smat(m, n, const_cast<float *>(A), ldA, sB, bi, bj);
}
inline void blasfeo_unpack_tran_xmat(int m, int n, const blasfeo_dmat *sA, int ai, int aj,
                                     double *B, int ldB) {
    ::blasfeo_unpack_tran_dmat(m, n, const_cast<blasfeo_dmat *>(sA), ai, aj, B, ldB);
}
inline void blasfeo_unpack_tran_xmat(int m, int n, const blasfeo_smat *sA, int ai, int aj, float *B,
                                     int ldB) {
    ::blasfeo_unpack_tran_smat(m, n, const_cast<blasfeo_smat *>(sA), ai, aj, B, ldB);
}
inline void blasfeo_free_xmat(blasfeo_dmat *sA) { ::blasfeo_free_dmat(sA); }
inline void blasfeo_free_xmat(blasfeo_smat *sA) { ::blasfeo_free_smat(sA); }

template <class T>
struct blasfeo_owning_xmat {
    using value_type = T;
    using xmat_t     = blasfeo_xmat<value_type>;
    xmat_t mat{};
    blasfeo_owning_xmat() noexcept = default;
    blasfeo_owning_xmat(index_t m, index_t n) {
        blasfeo_allocate_xmat(static_cast<int>(m), static_cast<int>(n), &mat);
        if (!mat.mem)
            throw std::bad_alloc();
        // No need to deallocate if allocation failed
    }
    void pack(MatrixView<const value_type> A, index_t i = 0, index_t j = 0) {
        BATMAT_ASSUME(static_cast<int>(A.rows + i) <= mat.m);
        BATMAT_ASSUME(static_cast<int>(A.cols + j) <= mat.n);
        blasfeo_pack_xmat(static_cast<int>(A.rows), static_cast<int>(A.cols), A.data,
                          static_cast<int>(A.outer_stride), &mat, static_cast<int>(i),
                          static_cast<int>(j));
    }
    void unpack(MatrixView<value_type> A, index_t i = 0, index_t j = 0) const {
        BATMAT_ASSUME(static_cast<int>(A.rows + i) <= mat.m);
        BATMAT_ASSUME(static_cast<int>(A.cols + j) <= mat.n);
        blasfeo_unpack_xmat(static_cast<int>(A.rows), static_cast<int>(A.cols), &mat,
                            static_cast<int>(i), static_cast<int>(j), A.data,
                            static_cast<int>(A.outer_stride));
    }
    void pack_tran(MatrixView<const value_type> A, index_t i = 0, index_t j = 0) {
        BATMAT_ASSUME(static_cast<int>(A.cols + i) <= mat.m);
        BATMAT_ASSUME(static_cast<int>(A.rows + j) <= mat.n);
        blasfeo_pack_tran_xmat(static_cast<int>(A.rows), static_cast<int>(A.cols), A.data,
                               static_cast<int>(A.outer_stride), &mat, static_cast<int>(i),
                               static_cast<int>(j));
    }
    void unpack_tran(MatrixView<value_type> A, index_t i = 0, index_t j = 0) const {
        BATMAT_ASSUME(static_cast<int>(A.cols + i) <= mat.m);
        BATMAT_ASSUME(static_cast<int>(A.rows + j) <= mat.n);
        blasfeo_unpack_tran_xmat(static_cast<int>(A.rows), static_cast<int>(A.cols), &mat,
                                 static_cast<int>(i), static_cast<int>(j), A.data,
                                 static_cast<int>(A.outer_stride));
    }
    explicit blasfeo_owning_xmat(MatrixView<const value_type> A)
        : blasfeo_owning_xmat(A.rows, A.cols) {
        pack(A);
    }
    friend void swap(blasfeo_owning_xmat &a, blasfeo_owning_xmat &b) noexcept {
        using std::swap;
        swap(a.mat, b.mat);
    }
    blasfeo_owning_xmat(const blasfeo_owning_xmat &)            = delete;
    blasfeo_owning_xmat &operator=(const blasfeo_owning_xmat &) = delete;
    blasfeo_owning_xmat(blasfeo_owning_xmat &&o) noexcept { swap(*this, o); }
    blasfeo_owning_xmat &operator=(blasfeo_owning_xmat &&o) noexcept {
        swap(*this, o);
        return *this;
    }
    ~blasfeo_owning_xmat() {
        if (mat.mem)
            blasfeo_free_xmat(&mat);
    }
    operator xmat_t *() { return &mat; }
    operator const xmat_t *() const { return &mat; }
};

struct OCPDataRiccati {
    index_t N  = 31;
    index_t nx = 40, nu = 10, ny = 10;

    using blasfeo_mat = blasfeo_owning_xmat<real_t>;
    static std::vector<blasfeo_mat> allocate(index_t n, index_t r, index_t c) {
        std::vector<blasfeo_mat> mat(n);
        std::ranges::generate(mat, [&] { return blasfeo_mat(r, c); });
        return mat;
    }

    std::vector<blasfeo_mat> Fᵀ  = allocate(N, nu + nx, nx);
    std::vector<blasfeo_mat> Gᵀ  = allocate(N + 1, nu + nx, ny);
    std::vector<blasfeo_mat> RSQ = allocate(N + 1, nu + nx, nu + nx);

    static OCPDataRiccati from_riccati(const hyhound::ocp::OCPDataRiccati &ric) {
        OCPDataRiccati ocp{.N = ric.N, .nx = ric.nx, .nu = ric.nu, .ny = ric.ny};
        for (index_t i = 0; i < ocp.N; ++i) {
            ocp.Fᵀ[i].pack_tran(vw(ric.F(i)));
            ocp.Gᵀ[i].pack_tran(vw(ric.G(i)));
            ocp.RSQ[i].pack(vw(ric.H(i)));
        }
        ocp.Gᵀ[ocp.N].pack_tran(vw(ric.C(ocp.N)));
        ocp.RSQ[ocp.N].pack(vw(ric.Q(ocp.N)));
        return ocp;
    }
};

struct RiccatiFactor {
    const OCPDataRiccati &ocp;
    using blasfeo_mat = OCPDataRiccati::blasfeo_mat;

    std::vector<blasfeo_mat> L = ocp.allocate(ocp.N + 1, ocp.nu + ocp.nx, ocp.nu + ocp.nx);
    blasfeo_mat V              = blasfeo_mat(ocp.nu + ocp.nx, ocp.nx + ocp.ny);
};

void factor(RiccatiFactor &factor, Eigen::Ref<const Eigen::MatrixX<real_t>>);
void update(RiccatiFactor &factor, Eigen::Ref<const Eigen::MatrixX<real_t>>);
void solve(RiccatiFactor &factor);

} // namespace CYQLONE_NS(cyqlone::blasfeo)
