#pragma once

#include <cyqlone/config.hpp>
#include <cyqlone/reduce.hpp>
#include <batmat/assume.hpp>
#include <batmat/linalg/copy.hpp>
#include <batmat/linalg/shift.hpp>
#include <batmat/linalg/simdify.hpp>
#include <batmat/ops/rotate.hpp>
#include <batmat/simd.hpp>
#include <array>
#include <concepts>
#include <tuple>
#include <utility>

namespace cyqlone::linalg {

using namespace batmat::linalg;

/// @cond DETAIL

namespace detail {

template <class T, class Abi, StorageOrder O, class F, class X, class... Xs>
[[gnu::always_inline]] inline void iter_elems(F &&fun, X &&x, Xs &&...xs) {
    using types = simd_view_types<T, Abi>;
    if constexpr (O == StorageOrder::ColMajor) {
        for (index_t c = 0; c < x.cols(); ++c)
            for (index_t r = 0; r < x.rows(); ++r)
                fun(types::aligned_load(&x(0, r, c)), types::aligned_load(&xs(0, r, c))...);
    } else {
        for (index_t r = 0; r < x.rows(); ++r)
            for (index_t c = 0; c < x.cols(); ++c)
                fun(types::aligned_load(&x(0, r, c)), types::aligned_load(&xs(0, r, c))...);
    }
}

template <class T, class Abi, StorageOrder O, class F, class X, class... Xs>
[[gnu::always_inline]] inline void iter_elems_store(F &&fun, X &&x, Xs &&...xs) {
    using types = simd_view_types<T, Abi>;
    if constexpr (O == StorageOrder::ColMajor) {
        for (index_t c = 0; c < x.cols(); ++c)
            for (index_t r = 0; r < x.rows(); ++r)
                types::aligned_store(fun(types::aligned_load(&xs(0, r, c))...), &x(0, r, c));
    } else {
        for (index_t r = 0; r < x.rows(); ++r)
            for (index_t c = 0; c < x.cols(); ++c)
                types::aligned_store(fun(types::aligned_load(&xs(0, r, c))...), &x(0, r, c));
    }
}

template <class T, class Abi, StorageOrder O, class F, class X0, class X1, class... Xs>
[[gnu::always_inline]] inline void iter_elems_store2(F &&fun, X0 &&x0, X1 &&x1, Xs &&...xs) {
    using types = simd_view_types<T, Abi>;
    if constexpr (O == StorageOrder::ColMajor) {
        for (index_t c = 0; c < x0.cols(); ++c)
            for (index_t r = 0; r < x0.rows(); ++r) {
                auto [r0, r1] = fun(types::aligned_load(&xs(0, r, c))...);
                types::aligned_store(r0, &x0(0, r, c));
                types::aligned_store(r1, &x1(0, r, c));
            }
    } else {
        for (index_t r = 0; r < x0.rows(); ++r)
            for (index_t c = 0; c < x0.cols(); ++c) {
                auto [r0, r1] = fun(types::aligned_load(&xs(0, r, c))...);
                types::aligned_store(r0, &x0(0, r, c));
                types::aligned_store(r1, &x1(0, r, c));
            }
    }
}

template <class T, class Abi, StorageOrder O, class F, class... Ys, class... Xs>
[[gnu::always_inline]] inline void iter_elems_store_n(F &&fun, std::tuple<Ys...> ys, Xs &&...xs) {
    using std::get;
    using types        = simd_view_types<T, Abi>;
    const index_t rows = std::get<0>(ys).rows(), cols = std::get<0>(ys).cols();
    if constexpr (O == StorageOrder::ColMajor) {
        for (index_t c = 0; c < cols; ++c)
            for (index_t r = 0; r < rows; ++r) {
                auto rs = fun(types::aligned_load(&xs(0, r, c))...);
                static_assert(std::tuple_size_v<decltype(rs)> == sizeof...(Ys));
                [&]<size_t... Is>(std::index_sequence<Is...>) {
                    ((types::aligned_store(get<Is>(rs), &get<Is>(ys)(0, r, c))), ...);
                }(std::index_sequence_for<Ys...>());
            }
    } else {
        for (index_t r = 0; r < rows; ++r)
            for (index_t c = 0; c < cols; ++c) {
                auto rs = fun(types::aligned_load(&xs(0, r, c))...);
                static_assert(std::tuple_size_v<decltype(rs)> == sizeof...(Ys));
                [&]<size_t... Is>(std::index_sequence<Is...>) {
                    ((types::aligned_store(get<Is>(rs), &get<Is>(ys)(0, r, c))), ...);
                }(std::index_sequence_for<Ys...>());
            }
    }
}

template <class T, class Abi, StorageOrder O0, class Tinit, class F, class R, class... Args>
auto reduce(Tinit init, F fun, R reduce, view<const T, Abi, O0> x0, const Args &...xs) {
    BATMAT_ASSERT(((x0.rows() == xs.rows()) && ...));
    BATMAT_ASSERT(((x0.cols() == xs.cols()) && ...));
    BATMAT_ASSERT(((x0.depth() == xs.depth()) && ...));
    BATMAT_ASSERT(((x0.batch_size() == xs.batch_size()) && ...));
    iter_elems<T, Abi, O0>([&](auto... args) { init = fun(init, args...); }, x0, xs...);
    return reduce(init);
}

template <class T, class Abi, StorageOrder OA>
[[gnu::flatten]] cyqlone::norms<T>::result norms_all(view<const T, Abi, OA> A) {
    using simd  = batmat::datapar::simd<T, Abi>;
    using norms = cyqlone::norms<T, simd>;
    return reduce<T, Abi>(norms::zero_simd(), norms(), norms(), A);
}

/// Dot product.
template <class T, class Abi, StorageOrder OA, StorageOrder OB>
[[gnu::flatten]] T dot(view<const T, Abi, OA> a, view<const T, Abi, OB> b) {
    using simd       = batmat::datapar::simd<T, Abi>;
    auto fma         = [](auto accum, auto ai, auto bi) { return ai * bi + accum; };
    auto simd_reduce = [](auto accum) { return reduce(accum); };
    return reduce<T, Abi>(simd{0}, fma, simd_reduce, a, b);
}

/// Squared 2-norm.
template <class T, class Abi, StorageOrder OA>
[[gnu::flatten]] T norm_2_sq(view<const T, Abi, OA> a) {
    using simd       = batmat::datapar::simd<T, Abi>;
    auto fma         = [](auto accum, auto ai) { return ai * ai + accum; };
    auto simd_reduce = [](auto accum) { return reduce(accum); };
    return reduce<T, Abi>(simd{0}, fma, simd_reduce, a);
}

/// Hadamard (elementwise) product.
template <class T, class Abi, StorageOrder OA, StorageOrder OB, StorageOrder OC>
[[gnu::flatten]] void hadamard(view<const T, Abi, OA> A, view<const T, Abi, OB> B,
                               view<T, Abi, OC> C) {
    BATMAT_ASSERT(A.rows() == B.rows());
    BATMAT_ASSERT(A.cols() == B.cols());
    BATMAT_ASSERT(A.rows() == C.rows());
    BATMAT_ASSERT(A.cols() == C.cols());
    iter_elems_store<T, Abi, OC>([&](auto Ai, auto Bi) { return Ai * Bi; }, C, A, B);
}

/// Elementwise clamping z = max(lo, min(x, hi)).
template <class T, class Abi, StorageOrder O>
[[gnu::flatten]] void clamp(view<const T, Abi, O> x, view<const T, Abi, O> lo,
                            view<const T, Abi, O> hi, view<T, Abi, O> z) {
    BATMAT_ASSERT(x.rows() == lo.rows());
    BATMAT_ASSERT(x.cols() == lo.cols());
    BATMAT_ASSERT(x.rows() == hi.rows());
    BATMAT_ASSERT(x.cols() == hi.cols());
    BATMAT_ASSERT(x.rows() == z.rows());
    BATMAT_ASSERT(x.cols() == z.cols());
    const auto clamp = [&](auto xi, auto loi, auto hii) { return fmax(loi, fmin(xi, hii)); };
    iter_elems_store<T, Abi, O>(clamp, z, x, lo, hi);
}

/// Elementwise clamping residual z = x - max(lo, min(x, hi)).
template <class T, class Abi, StorageOrder O>
[[gnu::flatten]] void clamp_resid(view<const T, Abi, O> x, view<const T, Abi, O> lo,
                                  view<const T, Abi, O> hi, view<T, Abi, O> z) {
    BATMAT_ASSERT(x.rows() == lo.rows());
    BATMAT_ASSERT(x.cols() == lo.cols());
    BATMAT_ASSERT(x.rows() == hi.rows());
    BATMAT_ASSERT(x.cols() == hi.cols());
    BATMAT_ASSERT(x.rows() == z.rows());
    BATMAT_ASSERT(x.cols() == z.cols());
    using simd             = batmat::datapar::simd<T, Abi>;
    const auto clamp_resid = [&](auto xi, auto loi, auto hii) {
        return fmax(xi - hii, fmin(simd{0}, xi - loi));
    };
    iter_elems_store<T, Abi, O>(clamp_resid, z, x, lo, hi);
}

/// Linear combination of vectors z = beta * z + sum_i alpha_i * x_i.
template <class T, class Abi, T Beta, StorageOrder O, class... Xs>
[[gnu::flatten]] void gaxpby(view<T, Abi, O> z, const std::array<T, sizeof...(Xs)> &alphas,
                             const Xs &...xs) {
    BATMAT_ASSERT(((z.rows() == xs.rows()) && ...));
    BATMAT_ASSERT(((z.cols() == xs.cols()) && ...));
    if constexpr (Beta == 0)
        iter_elems_store<T, Abi, O>(
            [&](auto... xis) {
                return [&]<std::size_t... Is>(std::index_sequence<Is...>, auto... xis) {
                    return ((xis * alphas[Is]) + ...);
                }(std::make_index_sequence<sizeof...(Xs)>(), xis...);
            },
            z, xs...);
    else
        iter_elems_store<T, Abi, O>(
            [&](auto zi, auto... xis) {
                return [&]<std::size_t... Is>(std::index_sequence<Is...>, auto... xis) {
                    return zi * Beta + ((xis * alphas[Is]) + ...);
                }(std::make_index_sequence<sizeof...(Xs)>(), xis...);
            },
            z, z, xs...);
}

/// Negate a matrix or vector.
/// @todo: add Negate option to batmat::linalg::copy and remove this function, then this also
///        supports transposition.
template <class T, class Abi, int Rotate, StorageOrder O>
[[gnu::flatten]] void negate(view<const T, Abi, O> A, view<T, Abi, O> B) {
    BATMAT_ASSERT(A.rows() == B.rows());
    BATMAT_ASSERT(A.cols() == B.cols());
    using batmat::ops::rotl;
    iter_elems_store<T, Abi, O>([&](auto Ai) { return -rotl<Rotate>(Ai); }, B, A);
}

/// Subtract two matrices or vectors C = A - B.
template <class T, class Abi, int Rotate, StorageOrder O>
[[gnu::flatten]] void sub(view<const T, Abi, O> A, view<const T, Abi, O> B, view<T, Abi, O> C) {
    BATMAT_ASSERT(A.rows() == B.rows());
    BATMAT_ASSERT(A.cols() == B.cols());
    BATMAT_ASSERT(A.rows() == C.rows());
    BATMAT_ASSERT(A.cols() == C.cols());
    using batmat::ops::rotl;
    iter_elems_store<T, Abi, O>([&](auto Ai, auto Bi) { return Ai - rotl<Rotate>(Bi); }, C, A, B);
}

/// Add two matrices or vectors C = A + B.
template <class T, class Abi, int Rotate, StorageOrder O>
[[gnu::flatten]] void add(view<const T, Abi, O> A, view<const T, Abi, O> B, view<T, Abi, O> C) {
    BATMAT_ASSERT(A.rows() == B.rows());
    BATMAT_ASSERT(A.cols() == B.cols());
    BATMAT_ASSERT(A.rows() == C.rows());
    BATMAT_ASSERT(A.cols() == C.cols());
    using batmat::ops::rotl;
    iter_elems_store<T, Abi, O>([&](auto Ai, auto Bi) { return Ai + rotl<Rotate>(Bi); }, C, A, B);
}

} // namespace detail

/// @endcond

/// @name Single-batch operations
/// @{

/// Compute the norms (max, 1-norm, and 2-norm) of a vector.
template <simdifiable Vx>
norms<simdified_value_t<Vx>>::result norms_all(Vx &&x) {
    return detail::norms_all<simdified_value_t<Vx>, simdified_abi_t<Vx>>(simdify(x).as_const());
}

/// Compute the infinity norm of a vector.
template <simdifiable Vx>
simdified_value_t<Vx> norm_inf(Vx &&x) {
    return norms_all(std::forward<Vx>(x)).norminf();
}

/// Compute the 1-norm of a vector.
template <simdifiable Vx>
simdified_value_t<Vx> norm_1(Vx &&x) {
    return norms_all(std::forward<Vx>(x)).norm1();
}

/// Compute the squared 2-norm of a vector.
template <simdifiable Vx>
simdified_value_t<Vx> norm_2_squared(Vx &&x) {
    return detail::norm_2_sq<simdified_value_t<Vx>, simdified_abi_t<Vx>>(simdify(x).as_const());
}

/// Compute the 2-norm of a vector.
template <simdifiable Vx>
simdified_value_t<Vx> norm_2(Vx &&x) {
    using std::sqrt;
    return sqrt(norm_2_squared(std::forward<Vx>(x)));
}

/// Compute the dot product of two vectors.
template <simdifiable Vx, simdifiable Vy>
    requires simdify_compatible<Vx, Vy>
simdified_value_t<Vx> dot(Vx &&x, Vy &&y) {
    return detail::dot<simdified_value_t<Vx>, simdified_abi_t<Vx>>(simdify(x).as_const(),
                                                                   simdify(y).as_const());
}

/// Compute the Hadamard (elementwise) product of two vectors z = x ⊙ y.
template <simdifiable Vx, simdifiable Vy, simdifiable Vz>
    requires simdify_compatible<Vx, Vy, Vz>
void hadamard(Vx &&x, Vy &&y, Vz &&z) {
    detail::hadamard<simdified_value_t<Vx>, simdified_abi_t<Vx>>(simdify(x).as_const(),
                                                                 simdify(y).as_const(), simdify(z));
}

/// Compute the Hadamard (elementwise) product of two vectors x = x ⊙ y.
template <simdifiable Vx, simdifiable Vy>
    requires simdify_compatible<Vx, Vy>
void hadamard(Vx &&x, Vy &&y) {
    detail::hadamard<simdified_value_t<Vx>, simdified_abi_t<Vx>>(simdify(x).as_const(),
                                                                 simdify(y).as_const(), simdify(x));
}

/// Elementwise clamping z = max(lo, min(x, hi)).
template <simdifiable Vx, simdifiable Vlo, simdifiable Vhi, simdifiable Vz>
    requires simdify_compatible<Vx, Vlo, Vhi, Vz>
void clamp(Vx &&x, Vlo &&lo, Vhi &&hi, Vz &&z) {
    detail::clamp<simdified_value_t<Vx>, simdified_abi_t<Vx>>(
        simdify(x).as_const(), simdify(lo).as_const(), simdify(hi).as_const(), simdify(z));
}

/// Elementwise clamping residual z = x - max(lo, min(x, hi)).
template <simdifiable Vx, simdifiable Vlo, simdifiable Vhi, simdifiable Vz>
    requires simdify_compatible<Vx, Vlo, Vhi, Vz>
void clamp_resid(Vx &&x, Vlo &&lo, Vhi &&hi, Vz &&z) {
    detail::clamp_resid<simdified_value_t<Vx>, simdified_abi_t<Vx>>(
        simdify(x).as_const(), simdify(lo).as_const(), simdify(hi).as_const(), simdify(z));
}

/// Add scaled vector z = αx + βy.
template <simdifiable Vx, simdifiable Vy, simdifiable Vz, //
          std::convertible_to<simdified_value_t<Vx>> Ta,
          std::convertible_to<simdified_value_t<Vx>> Tb>
    requires simdify_compatible<Vx, Vy, Vz>
void axpby(Ta alpha, Vx &&x, Tb beta, Vy &&y, Vz &&z) {
    detail::gaxpby<simdified_value_t<Vx>, simdified_abi_t<Vx>, simdified_value_t<Vx>{0}>(
        simdify(z), {{alpha, beta}}, simdify(x).as_const(), simdify(y).as_const());
}

/// Add scaled vector y = αx + βy.
template <simdifiable Vx, simdifiable Vy, //
          std::convertible_to<simdified_value_t<Vx>> Ta,
          std::convertible_to<simdified_value_t<Vx>> Tb>
    requires simdify_compatible<Vx, Vy>
void axpby(Ta alpha, Vx &&x, Tb beta, Vy &&y) {
    detail::gaxpby<simdified_value_t<Vx>, simdified_abi_t<Vx>, simdified_value_t<Vx>{0}>(
        simdify(y), {{alpha, beta}}, simdify(x).as_const(), simdify(y).as_const());
}

/// Add scaled vector y = ∑ᵢ αᵢxᵢ + βy.
template <auto Beta = 1, simdifiable Vy, simdifiable... Vx>
    requires simdify_compatible<Vy, Vx...>
void axpy(Vy &&y, const std::array<simdified_value_t<Vy>, sizeof...(Vx)> &alphas, Vx &&...x) {
    detail::gaxpby<simdified_value_t<Vy>, simdified_abi_t<Vy>, simdified_value_t<Vy>{Beta}>(
        simdify(y), alphas, simdify(x).as_const()...);
}

/// Add scaled vector z = αx + y.
template <simdifiable Vx, simdifiable Vy, simdifiable Vz,
          std::convertible_to<simdified_value_t<Vx>> Ta>
    requires simdify_compatible<Vx, Vy, Vz>
void axpy(Ta alpha, Vx &&x, Vy &&y, Vz &&z) {
    axpby(alpha, x, 1, y, z);
}

/// Add scaled vector y = αx + βy (where β is a compile-time constant).
template <auto Beta = 1, simdifiable Vx, simdifiable Vy,
          std::convertible_to<simdified_value_t<Vx>> Ta>
    requires simdify_compatible<Vx, Vy>
void axpy(Ta alpha, Vx &&x, Vy &&y) {
    detail::gaxpby<simdified_value_t<Vx>, simdified_abi_t<Vx>, simdified_value_t<Vx>{Beta}>(
        simdify(y), {{alpha}}, simdify(x).as_const());
}

/// Negate a matrix or vector B = -A.
template <simdifiable VA, simdifiable VB, int Rotate = 0>
    requires simdify_compatible<VA, VB>
void negate(VA &&A, VB &&B, with_rotate_t<Rotate> = {}) {
    detail::negate<simdified_value_t<VA>, simdified_abi_t<VA>, Rotate>(simdify(A).as_const(),
                                                                       simdify(B));
}

/// Negate a matrix or vector A = -A.
template <simdifiable VA, int Rotate = 0>
void negate(VA &&A, with_rotate_t<Rotate> = {}) {
    detail::negate<simdified_value_t<VA>, simdified_abi_t<VA>, Rotate>(simdify(A).as_const(),
                                                                       simdify(A));
}

/// Subtract two matrices or vectors C = A - B. Rotate affects B.
template <simdifiable VA, simdifiable VB, simdifiable VC, int Rotate = 0>
    requires simdify_compatible<VA, VB, VC>
void sub(VA &&A, VB &&B, VC &&C, with_rotate_t<Rotate> = {}) {
    detail::sub<simdified_value_t<VA>, simdified_abi_t<VA>, Rotate>(
        simdify(A).as_const(), simdify(B).as_const(), simdify(C));
}

/// Subtract two matrices or vectors A = A - B. Rotate affects B.
template <simdifiable VA, simdifiable VB, int Rotate = 0>
    requires simdify_compatible<VA, VB>
void sub(VA &&A, VB &&B, with_rotate_t<Rotate> = {}) {
    detail::sub<simdified_value_t<VA>, simdified_abi_t<VA>, Rotate>(
        simdify(A).as_const(), simdify(B).as_const(), simdify(A));
}

/// Add two matrices or vectors C = A + B. Rotate affects B.
template <simdifiable VA, simdifiable VB, simdifiable VC, int Rotate = 0>
    requires simdify_compatible<VA, VB, VC>
void add(VA &&A, VB &&B, VC &&C, with_rotate_t<Rotate> = {}) {
    detail::add<simdified_value_t<VA>, simdified_abi_t<VA>, Rotate>(
        simdify(A).as_const(), simdify(B).as_const(), simdify(C));
}

/// Add two matrices or vectors A = A + B. Rotate affects B.
template <simdifiable VA, simdifiable VB, int Rotate = 0>
    requires simdify_compatible<VA, VB>
void add(VA &&A, VB &&B, with_rotate_t<Rotate> = {}) {
    detail::add<simdified_value_t<VA>, simdified_abi_t<VA>, Rotate>(
        simdify(A).as_const(), simdify(B).as_const(), simdify(A));
}

/// Apply a function to all elements of the given matrices or vectors.
template <class F, simdifiable VA, simdifiable... VAs>
    requires simdify_compatible<VA, VAs...>
void for_each_elementwise(F &&fun, VA &&A, VAs &&...As) {
    static constexpr auto storage_order = simdified_view_t<VA>::storage_order;
    detail::iter_elems<simdified_value_t<VA>, simdified_abi_t<VA>, storage_order>(
        std::forward<F>(fun), simdify(A).as_const(), simdify(As).as_const()...);
}

/// Apply a function to all elements of the given matrices or vectors, storing the result in the
/// first argument.
template <class F, simdifiable VA, simdifiable... VAs>
    requires simdify_compatible<VA, VAs...>
void transform_elementwise(F &&fun, VA &&A, VAs &&...As) {
    static constexpr auto storage_order = simdified_view_t<VA>::storage_order;
    detail::iter_elems_store<simdified_value_t<VA>, simdified_abi_t<VA>, storage_order>(
        std::forward<F>(fun), simdify(A), simdify(As).as_const()...);
}

/// Apply a function to all elements of the given matrices or vectors, storing the results in the
/// first two arguments.
template <class F, simdifiable VA, simdifiable VB, simdifiable... VAs>
    requires simdify_compatible<VA, VB, VAs...>
void transform2_elementwise(F &&fun, VA &&A, VB &&B, VAs &&...As) {
    static constexpr auto storage_order = simdified_view_t<VA>::storage_order;
    detail::iter_elems_store2<simdified_value_t<VA>, simdified_abi_t<VA>, storage_order>(
        std::forward<F>(fun), simdify(A), simdify(B), simdify(As).as_const()...);
}

/// Apply a function to all elements of the given matrices or vectors, storing the results in the
/// tuple of matrices given as the first argument.
template <class F, simdifiable... VAs, simdifiable... VBs>
    requires simdify_compatible<VAs..., VBs...>
void transform_n_elementwise(F &&fun, std::tuple<VAs...> As, VBs &&...Bs) {
    using VA0                           = std::tuple_element_t<0, decltype(As)>;
    static constexpr auto storage_order = simdified_view_t<VA0>::storage_order;
    detail::iter_elems_store_n<simdified_value_t<VA0>, simdified_abi_t<VA0>, storage_order>(
        std::forward<F>(fun),
        std::apply([](auto &&...a) { return std::make_tuple(simdify(a)...); }, As),
        simdify(Bs).as_const()...);
}

/// @}

/// @name Multi-batch operations
/// @{

template <simdifiable_multi V> // TODO: move to batmat
using simdified_value_t = typename simdified_multi_view_type<V>::value_type;
template <simdifiable_multi V> // TODO: move to batmat
using simdified_abi_t = typename simdified_multi_view_type<V>::abi_type;
template <simdifiable_multi V, simdifiable_multi... Vs> // TODO: move to batmat
inline constexpr bool simdify_compatible =
    (std::is_same_v<simdified_value_t<V>, simdified_value_t<Vs>> && ...) &&
    (std::is_same_v<simdified_abi_t<V>, simdified_abi_t<Vs>> && ...);

/// Compute the norms (max, 1-norm, and 2-norm) of a vector.
template <simdifiable_multi Vx>
norms<simdified_value_t<Vx>>::result norms_all(Vx &&x) {
    typename norms<simdified_value_t<Vx>>::result result{};
    for (index_t b = 0; b < x.num_batches(); ++b)
        result = norms<simdified_value_t<Vx>>{}(result, norms_all(x.batch(b)));
    return result;
}

/// Compute the infinity norm of a vector.
template <simdifiable_multi Vx>
simdified_value_t<Vx> norm_inf(Vx &&x) {
    return norms_all(std::forward<Vx>(x)).norminf();
}

/// Compute the 1-norm of a vector.
template <simdifiable_multi Vx>
simdified_value_t<Vx> norm_1(Vx &&x) {
    return norms_all(std::forward<Vx>(x)).norm1();
}

/// Compute the squared 2-norm of a vector.
template <simdifiable_multi Vx>
simdified_value_t<Vx> norm_2_squared(Vx &&x) {
    simdified_value_t<Vx> sumsq{};
    for (index_t b = 0; b < x.num_batches(); ++b)
        sumsq += norm_2_squared(x.batch(b));
    return sumsq;
}

/// Compute the 2-norm of a vector.
template <simdifiable_multi Vx>
simdified_value_t<Vx> norm_2(Vx &&x) {
    using std::sqrt;
    return sqrt(norm_2_squared(std::forward<Vx>(x)));
}

/// Compute the dot product of two vectors.
template <simdifiable_multi Vx, simdifiable_multi Vy>
    requires simdify_compatible<Vx, Vy>
simdified_value_t<Vx> dot(Vx &&x, Vy &&y) {
    BATMAT_ASSERT(x.num_batches() == y.num_batches());
    simdified_value_t<Vx> result{};
    for (index_t b = 0; b < x.num_batches(); ++b)
        result += dot(x.batch(b), y.batch(b));
    return result;
}

/// Compute the Hadamard (elementwise) product of two vectors z = x ⊙ y.
template <simdifiable_multi Vx, simdifiable_multi Vy, simdifiable_multi Vz>
    requires simdify_compatible<Vx, Vy, Vz>
void hadamard(Vx &&x, Vy &&y, Vz &&z) {
    BATMAT_ASSERT(x.num_batches() == y.num_batches());
    BATMAT_ASSERT(x.num_batches() == z.num_batches());
    for (index_t b = 0; b < x.num_batches(); ++b)
        hadamard(x.batch(b), y.batch(b), z.batch(b));
}

/// Compute the Hadamard (elementwise) product of two vectors x = x ⊙ y.
template <simdifiable_multi Vx, simdifiable_multi Vy>
    requires simdify_compatible<Vx, Vy>
void hadamard(Vx &&x, Vy &&y) {
    BATMAT_ASSERT(x.num_batches() == y.num_batches());
    for (index_t b = 0; b < x.num_batches(); ++b)
        hadamard(x.batch(b), y.batch(b));
}

/// Elementwise clamping z = max(lo, min(x, hi)).
template <simdifiable_multi Vx, simdifiable_multi Vlo, simdifiable_multi Vhi, simdifiable_multi Vz>
    requires simdify_compatible<Vx, Vlo, Vhi, Vz>
void clamp(Vx &&x, Vlo &&lo, Vhi &&hi, Vz &&z) {
    BATMAT_ASSERT(x.num_batches() == lo.num_batches());
    BATMAT_ASSERT(x.num_batches() == hi.num_batches());
    BATMAT_ASSERT(x.num_batches() == z.num_batches());
    for (index_t b = 0; b < x.num_batches(); ++b)
        clamp(x.batch(b), lo.batch(b), hi.batch(b), z.batch(b));
}

/// Elementwise clamping residual z = x - max(lo, min(x, hi)).
template <simdifiable_multi Vx, simdifiable_multi Vlo, simdifiable_multi Vhi, simdifiable_multi Vz>
    requires simdify_compatible<Vx, Vlo, Vhi, Vz>
void clamp_resid(Vx &&x, Vlo &&lo, Vhi &&hi, Vz &&z) {
    BATMAT_ASSERT(x.num_batches() == lo.num_batches());
    BATMAT_ASSERT(x.num_batches() == hi.num_batches());
    BATMAT_ASSERT(x.num_batches() == z.num_batches());
    for (index_t b = 0; b < x.num_batches(); ++b)
        clamp_resid(x.batch(b), lo.batch(b), hi.batch(b), z.batch(b));
}

/// Add scaled vector z = αx + βy.
template <simdifiable_multi Vx, simdifiable_multi Vy, simdifiable_multi Vz, //
          std::convertible_to<simdified_value_t<Vx>> Ta,
          std::convertible_to<simdified_value_t<Vx>> Tb>
    requires simdify_compatible<Vx, Vy, Vz>
void axpby(Ta alpha, Vx &&x, Tb beta, Vy &&y, Vz &&z) {
    BATMAT_ASSERT(x.num_batches() == y.num_batches());
    BATMAT_ASSERT(x.num_batches() == z.num_batches());
    for (index_t b = 0; b < x.num_batches(); ++b)
        axpby(alpha, x.batch(b), beta, y.batch(b), z.batch(b));
}

/// Add scaled vector y = αx + βy.
template <simdifiable_multi Vx, simdifiable_multi Vy, //
          std::convertible_to<simdified_value_t<Vx>> Ta,
          std::convertible_to<simdified_value_t<Vx>> Tb>
    requires simdify_compatible<Vx, Vy>
void axpby(Ta alpha, Vx &&x, Tb beta, Vy &&y) {
    BATMAT_ASSERT(x.num_batches() == y.num_batches());
    for (index_t b = 0; b < x.num_batches(); ++b)
        axpby(alpha, x.batch(b), beta, y.batch(b));
}

/// Add scaled vector y = ∑ᵢ αᵢxᵢ + βy.
template <auto Beta = 1, simdifiable_multi Vy, simdifiable_multi... Vx>
    requires simdify_compatible<Vy, Vx...>
void axpy(Vy &&y, const std::array<simdified_value_t<Vy>, sizeof...(Vx)> &alphas, Vx &&...x) {
    BATMAT_ASSERT(((y.num_batches() == x.num_batches()) && ...));
    for (index_t b = 0; b < y.num_batches(); ++b)
        axpy<Beta>(y.batch(b), alphas, x.batch(b)...);
}

/// Add scaled vector z = αx + y.
template <simdifiable_multi Vx, simdifiable_multi Vy, simdifiable_multi Vz,
          std::convertible_to<simdified_value_t<Vx>> Ta>
    requires simdify_compatible<Vx, Vy, Vz>
void axpy(Ta alpha, Vx &&x, Vy &&y, Vz &&z) {
    axpby(alpha, x, 1, y, z);
}

/// Add scaled vector y = αx + βy (where β is a compile-time constant).
template <auto Beta = 1, simdifiable_multi Vx, simdifiable_multi Vy,
          std::convertible_to<simdified_value_t<Vx>> Ta>
    requires simdify_compatible<Vx, Vy>
void axpy(Ta alpha, Vx &&x, Vy &&y) {
    BATMAT_ASSERT(x.num_batches() == y.num_batches());
    for (index_t b = 0; b < x.num_batches(); ++b)
        axpy<Beta>(alpha, x.batch(b), y.batch(b));
}

/// Negate a matrix or vector B = -A.
template <simdifiable_multi VA, simdifiable_multi VB, int Rotate = 0>
    requires simdify_compatible<VA, VB>
void negate(VA &&A, VB &&B, with_rotate_t<Rotate> rot = {}) {
    BATMAT_ASSERT(A.num_batches() == B.num_batches());
    for (index_t b = 0; b < A.num_batches(); ++b)
        negate(A.batch(b), B.batch(b), rot);
}

/// Negate a matrix or vector A = -A.
template <simdifiable_multi VA, int Rotate = 0>
void negate(VA &&A, with_rotate_t<Rotate> rot = {}) {
    for (index_t b = 0; b < A.num_batches(); ++b)
        negate(A.batch(b), rot);
}

/// Subtract two matrices or vectors C = A - B. Rotate affects B.
template <simdifiable_multi VA, simdifiable_multi VB, simdifiable_multi VC, int Rotate = 0>
    requires simdify_compatible<VA, VB, VC>
void sub(VA &&A, VB &&B, VC &&C, with_rotate_t<Rotate> rot = {}) {
    BATMAT_ASSERT(A.num_batches() == B.num_batches());
    BATMAT_ASSERT(A.num_batches() == C.num_batches());
    for (index_t b = 0; b < A.num_batches(); ++b)
        sub(A.batch(b), B.batch(b), C.batch(b), rot);
}

/// Subtract two matrices or vectors A = A - B. Rotate affects B.
template <simdifiable_multi VA, simdifiable_multi VB, int Rotate = 0>
    requires simdify_compatible<VA, VB>
void sub(VA &&A, VB &&B, with_rotate_t<Rotate> rot = {}) {
    BATMAT_ASSERT(A.num_batches() == B.num_batches());
    for (index_t b = 0; b < A.num_batches(); ++b)
        sub(A.batch(b), B.batch(b), rot);
}

/// Add two matrices or vectors C = A + B. Rotate affects B.
template <simdifiable_multi VA, simdifiable_multi VB, simdifiable_multi VC, int Rotate = 0>
    requires simdify_compatible<VA, VB, VC>
void add(VA &&A, VB &&B, VC &&C, with_rotate_t<Rotate> rot = {}) {
    BATMAT_ASSERT(A.num_batches() == B.num_batches());
    BATMAT_ASSERT(A.num_batches() == C.num_batches());
    for (index_t b = 0; b < A.num_batches(); ++b)
        add(A.batch(b), B.batch(b), C.batch(b), rot);
}

/// Add two matrices or vectors A = A + B. Rotate affects B.
template <simdifiable_multi VA, simdifiable_multi VB, int Rotate = 0>
    requires simdify_compatible<VA, VB>
void add(VA &&A, VB &&B, with_rotate_t<Rotate> rot = {}) {
    BATMAT_ASSERT(A.num_batches() == B.num_batches());
    for (index_t b = 0; b < A.num_batches(); ++b)
        add(A.batch(b), B.batch(b), rot);
}

/// Apply a function to all elements of the given matrices or vectors.
template <class F, simdifiable_multi VA, simdifiable_multi... VAs>
    requires simdify_compatible<VA, VAs...>
void for_each_elementwise(F &&fun, VA &&A, VAs &&...As) {
    BATMAT_ASSERT(((A.num_batches() == As.num_batches()) && ...));
    for (index_t b = 0; b < A.num_batches(); ++b)
        for_each_elementwise(fun, A.batch(b), As.batch(b)...);
}

/// Apply a function to all elements of the given matrices or vectors, storing the result in the
/// first argument.
template <class F, simdifiable_multi VA, simdifiable_multi... VAs>
    requires simdify_compatible<VA, VAs...>
void transform_elementwise(F &&fun, VA &&A, VAs &&...As) {
    BATMAT_ASSERT(((A.num_batches() == As.num_batches()) && ...));
    for (index_t b = 0; b < A.num_batches(); ++b)
        transform_elementwise(fun, A.batch(b), As.batch(b)...);
}

/// Apply a function to all elements of the given matrices or vectors, storing the results in the
/// first two arguments.
template <class F, simdifiable_multi VA, simdifiable_multi VB, simdifiable_multi... VAs>
    requires simdify_compatible<VA, VB, VAs...>
void transform2_elementwise(F &&fun, VA &&A, VB &&B, VAs &&...As) {
    BATMAT_ASSERT(A.num_batches() == B.num_batches());
    BATMAT_ASSERT(((A.num_batches() == As.num_batches()) && ...));
    for (index_t b = 0; b < A.num_batches(); ++b)
        transform2_elementwise(fun, A.batch(b), B.batch(b), As.batch(b)...);
}

/// Apply a function to all elements of the given matrices or vectors, storing the results in the
/// tuple of matrices given as the first argument.
template <class F, simdifiable_multi... VAs, simdifiable_multi... VBs>
    requires simdify_compatible<VAs..., VBs...>
void transform_n_elementwise(F &&fun, std::tuple<VAs...> As, VBs &&...Bs) {
    using std::get;
    auto &&a0 = get<0>(As);
    BATMAT_ASSERT(((a0.num_batches() == Bs.num_batches()) && ...));
    BATMAT_ASSERT([&]<std::size_t... Is>(std::index_sequence<Is...>) {
        return ((a0.num_batches() == get<Is>(As).num_batches()) && ...);
    }(std::make_index_sequence<sizeof...(VAs)>()));
    for (index_t b = 0; b < a0.num_batches(); ++b)
        transform_n_elementwise(
            fun, std::apply([&](auto &&...a) { return std::make_tuple(a.batch(b)...); }, As),
            Bs.batch(b)...);
}

/// @}

} // namespace cyqlone::linalg
