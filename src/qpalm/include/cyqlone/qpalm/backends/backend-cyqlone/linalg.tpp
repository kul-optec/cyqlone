#pragma once

#include <cyqlone/linalg.hpp>
#include <cyqlone/qpalm/backends/ocp-backend-cyqlone.tpp>

#include <array>
#include <span>

namespace CYQLONE_NS(cyqlone::qpalm) {

template <index_t VL, StorageOrder DefaultOrder>
template <class T, class U>
void CyQPALMBackend<VL, DefaultOrder>::xaxpy(Context &ctx, real_t a, const T &x, U &y) {
    const auto xaxpy = [a](auto, auto, auto xi, auto yi) { linalg::axpy(a, xi, yi); };
    ocp.foreach_stage(ctx, xaxpy, x, y);
}

template <index_t VL, StorageOrder DefaultOrder>
template <class T, class U>
void CyQPALMBackend<VL, DefaultOrder>::xcopy(Context &ctx, const T &x, U &y) const {
    const auto xcopy = [](auto, auto, auto xi, auto yi) { batmat::linalg::copy(xi, yi); };
    ocp.foreach_stage(ctx, xcopy, x, y);
}

template <index_t VL, StorageOrder DefaultOrder>
template <class T, class U>
void CyQPALMBackend<VL, DefaultOrder>::set_constant(Context &ctx, T &x, const U &y) const {
    const auto set_constant = [y](auto, auto, auto xi) { batmat::linalg::fill(y, xi); };
    ocp.foreach_stage(ctx, set_constant, x);
}

template <index_t VL, StorageOrder DefaultOrder>
template <class T>
void CyQPALMBackend<VL, DefaultOrder>::scale(Context &ctx, real_t s, T &x) const {
    const auto scale = [&](auto, auto, auto xi) { linalg::axpy<0>(s, xi, xi); };
    ocp.foreach_stage(ctx, scale, x);
}

template <index_t VL, StorageOrder DefaultOrder>
real_t CyQPALMBackend<VL, DefaultOrder>::dot(Context &ctx, const var_vec_t &a,
                                             const var_vec_t &b) const {
    real_t sum     = 0;
    const auto dot = [&](auto, auto, auto ai, auto bi) { sum += linalg::dot(ai, bi); };
    ocp.foreach_stage(ctx, dot, a, b);
    return ctx.reduce(sum);
}

template <index_t VL, StorageOrder DefaultOrder>
template <class... Args>
void CyQPALMBackend<VL, DefaultOrder>::local_dots(std::span<real_t, 1 + sizeof...(Args) / 2> out,
                                                  const auto &a, const auto &b,
                                                  const Args &...others) const {
    out[0] += linalg::dot(a, b);
    if constexpr (sizeof...(Args) > 0)
        local_dots(out.template subspan<1>(), others...);
}

template <index_t VL, StorageOrder DefaultOrder>
template <class... Args>
std::array<real_t, sizeof...(Args) / 2>
CyQPALMBackend<VL, DefaultOrder>::dots(Context &ctx, const Args &...args) const {
    using local_sums_t = std::array<real_t, sizeof...(Args) / 2>;
    local_sums_t local_sums{};
    const auto dots = [&](auto, auto, auto... batches) { local_dots(local_sums, batches...); };
    ocp.foreach_stage(ctx, dots, args...);
    return ctx.reduce(local_sums, [](local_sums_t a, local_sums_t b) {
        local_sums_t c{};
        for (size_t i = 0; i < a.size(); ++i)
            c[i] = a[i] + b[i];
        return c;
    });
}

template <index_t VL, StorageOrder DefaultOrder>
template <class T>
auto CyQPALMBackend<VL, DefaultOrder>::norm_inf_l1_sq(Context &ctx, const T &x) const {
    GUANAQO_TRACE("norm_inf_l1_sq", 0, 4 * x.batch_size() * x.rows() * ocp.n);
    auto nrm                  = norms.zero();
    const auto norm_inf_l1_sq = [&](auto, auto, auto xi) {
        nrm = norms(nrm, linalg::norms_all(xi));
    };
    ocp.foreach_stage(ctx, norm_inf_l1_sq, x);
    return ctx.reduce(nrm, norms);
}

template <index_t VL, StorageOrder DefaultOrder>
template <class T>
real_t CyQPALMBackend<VL, DefaultOrder>::norm_inf(Context &ctx, const T &x) const {
    using std::isfinite;
    auto nrm = norm_inf_l1_sq(ctx, x);
    return isfinite(nrm.asum) ? nrm.max : nrm.asum;
}

template <index_t VL, StorageOrder DefaultOrder>
template <class T>
real_t CyQPALMBackend<VL, DefaultOrder>::norm_squared(Context &ctx, const T &x) const {
    real_t sumsq            = 0;
    const auto norm_squared = [&](auto, auto, auto xi) { sumsq += linalg::norm_2_squared(xi); };
    ocp.foreach_stage(ctx, norm_squared, x);
    return ctx.reduce(sumsq);
}

} // namespace CYQLONE_NS(cyqlone::qpalm)
