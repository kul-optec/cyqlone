#pragma once

#include <cyqlone/config.hpp>
#include <cyqlone/ocp.hpp>
#include <random>

namespace cyqlone {

/// Generates a random linear OCP with the given dimensions and random seed.
/// @ingroup topic-ocp-formulations
inline LinearOCPStorage generate_random_ocp(OCPDim dim, uint_fast32_t seed = 0) {
    LinearOCPStorage ocp{dim};
    auto [N, nx, nu, ny, ny_N] = ocp.dim;

    std::mt19937 rng{seed};
    std::uniform_real_distribution<real_t> uni{-1, 1};
    std::bernoulli_distribution bernoulli{0.5};

    // Dynamics and constraint matrices
    for (index_t i = 0; i < N; ++i) {
        auto Ai = ocp.A(i), Bi = ocp.B(i), Ci = ocp.C(i), Di = ocp.D(i);
        auto Qi = ocp.Q(i), Ri = ocp.R(i), Si = ocp.S(i), Hi = ocp.H(i);
        Ai.generate([&] { return uni(rng) * 0.9; });
        Bi.generate([&] { return uni(rng); });
        Ci.generate([&] { return uni(rng); });
        Di.generate([&] { return uni(rng); });
        Qi.generate([&] { return uni(rng); });
        Ri.generate([&] { return uni(rng); });
        Si.generate([&] { return uni(rng); });
        for (index_t j = 0; j < nx; ++j)
            Qi(j, j) += 2 * static_cast<real_t>(nx + nu);
        for (index_t j = 0; j < nu; ++j)
            Ri(j, j) += 2 * static_cast<real_t>(nx + nu);
        for (index_t c = 0; c < nx + nu; ++c)
            for (index_t r = c + 1; r < nx + nu; ++r)
                Hi(c, r) = Hi(r, c);
    }
    auto Ci = ocp.C(N), Qi = ocp.Q(N);
    Ci.generate([&] { return uni(rng); });
    Qi.generate([&] { return uni(rng); });
    for (index_t j = 0; j < nx; ++j)
        Qi(j, j) += 2 * static_cast<real_t>(nx);
    for (index_t c = 0; c < nx; ++c)
        for (index_t r = c + 1; r < nx; ++r)
            Qi(c, r) = Qi(r, c);
    return ocp;
}

} // namespace cyqlone
