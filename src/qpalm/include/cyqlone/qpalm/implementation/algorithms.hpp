#pragma once

#include <guanaqo/trace.hpp>
#include <algorithm>
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

template <class R, class F>
static decltype(auto) partition(R &&range, F key) {
    GUANAQO_TRACE("partition", 0, std::ranges::ssize(range));
    return std::ranges::partition(range, key);
}

template <class R, class F>
static decltype(auto) min_element(R &&range, F key) {
    GUANAQO_TRACE("min_element", 0, std::ranges::ssize(range));
    return std::ranges::min_element(range, {}, key);
}

} // namespace cyqlone::qpalm
