#pragma once

#include <cyqlone/config.hpp>
#include <cyqlone/cyqlone-params.hpp>
#include <cyqlone/cyqlone-storage.hpp>
#include <cyqlone/qpalm/solver.hpp>
#include <guanaqo/mat-view.hpp>

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

struct CyQPALMBackendSettings {
    index_t processors   = 8;
    bool print_residuals = false;
    int print_precision  = 3;
    // bool init_x0               = false; // TODO
    double changing_constr_factor = 0.05;
    index_t max_update_count      = 5;
    bool detailed_timings         = false;
    cyqlone::TricyqleParams<real_t> tricyqle_params{};
    uint32_t spin_count           = std::numeric_limits<uint32_t>::max();
    WarmStartingStrategy strategy = WarmStartingStrategy::Copy;
};

struct CyQPALMBackendStats {
    index_t num_updates  = 0;
    index_t rank_updates = 0;
    index_t num_factor   = 0;
};

} // namespace cyqlone::qpalm

namespace CYQLONE_NS(cyqlone::qpalm) {

using guanaqo::StorageOrder;

template <index_t VL, StorageOrder DefaultOrder = StorageOrder::ColMajor>
struct CyQPALMBackend;

namespace detail {

template <index_t VL, StorageOrder DefaultOrder>
struct backend_stats_type<CyQPALMBackend<VL, DefaultOrder>> {
    using type = CyQPALMBackendStats;
};

} // namespace detail

template <index_t VL, StorageOrder DefaultOrder = StorageOrder::ColMajor>
struct unique_CyQPALMBackend : std::unique_ptr<CyQPALMBackend<VL, DefaultOrder>> {
    unique_CyQPALMBackend()                                             = default;
    unique_CyQPALMBackend(unique_CyQPALMBackend &&) noexcept            = default;
    unique_CyQPALMBackend &operator=(unique_CyQPALMBackend &&) noexcept = default;
    ~unique_CyQPALMBackend();
    unique_CyQPALMBackend(std::unique_ptr<CyQPALMBackend<VL, DefaultOrder>> &&o) noexcept
        : std::unique_ptr<CyQPALMBackend<VL, DefaultOrder>>{std::move(o)} {}
};

template <index_t VL, StorageOrder DefaultOrder>
struct detail::backend_type<unique_CyQPALMBackend<VL, DefaultOrder>> {
    using type = CyQPALMBackend<VL, DefaultOrder>;
};

template <index_t VL, StorageOrder DefaultOrder = StorageOrder::ColMajor>
unique_CyQPALMBackend<VL, DefaultOrder>
make_cyqpalm_backend(const CyqloneStorage<real_t> &ocp, CyqloneData data,
                           const CyQPALMBackendSettings &settings);

template <index_t VL, StorageOrder DefaultOrder = StorageOrder::ColMajor>
void update_cyqpalm_backend(CyQPALMBackend<VL, DefaultOrder> &backend,
                                  const CyqloneStorage<real_t> &ocp);

template <index_t VL, StorageOrder DefaultOrder = StorageOrder::ColMajor>
void update_cyqpalm_backend(CyQPALMBackend<VL, DefaultOrder> &backend,
                                  const LinearOCPStorage &ocp);

} // namespace CYQLONE_NS(cyqlone::qpalm)
