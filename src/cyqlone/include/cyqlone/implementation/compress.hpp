#pragma once

#include <cyqlone/config.hpp>
#include <batmat/assume.hpp>
#include <batmat/linalg/simdify.hpp>
#include <batmat/linalg/uview.hpp>
#include <batmat/loop.hpp>
#include <batmat/ops/gather.hpp>
#include <guanaqo/trace.hpp>

namespace cyqlone::linalg {

namespace detail {

using batmat::linalg::simd_view_types;
using batmat::linalg::StorageOrder;
using batmat::linalg::view;

template <class T, class Abi, index_t N = 8, StorageOrder OAi, StorageOrder OAo>
index_t compress_masks_sqrt(view<const T, Abi, OAi> A_in, view<const T, Abi> S_in,
                            view<T, Abi, OAo> A_out, view<T, Abi> S_sign_out = {}) {
    GUANAQO_TRACE("compress_masks_sqrt", 0, (A_in.rows() + 1) * A_in.cols() * A_in.depth());
    using batmat::ops::gather;
    using std::abs;
    using std::copysign;
    using std::sqrt;
    BATMAT_ASSERT(A_in.rows() == A_out.rows());
    BATMAT_ASSERT(A_in.cols() == A_out.cols());
    BATMAT_ASSERT(A_in.depth() == A_out.depth());
    BATMAT_ASSERT(A_in.depth() == S_in.depth());
    BATMAT_ASSERT(A_in.cols() == S_in.rows());
    BATMAT_ASSERT(S_sign_out.rows() == 0 || A_in.depth() == S_sign_out.depth());
    BATMAT_ASSERT(S_sign_out.rows() == 0 || A_out.cols() == S_sign_out.rows());
    const auto C = A_in.cols();
    const auto R = A_in.rows();
    if (C == 0)
        return 0;
    BATMAT_ASSUME(R > 0);
    using types              = simd_view_types<T, Abi>;
    using simd               = typename types::simd;
    using isimd              = typename types::isimd;
    static constexpr auto VL = simd::size();

    isimd hist[N]{};
    index_t j = 0;
    static const isimd iota{[](auto i) { return i; }};

    const auto commit_no_shift = [&](auto h_commit) {
        h_commit -= 1;
        auto gather_S = [&] {
            const auto bs       = static_cast<index_t>(S_in.batch_size());
            const isimd offsets = h_commit * bs + iota;
            auto gather_S       = gather<T, VL>(&S_in(0, 0, 0), offsets);
            if (S_sign_out.rows() > 0)
                types::aligned_store(copysign(simd{0}, gather_S), &S_sign_out(0, j, 0));
            return gather_S;
        }();
        for (index_t r = 0; r < R; ++r) {
            const auto bs       = static_cast<index_t>(A_in.batch_size());
            const auto stride   = (OAi == StorageOrder::ColMajor ? bs * A_in.outer_stride() : bs);
            const isimd offsets = h_commit * stride + iota;
            auto gather_A       = gather<T, VL>(&A_in(0, r, 0), offsets);
            types::aligned_store(sqrt(abs(gather_S)) * gather_A, &A_out(0, r, j));
        }
        ++j;
    };
    const auto commit = [&] [[gnu::always_inline]] () {
        const isimd h = hist[0];
        BATMAT_FULLY_UNROLLED_FOR (index_t k = 1; k < N; ++k)
            hist[k - 1] = hist[k];
        hist[N - 1] = 0;
        commit_no_shift(h);
    };

    isimd c1_simd{0};
    for (index_t c = 0; c < C; ++c) {
        c1_simd += 1; // current column index + 1
        const simd Sc = types::aligned_load(&S_in(0, c, 0));
        auto Sc_msk   = Sc != 0;
        BATMAT_FULLY_UNROLLED_FOR (auto &h : hist) {
#if BATMAT_WITH_GSI_HPC_SIMD
            const auto msk = (h == 0) && Sc_msk;
            h              = isimd{[&](int i) { return msk[i] ? c1_simd[i] : h[i]; }}; // TODO
            Sc_msk         = Sc_msk && (!msk);
#else
            const auto msk = (h == 0) && Sc_msk.__cvt();
            where(msk, h)  = c1_simd;
            Sc_msk         = Sc_msk && (!msk).__cvt();
#endif
        }
        // Masks of all ones can already be written to memory
        while (none_of(hist[0] == 0) || (c + 1 == C && any_of(hist[0] != 0)))
            commit();
        // If there are still bits set in the mask.
        if (any_of(Sc_msk)) {
            // Check if there's an empty slot (always at the end)
            if (any_of(hist[N - 1] != 0))
                // If not, commit the first slot to make room.
                commit();
            // Find the first empty slot, and add the remaining bits.
            BATMAT_FULLY_UNROLLED_FOR (auto &h : hist)
                if (all_of(h == 0))
#if BATMAT_WITH_GSI_HPC_SIMD
                    h = isimd{[&](int i) { return Sc_msk[i] ? c1_simd[i] : h[i]; }}; // TODO
#else
                    where(Sc_msk.__cvt(), h) = c1_simd;
#endif
        }
        // Invariant: first registers in the buffer contain fewest zeros
        BATMAT_FULLY_UNROLLED_FOR (index_t i = 1; i < N; ++i)
#if BATMAT_WITH_GSI_HPC_SIMD
            assert(reduce_count(hist[i] != 0) <= reduce_count(hist[i - 1] != 0));
#else
            assert(popcount(hist[i] != 0) <= popcount(hist[i - 1] != 0));
#endif
    }
    if (any_of(hist[0] != 0))
        commit_no_shift(hist[0]);
    return j;
}

} // namespace detail

using batmat::linalg::simdifiable;

template <index_t N = 8, simdifiable VA, simdifiable VS, simdifiable VAo>
index_t compress_masks_sqrt(VA &&Ain, VS &&Sin, VAo &&Aout) {
    using batmat::linalg::simdified_abi_t;
    using batmat::linalg::simdified_value_t;
    using batmat::linalg::simdify;
    return detail::compress_masks_sqrt<simdified_value_t<VA>, simdified_abi_t<VA>, N>(
        simdify(Ain).as_const(), simdify(Sin).as_const(), simdify(Aout));
}

template <index_t N = 8, simdifiable VA, simdifiable VS, simdifiable VAo, simdifiable VSo>
index_t compress_masks_sqrt(VA &&Ain, VS &&Sin, VAo &&Aout, VSo &&Sout) {
    using batmat::linalg::simdified_abi_t;
    using batmat::linalg::simdified_value_t;
    using batmat::linalg::simdify;
    return detail::compress_masks_sqrt<simdified_value_t<VA>, simdified_abi_t<VA>, N>(
        simdify(Ain).as_const(), simdify(Sin).as_const(), simdify(Aout), simdify(Sout));
}

} // namespace cyqlone::linalg
