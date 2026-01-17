#pragma once

#include <cyqlone/config.hpp>
#include <cmath>
#include <type_traits>

namespace cyqlone {

template <class T>
class NeumaierSum {
  public:
    T sum, compensation{};
    NeumaierSum(T sum, T compensation) : sum{sum}, compensation{compensation} {}
    NeumaierSum(T value = {}) : sum(value) {}
    operator T() const { return sum + compensation; }

    NeumaierSum operator-() const { return {-sum, -compensation}; }
    NeumaierSum &operator+=(T v) {
        using std::abs;
        T t = sum + v;
        if constexpr (std::is_floating_point_v<T>) {
            if (abs(sum) >= abs(v))
                compensation += (sum - t) + v;
            else
                compensation += (v - t) + sum;
        } else {
            auto m = abs(sum) >= abs(v);
#if BATMAT_WITH_GSI_HPC_SIMD
            compensation =
                select(m, compensation + ((sum - t) + v), compensation + ((v - t) + sum));
#else
            where(m, compensation) += (sum - t) + v;
            where(!m, compensation) += (v - t) + sum;
#endif
        }
        sum = t;
        return *this;
    }
    NeumaierSum &operator+=(const NeumaierSum &other) {
        *this += other.sum;
        *this += other.compensation;
        return *this;
    }
    NeumaierSum &operator-=(T v) { return *this += -v; }
    NeumaierSum &operator-=(const NeumaierSum &other) {
        *this -= other.sum;
        *this -= other.compensation;
        return *this;
    }

    friend NeumaierSum operator+(NeumaierSum lhs, T rhs) {
        lhs += rhs;
        return lhs;
    }
    friend NeumaierSum operator+(T lhs, NeumaierSum rhs) {
        rhs += lhs;
        return rhs;
    }
    friend NeumaierSum operator+(NeumaierSum lhs, NeumaierSum rhs) {
        lhs += rhs;
        return lhs;
    }

    friend NeumaierSum operator-(NeumaierSum lhs, T rhs) {
        lhs -= rhs;
        return lhs;
    }
    friend NeumaierSum operator-(T lhs, NeumaierSum rhs) { return lhs + (-rhs); }
    friend NeumaierSum operator-(NeumaierSum lhs, const NeumaierSum &rhs) {
        lhs -= rhs;
        return lhs;
    }
    friend NeumaierSum operator*(NeumaierSum lhs, NeumaierSum rhs) {
        NeumaierSum res = lhs.sum * rhs.sum;
        res += lhs.sum * rhs.compensation;
        res += lhs.compensation * rhs.sum;
        res += lhs.compensation * rhs.compensation;
        return res;
    }
    friend NeumaierSum operator*(NeumaierSum lhs, T rhs) {
        return {lhs.sum * rhs, lhs.compensation * rhs};
    }
    friend NeumaierSum operator*(T lhs, NeumaierSum rhs) {
        return {rhs.sum * lhs, rhs.compensation * lhs};
    }
};

} // namespace cyqlone
