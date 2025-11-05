#pragma once

#include <cyqlone/config.hpp>
#include <cmath>

namespace CYQLONE_NAMESPACE {

template <class T, class simd = void>
struct norms;
template <class T>
struct norms<T>;

template <class T, class simd>
struct norms : norms<T> {
    using result = typename norms<T>::result;
    struct result_simd {
        simd max;
        simd asum;
        simd sumsq;
    };

    using norms<T>::operator();

    result_simd operator()(result_simd accum, simd t) const {
        using std::abs;
        using std::max;
        auto at = abs(t);
        return {.max = max(at, accum.max), .asum = at + accum.asum, .sumsq = t * t + accum.sumsq};
    }

    result operator()(result_simd accum) const {
        return {hmax(accum.max), reduce(accum.asum), reduce(accum.sumsq)};
    }

    using norms<T>::zero;
    static result_simd zero_simd() { return {}; }
};

template <class T>
struct norms<T, void> {
    struct result {
        real_t max;
        real_t asum;
        real_t sumsq;
    };

    result operator()(result accum, T t) const {
        using std::abs;
        using std::max;
        auto at = abs(t);
        return {.max = max(at, accum.max), .asum = at + accum.asum, .sumsq = t * t + accum.sumsq};
    }

    result operator()(result accum, result t) const {
        return {std::max(accum.max, t.max), accum.asum + t.asum, accum.sumsq + t.sumsq};
    }

    static result zero() { return {}; }
};

} // namespace CYQLONE_NAMESPACE
