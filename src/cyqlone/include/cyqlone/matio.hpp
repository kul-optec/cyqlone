#pragma once

#include <cyqlone/ocp.hpp>
#include <batmat/matrix/view.hpp>
#include <filesystem>
#include <memory>
#include <vector>

// from <matio.h>
struct _mat_t; // NOLINT(*-reserved-identifier)

namespace cyqlone {

using mat_t      = ::_mat_t;
using MatFilePtr = std::unique_ptr<mat_t, int (*)(mat_t *)>;

enum class MatioOpenMode {
    Read,
    Write,
};

MatFilePtr open_mat(const std::filesystem::path &filename,
                    MatioOpenMode mode = MatioOpenMode::Read);
MatFilePtr create_mat(const std::filesystem::path &filename);
void add_to_mat(mat_t *mat, const std::string &varname,
                guanaqo::MatrixView<const real_t, index_t> data);
void add_to_mat(mat_t *mat, const std::string &varname,
                batmat::matrix::View<const real_t, index_t> data);
void add_to_mat(mat_t *mat, const LinearOCPStorage &ocp);
void ocp_dump_mat(const std::filesystem::path &filename, const LinearOCPStorage &ocp);
void read_from_mat(mat_t *mat, LinearOCPStorage &ocp);
void read_from_mat(mat_t *mat, const std::string &filename, std::vector<real_t> &data);
void read_from_mat(mat_t *mat, const std::string &filename, std::span<real_t> data);

} // namespace cyqlone
