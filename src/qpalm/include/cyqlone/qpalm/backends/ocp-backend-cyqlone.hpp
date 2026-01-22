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

struct CyqloneBackendSettings {
    index_t processors   = 8;
    bool print_residuals = false;
    int print_precision  = 3;
    // bool init_x0               = false; // TODO
    double changing_constr_factor       = 0.05;
    index_t max_update_count            = 5;
    bool detailed_timings               = false;
    bool enable_prefetching             = true;
    index_t pcg_max_iter                = 100;
    real_t pcg_tolerance                = std::numeric_limits<real_t>::epsilon() / 10;
    bool pcg_print_resid                = false;
    SolveMethod solve_method            = SolveMethod::StairPCG;
    double pcr_max_update_fraction      = 0.6;
    double cr_max_update_fraction       = 0.9;
    index_t parallel_solve_cr_threshold = 10;
    uint32_t spin_count                 = std::numeric_limits<uint32_t>::max();
    WarmStartingStrategy strategy       = WarmStartingStrategy::Copy;
};

struct CyqloneBackendStats {
    index_t num_updates  = 0;
    index_t rank_updates = 0;
    index_t num_factor   = 0;
};

} // namespace cyqlone::qpalm

namespace CYQLONE_NS(cyqlone::qpalm) {

using guanaqo::StorageOrder;

template <index_t VL, StorageOrder DefaultOrder = StorageOrder::ColMajor>
struct CyqloneBackend;

namespace detail {

template <index_t VL, StorageOrder DefaultOrder>
struct backend_stats_type<CyqloneBackend<VL, DefaultOrder>> {
    using type = CyqloneBackendStats;
};

} // namespace detail

template <index_t VL, StorageOrder DefaultOrder = StorageOrder::ColMajor>
struct unique_CyqloneBackend : std::unique_ptr<CyqloneBackend<VL, DefaultOrder>> {
    unique_CyqloneBackend()                                             = default;
    unique_CyqloneBackend(unique_CyqloneBackend &&) noexcept            = default;
    unique_CyqloneBackend &operator=(unique_CyqloneBackend &&) noexcept = default;
    ~unique_CyqloneBackend();
    unique_CyqloneBackend(std::unique_ptr<CyqloneBackend<VL, DefaultOrder>> &&o) noexcept
        : std::unique_ptr<CyqloneBackend<VL, DefaultOrder>>{std::move(o)} {}
};

template <index_t VL, StorageOrder DefaultOrder>
struct detail::backend_type<unique_CyqloneBackend<VL, DefaultOrder>> {
    using type = CyqloneBackend<VL, DefaultOrder>;
};

template <index_t VL, StorageOrder DefaultOrder = StorageOrder::ColMajor>
unique_CyqloneBackend<VL, DefaultOrder>
make_qpalm_cyqlone_backend(const CyqloneStorage<real_t> &ocp, CyqloneData data,
                           const CyqloneBackendSettings &settings);

template <index_t VL, StorageOrder DefaultOrder = StorageOrder::ColMajor>
void update_qpalm_cyqlone_backend(CyqloneBackend<VL, DefaultOrder> &backend,
                                  const CyqloneStorage<real_t> &ocp);

template <index_t VL, StorageOrder DefaultOrder = StorageOrder::ColMajor>
void update_qpalm_cyqlone_backend(CyqloneBackend<VL, DefaultOrder> &backend,
                                  const LinearOCPStorage &ocp);

} // namespace CYQLONE_NS(cyqlone::qpalm)
