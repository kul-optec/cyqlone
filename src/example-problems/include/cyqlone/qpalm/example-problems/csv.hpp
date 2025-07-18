#pragma once

#include <cyqlone/config.hpp>
#include <cyqlone/example-problems/export.h>
#include <cyqlone/ocp.hpp>

#include <filesystem>
#include <vector>

namespace cyqlone::qpalm::problems {

namespace fs = std::filesystem;

struct CSVProblem {
    LinearOCPStorage ocp;
    std::vector<real_t> rhs_eq;
    std::vector<real_t> rhs_ineq_lb, rhs_ineq_ub;
    std::vector<real_t> qr;
};

CSVProblem CYQLONE_EXAMPLE_PROBLEMS_EXPORT load_from_csv(const fs::path &folder,
                                                         const std::string &name);

} // namespace cyqlone::qpalm::problems
