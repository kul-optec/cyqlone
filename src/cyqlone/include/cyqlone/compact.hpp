#pragma once

#include <cyqlone/config.hpp>
#include <guanaqo/mat-view.hpp>
#include <cassert>
#include <concepts>

#include <batmat/config.hpp>
#include <batmat/linalg/uview.hpp>
#include <batmat/matrix/view.hpp>

namespace CYQLONE_NAMESPACE::compact {

using batmat::linalg::StorageOrder;

template <class T, class Abi, StorageOrder O>
struct CompactBLAS {
    using value_type                   = T;
    using simd_types                   = batmat::linalg::simd_view_types<value_type, Abi>;
    using simd                         = typename simd_types::simd;
    using single_batch_view            = typename simd_types::template view<const value_type, O>;
    using mut_single_batch_view        = typename simd_types::template view<value_type, O>;
    using mut_single_batch_view_scalar = typename simd_types::template scalar_view<value_type, O>;
    using mut_batch_view_scalar = typename simd_types::template multi_scalar_view<value_type, O>;
    using batch_view            = typename simd_types::template multi_view<const value_type, O>;
    using mut_batch_view        = typename simd_types::template multi_view<value_type, O>;
    static constexpr index_t simd_stride = typename simd_types::simd_stride_t();

    static void unpack(single_batch_view A, mut_single_batch_view_scalar B);
    static void unpack(single_batch_view A, mut_batch_view_scalar B);
    static void unpack(batch_view A, mut_batch_view_scalar B);

    static void unpack_L(single_batch_view A, mut_single_batch_view_scalar B);
    static void unpack_L(single_batch_view A, mut_batch_view_scalar B);
    static void unpack_L(batch_view A, mut_batch_view_scalar B);

    /// Cholesky downdate
    static void xshh(mut_single_batch_view L, mut_single_batch_view A);
    static void xshh(mut_batch_view L, mut_batch_view A);
    static void xshh_ref(mut_single_batch_view L, mut_single_batch_view A);

    /// C ← A ⊙ B
    static void xhadamard(single_batch_view A, single_batch_view B, mut_single_batch_view C);
    /// B ← A ⊙ B
    static void xhadamard(single_batch_view A, mut_single_batch_view B);
    static void xhadamard(batch_view A, mut_batch_view B);

    /// A ← -A
    static void xneg(mut_single_batch_view A);
    static void xneg(mut_batch_view A);

    /// y += a x
    static void xaxpy(value_type a, single_batch_view x, mut_single_batch_view y);
    static void xaxpy(value_type a, batch_view x, mut_batch_view y);

    /// y ← a x + b y
    static void xaxpby(value_type a, single_batch_view x, value_type b, mut_single_batch_view y);
    static void xaxpby(value_type a, batch_view x, value_type b, mut_batch_view y);

    /// L += A
    static void xadd_L(single_batch_view A, mut_single_batch_view B);

    /// Sum
    template <int Rot = 0, class OutView, class View, class... Views>
    static void xadd_copy_impl(OutView out, View x1, Views... xs)
        requires(((std::same_as<OutView, mut_batch_view> && std::same_as<View, batch_view>) &&
                  ... && std::same_as<Views, batch_view>) ||
                 ((std::same_as<OutView, mut_single_batch_view> &&
                   std::same_as<View, single_batch_view>) &&
                  ... && std::same_as<Views, single_batch_view>));
    template <int Rot = 0, class... Views>
    static void xadd_copy(mut_batch_view out, Views... xs) {
        xadd_copy_impl<Rot>(out, batch_view{xs}...);
    }
    template <int Rot = 0, class... Views>
    static void xadd_copy(mut_single_batch_view out, Views... xs) {
        xadd_copy_impl<Rot>(out, single_batch_view{xs}...);
    }
    template <class OutView, class View, class... Views>
    static void xsub_copy_impl(OutView out, View x1, Views... xs)
        requires(((std::same_as<OutView, mut_batch_view> && std::same_as<View, batch_view>) &&
                  ... && std::same_as<Views, batch_view>) ||
                 ((std::same_as<OutView, mut_single_batch_view> &&
                   std::same_as<View, single_batch_view>) &&
                  ... && std::same_as<Views, single_batch_view>));
    template <class... Views>
    static void xsub_copy(mut_batch_view out, Views... xs) {
        xsub_copy_impl(out, batch_view{xs}...);
    }
    template <class... Views>
    static void xsub_copy(mut_single_batch_view out, Views... xs) {
        xsub_copy_impl(out, single_batch_view{xs}...);
    }
    template <int Rot = 0, class OutView, class View, class... Views>
    static void xadd_neg_copy_impl(OutView out, View x1, Views... xs)
        requires(((std::same_as<OutView, mut_batch_view> && std::same_as<View, batch_view>) &&
                  ... && std::same_as<Views, batch_view>) ||
                 ((std::same_as<OutView, mut_single_batch_view> &&
                   std::same_as<View, single_batch_view>) &&
                  ... && std::same_as<Views, single_batch_view>));
    template <int Rot = 0, class... Views>
    static void xadd_neg_copy(mut_batch_view out, Views... xs) {
        xadd_neg_copy_impl<Rot>(out, batch_view{xs}...);
    }
    template <int Rot = 0, class... Views>
    static void xadd_neg_copy(mut_single_batch_view out, Views... xs) {
        xadd_neg_copy_impl<Rot>(out, single_batch_view{xs}...);
    }
    template <int Rot = 0>
    static void xsub(mut_single_batch_view A, single_batch_view B);

    /// Dot product
    static value_type xdot(single_batch_view x, single_batch_view y);
    static value_type xdot(batch_view x, batch_view y);
    /// Square of the 2-norm
    static value_type xnrm2sq(single_batch_view x);
    static value_type xnrm2sq(batch_view x);
    /// Infinity/max norm
    static value_type xnrminf(single_batch_view x);
    static value_type xnrminf(batch_view x);

    template <class T0, class F, class R, class... Args>
    static auto xreduce(T0 init, F fun, R reduce, single_batch_view x0, const Args &...xs) {
        const index_t m = x0.rows(), n = x0.cols();
        assert(((x0.rows() == xs.rows()) && ...));
        assert(((x0.cols() == xs.cols()) && ...));
        assert(((x0.depth() == xs.depth()) && ...));
        assert(((x0.batch_size() == xs.batch_size()) && ...));
        for (index_t c = 0; c < n; ++c)
            for (index_t r = 0; r < m; ++r)
                init = fun(init, simd_types::aligned_load(&x0(0, r, c)),
                           simd_types::aligned_load(&xs(0, r, c))...);
        return reduce(init);
    }

    template <class T0, class F, class R, class... Args>
    static auto xreduce_enumerate(T0 init, F fun, R reduce, single_batch_view x0,
                                  const Args &...xs) {
        const index_t m = x0.rows(), n = x0.cols();
        assert(((x0.rows() == xs.rows()) && ...));
        assert(((x0.cols() == xs.cols()) && ...));
        assert(((x0.depth() == xs.depth()) && ...));
        assert(((x0.batch_size() == xs.batch_size()) && ...));
        const index_t i = 0;
        for (index_t c = 0; c < n; ++c)
            for (index_t r = 0; r < m; ++r)
                init = fun(std::make_tuple(i, r, c), init, simd_types::aligned_load(&x0(i, r, c)),
                           simd_types::aligned_load(&xs(i, r, c))...);
        return reduce(init);
    }

    template <class T0, class F, class R, class... Args>
    static auto xreduce(T0 init, F fun, R reduce, batch_view x0, const Args &...xs) {
        const auto Bs   = static_cast<index_t>(x0.batch_size());
        const index_t m = x0.rows(), n = x0.cols();
        assert(((x0.rows() == xs.rows()) && ...));
        assert(((x0.cols() == xs.cols()) && ...));
        assert(((x0.depth() == xs.depth()) && ...));
        assert(((x0.batch_size() == xs.batch_size()) && ...));
        const index_t N_batched = x0.depth();
        index_t i;
        for (i = 0; i + Bs <= N_batched; i += Bs)
            for (index_t c = 0; c < n; ++c)
                for (index_t r = 0; r < m; ++r)
                    init = fun(init, simd_types::aligned_load(&x0(i, r, c)),
                               simd_types::aligned_load(&xs(i, r, c))...);
        auto accum_scal = reduce(init);
        for (; i < x0.depth(); ++i)
            for (index_t c = 0; c < n; ++c)
                for (index_t r = 0; r < m; ++r)
                    accum_scal = fun(accum_scal, x0(i, r, c), (xs(i, r, c))...);
        return accum_scal;
    }

    template <class T0, class F, class R, class... Args>
    static auto xreduce_enumerate(T0 init, F fun, R reduce, batch_view x0, const Args &...xs) {
        const auto Bs   = static_cast<index_t>(x0.batch_size());
        const index_t m = x0.rows(), n = x0.cols();
        assert(((x0.rows() == xs.rows()) && ...));
        assert(((x0.cols() == xs.cols()) && ...));
        assert(((x0.depth() == xs.depth()) && ...));
        assert(((x0.batch_size() == xs.batch_size()) && ...));
        const index_t N_batched = x0.depth() - 1;
        index_t i;
        for (i = 0; i + Bs < N_batched; i += Bs)
            for (index_t c = 0; c < n; ++c)
                for (index_t r = 0; r < m; ++r)
                    init =
                        fun(std::make_tuple(i, r, c), init, simd_types::aligned_load(&x0(i, r, c)),
                            simd_types::aligned_load(&xs(i, r, c))...);
        auto accum_scal = reduce(init);
        for (; i < x0.depth(); ++i)
            for (index_t c = 0; c < n; ++c)
                for (index_t r = 0; r < m; ++r)
                    accum_scal =
                        fun(std::make_tuple(i, r, c), accum_scal, x0(i, r, c), (xs(i, r, c))...);
        return accum_scal;
    }

    /// y = x - clamp(x, l, u)
    static void proj_diff(single_batch_view x, single_batch_view l, single_batch_view u,
                          mut_single_batch_view y);
};

} // namespace CYQLONE_NAMESPACE::compact

#include "compact/elementwise.tpp"
#include "compact/pack.tpp"
