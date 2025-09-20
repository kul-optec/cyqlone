#pragma once

#include <cmath>
#include <type_traits>

namespace cyqlone {

template <class T>
class NeumaierSum {
    static_assert(std::is_floating_point_v<T>);

    T sum, compensation = 0;
    NeumaierSum(T sum, T compensation) : sum(sum), compensation(compensation) {}

  public:
    NeumaierSum(T value = {}) : sum(value), compensation(0) {}
    operator T() const { return sum + compensation; }

    NeumaierSum operator-() const { return {-sum, -compensation}; }
    NeumaierSum &operator+=(T v) {
        using std::abs;
        T t = sum + v;
        if (abs(sum) >= abs(v))
            compensation += (sum - t) + v;
        else
            compensation = (v - t) + sum;
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
};

} // namespace cyqlone
