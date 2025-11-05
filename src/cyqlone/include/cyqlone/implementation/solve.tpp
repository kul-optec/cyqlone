#include <cyqlone/cyqlone.hpp>

#include <batmat/assume.hpp>
#include <batmat/config.hpp>
#include <batmat/loop.hpp>

#include <batmat/linalg/copy.hpp>
#include <batmat/linalg/gemm.hpp>
#include <batmat/linalg/trsm.hpp>

namespace CYQLONE_NAMESPACE {
using namespace batmat::linalg;

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::solve_active([[maybe_unused]] index_t l,
                                                      [[maybe_unused]] index_t biY,
                                                      [[maybe_unused]] mut_view<> λ) const {
    // TODO: nothing?
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::solve_active_secondary(index_t l, index_t biU,
                                                                mut_view<> λ) const {
    const index_t num_stages = ceil_N >> lP;
    const index_t offset     = 1 << l;
    const index_t biD        = sub_wrap_PmV(biU, offset);
    const index_t biY        = sub_wrap_PmV(biD, offset);
    const index_t diU        = biU * num_stages;
    const index_t diD        = biD * num_stages;
    const index_t diY        = biY * num_stages;
    { // b[diD] -= U[biU] b[diU]
        GUANAQO_TRACE("Subtract Ub", biD);
        gemm_sub(coupling_U.batch(biU), λ.batch(diU), λ.batch(diD));
    }
    { // b[diD] -= Y[biY] b[diY]
        GUANAQO_TRACE("Subtract Yb", biD);
        biD == 0 ? gemm_sub(coupling_Y.batch(biY), λ.batch(diY), λ.batch(diD), {}, with_rotate_C<1>,
                            with_rotate_D<1>, with_mask_D<1>)
                 : gemm_sub(coupling_Y.batch(biY), λ.batch(diY), λ.batch(diD));
    }
    // solve D⁻¹[diD] d[diD]
    if (is_active(l + 1, biD)) {
        GUANAQO_TRACE("Solve b", biD);
        trsm(tril(coupling_D.batch(biD)), λ.batch(diD));
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::solve_riccati_forward(Context &ctx, mut_view<> ux,
                                                               mut_view<> λ) const {
    const index_t ti         = ctx.index;
    const index_t num_stages = ceil_N >> lP;    // number of stages per thread
    const index_t di0        = ti * num_stages; // data batch index
    const index_t biI        = sub_wrap_PmV(ti, 1);
    const index_t diI        = biI * num_stages;
    const index_t di_last    = di0 + num_stages - 1;
    const index_t k0         = ti * num_stages; // stage index
    const index_t nux        = nu + nx;
    auto R̂ŜQ̂                 = riccati_R̂ŜQ̂.batch(ti);
    auto B̂                   = riccati_ÂB̂.batch(ti).right_cols(num_stages * nu);
    auto Â                   = riccati_ÂB̂.batch(ti).left_cols(num_stages * nx);
    auto BAᵀ                 = riccati_BAᵀ.batch(ti);
    for (index_t i = 0; i < num_stages; ++i) {
        index_t k  = sub_wrap_N(k0, i);
        index_t di = di0 + i;
        auto R̂ŜQ̂i  = R̂ŜQ̂.middle_cols(i * nux, nux);
        auto Q̂i    = R̂ŜQ̂i.bottom_right(nx, nx);
        auto B̂i    = B̂.middle_cols(i * nu, nu);
        auto Âi    = Â.middle_cols(i * nx, nx);
        {
            GUANAQO_TRACE("Riccati solve QRS", k);
            // l = LR⁻¹ r, q = LQ⁻¹(q - LS l)
            trsm(tril(R̂ŜQ̂i), ux.batch(di));
            // λ0 -= LB̂ l
            gemm_sub(B̂i, ux.batch(di).top_rows(nu), λ.batch(di0));
        }
        if (i + 1 < num_stages) {
            [[maybe_unused]] const auto k_next = sub_wrap_N(k, 1);
            const auto di_next                 = di + 1;
            auto BAᵀi                          = BAᵀ.middle_cols(i * nx, nx);
            GUANAQO_TRACE("Riccati solve b", k_next);
            // λ0 += Â λ
            gemm_add(Âi, λ.batch(di_next), λ.batch(di0));
            // b = LQᵀb + q
            trmm(tril(Q̂i).transposed(), λ.batch(di_next));
            compact_blas::xadd_copy(simdify(λ.batch(di_next)), simdify(λ.batch(di_next)),
                                    simdify(ux.batch(di).bottom_rows(nx)));
            // l += LB λ, q += LA λ
            gemm_add(BAᵀi, λ.batch(di_next), ux.batch(di_next));
        } else {
            GUANAQO_TRACE("Riccati last", k);
            // λ0 -= Â λ
            gemm_sub(Âi, ux.batch(di).bottom_rows(nx), λ.batch(di0));
        }
    }
    ctx.arrive_and_wait();
    GUANAQO_TRACE("Riccati coupling I", k0);
    // b = LQ⁻ᵀ x + b
    const bool x_lanes = ti == 0; // first stage wraps around
    auto x_last        = ux.batch(di_last).bottom_rows(nx);
    auto λI            = λ.batch(diI);
    trsm(tril(R̂ŜQ̂.right_cols(nx).bottom_rows(nx)).transposed(), x_last);
    x_lanes ? compact_blas::template xadd_copy<-1>(simdify(λI), simdify(x_last), simdify(λI))
            : compact_blas::xadd_copy(simdify(λI), simdify(x_last), simdify(λI));
    compact_blas::xneg(simdify(λI)); // TODO: merge
    if (is_active(0, biI)) {
        trsm(tril(coupling_D.batch(biI)), λI);
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::solve_riccati_forward_alt(Context &ctx, mut_view<> ux,
                                                                   mut_view<> λ,
                                                                   mut_view<> work) const {
    const index_t ti         = ctx.index;
    const index_t num_stages = ceil_N >> lP;    // number of stages per thread
    const index_t di0        = ti * num_stages; // data batch index
    const index_t biI        = sub_wrap_PmV(ti, 1);
    const index_t diI        = biI * num_stages;
    const index_t di_last    = di0 + num_stages - 1;
    const index_t k0         = ti * num_stages; // stage index
    const index_t nux        = nu + nx;
    auto R̂ŜQ̂                 = riccati_R̂ŜQ̂.batch(ti);
    auto B̂                   = riccati_ÂB̂.batch(ti).right_cols(num_stages * nu);
    auto Â                   = riccati_ÂB̂.batch(ti).left_cols(num_stages * nx);
    auto w                   = work.batch(ti);
    for (index_t i = 0; i < num_stages; ++i) {
        index_t k  = sub_wrap_N(k0, i);
        index_t di = di0 + i;
        auto R̂ŜQ̂i  = R̂ŜQ̂.middle_cols(i * nux, nux);
        auto R̂i    = R̂ŜQ̂i.top_left(nu, nu);
        auto Ŝi    = R̂ŜQ̂i.bottom_left(nx, nu);
        auto Q̂i    = R̂ŜQ̂i.bottom_right(nx, nx);
        auto B̂i    = B̂.middle_cols(i * nu, nu);
        auto Âi    = Â.middle_cols(i * nx, nx);
        {
            GUANAQO_TRACE("Riccati solve ux", k);
            // l = LR⁻¹ r
            trsm(tril(R̂i), ux.batch(di).top_rows(nu));
            // p = q - LS l
            gemm_sub(Ŝi, ux.batch(di).top_rows(nu), ux.batch(di).bottom_rows(nx));
            // λ0 -= LB̂ l
            gemm_sub(B̂i, ux.batch(di).top_rows(nu), λ.batch(di0));
        }
        if (i + 1 < num_stages) {
            [[maybe_unused]] const auto k_next = sub_wrap_N(k, 1);
            const auto di_next                 = di + 1;
            GUANAQO_TRACE("Riccati solve b", k_next);
            // λ0 += Â b
            gemm_add(Âi, λ.batch(di_next), λ.batch(di0));
            // b' = LQᵀb
            copy(λ.batch(di_next), w);
            trmm(tril(Q̂i).transposed(), w);
            // d' = LQ b'
            trmm(tril(Q̂i), w);
            // d = LQ LQᵀ b + p
            compact_blas::xadd_copy(simdify(w), simdify(w), simdify(ux.batch(di).bottom_rows(nx)));
            // l += Bᵀd, q += Aᵀd
            gemm_add(data_BA.batch(di_next).transposed(), w, ux.batch(di_next));
        } else {
            GUANAQO_TRACE("Riccati solve last", k);
            // q = LQ⁻¹ p
            trsm(tril(Q̂i), ux.batch(di).bottom_rows(nx));
            // λ0 -= LÂ q
            gemm_sub(Âi, ux.batch(di).bottom_rows(nx), λ.batch(di0));
        }
    }
    ctx.arrive_and_wait();
    GUANAQO_TRACE("Riccati coupling I", k0);
    // b = LQ⁻ᵀ x + b
    const bool x_lanes = ti == 0; // first stage wraps around
    auto x_last        = ux.batch(di_last).bottom_rows(nx);
    auto λI            = λ.batch(diI);
    trsm(tril(R̂ŜQ̂.right_cols(nx).bottom_rows(nx)).transposed(), x_last);
    // Note: this leaves LQ⁻ᵀ q in x_last,
    //       which is reused during the backward solve
    x_lanes ? compact_blas::template xadd_copy<-1>(simdify(λI), simdify(x_last), simdify(λI))
            : compact_blas::xadd_copy(simdify(λI), simdify(x_last), simdify(λI));
    compact_blas::xneg(simdify(λI)); // TODO: merge
    if (is_active(0, biI)) {
        trsm(tril(coupling_D.batch(biI)), λI);
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::solve_forward(Context &ctx, mut_view<> ux, mut_view<> λ,
                                                       mut_batch_view<> work_pcg,
                                                       mut_view<> work) const {
    const index_t ti = ctx.index;
    alt ? solve_riccati_forward_alt(ctx, ux, λ, work) : solve_riccati_forward(ctx, ux, λ);
    for (index_t l = 0; l < lP - lvl; ++l) {
        ctx.arrive_and_wait();
        const index_t offset = 1 << l;
        const auto biY       = sub_wrap_PmV(ti, offset);
        const auto biU       = ti;
        if (is_active(l, biY)) {
            solve_active(l, biY, λ);
        } else if (is_active(l, biU)) {
            solve_active_secondary(l, biU, λ);
        }
    }
    if (lP - lvl == 0 || ti == 1 << (lP - lvl - 1))
        solve_pcg(λ.batch(0), work_pcg);
    ctx.arrive_and_wait();
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::solve_reverse_active(index_t l, index_t bi,
                                                              mut_view<> λ) const {
    const index_t offset     = 1 << l;
    const index_t num_stages = ceil_N >> lP;
    const index_t biY        = add_wrap_PmV(bi, offset);
    const index_t biU        = sub_wrap_PmV(bi, offset);
    const index_t di         = bi * num_stages;
    const index_t diY        = biY * num_stages;
    const index_t diU        = biU * num_stages;
    const bool x_lanes       = diY == 0;
    GUANAQO_TRACE("Solve coupling reverse", bi);
    x_lanes ? gemm_sub(coupling_Y.batch(bi).transposed(), λ.batch(diY), λ.batch(di), {},
                       with_shift_B<1>)
            : gemm_sub(coupling_Y.batch(bi).transposed(), λ.batch(diY), λ.batch(di));
    gemm_sub(coupling_U.batch(bi).transposed(), λ.batch(diU), λ.batch(di));
    trsm(tril(coupling_D.batch(bi)).transposed(), λ.batch(di));
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::solve_riccati_reverse(Context &ctx, mut_view<> ux,
                                                               mut_view<> λ,
                                                               mut_view<> work) const {
    const index_t ti         = ctx.index;
    const index_t num_stages = ceil_N >> lP;    // number of stages per thread
    const index_t di0        = ti * num_stages; // data batch index
    const index_t biI        = sub_wrap_PmV(ti, 1);
    const index_t diI        = biI * num_stages;
    const index_t k0         = ti * num_stages; // stage index
    const index_t nux        = nu + nx;
    auto R̂ŜQ̂                 = riccati_R̂ŜQ̂.batch(ti);
    auto B̂                   = riccati_ÂB̂.batch(ti).right_cols(num_stages * nu);
    auto Â                   = riccati_ÂB̂.batch(ti).left_cols(num_stages * nx);
    auto BAᵀ                 = riccati_BAᵀ.batch(ti);

    for (index_t i = num_stages; i-- > 0;) {
        index_t k  = sub_wrap_N(k0, i);
        index_t di = di0 + i;
        auto R̂ŜQ̂i  = R̂ŜQ̂.middle_cols(i * nux, nux);
        auto Q̂i    = R̂ŜQ̂i.bottom_right(nx, nx);
        auto R̂i    = R̂ŜQ̂i.top_left(nu, nu);
        auto Ŝi    = R̂ŜQ̂i.bottom_left(nx, nu);
        auto B̂i    = B̂.middle_cols(i * nu, nu);
        auto Âi    = Â.middle_cols(i * nx, nx);
        if (i + 1 < num_stages) {
            [[maybe_unused]] const auto k_next = sub_wrap_N(k, 1);
            const auto di_next                 = di + 1;
            GUANAQO_TRACE("Riccati solve b", k_next);
            auto BAᵀi = BAᵀ.middle_cols(i * nx, nx);
            // b -= LBᵀ u + LAᵀ x
            gemm_sub(BAᵀi.transposed(), ux.batch(di_next), λ.batch(di_next));
            compact_blas::xneg(simdify(λ.batch(di_next))); // TODO
            // q -= b
            compact_blas::xadd_copy(simdify(ux.batch(di).bottom_rows(nx)),
                                    simdify(ux.batch(di).bottom_rows(nx)),
                                    simdify(λ.batch(di_next)));
            trsm(tril(Q̂i), λ.batch(di_next));
            gemm_add(Âi.transposed(), λ.batch(di0), λ.batch(di_next));
        } else {
            // x_last = LQ⁻ᵀ(q_last + LQ⁻¹ λ - LÂᵀ λ)
            GUANAQO_TRACE("Riccati last", k);
            // λ0 -= Â λ
            const auto x_last  = ux.batch(di).bottom_rows(nx);
            const bool x_lanes = ti == 0;
            const auto w       = work.batch(ti);
            x_lanes ? compact_blas::template xadd_copy<1>(simdify(w), simdify(λ.batch(diI)))
                          : compact_blas::template xadd_copy<0>(simdify(w), simdify(λ.batch(diI)));
            // LQ⁻¹ λ
            trsm(tril(Q̂i), w);
            // LQ⁻¹ λ - LÂᵀ λ
            gemm_sub(Âi.transposed(), λ.batch(di0), w);
            // x_last = LQ⁻ᵀ(LQ⁻¹ λ - LÂᵀ λ)
            trsm(tril(Q̂i).transposed(), w);
            compact_blas::xadd_copy(simdify(x_last), simdify(x_last), simdify(w));
        }
        if (i + 1 < num_stages) {
            GUANAQO_TRACE("Riccati solve QRS", k);
            // l -= LB̂ᵀ λ0
            gemm_sub(B̂i.transposed(), λ.batch(di0), ux.batch(di).top_rows(nu));
            // x = LQ⁻ᵀ q, u = LR⁻ᵀ (l - LSᵀ x)
            trsm(tril(R̂ŜQ̂i).transposed(), ux.batch(di));
        } else {
            GUANAQO_TRACE("Riccati solve QRS", k);
            // l -= LB̂ᵀ λ0 + LSᵀ q
            gemm_sub(B̂i.transposed(), λ.batch(di0), ux.batch(di).top_rows(nu));
            gemm_sub(Ŝi.transposed(), ux.batch(di).bottom_rows(nx), ux.batch(di).top_rows(nu));
            // x = LQ⁻ᵀ q, u = LR⁻ᵀ (l - LSᵀ x)
            trsm(tril(R̂i).transposed(), ux.batch(di).top_rows(nu));
        }
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::solve_riccati_reverse_alt(Context &ctx, mut_view<> ux,
                                                                   mut_view<> λ,
                                                                   mut_view<> work) const {
    const index_t ti         = ctx.index;
    const index_t num_stages = ceil_N >> lP;    // number of stages per thread
    const index_t di0        = ti * num_stages; // data batch index
    const index_t biI        = sub_wrap_PmV(ti, 1);
    const index_t diI        = biI * num_stages;
    const index_t k0         = ti * num_stages; // stage index
    const index_t nux        = nu + nx;
    auto R̂ŜQ̂                 = riccati_R̂ŜQ̂.batch(ti);
    auto B̂                   = riccati_ÂB̂.batch(ti).right_cols(num_stages * nu);
    auto Â                   = riccati_ÂB̂.batch(ti).left_cols(num_stages * nx);
    const auto w             = work.batch(ti);

    for (index_t i = num_stages; i-- > 0;) {
        index_t k  = sub_wrap_N(k0, i);
        index_t di = di0 + i;
        auto R̂ŜQ̂i  = R̂ŜQ̂.middle_cols(i * nux, nux);
        auto Q̂i    = R̂ŜQ̂i.bottom_right(nx, nx);
        auto R̂i    = R̂ŜQ̂i.top_left(nu, nu);
        auto Ŝi    = R̂ŜQ̂i.bottom_left(nx, nu);
        auto B̂i    = B̂.middle_cols(i * nu, nu);
        auto Âi    = Â.middle_cols(i * nx, nx);
        if (i + 1 < num_stages) {
            [[maybe_unused]] const auto k_next = sub_wrap_N(k, 1);
            const auto di_next                 = di + 1;
            GUANAQO_TRACE("Riccati solve rev", k_next);
            auto BAi = data_BA.batch(di_next);

            // w = p
            copy(ux.batch(di).bottom_rows(nx), w);
            // x = A x(next) + B u(next) - b(next)
            compact_blas::xadd_neg_copy(simdify(ux.batch(di).bottom_rows(nx)),
                                        simdify(λ.batch(di_next)));
            gemm_add(BAi, ux.batch(di_next), ux.batch(di).bottom_rows(nx));
            // u = LR⁻ᵀ(l - LSᵀ x - LB̂ᵀ λ(last))
            gemm_sub(B̂i.transposed(), λ.batch(di0), ux.batch(di).top_rows(nu));
            gemm_sub(Ŝi.transposed(), ux.batch(di).bottom_rows(nx), ux.batch(di).top_rows(nu));
            trsm(tril(R̂i).transposed(), ux.batch(di).top_rows(nu));

            // λ(next) = LQ LQᵀ x + Âᵀ λ(last) - p
            copy(ux.batch(di).bottom_rows(nx), λ.batch(di_next));
            trmm(tril(Q̂i).transposed(), λ.batch(di_next));
            trmm(tril(Q̂i), λ.batch(di_next));
            gemm_add(Âi.transposed(), λ.batch(di0), λ.batch(di_next));
            compact_blas::xsub_copy(simdify(λ.batch(di_next)), simdify(λ.batch(di_next)),
                                    simdify(w));
        } else {
            // x_last = LQ⁻ᵀ(q_last + LQ⁻¹ λ - LÂᵀ λ)
            GUANAQO_TRACE("Riccati solve rev", k);
            // λ0 -= Â λ
            const auto x_last  = ux.batch(di).bottom_rows(nx);
            const bool x_lanes = ti == 0;
            x_lanes ? compact_blas::template xadd_copy<1>(simdify(w), simdify(λ.batch(diI)))
                    : compact_blas::template xadd_copy<0>(simdify(w), simdify(λ.batch(diI)));
            // LQ⁻¹ λ
            trsm(tril(Q̂i), w);
            // LQ⁻¹ λ - LÂᵀ λ
            gemm_sub(Âi.transposed(), λ.batch(di0), w);
            // w = LQ⁻ᵀ(LQ⁻¹ λ - LÂᵀ λ)
            trsm(tril(Q̂i).transposed(), w);
            // x_last = LQ⁻ᵀ(q_last + LQ⁻¹ λ - LÂᵀ λ)
            compact_blas::xadd_copy(simdify(x_last), simdify(x_last), simdify(w));

            // u -= LB̂ᵀ λ0 + LSᵀ q
            gemm_sub(B̂i.transposed(), λ.batch(di0), ux.batch(di).top_rows(nu));
            gemm_sub(Ŝi.transposed(), ux.batch(di).bottom_rows(nx), ux.batch(di).top_rows(nu));
            // u = LR⁻ᵀ u
            trsm(tril(R̂i).transposed(), ux.batch(di).top_rows(nu));
        }
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::solve_reverse(Context &ctx, mut_view<> ux, mut_view<> λ,
                                                       mut_view<> work) const {
    const index_t ti = ctx.index;
    for (index_t l = lP - lvl; l-- > 0;) {
        const index_t offset = 1 << l;
        const auto bi        = sub_wrap_PmV(ti, offset);
        if (is_active(l, bi))
            solve_reverse_active(l, bi, λ);
        ctx.arrive_and_wait();
    }
    alt ? solve_riccati_reverse_alt(ctx, ux, λ, work) : solve_riccati_reverse(ctx, ux, λ, work);
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::solve(Context &ctx, mut_view<> ux, mut_view<> λ,
                                               mut_batch_view<> work_pcg,
                                               mut_view<> work_riccati) const {
    solve_forward(ctx, ux, λ, work_pcg, work_riccati);
    solve_reverse(ctx, ux, λ, work_riccati); // TODO: check thread access pattern forward/reverse
}

} // namespace CYQLONE_NAMESPACE
