#pragma once

#include <cyqlone/config.hpp>
#include <batmat/assume.hpp>
#include <guanaqo/trace.hpp>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <ranges>
#if CYQLONE_WITH_SKA_SORT
#include <ska_sort.hpp>
#endif

namespace CYQLONE_NAMESPACE::qpalm {

template <class R, class F>
static void sort(R &&range, F key) {
    GUANAQO_TRACE("sort", 0, std::ranges::ssize(range));
#if CYQLONE_WITH_SKA_SORT
    ska_sort(std::ranges::begin(range), std::ranges::end(range), key);
#else
    std::sort(std::ranges::begin(range), std::ranges::end(range),
              [&](auto a, auto b) { return key(a) < key(b); });
#endif
}

template <class R, class I, class F>
static void nth_element(R &&range, I mid, F key) {
    GUANAQO_TRACE("nth_element", 0, std::ranges::ssize(range));
    std::nth_element(std::ranges::begin(range), mid, std::ranges::end(range),
                     [&](auto a, auto b) { return key(a) < key(b); });
}

/// A variant of std::ranges::partition where the first element of the return value is the smallest
/// element of the "false" partition.
template <std::ranges::bidirectional_range R, class F, class C>
static std::ranges::subrange<std::ranges::iterator_t<R>> partition_min(R &&range, F pred, C cmp) {
    GUANAQO_TRACE("partition_min", 0, std::ranges::ssize(range));
    auto first       = std::ranges::begin(range);
    const auto last  = std::ranges::end(range);
    const auto lasti = std::ranges::next(first, last);
    auto tail = lasti, min_it = lasti; // Track the minimum element of the "false" partition

    // Return the "false" partition, but first make sure that the minimum element is ordered first
    auto done = [&] {
        BATMAT_ASSUME(first == tail);
        if (min_it != first) {
            BATMAT_ASSUME(first != lasti);
            BATMAT_ASSUME(min_it != lasti);
            std::ranges::iter_swap(min_it, first);
        }
        return std::ranges::subrange<std::ranges::iterator_t<R>>(first, lasti);
    };

    while (true) {
        // Find the first element that violates the predicate
        while (true) {
            if (first == tail) {
                return done();
            } else if (pred(*first)) {
                BATMAT_ASSUME(first != last);
                ++first;
            } else {
                break;
            }
        }
        BATMAT_ASSUME(tail != first);
        --tail;
        // Find the last element that satisfies the predicate
        while (true) {
            if (first == tail) {
                if (min_it == last || cmp(*tail, *min_it))
                    min_it = tail;
                return done();
            } else if (pred(*tail)) {
                break;
            } else {
                BATMAT_ASSUME(tail != first);
                if (min_it == last || cmp(*tail, *min_it))
                    min_it = tail;
                --tail;
            }
        }
        std::ranges::iter_swap(first, tail);
        if (min_it == last || cmp(*tail, *min_it))
            min_it = tail;
        BATMAT_ASSUME(first != last);
        ++first;
    }
}

template <std::ranges::forward_range R, class F>
static decltype(auto) partition(R &&range, F key) {
    GUANAQO_TRACE("partition", 0, std::ranges::ssize(range));
    return std::ranges::partition(range, key);
}
template <std::permutable I, std::sentinel_for<I> S, class F>
static decltype(auto) partition(I first, S last, F key) {
    GUANAQO_TRACE("partition", 0, std::ranges::distance(first, last));
    return std::ranges::partition(first, last, key);
}

template <class R, class F>
static decltype(auto) min_element(R &&range, F key) {
    GUANAQO_TRACE("min_element", 0, std::ranges::ssize(range));
    return std::ranges::min_element(range, {}, key);
}

template <class I, class T, class BinOp, class UnOp>
T transform_reduce(I first, I last, T init, BinOp binary_op, UnOp unary_op) {
    GUANAQO_TRACE("transform_reduce", 0, std::ranges::distance(first, last));
    return std::transform_reduce(first, last, init, binary_op, unary_op);
}

} // namespace CYQLONE_NAMESPACE::qpalm
