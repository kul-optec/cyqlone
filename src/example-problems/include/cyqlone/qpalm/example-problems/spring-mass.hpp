#pragma once

#include <cyqlone/config.hpp>
#include <cyqlone/example-problems/export.h>
#include <cyqlone/ocp.hpp>

#include <vector>

namespace cyqlone::qpalm::problems {

struct SpringMassParams {
    real_t friction = 0;      ///< friction coefficient
    real_t k_spring = 1;      ///< spring constant between masses
    real_t F_max    = 0.5;    ///< maximum actuator force
    real_t p_max    = 4;      ///< maximum displacement of each mass
    real_t p_min    = -p_max; ///< minimum displacement of each mass
    real_t p_min_f  = p_min;  ///< minimum displacement of each mass for the final stage
    real_t p_max_f  = p_max;  ///< maximum displacement of each mass for the final stage
    real_t v_max    = 0;      ///< maximum velocity of each mass (zero = no limit)
    real_t v_max_f  = v_max;  ///< maximum velocity of each mass for the final stage
    real_t width    = 1;      ///< width of the setup (distance between the walls)
    index_t N_horiz = 32;     ///< number of discretization steps
    real_t T_horiz  = 15;     ///< time horizon
    real_t q_vel    = 1;      ///< scaling factor for cost terms for the velocity
    real_t q_pos    = 1;      ///< scaling factor for cost terms for the position
    real_t q_vel_f  = q_vel;  ///< scaling factor for terminal cost term for the velocity
    real_t q_pos_f  = q_pos;  ///< scaling factor for terminal cost term for the position
    real_t r_act    = 1;      ///< scaling factor for cost terms for the actuators
    std::vector<real_t> masses{1, 1, 1, 1, 1, 1};
    index_t n_actuators = static_cast<index_t>(masses.size()) - 1; ///< number of actuators
    enum ActuatorPlacement {
        IndividualActuators,
        RandomActuators,
        RandomPairsOfActuators,
        WangBoydActuators,
    } actuator_placement = IndividualActuators;
    uint64_t seed        = 0; ///< random seed for actuator placement

    static SpringMassParams wang_boyd_2008(index_t n_masses, index_t N_horiz = 30,
                                           uint64_t seed = 0) {
        // Notes: Setting the width to zero does not match Fig. 1 in Wang & Boyd (2008), but it
        //        does match the values for q, qf and r in Wang & Boyd (2010) Sec. V.A.
        //        We use Qf = I: it is left unspecified in Wang & Boyd (2008), and determined
        //        heuristically in Wang & Boyd (2010).
        return {
            .friction           = 0,
            .k_spring           = 1,
            .F_max              = 0.5,
            .p_max              = 4,
            .width              = 0,
            .N_horiz            = N_horiz,
            .T_horiz            = 15, // Ts = 0.5s for horizon N = 30
            .q_vel              = 1,  // Q = I
            .q_pos              = 1,
            .q_vel_f            = 1, // Qf = I
            .q_pos_f            = 1,
            .r_act              = 1, // R = I
            .masses             = std::vector<real_t>(n_masses, 1),
            .n_actuators        = (n_masses + 1) / 2,
            .actuator_placement = WangBoydActuators,
            .seed               = seed,
        };
    }

    static SpringMassParams wang_boyd_2008_width(index_t n_masses, index_t N_horiz = 30,
                                                 uint64_t seed                     = 0,
                                                 double steady_state_spring_length = 0.1) {
        auto params  = wang_boyd_2008(n_masses, N_horiz, seed);
        params.width = steady_state_spring_length * static_cast<real_t>(n_masses + 1);
        return params;
    }

    static SpringMassParams domahidi_2012(index_t n_masses, index_t N_horiz, uint64_t seed = 0) {
        const real_t Ts = 0.5;
        return {
            .friction           = 0,
            .k_spring           = 1,
            .F_max              = 0.5,
            .p_max              = 4,
            .v_max              = 4,
            .width              = 0, // q = 0, r = 0
            .N_horiz            = N_horiz,
            .T_horiz            = Ts * static_cast<real_t>(N_horiz),
            .q_vel              = 3, // Q = 3I
            .q_pos              = 3,
            .q_vel_f            = 3, // Qf = Q
            .q_pos_f            = 3,
            .r_act              = 1, // R = I
            .masses             = std::vector<real_t>(n_masses, 1),
            .n_actuators        = n_masses - 1,
            .actuator_placement = IndividualActuators,
            .seed               = seed,
        };
    }

    static SpringMassParams active_state_constr(index_t n_masses = 18, index_t N_horiz = 256,
                                                uint64_t seed = 0) {
        std::vector<real_t> masses(n_masses, 1.0);
        if (n_masses >= 3) {
            // Lighter masses for stiffer dynamics
            masses[n_masses / 2 - 1] = 0.2;
            masses[n_masses / 2 + 0] = 0.5;
            masses[n_masses / 2 + 1] = 0.2;
        }
        return {
            .friction           = 0,
            .k_spring           = 1,
            .F_max              = 0.5,
            .p_max              = 0.01,
            .p_min              = -1,
            .p_min_f            = -1e-3,
            .p_max_f            = +1e-3,
            .v_max              = 2,
            .v_max_f            = 1e-3,
            .width              = 0.1 * static_cast<real_t>(n_masses + 1),
            .N_horiz            = N_horiz,
            .T_horiz            = 15,
            .q_vel              = 1,
            .q_pos              = 1,
            .q_vel_f            = 1e2,
            .q_pos_f            = 1e1,
            .r_act              = 1,
            .masses             = std::move(masses),
            .n_actuators        = n_masses / 2,
            .actuator_placement = RandomActuators,
            .seed               = seed,
        };
    }
};

struct SpringMassProblem {
    LinearOCPStorage ocp;
    std::vector<real_t> ref;
};

} // namespace cyqlone::qpalm::problems

namespace CYQLONE_NS(cyqlone::qpalm::problems) {

SpringMassProblem CYQLONE_EXAMPLE_PROBLEMS_EXPORT spring_mass(SpringMassParams p);

} // namespace CYQLONE_NS(cyqlone::qpalm::problems)
