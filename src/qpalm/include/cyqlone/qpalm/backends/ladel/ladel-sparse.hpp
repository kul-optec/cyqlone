#pragma once

#include <ladel_types.h> // ladel_sparse_matrix

#include <memory> // unique_ptr
#include <span>

#include <cyqlone/config.hpp>
#include <guanaqo/linalg/sparsity.hpp>

namespace cyqlone::qpalm {

using sparse_csc_t = guanaqo::linalg::sparsity::SparseCSC<ladel_int, ladel_int>;

namespace alloc {
struct ladel_sparse_matrix_deleter {
    void operator()(ladel_sparse_matrix *) const;
};
struct ladel_work_deleter {
    void operator()(ladel_work *) const;
};
struct ladel_factor_deleter {
    void operator()(ladel_factor *) const;
};
struct ladel_symbolics_deleter {
    void operator()(ladel_symbolics *) const;
};
} // namespace alloc

/// Smart pointer that automatically cleans up an owning ladel_sparse_matrix
/// object.
using ladel_sparse_matrix_ptr = std::unique_ptr< //
    ladel_sparse_matrix, alloc::ladel_sparse_matrix_deleter>;
/// Smart pointer that automatically cleans up an owning ladel_work object.
using ladel_work_ptr = std::unique_ptr< //
    ladel_work, alloc::ladel_work_deleter>;
/// Smart pointer that automatically cleans up an owning ladel_factor object.
using ladel_factor_ptr = std::unique_ptr< //
    ladel_factor, alloc::ladel_factor_deleter>;
/// Smart pointer that automatically cleans up an owning ladel_symbolics object.
using ladel_symbolics_ptr = std::unique_ptr< //
    ladel_symbolics, alloc::ladel_symbolics_deleter>;

/// Convert a sparsity view to a LADEL sparse matrix, without creating a copy.
/// @note     The returned object contains pointers to the data of @p mat, so do
///           not reallocate or deallocate using the @c ladel_sparse_free
///           and similar functions. Modifications of the returned LADEL matrix
///           will affect the original matrix, so make sure that the
///           representation remains consistent.
/// @warning  Immutable pointers will be `const_cast`’ed to mutable pointers.
///           It is the responsibility of the user to treat the returned matrix
///           as immutable.
ladel_sparse_matrix sparse_to_ladel_view(sparse_csc_t mat,
                                         std::span<const ladel_double> values = {});

/// Create an LADEL sparse matrix of the given dimensions.
/// @param  rows        Number of rows.
/// @param  cols        Number of columns.
/// @param  nnz         Number of nonzeros.
/// @param  symmetry    Either @c UNSYMMETRIC, @c UPPER or @c LOWER.
/// @param  values      Whether to allocate the array of nonzero values.
/// @param  nonzeros    Whether to allocate the array of nonzero counts.
/// @see ladel_sparse_alloc
ladel_sparse_matrix_ptr ladel_sparse_create(index_t rows, index_t cols, index_t nnz,
                                            ladel_int symmetry, bool values = true,
                                            bool nonzeros = false);

/// Similar to @ref sparse_to_ladel_view, but creates a copy of all data, in
/// such a way that the returned matrix is completely decoupled from @p mat, and
/// such that it can be reallocated and deallocated by the @c ladel_sparse_free
/// and similar functions.
ladel_sparse_matrix_ptr sparse_to_ladel_copy(sparse_csc_t mat,
                                             std::span<const ladel_double> values = {});

/// @see ladel_workspace_allocate
ladel_work_ptr ladel_workspace_create(ladel_int ncol);
/// @see ladel_symbolics_alloc
ladel_symbolics_ptr ladel_symbolics_create(ladel_int ncol);

} // namespace cyqlone::qpalm
