#pragma once

#include <cyqlone/config.hpp>
#include <cyqlone/cyqlone-storage.hpp>
#include <cyqlone/qpalm/solver.hpp>

#include <memory>
#include <span>

namespace cyqlone::qpalm {

template <index_t VL>
struct CyclOCPBackend;

struct CyclOCPData {
    std::span<const real_t> initial_variables = {}, initial_inequality_multipliers = {},
                            initial_equality_multipliers = {};
};

struct CyclOCPBackendSettings {
    index_t log_processors = 3;
    bool print_residuals   = false;
    int print_precision    = 3;
    // bool init_x0               = false; // TODO
    bool factor_alt               = true;
    double changing_constr_factor = 0.05;
    index_t max_update_count      = 5;
    bool detailed_timings         = false;
    index_t pcg_max_iter          = 100;
    real_t pcg_tolerance          = std::numeric_limits<real_t>::epsilon() / 10;
    bool pcg_print_resid          = false;
    bool use_stair_preconditioner = true;
};

template <index_t VL>
struct unique_CyclOCPBackend : std::unique_ptr<CyclOCPBackend<VL>> {
    unique_CyclOCPBackend()                                             = default;
    unique_CyclOCPBackend(unique_CyclOCPBackend &&) noexcept            = default;
    unique_CyclOCPBackend &operator=(unique_CyclOCPBackend &&) noexcept = default;
    ~unique_CyclOCPBackend();
    unique_CyclOCPBackend(std::unique_ptr<CyclOCPBackend<VL>> &&o) noexcept
        : std::unique_ptr<CyclOCPBackend<VL>>{std::move(o)} {}
};

template <index_t VL>
struct detail::backend_type<unique_CyclOCPBackend<VL>> {
    using type = CyclOCPBackend<VL>;
};

template <index_t VL>
unique_CyclOCPBackend<VL> make_qpalm_cyclocp_backend(const CyqloneStorage<real_t> &ocp,
                                                     CyclOCPData data,
                                                     const CyclOCPBackendSettings &settings);

} // namespace cyqlone::qpalm
