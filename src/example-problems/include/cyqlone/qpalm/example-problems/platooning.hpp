#pragma once

#include <cyqlone/config.hpp>
#include <cyqlone/example-problems/export.h>
#include <cyqlone/ocp.hpp>

#include <vector>

namespace CYQLONE_NAMESPACE::qpalm::problems {

struct PlatooningParams {
    real_t friction   = 0.1;
    real_t F_max      = 20;
    real_t v_max      = 1.6;
    real_t dist_min   = 5;
    real_t dist_init  = 2 * dist_min;
    real_t p_target   = 100;
    index_t N_horiz   = 512;
    real_t T_horiz    = 30;
    real_t scale_cost = 1e-3;
    std::vector<real_t> masses{100, 150, 130, 70, 180};
};

struct PlatooningProblem {
    LinearOCPStorage ocp;
    std::vector<real_t> ref;
};

PlatooningProblem CYQLONE_EXAMPLE_PROBLEMS_EXPORT platooning(PlatooningParams p);

} // namespace CYQLONE_NAMESPACE::qpalm::problems
