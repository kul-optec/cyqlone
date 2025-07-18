#pragma once

#include <cyqlone/qpalm/solver.hpp>
#include <guanaqo/linalg/sparsity.hpp>

#include <memory>
#include <span>

namespace cyqlone::qpalm {

struct LadelSchurBackend;

struct QPData {
    guanaqo::linalg::Sparsity Q_sparsity, A_sparsity;
    std::span<const double> Q, A;
    std::span<const double> q, b_min, b_max;
    double c;
};

struct LadelSchurBackendSettings {};

struct unique_LadelSchurBackend : std::unique_ptr<LadelSchurBackend> {
    unique_LadelSchurBackend()                                                = default;
    unique_LadelSchurBackend(unique_LadelSchurBackend &&) noexcept            = default;
    unique_LadelSchurBackend &operator=(unique_LadelSchurBackend &&) noexcept = default;
    ~unique_LadelSchurBackend();
    unique_LadelSchurBackend(std::unique_ptr<LadelSchurBackend> &&o) noexcept;
};

template <>
struct detail::backend_type<unique_LadelSchurBackend> {
    using type = LadelSchurBackend;
};

unique_LadelSchurBackend make_qpalm_ladel_schur_backend(QPData data,
                                                        const LadelSchurBackendSettings &settings);

} // namespace cyqlone::qpalm
