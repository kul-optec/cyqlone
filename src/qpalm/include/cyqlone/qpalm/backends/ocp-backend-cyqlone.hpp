#pragma once

#include <cyqlone/config.hpp>
#include <cyqlone/cyqlone-storage.hpp>
#include <cyqlone/qpalm/solver.hpp>

#include <cstdint>
#include <limits>
#include <memory>
#include <span>

namespace cyqlone::qpalm {

struct CyqloneData {
    std::span<const real_t> initial_variables = {}, initial_inequality_multipliers = {},
                            initial_equality_multipliers = {};
};

enum class WarmStartingStrategy {
    Zeros,
    Copy,
    Shift,
    ShiftNoInequality,
};

struct CyqloneBackendSettings {
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
    uint32_t spin_count           = std::numeric_limits<uint32_t>::max();
    WarmStartingStrategy strategy = WarmStartingStrategy::Copy;
};

} // namespace cyqlone::qpalm

namespace CYQLONE_NS(cyqlone::qpalm) {

template <index_t VL>
struct CyqloneBackend;

template <index_t VL>
struct unique_CyqloneBackend : std::unique_ptr<CyqloneBackend<VL>> {
    unique_CyqloneBackend()                                             = default;
    unique_CyqloneBackend(unique_CyqloneBackend &&) noexcept            = default;
    unique_CyqloneBackend &operator=(unique_CyqloneBackend &&) noexcept = default;
    ~unique_CyqloneBackend();
    unique_CyqloneBackend(std::unique_ptr<CyqloneBackend<VL>> &&o) noexcept
        : std::unique_ptr<CyqloneBackend<VL>>{std::move(o)} {}
};

template <index_t VL>
struct detail::backend_type<unique_CyqloneBackend<VL>> {
    using type = CyqloneBackend<VL>;
};

template <index_t VL>
unique_CyqloneBackend<VL> make_qpalm_cyqlone_backend(const CyqloneStorage<real_t> &ocp,
                                                     CyqloneData data,
                                                     const CyqloneBackendSettings &settings);

template <index_t VL>
void update_qpalm_cyqlone_backend(CyqloneBackend<VL> &backend, const CyqloneStorage<real_t> &ocp);

template <index_t VL>
void update_qpalm_cyqlone_backend(CyqloneBackend<VL> &backend, const LinearOCPStorage &ocp);

} // namespace CYQLONE_NS(cyqlone::qpalm)
