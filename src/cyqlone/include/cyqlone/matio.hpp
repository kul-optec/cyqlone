#pragma once

#include <cyqlone/ocp.hpp>
#include <filesystem>

namespace cyqlone {

void ocp_dump_mat(const LinearOCPStorage &ocp, const std::filesystem::path &filename);

} // namespace cyqlone
