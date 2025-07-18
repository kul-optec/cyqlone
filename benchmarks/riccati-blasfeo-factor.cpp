#include "riccati-blasfeo.hpp"
#include <guanaqo/trace.hpp>

namespace cyqlone::blasfeo {

void blasfeo_xcolsc(int kmax, double alpha, blasfeo_dmat *sA, int ai, int aj) {
    ::blasfeo_dcolsc(kmax, alpha, sA, ai, aj);
}
void blasfeo_xcolsc(int kmax, float alpha, blasfeo_smat *sA, int ai, int aj) {
    ::blasfeo_scolsc(kmax, alpha, sA, ai, aj);
}
void blasfeo_xgecp(int m, int n, const blasfeo_dmat *sA, int ai, int aj, blasfeo_dmat *sB, int bi,
                   int bj) {
    ::blasfeo_dgecp(m, n, const_cast<blasfeo_dmat *>(sA), ai, aj, sB, bi, bj);
}
void blasfeo_xgecp(int m, int n, const blasfeo_smat *sA, int ai, int aj, blasfeo_smat *sB, int bi,
                   int bj) {
    ::blasfeo_sgecp(m, n, const_cast<blasfeo_smat *>(sA), ai, aj, sB, bi, bj);
}
void blasfeo_xsyrk_xpotrf_ln(int m, int k, const blasfeo_dmat *sA, int ai, int aj,
                             const blasfeo_dmat *sB, int bi, int bj, const blasfeo_dmat *sC, int ci,
                             int cj, blasfeo_dmat *sD, int di, int dj) {
    GUANAQO_TRACE("blasfeo_syrk_potrf", 0,
                  m * (m + 1) * k / 2 + (m - 1) * m * (m + 1) / 6 + m * (m - 1) / 2 + 2 * m);
    blasfeo_dsyrk_dpotrf_ln(m, k, const_cast<blasfeo_dmat *>(sA), ai, aj,
                            const_cast<blasfeo_dmat *>(sB), bi, bj, const_cast<blasfeo_dmat *>(sC),
                            ci, cj, sD, di, dj);
}
void blasfeo_xsyrk_xpotrf_ln(int m, int k, const blasfeo_smat *sA, int ai, int aj,
                             const blasfeo_smat *sB, int bi, int bj, const blasfeo_smat *sC, int ci,
                             int cj, blasfeo_smat *sD, int di, int dj) {
    GUANAQO_TRACE("blasfeo_syrk_potrf", 0,
                  m * (m + 1) * k / 2 + (m - 1) * m * (m + 1) / 6 + m * (m - 1) / 2 + 2 * m);
    blasfeo_ssyrk_spotrf_ln(m, k, const_cast<blasfeo_smat *>(sA), ai, aj,
                            const_cast<blasfeo_smat *>(sB), bi, bj, const_cast<blasfeo_smat *>(sC),
                            ci, cj, sD, di, dj);
}
void blasfeo_xtrmm_rlnn(int m, int n, double alpha, const blasfeo_dmat *sA, int ai, int aj,
                        const blasfeo_dmat *sB, int bi, int bj, blasfeo_dmat *sD, int di, int dj) {
    GUANAQO_TRACE("blasfeo_trmm", 0, n * (n + 1) * m / 2);
    blasfeo_dtrmm_rlnn(m, n, alpha, const_cast<blasfeo_dmat *>(sA), ai, aj,
                       const_cast<blasfeo_dmat *>(sB), bi, bj, sD, di, dj);
}
void blasfeo_xtrmm_rlnn(int m, int n, float alpha, const blasfeo_smat *sA, int ai, int aj,
                        const blasfeo_smat *sB, int bi, int bj, blasfeo_smat *sD, int di, int dj) {
    GUANAQO_TRACE("blasfeo_trmm", 0, n * (n + 1) * m / 2);
    blasfeo_strmm_rlnn(m, n, alpha, const_cast<blasfeo_smat *>(sA), ai, aj,
                       const_cast<blasfeo_smat *>(sB), bi, bj, sD, di, dj);
}

void factor(RiccatiFactor &factor, Eigen::Ref<const Eigen::MatrixX<real_t>> Σ) {
    using std::sqrt;
    const auto &ocp = factor.ocp;
    const auto nx   = static_cast<int>(ocp.nx);
    const auto nu   = static_cast<int>(ocp.nu);
    const auto nux  = static_cast<int>(ocp.nu + ocp.nx);
    /* j = N */ {
        GUANAQO_TRACE("riccati", ocp.N);
        const auto &CNᵀ = ocp.Gᵀ[ocp.N];
        int ic          = 0;
        for (int i = 0; i < ocp.ny; ++i) {
            if (Σ(i, ocp.N) == 0)
                continue;
            // Copy one constraint gradient to a column of V
            blasfeo_xgecp(nx, 1, CNᵀ, 0, i, factor.V, 0, ic);
            // Scale this column by √Σ
            blasfeo_xcolsc(nx, sqrt(Σ(i, ocp.N)), factor.V, 0, ic);
            ++ic;
        }
        blasfeo_xsyrk_xpotrf_ln(nx, ic, factor.V, 0, 0, factor.V, 0, 0, ocp.RSQ[ocp.N], 0, 0,
                                factor.L[ocp.N], nu, nu);
    }
    for (index_t j = ocp.N; j-- > 0;) {
        GUANAQO_TRACE("riccati", j);
        const auto &Gjᵀ = ocp.Gᵀ[j];
        blasfeo_xtrmm_rlnn(nux, nx, 1, factor.L[j + 1], nu, nu, ocp.Fᵀ[j], 0, 0, factor.V, 0, 0);
        int ic = nx;
        for (int i = 0; i < ocp.ny; ++i) {
            if (Σ(i, j) == 0)
                continue;
            // Copy one constraint gradient to a column of V
            blasfeo_xgecp(nux, 1, Gjᵀ, 0, i, factor.V, 0, ic);
            // Scale this column by √Σ
            blasfeo_xcolsc(nux, sqrt(Σ(i, j)), factor.V, 0, ic);
            ++ic;
        }
        blasfeo_xsyrk_xpotrf_ln(nux, ic, factor.V, 0, 0, factor.V, 0, 0, ocp.RSQ[j], 0, 0,
                                factor.L[j], 0, 0);
    }
}

} // namespace cyqlone::blasfeo
