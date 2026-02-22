#pragma once

/// @file
/// Sparse matrix utilities.
/// @ingroup topic-utilities

#include <cyqlone/config.hpp>
#include <batmat/assume.hpp>
#include <guanaqo/linalg/sparsity.hpp>

#include <ranges>
#include <vector>

namespace cyqlone {

/// @addtogroup topic-utilities
/// @{

using guanaqo::linalg::sparsity::SparseCOO;
using guanaqo::linalg::sparsity::Symmetry;

/// A sparse matrix in COO format.
struct SparseMatrix {
    const std::vector<index_t> row_indices, col_indices;
    const std::vector<real_t> values;
    const SparseCOO<index_t> sparsity;

    [[nodiscard]] auto iter_coo() const {
        return std::views::zip(row_indices, col_indices, values);
    }
};

/// A builder for constructing a SparseMatrix incrementally.
struct SparseMatrixBuilder {
    index_t rows = -1, cols = -1;
    Symmetry symmetry = Symmetry::Unsymmetric;
    std::vector<index_t> row_indices{}, col_indices{};
    std::vector<real_t> values{};

    void add(index_t row, index_t col, real_t value) {
        BATMAT_ASSUME(row >= 0);
        BATMAT_ASSUME(col >= 0);
        if (rows >= 0)
            BATMAT_ASSUME(row < rows);
        if (cols >= 0)
            BATMAT_ASSUME(col < cols);
        row_indices.push_back(row);
        col_indices.push_back(col);
        values.push_back(value);
    }

    [[nodiscard]] SparseMatrix build() const & {
        return {.row_indices = row_indices,
                .col_indices = col_indices,
                .values      = values,
                .sparsity    = SparseCOO<index_t>{.rows        = rows,
                                                  .cols        = cols,
                                                  .symmetry    = symmetry,
                                                  .row_indices = std::span{row_indices},
                                                  .col_indices = std::span{col_indices}}};
    }

    [[nodiscard]] SparseMatrix build() && {
        return {.row_indices = std::move(row_indices),
                .col_indices = std::move(col_indices),
                .values      = std::move(values),
                .sparsity    = SparseCOO<index_t>{.rows        = rows,
                                                  .cols        = cols,
                                                  .symmetry    = symmetry,
                                                  .row_indices = std::span{row_indices},
                                                  .col_indices = std::span{col_indices}}};
    }
};

/// @}

} // namespace cyqlone
