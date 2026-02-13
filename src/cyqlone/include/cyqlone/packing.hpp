#pragma once

#include <cyqlone/config.hpp>
#include <batmat/linalg/simdify.hpp>
#include <batmat/linalg/structure.hpp>
#include <batmat/linalg/uview.hpp>
#include <batmat/loop.hpp>
#include <batmat/lut.hpp>
#include <batmat/ops/transpose.hpp>
#include <guanaqo/mat-view.hpp>
#include <guanaqo/trace.hpp>
#include <type_traits>

namespace cyqlone::linalg {

using namespace batmat::linalg;

/// @cond DETAIL

namespace detail {

using batmat::index_constant;

#if 0
template <class T>
using scalar_simd_size = batmat::datapar::simd_size<T, batmat::datapar::deduced_abi<T, 1>>;
template <class T, StorageOrder O>
using scalar_view =
    batmat::matrix::View<T, index_t, scalar_simd_size<std::remove_cv_t<T>>, index_t, index_t, O>;
#else
template <class T, class D, class L, StorageOrder O>
using scalar_view = batmat::matrix::View<T, index_t, index_constant<1>, D, L, O>;
#endif

template <class TA, class Abi, MatrixStructure Struc = MatrixStructure::General, StorageOrder OA,
          class TB, class DB, class LB, StorageOrder OB>
    requires(OA == StorageOrder::ColMajor && OB == StorageOrder::ColMajor)
inline void unpack_full(view<TA, Abi, OA> A, scalar_view<TB, DB, LB, OB> B) {
    static_assert(std::is_const_v<TA> ^ std::is_const_v<TB>);
    static_assert(typename decltype(B)::batch_size_type() == 1);
    static constexpr bool Pack = std::is_const_v<TB>;
    BATMAT_ASSERT(B.rows() == A.rows());
    BATMAT_ASSERT(B.cols() == A.cols());
    BATMAT_ASSERT(static_cast<index_t>(B.depth()) == static_cast<index_t>(A.depth()));
    GUANAQO_TRACE("unpack", 0, A.rows() * A.cols() * A.depth());
    using enum MatrixStructure;
    using T                    = std::remove_const_t<TA>;
    static constexpr index_t v = typename decltype(A)::batch_size_type();
    static constexpr auto lut  = batmat::make_1d_lut<v>([]<index_t R>(index_constant<R>) {
        return Pack ? batmat::ops::transpose<R + 1, v, T> : batmat::ops::transpose<v, R + 1, T>;
    });
    const auto ldA             = A.outer_stride() * v;
    const auto ldB             = B.outer_stride();
    const auto batch_stride_B  = B.layer_stride();
    TA *pA                     = A.data;
    const auto pAend           = pA + A.outer_size() * ldA;
    TB *pB                     = B.data;
    auto inner_count           = Struc == LowerTriangular ? std::max(A.inner_size(), A.outer_size())
                                 : Struc == UpperTriangular ? index_t{1}
                                                            : A.inner_size();
    using std::clamp;
    while (pA < pAend) {
        TA *pA_ = pA;
        TB *pB_ = pB;
        batmat::foreach_chunked(
            0, clamp(inner_count, index_t{0}, A.inner_size()), v,
            [&](index_t) {
                if constexpr (Pack)
                    batmat::ops::transpose<v, v>(pB_, batch_stride_B, pA_, v);
                else
                    batmat::ops::transpose<v, v>(pA_, v, pB_, batch_stride_B);
                pA_ += v * v;
                pB_ += v;
            },
            [&](index_t, index_t nr) {
                if constexpr (Pack)
                    lut[nr - 1](pB_, batch_stride_B, pA_, v);
                else
                    lut[nr - 1](pA_, v, pB_, batch_stride_B);
            });
        pA += ldA;
        pB += ldB;
        if constexpr (Struc == LowerTriangular) {
            --inner_count;
            if (inner_count < A.inner_size()) {
                pA += v;
                pB += 1;
            }
        } else if (Struc == UpperTriangular) {
            ++inner_count;
        }
    }
}

template <class TA, class Abi, MatrixStructure Struc = MatrixStructure::General, StorageOrder OA,
          class TB, class DB, class LB, StorageOrder OB>
    requires(OA == StorageOrder::RowMajor && OB == StorageOrder::RowMajor)
inline void unpack_full(view<const TA, Abi, OA> A, scalar_view<TB, DB, LB, OB> B) {
    return unpack_full<TA, Abi, transpose(Struc)>(A.transposed(), B.transposed());
}

// TODO: mismatched storage order is currently not supported.

/// @todo Test this function. Then benchmark it against unpack_full: if this variant is not much
///       slower, we should consider using it for all unpacking so we don't need two variants.
template <class T, class Abi, StorageOrder OA, class DB, class LB, StorageOrder OB>
    requires(OA == OB)
inline void unpack_partial(view<const T, Abi, OA> A, scalar_view<T, DB, LB, OB> B) {
    if (B.depth() >= A.depth())
        return unpack_full(A, B.first_layers(A.depth()));
    static_assert(OA == StorageOrder::ColMajor); // TODO: row major
    GUANAQO_TRACE("unpack", 0, A.rows() * A.cols() * A.depth());
    static constexpr index_t v = A.batch_size();
    static constexpr auto lut  = batmat::make_1d_lut<v>(
        []<index_t R>(index_constant<R>) { return batmat::ops::transpose_dyn<v, R + 1, T>; });
    for (index_t c = 0; c < A.cols(); ++c)
        batmat::foreach_chunked(
            0, A.rows(), v,
            [&](index_t r) {
                batmat::ops::transpose_dyn<v, v>(&A(0, r, c), v, &B(0, r, c), B.layer_stride(),
                                                 B.depth());
            },
            [&](index_t r, index_t nr) {
                lut[nr - 1](A.block(r, c, nr, 1).data, v, B.block(r, c, nr, 1).data,
                            B.layer_stride(), B.depth());
            });
}

} // namespace detail

/// @endcond

/// @name Packing and unpacking
/// @{

/// Copy a compact batch of matrices @p A to multiple scalar matrices @p B.
/// @post A(l, r, c) == B(l, r, c) for all valid l, r, c.
template <simdifiable VA, class VB>
    requires(std::is_same_v<simdified_value_t<VA>, typename std::remove_cvref_t<VB>::value_type> &&
             typename std::remove_cvref_t<VB>::batch_size_type() == 1)
void unpack(VA &&A, VB &&B) {
    detail::unpack_full<const simdified_value_t<VA>, simdified_abi_t<VA>>(
        simdify(A).as_const(), B.view().first_layers(A.depth()));
    // TODO: make sure that .view() is supported for all relevant VB.
}

/// Copy multiple scalar matrices @p A to a compact batch of matrices @p B.
/// @post A(l, r, c) == B(l, r, c) for all valid l, r, c.
template <class VA, simdifiable VB>
    requires(std::is_same_v<typename std::remove_cvref_t<VA>::value_type, simdified_value_t<VB>> &&
             typename std::remove_cvref_t<VA>::batch_size_type() == 1)
void pack(VA &&A, VB &&B) {
    detail::unpack_full<simdified_value_t<VB>, simdified_abi_t<VB>>(
        simdify(B), A.view().first_layers(B.depth()).as_const());
    // TODO: make sure that .view() is supported for all relevant VA.
}

/// @}

} // namespace cyqlone::linalg
