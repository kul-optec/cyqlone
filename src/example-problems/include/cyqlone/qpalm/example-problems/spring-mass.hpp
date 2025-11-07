#pragma once

#include <cyqlone/config.hpp>
#include <cyqlone/example-problems/export.h>
#include <cyqlone/ocp.hpp>

#include <vector>

namespace CYQLONE_NAMESPACE::qpalm::problems {

struct SpringMassParams {
    real_t friction = 0;   ///< friction coefficient
    real_t k_spring = 1;   ///< spring constant between masses
    real_t F_max    = 0.5; ///< maximum actuator force
    real_t p_max    = 4;   ///< maximum displacement of each mass
    real_t width    = 1;   ///< width of the setup (distance between the walls)
    index_t N_horiz = 32;  ///< number of discretization steps
    real_t T_horiz  = 15;  ///< time horizon
    real_t q_vel    = 1;   ///< scaling factor for cost terms for the velocity
    real_t q_pos    = 1;   ///< scaling factor for cost terms for the position
    real_t r_act    = 1;   ///< scaling factor for cost terms for the actuators
    std::vector<real_t> masses{1, 1, 1, 1, 1, 1};
    index_t n_actuators = static_cast<index_t>(masses.size()) - 1; ///< number of actuators
    enum ActuatorPlacement {
        IndividualActuators,
        RandomActuators,
        RandomPairsOfActuators,
        WangBoydActuators,
    } actuator_placement = IndividualActuators;
    uint64_t seed        = 0; ///< random seed for actuator placement
};

struct SpringMassProblem {
    LinearOCPStorage ocp;
    std::vector<real_t> ref;
};

SpringMassProblem CYQLONE_EXAMPLE_PROBLEMS_EXPORT spring_mass(SpringMassParams p);

} // namespace CYQLONE_NAMESPACE::qpalm::problems
