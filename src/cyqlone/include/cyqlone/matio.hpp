#pragma once

/// @file
/// Functions for exporting and loading matrices and OCP data to and from .mat files
/// @ingroup topic-util-matio

#include <cyqlone/config.hpp>
#include <cyqlone/ocp.hpp>
#include <cyqlone/sparse.hpp>
#include <batmat/matrix/view.hpp>
#include <filesystem>
#include <memory>
#include <vector>

// from <matio.h>
struct _mat_t; // NOLINT(*-reserved-identifier)

namespace cyqlone {

/// @addtogroup topic-util-matio
/// @{

/// Incomplete matio struct type.
using mat_t = ::_mat_t;
/// Owning handle to a matio file. The file will be closed when the handle goes out of scope.
using MatFilePtr = std::unique_ptr<mat_t, int (*)(mat_t *)>;

enum class MatioOpenMode {
    Read,
    Write,
};

/// Opens a .mat file for reading or writing.
/// @throw runtime_error if the file cannot be opened.
MatFilePtr open_mat(const std::filesystem::path &filename,
                    MatioOpenMode mode = MatioOpenMode::Read);
/// Create and open a new .mat file for writing.
/// @throw runtime_error if the file cannot be opened.
MatFilePtr create_mat(const std::filesystem::path &filename);
/// Add a value to an open .mat file.
void add_to_mat(mat_t *mat, const std::string &varname, float value);
/// Add a value to an open .mat file.
void add_to_mat(mat_t *mat, const std::string &varname, double value);
/// Add a value to an open .mat file.
void add_to_mat(mat_t *mat, const std::string &varname, short value);
/// Add a value to an open .mat file.
void add_to_mat(mat_t *mat, const std::string &varname, int value);
/// Add a value to an open .mat file.
void add_to_mat(mat_t *mat, const std::string &varname, long value);
/// Add a value to an open .mat file.
void add_to_mat(mat_t *mat, const std::string &varname, long long value);
/// Add a value to an open .mat file.
void add_to_mat(mat_t *mat, const std::string &varname, unsigned short value);
/// Add a value to an open .mat file.
void add_to_mat(mat_t *mat, const std::string &varname, unsigned int value);
/// Add a value to an open .mat file.
void add_to_mat(mat_t *mat, const std::string &varname, unsigned long value);
/// Add a value to an open .mat file.
void add_to_mat(mat_t *mat, const std::string &varname, unsigned long long value);
/// Add a matrix to an open .mat file.
void add_to_mat(mat_t *mat, const std::string &varname,
                guanaqo::MatrixView<const double, index_t> data);
/// Add a matrix to an open .mat file.
void add_to_mat(mat_t *mat, const std::string &varname,
                guanaqo::MatrixView<const float, index_t> data);
/// Add a batch of matrices to an open .mat file.
void add_to_mat(mat_t *mat, const std::string &varname,
                batmat::matrix::View<const double, index_t> data);
/// Add a batch of matrices to an open .mat file.
void add_to_mat(mat_t *mat, const std::string &varname,
                batmat::matrix::View<const float, index_t> data);
/// Add a sparse matrix to an open .mat file.
/// On the Python side, you can use the following code to load the sparse matrix:
/// ```python
/// from scipy.io import loadmat
/// from scipy.sparse import coo_matrix
/// mat = loadmat("file.mat")
/// data = mat["varname"][0, 0]
/// shape = data["num_rows"].item(), data["num_cols"].item()
/// rows = data["row_indices"].flatten()
/// cols = data["col_indices"].flatten()
/// values = data["values"].flatten()
/// matrix = coo_matrix((values, (rows, cols)), shape=shape)
/// ```
void add_to_mat(mat_t *mat, const std::string &varname, const SparseMatrix &matrix);
/// Add the data from a LinearOCPStorage to an open .mat file.
void add_to_mat(mat_t *mat, const LinearOCPStorage &ocp);
/// Load a LinearOCPStorage from a .mat file.
void read_from_mat(mat_t *mat, LinearOCPStorage &ocp);
/// Load a vector from an open .mat file.
void read_from_mat(mat_t *mat, const std::string &varname, std::vector<float> &data);
/// Load a vector from an open .mat file.
void read_from_mat(mat_t *mat, const std::string &varname, std::vector<double> &data);
/// Load a vector from an open .mat file.
void read_from_mat(mat_t *mat, const std::string &varname, std::span<float> data);
/// Load a vector from an open .mat file.
void read_from_mat(mat_t *mat, const std::string &varname, std::span<double> data);
/// Dump the data from a LinearOCPStorage to a new .mat file.
void ocp_dump_mat(const std::filesystem::path &filename, const LinearOCPStorage &ocp);

/// @}

} // namespace cyqlone
