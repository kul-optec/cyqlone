#pragma once

#include <cyqlone/config.hpp>
#include <cyqlone/matio.hpp>
#include <cyqlone/ocp.hpp>
#include <cyqlone/sparse.hpp>
#include <batmat/assume.hpp>
#include <batmat/matrix/matrix.hpp>

using cyqlone::index_t;
using cyqlone::real_t;

#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>
#include <nanobind/stl/chrono.h>
#include <nanobind/stl/filesystem.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/vector.h>
namespace nb = nanobind;
using namespace nb::literals;

template <class T = const real_t>
using np_tensor3 = nb::ndarray<T, nb::ndim<3>, nb::f_contig, nb::device::cpu>;
template <class T = const real_t>
using np_matrix = nb::ndarray<T, nb::ndim<2>, nb::f_contig, nb::device::cpu>;
template <class T = const real_t>
using np_vector = nb::ndarray<T, nb::ndim<1>, nb::any_contig, nb::device::cpu>;
template <index_t VL, class T = const real_t>
using np_batched_view = nb::ndarray<T, nb::shape<VL, -1, -1, -1>, nb::device::cpu>;

namespace cyqlone {

struct PythonOCP {
    LinearOCPStorage ocp;

    PythonOCP() = default;
    PythonOCP(LinearOCPStorage ocp) : ocp{std::move(ocp)} {}
    PythonOCP(np_tensor3<> AB, np_tensor3<> CD, np_matrix<> CN, np_tensor3<> QRS, np_matrix<> QN,
              np_vector<> rhs_eq, np_vector<> rhs_lb, np_vector<> rhs_ub, np_vector<> qr)
        : ocp{.dim = {
                  .N_horiz = static_cast<index_t>(AB.shape(0)),
                  .nx      = static_cast<index_t>(AB.shape(1)),
                  .nu      = static_cast<index_t>(AB.shape(2) - AB.shape(1)),
                  .ny      = static_cast<index_t>(CD.shape(1)),
                  .ny_N    = static_cast<index_t>(CN.shape(0)),
              }} {
        auto [N, nx, nu, ny, ny_N] = ocp.dim;
        index_t num_var            = N * (nx + nu) + nx;
        index_t num_eq_constr      = (N + 1) * nx;
        index_t num_ineq_constr    = N * ny + ny_N;
        if (static_cast<index_t>(CD.shape(0)) != N)
            throw std::invalid_argument("Invalid horizon length CD");
        if (static_cast<index_t>(QRS.shape(0)) != N)
            throw std::invalid_argument("Invalid horizon length QRS");
        if (static_cast<index_t>(CD.shape(2)) != nx + nu)
            throw std::invalid_argument("Invalid number of columns CD");
        if (static_cast<index_t>(QRS.shape(1)) != nx + nu)
            throw std::invalid_argument("Invalid number of rows QRS");
        if (static_cast<index_t>(QRS.shape(2)) != nx + nu)
            throw std::invalid_argument("Invalid number of columns QRS");
        if (static_cast<index_t>(CN.shape(1)) != nx)
            throw std::invalid_argument("Invalid number of columns CN");
        if (static_cast<index_t>(QN.shape(0)) != nx)
            throw std::invalid_argument("Invalid number of rows QN");
        if (static_cast<index_t>(QN.shape(1)) != nx)
            throw std::invalid_argument("Invalid number of cols QN");
        if (static_cast<index_t>(rhs_eq.shape(0)) != num_eq_constr)
            throw std::invalid_argument("Invalid size rhs_eq");
        if (static_cast<index_t>(rhs_lb.shape(0)) != num_ineq_constr)
            throw std::invalid_argument("Invalid size rhs_lb");
        if (static_cast<index_t>(rhs_ub.shape(0)) != num_ineq_constr)
            throw std::invalid_argument("Invalid size rhs_ub");
        if (static_cast<index_t>(qr.shape(0)) != num_var)
            throw std::invalid_argument("Invalid size qr");

        auto ABv = AB.view(), CDv = CD.view(), QRSv = QRS.view();
        for (index_t i = 0; i < N; ++i) {
            for (index_t c = 0; c < nx + nu; ++c)
                for (index_t r = 0; r < nx; ++r)
                    ocp.AB(i)(r, c) = ABv(i, r, c);
            for (index_t c = 0; c < nx + nu; ++c)
                for (index_t r = 0; r < ny; ++r)
                    ocp.CD(i)(r, c) = CDv(i, r, c);
            for (index_t c = 0; c < nx + nu; ++c)
                for (index_t r = 0; r < nx + nu; ++r)
                    ocp.H(i)(r, c) = QRSv(i, r, c);
        }
        auto CNv = CN.view(), QNv = QN.view();
        for (index_t c = 0; c < nx; ++c)
            for (index_t r = 0; r < ny_N; ++r)
                ocp.CD(N)(r, c) = CNv(r, c);
        for (index_t c = 0; c < nx; ++c)
            for (index_t r = 0; r < nx; ++r)
                ocp.H(N)(r, c) = QNv(r, c);
        std::copy_n(rhs_eq.data(), rhs_eq.size(), ocp.b().data);
        std::copy_n(rhs_lb.data(), rhs_lb.size(), ocp.b_min().data);
        std::copy_n(rhs_ub.data(), rhs_ub.size(), ocp.b_max().data);
        std::copy_n(qr.data(), qr.size(), ocp.qr().data);
    }

    void dump_mat(const std::filesystem::path &filename) const {
        auto matfp = cyqlone::create_mat(filename);
        add_to_mat(matfp.get(), ocp);
    }

    void load_mat(const std::filesystem::path &filename) {
        auto matfp = cyqlone::open_mat(filename);
        read_from_mat(matfp.get(), ocp);
    }
};

template <class T, class I, guanaqo::StorageOrder O>
auto np_view_vec(guanaqo::MatrixView<T, I, std::integral_constant<I, 1>, O> matrix) {
    using np_array = nb::ndarray<nb::numpy, T, nb::ndim<1>, nb::any_contig, nb::device::cpu>;
    return np_array{
        matrix.data,
        {static_cast<size_t>(matrix.is_column_major ? matrix.rows : matrix.cols)},
        {},
        {1},
    };
}

template <class T, size_t E>
auto np_view_vec(std::span<T, E> vector) {
    using np_array = nb::ndarray<nb::numpy, T, nb::ndim<1>, nb::any_contig, nb::device::cpu>;
    return np_array{
        vector.data(),
        {vector.size()},
        {},
        {1},
    };
}

template <class T>
auto np_view_vec(std::vector<T> &vector) {
    return np_view_vec(std::span<T>{vector});
}

template <class T>
auto np_view_vec(const std::vector<T> &vector) {
    return np_view_vec(std::span<const T>{vector});
}

template <class T>
auto np_view_vec(std::vector<T> &&vector) = delete;

template <class T, class I, guanaqo::StorageOrder O>
auto np_view(guanaqo::MatrixView<T, I, std::integral_constant<I, 1>, O> matrix) {
    using order    = std::conditional_t<matrix.is_column_major, nb::f_contig, nb::c_contig>;
    using np_array = nb::ndarray<nb::numpy, T, nb::ndim<2>, order, nb::device::cpu>;
    return np_array{
        matrix.data,
        {static_cast<size_t>(matrix.rows), static_cast<size_t>(matrix.cols)},
        {},
        {static_cast<int64_t>(matrix.row_stride()), static_cast<int64_t>(matrix.col_stride())},
    };
}

template <class T, class I, class D, class L, guanaqo::StorageOrder O>
auto np_view(batmat::matrix::View<T, I, std::integral_constant<I, 1>, D, L, O> matrix) {
    using np_array = nb::ndarray<nb::numpy, T, nb::ndim<3>, nb::device::cpu>;
    return np_array{
        matrix.data,
        {
            static_cast<size_t>(matrix.depth()),
            static_cast<size_t>(matrix.rows()),
            static_cast<size_t>(matrix.cols()),
        },
        {},
        {
            static_cast<int64_t>(matrix.layer_stride()),
            static_cast<int64_t>(matrix.is_column_major ? 1 : matrix.outer_stride()),
            static_cast<int64_t>(matrix.is_column_major ? matrix.outer_stride() : 1),
        },
    };
}

template <class T, class I, index_t VL, class D, guanaqo::StorageOrder O>
auto np_copy(batmat::matrix::Matrix<T, I, std::integral_constant<I, VL>, D, O> matrix) {
    using enum guanaqo::StorageOrder;
    static_assert(O == ColMajor,
                  "Batched views are only supported for column-major storage"); // TODO
    BATMAT_ASSERT(matrix.depth() % VL == 0 && "Batched views require depth to be multiple of VL");
    using order    = std::conditional_t<O == ColMajor, nb::f_contig, nb::c_contig>;
    using np_array = nb::ndarray<nb::numpy, T, nb::shape<VL, -1, -1, -1>, order, nb::device::cpu>;
    auto *m        = new decltype(matrix){std::move(matrix)};
    auto deleter   = [](void *p) noexcept { delete static_cast<decltype(matrix) *>(p); };
    BATMAT_ASSERT(m->outer_stride() == m->rows());
    BATMAT_ASSERT(m->view().layer_stride() == m->outer_stride() * m->cols());
    np_array r{
        m->data(),
        {
            static_cast<size_t>(VL),
            static_cast<size_t>(m->rows()),
            static_cast<size_t>(m->cols()),
            static_cast<size_t>((m->depth() + VL - 1) / VL),
        },
        nb::capsule{m, deleter},
    };
    return r;
}

template <class T>
auto np_copy(std::vector<T> matrix) {
    using order    = nb::f_contig;
    using np_array = nb::ndarray<nb::numpy, T, nb::shape<-1>, order, nb::device::cpu>;
    auto *m        = new decltype(matrix){std::move(matrix)};
    auto deleter   = [](void *p) noexcept { delete static_cast<decltype(matrix) *>(p); };
    return np_array{m->data(), {m->size()}, nb::capsule{m, deleter}};
}

template <class... Args>
auto view(const nb::ndarray<Args...> &array)
    requires(array.Order == 'F' || array.Order == 'C')
{
    using I = index_t;
    using T = typename nb::ndarray<Args...>::Scalar;
    using enum guanaqo::StorageOrder;
    static constexpr auto O = array.Order == 'F' ? ColMajor : RowMajor;
    return guanaqo::MatrixView<T, I, std::integral_constant<I, 1>, O>{{
        .data         = array.data(),
        .rows         = static_cast<I>(array.shape(0)),
        .cols         = static_cast<I>(array.shape(1)),
        .outer_stride = static_cast<I>(array.stride(array.Order == 'F' ? 1 : 0)),
    }};
}

template <class T>
auto view(const np_vector<T> &array) {
    using I = index_t;
    using enum guanaqo::StorageOrder;
    return guanaqo::MatrixView<T, I, std::integral_constant<I, 1>, ColMajor>{{
        .data = array.data(),
        .rows = static_cast<I>(array.shape(0)),
        .cols = 1,
    }};
}

template <class T>
auto as_span(const np_vector<T> &array) {
    return std::span{array.data(), array.size()};
}

} // namespace cyqlone

NAMESPACE_BEGIN(NB_NAMESPACE)
NAMESPACE_BEGIN(detail)

template <>
struct type_caster<cyqlone::SparseMatrix> {
  public:
    using SparseMatrix = cyqlone::SparseMatrix;
    NB_TYPE_CASTER(SparseMatrix, const_name("scipy.sparse.coo_array"));

    bool from_python(handle, uint8_t, cleanup_list *) noexcept { return false; }

    template <typename T>
    static handle from_cpp(T *value, rv_policy policy, cleanup_list *cleanup) {
        if (!value)
            return none().release();
        return from_cpp(*value, policy, cleanup);
    }

    template <class T>
    static handle from_cpp(T &&src, rv_policy, cleanup_list *) {
        object coo_array = module_::import_("scipy.sparse").attr("coo_array");
        return coo_array(make_tuple(cyqlone::np_copy(std::forward<T>(src).values),
                                    make_tuple(cyqlone::np_copy(std::forward<T>(src).row_indices),
                                               cyqlone::np_copy(std::forward<T>(src).col_indices))),
                         "shape"_a = make_tuple(src.sparsity.rows, src.sparsity.cols))
            .release();
    }

    operator SparseMatrix() noexcept { return SparseMatrix{}; }
};

NAMESPACE_END(detail)
NAMESPACE_END(NB_NAMESPACE)
