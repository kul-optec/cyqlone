#pragma once

#include <cyqlone/config.hpp>
#include <cyqlone/example-problems/export.h>
#include <cyqlone/ocp.hpp>

#include <filesystem>

namespace CYQLONE_NS(cyqlone::qpalm::problems) {

namespace fs = std::filesystem;

LinearOCPStorage CYQLONE_EXAMPLE_PROBLEMS_EXPORT load_from_csv(const fs::path &folder,
                                                               const std::string &name);

} // namespace CYQLONE_NS(cyqlone::qpalm::problems)
