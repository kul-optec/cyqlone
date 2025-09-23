#include <cyqlone/compact.hpp>
#include <cyqlone/neumaier.hpp>

#include <batmat/loop.hpp>
#include <batmat/ops/rotate.hpp>
#include <guanaqo/trace.hpp>
#include <concepts>

namespace cyqlone::compact {

template <class T, class Abi, StorageOrder O>
void CompactBLAS<T, Abi, O>::xhadamard(single_batch_view A, mut_single_batch_view B) {
    GUANAQO_TRACE("xhadamard", 0, A.rows() * A.cols() * A.depth());
    assert(A.rows() == B.rows());
    assert(A.cols() == B.cols());
    const index_t n = A.rows(), m = A.cols();
    for (index_t j = 0; j < m; ++j)
        BATMAT_UNROLLED_IVDEP_FOR (8, index_t i = 0; i < n; ++i)
            simd_types::aligned_store(simd_types::aligned_load(&A(0, i, j)) *
                                          simd_types::aligned_load(&B(0, i, j)),
                                      &B(0, i, j));
}

template <class T, class Abi, StorageOrder O>
void CompactBLAS<T, Abi, O>::xhadamard(batch_view A, mut_batch_view B) {
    assert(A.ceil_depth() == B.ceil_depth());
    BATMAT_OMP(parallel for)
    for (index_t i = 0; i < A.num_batches(); ++i)
        xhadamard(A.batch(i), B.batch(i));
}

template <class T, class Abi, StorageOrder O>
void CompactBLAS<T, Abi, O>::xneg(mut_single_batch_view A) {
    GUANAQO_TRACE("xneg", 0, A.rows() * A.cols() * A.depth());
    const index_t n = A.rows(), m = A.cols();
    for (index_t j = 0; j < m; ++j)
        BATMAT_UNROLLED_IVDEP_FOR (8, index_t i = 0; i < n; ++i)
            simd_types::aligned_store(-simd_types::aligned_load(&A(0, i, j)), &A(0, i, j));
}

template <class T, class Abi, StorageOrder O>
void CompactBLAS<T, Abi, O>::xneg(mut_batch_view A) {
    BATMAT_OMP(parallel for)
    for (index_t i = 0; i < A.num_batches(); ++i)
        xneg(A.batch(i));
}

template <class T, class Abi, StorageOrder O>
void CompactBLAS<T, Abi, O>::xadd_L(single_batch_view A, mut_single_batch_view B) {
    [[maybe_unused]] const auto tri_op_cnt  = A.cols() * (A.cols() + 1) / 2,
                                rect_op_cnt = A.cols() * (A.rows() - A.cols());
    GUANAQO_TRACE("xadd_L", 0, (tri_op_cnt + rect_op_cnt) * A.depth());
    assert(A.rows() == B.rows());
    assert(A.cols() == B.cols());
    assert(A.rows() >= A.cols());
    const index_t n = A.rows(), m = A.cols();
    for (index_t j = 0; j < m; ++j)
        BATMAT_UNROLLED_IVDEP_FOR (8, index_t i = j; i < n; ++i)
            simd_types::aligned_store(simd_types::aligned_load(&B(0, i, j)) +
                                          simd_types::aligned_load(&A(0, i, j)),
                                      &B(0, i, j));
}

template <class T, class Abi, StorageOrder O>
void CompactBLAS<T, Abi, O>::xaxpy(value_type a, single_batch_view x, mut_single_batch_view y) {
    GUANAQO_TRACE("xaxpy", 0, x.rows() * x.cols() * x.depth());
    assert(x.rows() == y.rows());
    assert(x.cols() == y.cols());
    simd a_simd{a};
    const index_t n = x.rows();
    for (index_t j = 0; j < x.cols(); ++j) {
        BATMAT_UNROLLED_IVDEP_FOR (8, index_t i = 0; i < n; ++i) {
            simd_types::aligned_store(a_simd * simd_types::aligned_load(&x(0, i, j)) +
                                          simd_types::aligned_load(&y(0, i, j)),
                                      &y(0, i, j));
        }
    }
}

template <class T, class Abi, StorageOrder O>
void CompactBLAS<T, Abi, O>::xaxpy(value_type a, batch_view x, mut_batch_view y) {
    assert(x.ceil_depth() == y.ceil_depth());
    BATMAT_OMP(parallel for)
    for (index_t i = 0; i < x.num_batches(); ++i)
        xaxpy(a, x.batch(i), y.batch(i));
}

template <class T, class Abi, StorageOrder O>
void CompactBLAS<T, Abi, O>::xaxpby(value_type a, single_batch_view x, value_type b,
                                    mut_single_batch_view y) {
    assert(x.rows() == y.rows());
    assert(x.cols() == y.cols());
    const index_t n = x.rows();
    if (b == 0) {
        GUANAQO_TRACE("xaxpby", 0, x.rows() * x.cols() * x.depth());
        simd a_simd{a};
        for (index_t j = 0; j < x.cols(); ++j) {
            BATMAT_UNROLLED_IVDEP_FOR (8, index_t i = 0; i < n; ++i) {
                simd_types::aligned_store(a_simd * simd_types::aligned_load(&x(0, i, j)),
                                          &y(0, i, j));
            }
        }
    } else {
        GUANAQO_TRACE("xaxpby", 0, 2 * x.rows() * x.cols() * x.depth());
        simd a_simd{a}, b_simd{b};
        for (index_t j = 0; j < x.cols(); ++j) {
            BATMAT_UNROLLED_IVDEP_FOR (8, index_t i = 0; i < n; ++i) {
                simd_types::aligned_store(a_simd * simd_types::aligned_load(&x(0, i, j)) +
                                              b_simd * simd_types::aligned_load(&y(0, i, j)),
                                          &y(0, i, j));
            }
        }
    }
}

template <class T, class Abi, StorageOrder O>
void CompactBLAS<T, Abi, O>::xaxpby(value_type a, batch_view x, value_type b, mut_batch_view y) {
    assert(x.ceil_depth() == y.ceil_depth());
    BATMAT_OMP(parallel for)
    for (index_t i = 0; i < x.num_batches(); ++i)
        xaxpby(a, x.batch(i), b, y.batch(i));
}

template <class T, class Abi, StorageOrder O>
template <int Rot, class OutView, class View, class... Views>
void CompactBLAS<T, Abi, O>::xadd_copy_impl(OutView out, View x1, Views... xs)
    requires(((std::same_as<OutView, mut_batch_view> && std::same_as<View, batch_view>) && ... &&
              std::same_as<Views, batch_view>) ||
             ((std::same_as<OutView, mut_single_batch_view> &&
               std::same_as<View, single_batch_view>) &&
              ... && std::same_as<Views, single_batch_view>))
{
    assert(((x1.batch_size() == xs.batch_size()) && ...));
    assert(x1.batch_size() == out.batch_size());
    assert(((x1.depth() == xs.depth()) && ...));
    assert(x1.depth() == out.depth());
    assert(((x1.rows() == xs.rows()) && ...));
    assert(x1.rows() == out.rows());
    assert(((x1.cols() == xs.cols()) && ...));
    assert(x1.cols() == out.cols());
    index_t i;
    const auto Bs   = static_cast<index_t>(x1.batch_size());
    const index_t n = x1.rows(), m = x1.cols();
    using batmat::ops::rotr;
    for (i = 0; i <= static_cast<index_t>(x1.depth()) - Bs; i += Bs) {
        for (index_t c = 0; c < m; ++c) {
            BATMAT_UNROLLED_IVDEP_FOR (8, index_t r = 0; r < n; ++r) {
                simd_types::aligned_store(
                    (NeumaierSum(rotr<Rot>(simd_types::aligned_load(&x1(i, r, c)))) + ... +
                     simd_types::aligned_load(&xs(i, r, c))),
                    &out(i, r, c));
            }
        }
    }
    for (; i < static_cast<index_t>(x1.depth()); ++i)
        for (index_t c = 0; c < m; ++c)
            for (index_t r = 0; r < n; ++r)
                out(i, r, c) = (NeumaierSum(x1(i, r, c)) + ... + xs(i, r, c));
}

template <class T, class Abi, StorageOrder O>
template <class OutView, class View, class... Views>
void CompactBLAS<T, Abi, O>::xsub_copy_impl(OutView out, View x1, Views... xs)
    requires(((std::same_as<OutView, mut_batch_view> && std::same_as<View, batch_view>) && ... &&
              std::same_as<Views, batch_view>) ||
             ((std::same_as<OutView, mut_single_batch_view> &&
               std::same_as<View, single_batch_view>) &&
              ... && std::same_as<Views, single_batch_view>))
{
    assert(((x1.batch_size() == xs.batch_size()) && ...));
    assert(x1.batch_size() == out.batch_size());
    assert(((x1.depth() == xs.depth()) && ...));
    assert(x1.depth() == out.depth());
    assert(((x1.rows() == xs.rows()) && ...));
    assert(x1.rows() == out.rows());
    assert(((x1.cols() == xs.cols()) && ...));
    assert(x1.cols() == out.cols());
    index_t i;
    const auto Bs   = static_cast<index_t>(x1.batch_size());
    const index_t n = x1.rows(), m = x1.cols();
    for (i = 0; i <= static_cast<index_t>(x1.depth()) - Bs; i += Bs) {
        for (index_t c = 0; c < m; ++c) {
            BATMAT_UNROLLED_IVDEP_FOR (8, index_t r = 0; r < n; ++r) {
                simd_types::aligned_store(NeumaierSum(simd_types::aligned_load(&x1(i, r, c))) -
                                              (... + simd_types::aligned_load(&xs(i, r, c))),
                                          &out(i, r, c));
            }
        }
    }
    for (; i < static_cast<index_t>(x1.depth()); ++i)
        for (index_t c = 0; c < m; ++c)
            for (index_t r = 0; r < n; ++r)
                out(i, r, c) = NeumaierSum(x1(i, r, c)) - (... + xs(i, r, c));
}

template <class T, class Abi, StorageOrder O>
template <int Rot, class OutView, class View, class... Views>
void CompactBLAS<T, Abi, O>::xadd_neg_copy_impl(OutView out, View x1, Views... xs)
    requires(((std::same_as<OutView, mut_batch_view> && std::same_as<View, batch_view>) && ... &&
              std::same_as<Views, batch_view>) ||
             ((std::same_as<OutView, mut_single_batch_view> &&
               std::same_as<View, single_batch_view>) &&
              ... && std::same_as<Views, single_batch_view>))
{
    assert(((x1.batch_size() == xs.batch_size()) && ...));
    assert(x1.batch_size() == out.batch_size());
    assert(((x1.depth() == xs.depth()) && ...));
    assert(x1.depth() == out.depth());
    assert(((x1.rows() == xs.rows()) && ...));
    assert(x1.rows() == out.rows());
    assert(((x1.cols() == xs.cols()) && ...));
    assert(x1.cols() == out.cols());
    index_t i;
    const auto Bs   = static_cast<index_t>(x1.batch_size());
    const index_t n = x1.rows(), m = x1.cols();
    using batmat::ops::rotr;
    for (i = 0; i <= static_cast<index_t>(x1.depth()) - Bs; i += Bs) {
        for (index_t c = 0; c < m; ++c) {
            BATMAT_UNROLLED_IVDEP_FOR (8, index_t r = 0; r < n; ++r) {
                simd_types::aligned_store(
                    -(NeumaierSum(rotr<Rot>(simd_types::aligned_load(&x1(i, r, c)))) + ... +
                      simd_types::aligned_load(&xs(i, r, c))),
                    &out(i, r, c));
            }
        }
    }
    for (; i < static_cast<index_t>(x1.depth()); ++i)
        for (index_t c = 0; c < m; ++c)
            for (index_t r = 0; r < n; ++r)
                out(i, r, c) = -(NeumaierSum(x1(i, r, c)) + ... + xs(i, r, c));
}

template <class T, class Abi, StorageOrder O>
auto CompactBLAS<T, Abi, O>::xdot(single_batch_view x, single_batch_view y) -> value_type {
    GUANAQO_TRACE("xdot", 0, x.rows() * x.cols() * x.depth());
    using std::fma;
    // TODO: why does fma(xi, yi, accum) give such terrible code gen?
    return xreduce(
        NeumaierSum(simd{0}), [](auto accum, auto xi, auto yi) { return accum + xi * yi; },
        [](auto accum) { return NeumaierSum(reduce(accum.sum), reduce(accum.compensation)); }, x,
        y);
}

template <class T, class Abi, StorageOrder O>
auto CompactBLAS<T, Abi, O>::xdot(batch_view x, batch_view y) -> value_type {
    using std::fma;
    // TODO: why does fma(xi, yi, accum) give such terrible code gen?
    return xreduce(
        NeumaierSum(simd{0}), [](auto accum, auto xi, auto yi) { return accum + xi * yi; },
        [](auto accum) { return NeumaierSum(reduce(accum.sum), reduce(accum.compensation)); }, x,
        y);
}

template <class T, class Abi, StorageOrder O>
auto CompactBLAS<T, Abi, O>::xnrm2sq(batch_view x) -> value_type {
    using std::fma;
    return xreduce(
        simd{0}, [](auto accum, auto xi) { return xi * xi + accum; },
        [](auto accum) { return reduce(accum); }, x);
}

template <class T, class Abi, StorageOrder O>
auto CompactBLAS<T, Abi, O>::xnrminf(single_batch_view x) -> value_type {
    using std::abs;
    using std::fma;
    using std::isfinite;
    using std::max;
    auto [inf_nrm, l1_norm] = xreduce(
        std::array<simd, 2>{0, 0},
        [](auto accum, auto xi) { return std::array{max(abs(xi), accum[0]), abs(xi) + accum[1]}; },
        [](auto accum) { return std::array{hmax(accum[0]), reduce(accum[1])}; }, x);
    return isfinite(l1_norm) ? inf_nrm : l1_norm;
}

template <class T, class Abi, StorageOrder O>
auto CompactBLAS<T, Abi, O>::xnrminf(batch_view x) -> value_type {
    using batmat::datapar::hmax;
    using std::abs;
    using std::fma;
    using std::isfinite;
    using std::max;
    auto [inf_nrm, l1_norm] = xreduce(
        std::array<simd, 2>{0, 0},
        [](auto accum, auto xi) { return std::array{max(abs(xi), accum[0]), abs(xi) + accum[1]}; },
        [](auto accum) { return std::array{hmax(accum[0]), reduce(accum[1])}; }, x);
    return isfinite(l1_norm) ? inf_nrm : l1_norm;
}

template <class T, class Abi, StorageOrder O>
void CompactBLAS<T, Abi, O>::proj_diff(single_batch_view x, single_batch_view l,
                                       single_batch_view u, mut_single_batch_view y) {
    assert(x.rows() == y.rows());
    assert(x.rows() == l.rows());
    assert(x.rows() == u.rows());
    assert(x.cols() == y.cols());
    assert(x.cols() == l.cols());
    assert(x.cols() == u.cols());
    const index_t n = x.rows();
    for (index_t j = 0; j < x.cols(); ++j) {
        BATMAT_UNROLLED_IVDEP_FOR (8, index_t i = 0; i < n; ++i) {
            auto xij = simd_types::aligned_load(&x(0, i, j));
            auto lij = simd_types::aligned_load(&l(0, i, j));
            auto uij = simd_types::aligned_load(&u(0, i, j));
            simd_types::aligned_store(xij - max(lij, min(xij, uij)), &y(0, i, j));
        }
    }
}

} // namespace cyqlone::compact
