#pragma once

#include <guanaqo/trace.hpp>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <ranges>
#if CYQLONE_WITH_SKA_SORT
#include <ska_sort.hpp>
#endif

namespace cyqlone::qpalm {

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

} // namespace cyqlone::qpalm
