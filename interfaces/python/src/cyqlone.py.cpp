#include <cyqlone/config.hpp>
#include <cyqlone/matio.hpp>
#include <cyqlone/qpalm/backends/ocp-backend-cyqlone.hpp>
#include <cyqlone/qpalm/example-problems/conversion.hpp>
#include <cyqlone/qpalm/example-problems/platooning.hpp>
#include <cyqlone/qpalm/settings.hpp>
#include <cyqlone/qpalm/solver.hpp>
#include <batmat/assume.hpp>
#include <batmat/loop.hpp>
#include <batmat/openmp.h>
#if !BATMAT_WITH_OPENMP
#include <batmat/thread-pool.hpp>
#endif
#include <guanaqo/eigen/span.hpp> // TODO: remove
#include <guanaqo/eigen/view.hpp>
#include <batmat-version.h>
#include <cyqlone-version.h>

#include <stdexcept>

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

#if BATMAT_WITH_OPENMP
#include <omp.h>
#endif
#if GUANAQO_WITH_TRACING
#include <guanaqo/trace.hpp>
#include <nanobind/stl/filesystem.h>
#include <filesystem>
#include <fstream>
#endif

namespace CYQLONE_NAMESPACE {

struct PythonOCP {
    cyqlone::LinearOCPStorage ocp;

    PythonOCP() = default;
    PythonOCP(cyqlone::LinearOCPStorage ocp) : ocp{std::move(ocp)} {}
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

void register_ocp(nb::module_ &m) {
    nb::class_<PythonOCP>(m, "OCP")
        .def(nb::init<>())
        .def(nb::init<np_tensor3<>, np_tensor3<>, np_matrix<>, np_tensor3<>, np_matrix<>,
                      np_vector<>, np_vector<>, np_vector<>, np_vector<>>(),
             "AB"_a, "CD"_a, "CN"_a, "QRS"_a, "QN"_a, "rhs_eq"_a, "rhs_lb"_a, "rhs_ub"_a, "qr"_a)
        .def_prop_rw(
            "x0", [](PythonOCP &self) { return np_view_vec(self.ocp.b(0)); },
            [](PythonOCP &self, np_vector<> x) { self.ocp.b(0) = view(x); })
        .def_prop_rw(
            "rhs_eq", [](PythonOCP &self) { return np_view_vec(self.ocp.b()); },
            [](PythonOCP &self, np_vector<> x) { self.ocp.b() = view(x); })
        .def_prop_rw(
            "rhs_lb", [](PythonOCP &self) { return np_view_vec(self.ocp.b_min()); },
            [](PythonOCP &self, np_vector<> x) { self.ocp.b_min() = view(x); })
        .def_prop_rw(
            "rhs_ub", [](PythonOCP &self) { return np_view_vec(self.ocp.b_max()); },
            [](PythonOCP &self, np_vector<> x) { self.ocp.b_max() = view(x); })
        .def_prop_rw(
            "qr", [](PythonOCP &self) { return np_view_vec(self.ocp.qr()); },
            [](PythonOCP &self, np_vector<> x) { self.ocp.qr() = view(x); })
        .def_prop_ro("dim",
                     [](const PythonOCP &self) {
                         return nb::make_tuple(self.ocp.dim.N_horiz, self.ocp.dim.nx,
                                               self.ocp.dim.nu, self.ocp.dim.ny, self.ocp.dim.ny_N);
                     })
        .def(
            "A", [](PythonOCP &self, index_t i) { return np_view(self.ocp.A(i)); },
            nb::rv_policy::reference_internal, "i"_a)
        .def(
            "B", [](PythonOCP &self, index_t i) { return np_view(self.ocp.B(i)); },
            nb::rv_policy::reference_internal, "i"_a)
        .def(
            "C", [](PythonOCP &self, index_t i) { return np_view(self.ocp.C(i)); },
            nb::rv_policy::reference_internal, "i"_a)
        .def(
            "D", [](PythonOCP &self, index_t i) { return np_view(self.ocp.D(i)); },
            nb::rv_policy::reference_internal, "i"_a)
        .def(
            "Q", [](PythonOCP &self, index_t i) { return np_view(self.ocp.Q(i)); },
            nb::rv_policy::reference_internal, "i"_a)
        .def(
            "R", [](PythonOCP &self, index_t i) { return np_view(self.ocp.R(i)); },
            nb::rv_policy::reference_internal, "i"_a)
        .def(
            "S", [](PythonOCP &self, index_t i) { return np_view(self.ocp.S(i)); },
            nb::rv_policy::reference_internal, "i"_a)
        .def("dump_mat", &PythonOCP::dump_mat)
        .def("load_mat", &PythonOCP::load_mat);
    using cyqlone::qpalm::LinearOCPSparseQP;
    nb::class_<LinearOCPSparseQP>(m, "LinearOCPSparseQP")
        .def("__init__",
             [](LinearOCPSparseQP *sto, const PythonOCP &ocp) {
                 new (sto) LinearOCPSparseQP(LinearOCPSparseQP::build(ocp.ocp));
             })
        .def_prop_ro(
            "Q",
            [](const LinearOCPSparseQP &self) {
                auto scipy_sparse = nb::module_::import_("scipy.sparse");
                return scipy_sparse.attr("csc_array")(
                    nb::make_tuple(self.Q_values, self.Q_inner_idx, self.Q_outer_ptr),
                    "shape"_a = nb::make_tuple(self.Q_sparsity.rows, self.Q_sparsity.cols));
            },
            "Cost Hessian matrix in sparse CSC format (lower triangular part only)",
            nb::sig("@property\ndef Q(self) -> scipy.sparse.csc_array"))
        .def_prop_ro(
            "A",
            [](const LinearOCPSparseQP &self) {
                auto scipy_sparse = nb::module_::import_("scipy.sparse");
                return scipy_sparse.attr("csc_array")(
                    nb::make_tuple(self.A_values, self.A_inner_idx, self.A_outer_ptr),
                    "shape"_a = nb::make_tuple(self.A_sparsity.rows, self.A_sparsity.cols));
            },
            "Constraint Jacobian matrix in sparse CSC format",
            nb::sig("@property\ndef A(self) -> scipy.sparse.csc_array"))
        .def(
            "build_kkt_matrix",
            [](const LinearOCPSparseQP &self, real_t S, np_vector<> Σ, np_vector<const bool> J) {
                auto K            = self.build_kkt(S, as_span(Σ), as_span(J));
                auto scipy_sparse = nb::module_::import_("scipy.sparse");
                return scipy_sparse.attr("csc_array")(
                    nb::make_tuple(std::move(K.values), std::move(K.inner_idx),
                                   std::move(K.outer_ptr)),
                    "shape"_a = nb::make_tuple(K.sparsity.rows, K.sparsity.cols));
            },
            nb::sig("def build_kkt_matrix(self, S: float, Σ: NDArray[numpy.float64], "
                    "J: NDArray[numpy.float64]) -> scipy.sparse.csc_array"));
    nb::class_<cyqlone::qpalm::problems::PlatooningParams>(m, "PlatooningParams")
        .def(nb::init<>())
        .def_rw("friction", &cyqlone::qpalm::problems::PlatooningParams::friction)
        .def_rw("F_max", &cyqlone::qpalm::problems::PlatooningParams::F_max)
        .def_rw("v_max", &cyqlone::qpalm::problems::PlatooningParams::v_max)
        .def_rw("dist_min", &cyqlone::qpalm::problems::PlatooningParams::dist_min)
        .def_rw("dist_init", &cyqlone::qpalm::problems::PlatooningParams::dist_init)
        .def_rw("p_target", &cyqlone::qpalm::problems::PlatooningParams::p_target)
        .def_rw("N_horiz", &cyqlone::qpalm::problems::PlatooningParams::N_horiz)
        .def_rw("T_horiz", &cyqlone::qpalm::problems::PlatooningParams::T_horiz)
        .def_rw("scale_cost", &cyqlone::qpalm::problems::PlatooningParams::scale_cost)
        .def_rw("masses", &cyqlone::qpalm::problems::PlatooningParams::masses);
    m.def(
        "create_platooning_problem",
        [](const cyqlone::qpalm::problems::PlatooningParams &params) {
            auto p = platooning(params);
            return PythonOCP{std::move(p.ocp)};
        },
        "params"_a.sig("PlatooningParams()") = cyqlone::qpalm::problems::PlatooningParams{});
}

void register_settings(nb::module_ &m) {
    nb::enum_<cyqlone::qpalm::SolverStatus>(m, "SolverStatus")
        .value("Busy", cyqlone::qpalm::SolverStatus::Busy)
        .value("Converged", cyqlone::qpalm::SolverStatus::Converged)
        .value("MaxTime", cyqlone::qpalm::SolverStatus::MaxTime)
        .value("MaxIter", cyqlone::qpalm::SolverStatus::MaxIter)
        .value("NotFinite", cyqlone::qpalm::SolverStatus::NotFinite)
        .value("NoProgress", cyqlone::qpalm::SolverStatus::NoProgress)
        .value("Interrupted", cyqlone::qpalm::SolverStatus::Interrupted)
        .value("Exception", cyqlone::qpalm::SolverStatus::Exception);
    using DetailedStats = cyqlone::qpalm::DetailedStats;
    nb::class_<DetailedStats> detailed_stats(m, "DetailedStats");
    nb::enum_<DetailedStats::ExitReason>(detailed_stats, "ExitReason")
        .value("Busy", DetailedStats::ExitReason::Busy)
        .value("Converged", DetailedStats::ExitReason::Converged)
        .value("NoActiveSetChange", DetailedStats::ExitReason::NoActiveSetChange)
        .value("Fail", DetailedStats::ExitReason::Fail);
    nb::class_<DetailedStats::Entry>(detailed_stats, "Entry")
        .def_rw("outer_iter", &DetailedStats::Entry::outer_iter)
        .def_rw("inner_iter", &DetailedStats::Entry::inner_iter)
        .def_rw("stationarity", &DetailedStats::Entry::stationarity)
        .def_rw("ineq_constr_viol", &DetailedStats::Entry::ineq_constr_viol)
        .def_rw("eq_constr_viol", &DetailedStats::Entry::eq_constr_viol)
        .def_rw("linesearch_step_size", &DetailedStats::Entry::linesearch_step_size)
        .def_rw("linesearch_breakpoint_index", &DetailedStats::Entry::linesearch_breakpoint_index)
        .def_rw("num_active_constr", &DetailedStats::Entry::num_active_constr)
        .def_rw("num_changing_constr", &DetailedStats::Entry::num_changing_constr)
        .def_rw("exit_reason", &DetailedStats::Entry::exit_reason)
        .def("__getstate__",
             [](const DetailedStats::Entry &self) {
                 return nb::make_tuple(
                     // clang-format off
                    self.outer_iter,
                    self.inner_iter,
                    self.stationarity,
                    self.ineq_constr_viol,
                    self.eq_constr_viol,
                    self.linesearch_step_size,
                    self.linesearch_breakpoint_index,
                    self.num_active_constr,
                    self.num_changing_constr,
                    self.exit_reason
                     // clang-format on
                 );
             })
        .def("__setstate__", [](DetailedStats::Entry *self, nb::tuple t) {
            if (t.size() != 10)
                throw std::runtime_error("Invalid state!");
            using T = DetailedStats::Entry;
            new (self) T{
                // clang-format off
                    .outer_iter = nb::cast<decltype(T::outer_iter)>(t[0]),
                    .inner_iter = nb::cast<decltype(T::inner_iter)>(t[1]),
                    .stationarity = nb::cast<decltype(T::stationarity)>(t[2]),
                    .ineq_constr_viol = nb::cast<decltype(T::ineq_constr_viol)>(t[3]),
                    .eq_constr_viol = nb::cast<decltype(T::eq_constr_viol)>(t[4]),
                    .linesearch_step_size = nb::cast<decltype(T::linesearch_step_size)>(t[5]),
                    .linesearch_breakpoint_index = nb::cast<decltype(T::linesearch_breakpoint_index)>(t[6]),
                    .num_active_constr = nb::cast<decltype(T::num_active_constr)>(t[7]),
                    .num_changing_constr = nb::cast<decltype(T::num_changing_constr)>(t[8]),
                    .exit_reason = nb::cast<decltype(T::exit_reason)>(t[9])
                // clang-format on
            };
        });
    detailed_stats.def_ro("entries", &DetailedStats::entries)
        .def("__getstate__", [](const DetailedStats &self) { return nb::make_tuple(self.entries); })
        .def("__setstate__", [](DetailedStats *self, nb::tuple t) {
            if (t.size() != 1)
                throw std::runtime_error("Invalid state!");
            using T = DetailedStats;
            new (self) T{.entries = nb::cast<decltype(T::entries)>(t[0])};
        });
#if BATMAT_WITH_CPU_TIME
    nb::class_<guanaqo::TimingsCPU> timings_cpu(m, "TimingsCPU");
    timings_cpu.def(nb::init())
        .def("__copy__", [](const guanaqo::TimingsCPU &self) { return self; })
        .def(nb::pickle(
            [](const guanaqo::TimingsCPU &p) { // __getstate__
                return nb::make_tuple(
                    // clang-format off
                    p.num_invocations,
                    p.wall_time,
                    p.cpu_time)
                    // clang-format on
                    ;
            },
            [](nb::tuple t) { // __setstate__
                if (t.size() != 3)
                    throw std::runtime_error("Invalid state!");
                using T = guanaqo::TimingsCPU;
                return T{
                    // clang-format off
                    .num_invocations = nb::cast<decltype(T::num_invocations)>(t[0]),
                    .wall_time = nb::cast<decltype(T::wall_time)>(t[1]),
                    .cpu_time = nb::cast<decltype(T::cpu_time)>(t[2]),
                    // clang-format on
                };
            }))
        .def_rw("num_invocations", &guanaqo::TimingsCPU::num_invocations)
        .def_rw("wall_time", &guanaqo::TimingsCPU::wall_time)
        .def_rw("cpu_time", &guanaqo::TimingsCPU::cpu_time)
        .def("__str__", [](const guanaqo::TimingsCPU &self) {
            std::ostringstream ss;
            ss << self;
            return std::move(ss).str();
        });
    m.attr("DefaultTimings") = timings_cpu;
#else
    nb::class_<cyqlone::DefaultTimings>(m, "DefaultTimings")
        .def(nb::init())
        .def("__copy__", [](const cyqlone::DefaultTimings &self) { return self; })
        .def("__getstate__",
             [](const cyqlone::DefaultTimings &self) {
                 return nb::make_tuple(
                     // clang-format off
                    self.num_invocations,
                    self.wall_time)
                     // clang-format on
                     ;
             })
        .def("__setstate__",
             [](cyqlone::DefaultTimings *self, nb::tuple t) {
                 if (t.size() != 2)
                     throw std::runtime_error("Invalid state!");
                 using T = cyqlone::DefaultTimings;
                 new (self) T{{
                     // clang-format off
                    .num_invocations = nb::cast<decltype(T::num_invocations)>(t[0]),
                    .wall_time = nb::cast<decltype(T::wall_time)>(t[1]),
                     // clang-format on
                 }};
             })
        .def_rw("num_invocations", &cyqlone::DefaultTimings::num_invocations)
        .def_rw("wall_time", &cyqlone::DefaultTimings::wall_time)
        .def("__str__", [](const cyqlone::DefaultTimings &self) {
            std::ostringstream ss;
            ss << self;
            return std::move(ss).str();
        });
#endif
    nb::class_<cyqlone::qpalm::SolverTimings>(m, "SolverTimings")
        .def(nb::init())
        .def("__copy__", [](const cyqlone::qpalm::SolverTimings &self) { return self; })
        .def("__getstate__",
             [](const cyqlone::qpalm::SolverTimings &self) {
                 return nb::make_tuple(
                     // clang-format off
                    self.total,
                    self.scaling,
                    self.line_search,
                    self.recompute_inner,
                    self.recompute_outer,
                    self.mat_vec_M,
                    self.mat_vec_MT,
                    self.mat_vec_A,
                    self.mat_vec_AT,
                    self.mat_vec_Q,
                    self.active_set_change,
                    self.update_penalty,
                    self.update_regularization,
                    self.boost_regularization,
                    self.solve,
                    self.backend)
                     // clang-format on
                     ;
             })
        .def("__setstate__",
             [](cyqlone::qpalm::SolverTimings *self, nb::tuple t) {
                 if (t.size() != 16)
                     throw std::runtime_error("Invalid state!");
                 using T = cyqlone::qpalm::SolverTimings;
                 new (self) T{
                     // clang-format off
                    .total = nb::cast<decltype(T::total)>(t[0]),
                    .scaling = nb::cast<decltype(T::scaling)>(t[1]),
                    .line_search = nb::cast<decltype(T::line_search)>(t[2]),
                    .recompute_inner = nb::cast<decltype(T::recompute_inner)>(t[3]),
                    .recompute_outer = nb::cast<decltype(T::recompute_outer)>(t[4]),
                    .mat_vec_M = nb::cast<decltype(T::mat_vec_M)>(t[5]),
                    .mat_vec_MT = nb::cast<decltype(T::mat_vec_MT)>(t[6]),
                    .mat_vec_A = nb::cast<decltype(T::mat_vec_A)>(t[7]),
                    .mat_vec_AT = nb::cast<decltype(T::mat_vec_AT)>(t[8]),
                    .mat_vec_Q = nb::cast<decltype(T::mat_vec_Q)>(t[9]),
                    .active_set_change = nb::cast<decltype(T::active_set_change)>(t[10]),
                    .update_penalty = nb::cast<decltype(T::update_penalty)>(t[11]),
                    .update_regularization = nb::cast<decltype(T::update_regularization)>(t[12]),
                    .boost_regularization = nb::cast<decltype(T::boost_regularization)>(t[13]),
                    .solve = nb::cast<decltype(T::solve)>(t[14]),
                    .backend = nb::cast<decltype(T::backend)>(t[15]),
                     // clang-format on
                 };
             })
        .def_rw("total", &cyqlone::qpalm::SolverTimings::total)
        .def_rw("scaling", &cyqlone::qpalm::SolverTimings::scaling)
        .def_rw("line_search", &cyqlone::qpalm::SolverTimings::line_search)
        .def_rw("recompute_inner", &cyqlone::qpalm::SolverTimings::recompute_inner)
        .def_rw("recompute_outer", &cyqlone::qpalm::SolverTimings::recompute_outer)
        .def_rw("mat_vec_M", &cyqlone::qpalm::SolverTimings::mat_vec_M)
        .def_rw("mat_vec_MT", &cyqlone::qpalm::SolverTimings::mat_vec_MT)
        .def_rw("mat_vec_A", &cyqlone::qpalm::SolverTimings::mat_vec_A)
        .def_rw("mat_vec_AT", &cyqlone::qpalm::SolverTimings::mat_vec_AT)
        .def_rw("mat_vec_Q", &cyqlone::qpalm::SolverTimings::mat_vec_Q)
        .def_rw("active_set_change", &cyqlone::qpalm::SolverTimings::active_set_change)
        .def_rw("update_penalty", &cyqlone::qpalm::SolverTimings::update_penalty)
        .def_rw("update_regularization", &cyqlone::qpalm::SolverTimings::update_regularization)
        .def_rw("boost_regularization", &cyqlone::qpalm::SolverTimings::boost_regularization)
        .def_rw("solve", &cyqlone::qpalm::SolverTimings::solve)
        .def_rw("backend", &cyqlone::qpalm::SolverTimings::backend);
    nb::class_<cyqlone::qpalm::SolverStats>(m, "SolverStats")
        .def(nb::init())
        .def("__copy__", [](const cyqlone::qpalm::SolverStats &self) { return self; })
        .def_rw("inner_iter", &cyqlone::qpalm::SolverStats::inner_iter)
        .def_rw("outer_iter", &cyqlone::qpalm::SolverStats::outer_iter)
        .def_rw("stationarity", &cyqlone::qpalm::SolverStats::stationarity)
        .def_rw("primal_residual_norm", &cyqlone::qpalm::SolverStats::primal_residual_norm)
        .def_rw("max_penalty", &cyqlone::qpalm::SolverStats::max_penalty)
        .def_rw("timings", &cyqlone::qpalm::SolverStats::timings)
        .def_rw("detail", &cyqlone::qpalm::SolverStats::detail);
    nb::enum_<cyqlone::qpalm::WarmStartingStrategy>(m, "WarmStartingStrategy")
        .value("Zeros", cyqlone::qpalm::WarmStartingStrategy::Zeros)
        .value("Copy", cyqlone::qpalm::WarmStartingStrategy::Copy)
        .value("Shift", cyqlone::qpalm::WarmStartingStrategy::Shift)
        .value("ShiftNoInequality", cyqlone::qpalm::WarmStartingStrategy::ShiftNoInequality);
    nb::class_<cyqlone::qpalm::CyqloneBackendSettings>(m, "CyqloneBackendSettings")
        .def("__copy__", [](const cyqlone::qpalm::CyqloneBackendSettings &self) { return self; })
        .def(nb::init())
        .def_rw("log_processors", &cyqlone::qpalm::CyqloneBackendSettings::log_processors)
        .def_rw("print_residuals", &cyqlone::qpalm::CyqloneBackendSettings::print_residuals)
        .def_rw("print_precision", &cyqlone::qpalm::CyqloneBackendSettings::print_precision)
        .def_rw("factor_alt", &cyqlone::qpalm::CyqloneBackendSettings::factor_alt)
        .def_rw("changing_constr_factor",
                &cyqlone::qpalm::CyqloneBackendSettings::changing_constr_factor)
        .def_rw("max_update_count", &cyqlone::qpalm::CyqloneBackendSettings::max_update_count)
        .def_rw("detailed_timings", &cyqlone::qpalm::CyqloneBackendSettings::detailed_timings)
        .def_rw("pcg_max_iter", &cyqlone::qpalm::CyqloneBackendSettings::pcg_max_iter)
        .def_rw("pcg_tolerance", &cyqlone::qpalm::CyqloneBackendSettings::pcg_tolerance)
        .def_rw("pcg_print_resid", &cyqlone::qpalm::CyqloneBackendSettings::pcg_print_resid)
        .def_rw("use_stair_preconditioner",
                &cyqlone::qpalm::CyqloneBackendSettings::use_stair_preconditioner)
        .def_rw("spin_count", &cyqlone::qpalm::CyqloneBackendSettings::spin_count)
        .def_rw("strategy", &cyqlone::qpalm::CyqloneBackendSettings::strategy);
    nb::class_<cyqlone::qpalm::Settings>(m, "Settings")
        .def(nb::init())
        .def("__copy__", [](const cyqlone::qpalm::Settings &self) { return self; })
        .def_rw("max_outer_iter", &cyqlone::qpalm::Settings::max_outer_iter)
        .def_rw("max_inner_iter", &cyqlone::qpalm::Settings::max_inner_iter)
        .def_rw("max_total_inner_iter", &cyqlone::qpalm::Settings::max_total_inner_iter)
        .def_rw("max_time", &cyqlone::qpalm::Settings::max_time)
        .def_rw("tolerance", &cyqlone::qpalm::Settings::tolerance)
        .def_rw("dual_tolerance", &cyqlone::qpalm::Settings::dual_tolerance)
        .def_rw("eq_constr_tolerance", &cyqlone::qpalm::Settings::eq_constr_tolerance)
        .def_rw("initial_inner_tolerance", &cyqlone::qpalm::Settings::initial_inner_tolerance)
        .def_rw("ρ", &cyqlone::qpalm::Settings::ρ)
        .def_rw("θ", &cyqlone::qpalm::Settings::θ)
        .def_rw("Δy", &cyqlone::qpalm::Settings::Δy)
        .def_rw("Δy_always", &cyqlone::qpalm::Settings::Δy_always)
        .def_rw("max_penalty_y", &cyqlone::qpalm::Settings::max_penalty_y)
        .def_rw("initial_penalty_y", &cyqlone::qpalm::Settings::initial_penalty_y)
        .def_rw("Δx", &cyqlone::qpalm::Settings::Δx)
        .def_rw("max_penalty_x", &cyqlone::qpalm::Settings::max_penalty_x)
        .def_rw("boost_penalty_x", &cyqlone::qpalm::Settings::boost_penalty_x)
        .def_rw("initial_penalty_x", &cyqlone::qpalm::Settings::initial_penalty_x)
        .def_rw("proximal", &cyqlone::qpalm::Settings::proximal)
        .def_rw("recompute_eq_res", &cyqlone::qpalm::Settings::recompute_eq_res)
        .def_rw("recompute_inner", &cyqlone::qpalm::Settings::recompute_inner)
        .def_rw("recompute", &cyqlone::qpalm::Settings::recompute)
        .def_rw("verbose", &cyqlone::qpalm::Settings::verbose)
        .def_rw("max_no_changes_active_set", &cyqlone::qpalm::Settings::max_no_changes_active_set)
        .def_rw("linesearch_include_multipliers",
                &cyqlone::qpalm::Settings::linesearch_include_multipliers)
        .def_rw("force_linesearch_if_no_set_change",
                &cyqlone::qpalm::Settings::force_linesearch_if_no_set_change)
        .def_rw("force_linesearch_if_dir_deriv_pos",
                &cyqlone::qpalm::Settings::force_linesearch_if_dir_deriv_pos)
        .def_rw("detailed_stats", &cyqlone::qpalm::Settings::detailed_stats)
        .def_rw("scale_newton_step", &cyqlone::qpalm::Settings::scale_newton_step)
        .def_rw("print_directional_deriv", &cyqlone::qpalm::Settings::print_directional_deriv)
        .def_rw("print_linesearch_inputs", &cyqlone::qpalm::Settings::print_linesearch_inputs);
}

template <index_t VL>
struct PythonCyqloneSolver {
    using Backend = cyqlone::qpalm::unique_CyqloneBackend<VL>;
    using Solver  = cyqlone::qpalm::Solver<Backend>;
    Solver solver;
    PythonCyqloneSolver(const PythonOCP &ocp,
                        cyqlone::qpalm::CyqloneBackendSettings backend_settings,
                        cyqlone::qpalm::Settings qpalm_settings)
        : solver{cyqlone::qpalm::make_qpalm_cyqlone_backend<VL>(
                     cyqlone::CyqloneStorage<>::build(ocp.ocp), {}, backend_settings),
                 qpalm_settings} {}
};

template <class Solver, class BackendSettings>
void register_qpalm_solver(nb::module_ &m, const char *name) {
    nb::class_<Solver> solver(m, name);
    solver //
        .def(nb::init<const PythonOCP &, BackendSettings, cyqlone::qpalm::Settings>(), "ocp"_a,
             "backend_settings"_a, "qpalm_settings"_a)
        .def("__call__", [](Solver &self) { return self.solver(); })
        .def_prop_ro("solution",
                     [](Solver &self) {
                         if (!self.solver.has_result())
                             throw std::runtime_error("No solution available. Please solve first.");
                         return self.solver.get_solution();
                     })
        .def_prop_ro("equality_multipliers",
                     [](Solver &self) {
                         if (!self.solver.has_result())
                             throw std::runtime_error("No solution available. Please solve first.");
                         return self.solver.get_equality_multipliers();
                     })
        .def_prop_ro("inequality_multipliers",
                     [](Solver &self) {
                         if (!self.solver.has_result())
                             throw std::runtime_error("No solution available. Please solve first.");
                         return self.solver.get_inequality_multipliers();
                     })
        .def_prop_ro("stats", [](Solver &self) -> auto & { return self.solver.stats; })
        .def("warm_start_solution", [](Solver &self) { return self.solver.warm_start_solution(); })
        .def(
            "set_initial_guess",
            [](Solver &self, np_vector<> x, np_vector<> y, np_vector<> λ) {
                return self.solver.set_initial_guess(as_span(x), as_span(y), as_span(λ));
            },
            "x"_a, "y"_a, "λ"_a)
        .def(
            "get_initial_guess",
            [](Solver &self) -> nb::object {
                Eigen::VectorX<real_t> x(self.solver.get_num_variables()),
                    y(self.solver.get_num_inequality_constraints()),
                    λ(self.solver.get_num_equality_constraints());
                if (self.solver.get_initial_guess(guanaqo::as_span(x), guanaqo::as_span(y),
                                                  guanaqo::as_span(λ)))
                    return nb::make_tuple(std::move(x), std::move(y), std::move(λ));
                return nb::none();
            },
            nb::sig("def get_initial_guess(self) -> Optional[Tuple[NDArray[numpy.float64], "
                    "NDArray[numpy.float64], NDArray[numpy.float64]]]"))
        .def("update_data",
             [](Solver &self, const PythonOCP &ocp) {
                 BATMAT_ASSERT(self.solver.backend);
                 return update_qpalm_cyqlone_backend(*self.solver.backend, ocp.ocp);
             })
        .def("set_b_eq",
             [](Solver &self, np_vector<> b_eq) { return self.solver.set_b_eq(as_span(b_eq)); })
        .def("set_b_lb",
             [](Solver &self, np_vector<> b_lb) { return self.solver.set_b_lb(as_span(b_lb)); })
        .def("set_b_ub",
             [](Solver &self, np_vector<> b_ub) { return self.solver.set_b_ub(as_span(b_ub)); });
}

template <index_t VL>
void register_qpalm_cyqlone(nb::module_ &m) {
    register_qpalm_solver<PythonCyqloneSolver<VL>, cyqlone::qpalm::CyqloneBackendSettings>(
        m, "QPALM_Cyqlone");
}

template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

} // namespace CYQLONE_NAMESPACE

NB_MODULE(MODULE_NAME, m) {
    using namespace cyqlone;
    m.doc()                      = "Python interface to cyqlone's C++ implementation.";
    m.attr("__version__")        = CYQLONE_VERSION_FULL;
    m.attr("build_time")         = CYQLONE_BUILD_TIME;
    m.attr("commit_hash")        = CYQLONE_COMMIT_HASH;
    m.attr("batmat_version")     = BATMAT_VERSION_FULL;
    m.attr("batmat_build_time")  = BATMAT_BUILD_TIME;
    m.attr("batmat_commit_hash") = BATMAT_COMMIT_HASH;
    register_ocp(m);
    register_settings(m);
    auto simd8  = m.def_submodule("simd8");
    auto simd4  = m.def_submodule("simd4");
    auto scalar = m.def_submodule("scalar");
    register_qpalm_cyqlone<8>(simd8);
    register_qpalm_cyqlone<4>(simd4);
    register_qpalm_cyqlone<1>(scalar);

#if BATMAT_WITH_OPENMP
    m.def("omp_set_num_threads", omp_set_num_threads, "num_threads"_a);
#else
    m.def("pool_set_num_threads", batmat::pool_set_num_threads, "num_threads"_a);
#endif

#if GUANAQO_WITH_TRACING
    nb::class_<guanaqo::TraceLogger::Log>(m, "TraceLog")
        .def_ro("name", &guanaqo::TraceLogger::Log::name)
        .def_ro("instance", &guanaqo::TraceLogger::Log::instance)
        .def_prop_ro("start_time",
                     [](const guanaqo::TraceLogger::Log &self) { return self.start_time.count(); })
        .def_prop_ro("duration",
                     [](const guanaqo::TraceLogger::Log &self) { return self.duration.count(); })
        .def_ro("thread_id", &guanaqo::TraceLogger::Log::thread_id)
        .def_ro("flop_count", &guanaqo::TraceLogger::Log::flop_count);
    m.def("get_trace_log", [] {
        auto l = guanaqo::trace_logger.get_logs();
        return std::vector<guanaqo::TraceLogger::Log>{l.begin(), l.end()};
    });
    m.def("reset_trace_log", [] { guanaqo::trace_logger.reset(); });
    m.def(
        "dump_trace_log",
        [](const std::filesystem::path &filename) {
            std::filesystem::create_directories(filename.parent_path());
            std::ofstream csv{filename};
            guanaqo::TraceLogger::write_column_headings(csv) << '\n';
            for (const auto &log : guanaqo::trace_logger.get_logs())
                csv << log << '\n';
        },
        "filename"_a);
#endif
}
