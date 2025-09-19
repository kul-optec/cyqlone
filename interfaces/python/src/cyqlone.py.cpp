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
#include <guanaqo/eigen/span.hpp>
#include <guanaqo/eigen/view.hpp>
#include <batmat-version.h>
#include <cyqlone-version.h>

#include <optional>
#include <stdexcept>

using cyqlone::index_t;
using cyqlone::real_t;

#include <pybind11/chrono.h>
#include <pybind11/eigen/matrix.h>
#include <pybind11/eigen/tensor.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;
using namespace py::literals;
using tensor3   = Eigen::Tensor<real_t, 3>;
using cmtensor3 = Eigen::TensorMap<const tensor3>;
using crmat     = Eigen::Ref<const Eigen::MatrixX<real_t>>;
using crvec     = Eigen::Ref<const Eigen::VectorX<real_t>>;
using crbvec    = Eigen::Ref<const Eigen::VectorX<bool>>;
using cmmat     = Eigen::Map<const Eigen::MatrixX<real_t>>;
using cmvec     = Eigen::Map<const Eigen::VectorX<real_t>>;
using cmbvec    = Eigen::Map<const Eigen::VectorX<bool>>;

#if BATMAT_WITH_OPENMP
#include <omp.h>
#endif
#if GUANAQO_WITH_TRACING
#include <guanaqo/trace.hpp>
#include <pybind11/stl/filesystem.h>
#include <filesystem>
#include <fstream>
#endif

namespace cyqlone {

struct PythonOCP {
    cyqlone::LinearOCPStorage ocp;
    std::vector<real_t> rhs_lb, rhs_ub, rhs_eq, qr, x_guess, y_guess, p_guess;

    struct Init {
        cyqlone::LinearOCPStorage ocp;
        std::vector<real_t> rhs_lb, rhs_ub, rhs_eq, qr, x_guess, y_guess, p_guess;
    };

    PythonOCP() = default;
    PythonOCP(Init init)
        : ocp{std::move(init.ocp)}, rhs_lb{std::move(init.rhs_lb)}, rhs_ub{std::move(init.rhs_ub)},
          rhs_eq{std::move(init.rhs_eq)}, qr{std::move(init.qr)}, x_guess{std::move(init.x_guess)},
          y_guess{std::move(init.y_guess)}, p_guess{std::move(init.p_guess)} {}
    PythonOCP(cmtensor3 AB, cmtensor3 CD, crmat CN, cmtensor3 QRS, crmat QN, crvec rhs_eq,
              crvec rhs_lb, crvec rhs_ub, crvec qr, std::optional<crvec> x_guess,
              std::optional<crvec> y_guess, std::optional<crvec> p_guess)
        : ocp{.dim = {
                  .N_horiz = static_cast<index_t>(AB.dimension(0)),
                  .nx      = static_cast<index_t>(AB.dimension(1)),
                  .nu      = static_cast<index_t>(AB.dimension(2) - AB.dimension(1)),
                  .ny      = static_cast<index_t>(CD.dimension(1)),
                  .ny_N    = static_cast<index_t>(CN.rows()),
              }} {
        auto [N, nx, nu, ny, ny_N] = ocp.dim;
        index_t num_var            = N * (nx + nu) + nx;
        index_t num_eq_constr      = (N + 1) * nx;
        index_t num_ineq_constr    = N * ny + ny_N;
        this->rhs_eq.resize(num_eq_constr);
        this->rhs_lb.resize(num_ineq_constr);
        this->rhs_ub.resize(num_ineq_constr);
        this->qr.resize(num_var);
        if (CD.dimension(0) != N)
            throw std::invalid_argument("Invalid horizon length CD");
        if (QRS.dimension(0) != N)
            throw std::invalid_argument("Invalid horizon length QRS");
        if (CD.dimension(2) != nx + nu)
            throw std::invalid_argument("Invalid number of columns CD");
        if (QRS.dimension(1) != nx + nu)
            throw std::invalid_argument("Invalid number of rows QRS");
        if (QRS.dimension(2) != nx + nu)
            throw std::invalid_argument("Invalid number of columns QRS");
        if (CN.cols() != nx)
            throw std::invalid_argument("Invalid number of columns CN");
        if (QN.rows() != nx)
            throw std::invalid_argument("Invalid number of rows QN");
        if (QN.cols() != nx)
            throw std::invalid_argument("Invalid number of cols QN");
        if (rhs_eq.size() != num_eq_constr)
            throw std::invalid_argument("Invalid size rhs_eq");
        if (rhs_lb.size() != num_ineq_constr)
            throw std::invalid_argument("Invalid size rhs_lb");
        if (rhs_ub.size() != num_ineq_constr)
            throw std::invalid_argument("Invalid size rhs_ub");
        if (qr.size() != num_var)
            throw std::invalid_argument("Invalid size qr");
        if (x_guess && x_guess->size() != num_var)
            throw std::invalid_argument("Invalid size x_guess");
        if (y_guess && y_guess->size() != num_ineq_constr)
            throw std::invalid_argument("Invalid size y_guess");
        if (p_guess && p_guess->size() != num_eq_constr)
            throw std::invalid_argument("Invalid size p_guess");

        for (index_t i = 0; i < N; ++i) {
            for (index_t r = 0; r < nx; ++r)
                for (index_t c = 0; c < nx + nu; ++c)
                    ocp.AB(i)(r, c) = AB(i, r, c);
            for (index_t r = 0; r < ny; ++r)
                for (index_t c = 0; c < nx + nu; ++c)
                    ocp.CD(i)(r, c) = CD(i, r, c);
            for (index_t r = 0; r < nx + nu; ++r)
                for (index_t c = 0; c < nx + nu; ++c)
                    ocp.H(i)(r, c) = QRS(i, r, c);
        }
        for (index_t r = 0; r < ny_N; ++r)
            for (index_t c = 0; c < nx; ++c)
                ocp.CD(N)(r, c) = CN(r, c);
        for (index_t r = 0; r < nx; ++r)
            for (index_t c = 0; c < nx; ++c)
                ocp.H(N)(r, c) = QN(r, c);
        std::ranges::copy(rhs_eq, this->rhs_eq.begin());
        std::ranges::copy(rhs_lb, this->rhs_lb.begin());
        std::ranges::copy(rhs_ub, this->rhs_ub.begin());
        std::ranges::copy(qr, this->qr.begin());
        if (x_guess) {
            this->x_guess.resize(num_var);
            std::ranges::copy(*x_guess, this->x_guess.begin());
        }
        if (y_guess) {
            this->y_guess.resize(num_ineq_constr);
            std::ranges::copy(*y_guess, this->y_guess.begin());
        }
        if (p_guess) {
            this->p_guess.resize(num_eq_constr);
            std::ranges::copy(*p_guess, this->p_guess.begin());
        }
    }
};

void register_ocp(py::module_ &m) {
    py::class_<PythonOCP> ocp(m, "OCP");
    ocp.def(py::init<cmtensor3, cmtensor3, crmat, cmtensor3, crmat, crvec, crvec, crvec, crvec,
                     std::optional<crvec>, std::optional<crvec>, std::optional<crvec>>(),
            "AB"_a, "CD"_a, "CN"_a, "QRS"_a, "QN"_a, "rhs_eq"_a, "rhs_lb"_a, "rhs_ub"_a, "qr"_a,
            "x_guess"_a = py::none(), "y_guess"_a = py::none(), "p_guess"_a = py::none())
        .def_readwrite("rhs_eq", &PythonOCP::rhs_eq)
        .def_readwrite("rhs_lb", &PythonOCP::rhs_lb)
        .def_readwrite("rhs_ub", &PythonOCP::rhs_ub)
        .def_readwrite("qr", &PythonOCP::qr)
        .def_property_readonly("dim", [](const PythonOCP &ocp) {
            return py::make_tuple(ocp.ocp.dim.N_horiz, ocp.ocp.dim.nx, ocp.ocp.dim.nu,
                                  ocp.ocp.dim.ny, ocp.ocp.dim.ny_N);
        });
    using cyqlone::qpalm::LinearOCPSparseQP;
    py::class_<LinearOCPSparseQP>(m, "LinearOCPSparseQP")
        .def(py::init([](const PythonOCP &ocp) { return LinearOCPSparseQP::build(ocp.ocp); }))
        .def_property_readonly(
            "Q",
            [](const LinearOCPSparseQP &self) {
                auto scipy_sparse = py::module_::import("scipy.sparse");
                return scipy_sparse.attr("csc_array")(
                    py::make_tuple(self.Q_values, self.Q_inner_idx, self.Q_outer_ptr),
                    "shape"_a = py::make_tuple(self.Q_sparsity.rows, self.Q_sparsity.cols));
            })
        .def_property_readonly(
            "A",
            [](const LinearOCPSparseQP &self) {
                auto scipy_sparse = py::module_::import("scipy.sparse");
                return scipy_sparse.attr("csc_array")(
                    py::make_tuple(self.A_values, self.A_inner_idx, self.A_outer_ptr),
                    "shape"_a = py::make_tuple(self.A_sparsity.rows, self.A_sparsity.cols));
            })
        .def("build_kkt_matrix", [](const LinearOCPSparseQP &self, real_t S, crvec Σ, crbvec J) {
            auto K            = self.build_kkt(S, guanaqo::as_span(Σ), guanaqo::as_span(J));
            auto scipy_sparse = py::module_::import("scipy.sparse");
            return scipy_sparse.attr("csc_array")(
                py::make_tuple(K.values, K.inner_idx, K.outer_ptr),
                "shape"_a = py::make_tuple(K.sparsity.rows, K.sparsity.cols));
        });
    py::class_<cyqlone::qpalm::problems::PlatooningParams>(m, "PlatooningParams")
        .def(py::init<>())
        .def_readwrite("friction", &cyqlone::qpalm::problems::PlatooningParams::friction)
        .def_readwrite("Ts", &cyqlone::qpalm::problems::PlatooningParams::Ts)
        .def_readwrite("F_max", &cyqlone::qpalm::problems::PlatooningParams::F_max)
        .def_readwrite("v_max", &cyqlone::qpalm::problems::PlatooningParams::v_max)
        .def_readwrite("dist_min", &cyqlone::qpalm::problems::PlatooningParams::dist_min)
        .def_readwrite("dist_init", &cyqlone::qpalm::problems::PlatooningParams::dist_init)
        .def_readwrite("p_target", &cyqlone::qpalm::problems::PlatooningParams::p_target)
        .def_readwrite("N_horiz", &cyqlone::qpalm::problems::PlatooningParams::N_horiz)
        .def_readwrite("masses", &cyqlone::qpalm::problems::PlatooningParams::masses);
    m.def(
        "create_platooning_problem",
        [](const cyqlone::qpalm::problems::PlatooningParams &params) {
            auto p  = platooning(params);
            auto qr = cyqlone::qpalm::reference_to_gradient(p.ocp, p.ref);
            return PythonOCP{{
                .ocp     = std::move(p.ocp),
                .rhs_lb  = std::move(p.rhs_ineq_lb),
                .rhs_ub  = std::move(p.rhs_ineq_ub),
                .rhs_eq  = std::move(p.rhs_eq),
                .qr      = std::move(qr),
                .x_guess = {},
                .y_guess = {},
                .p_guess = {},
            }};
        },
        py::arg_v("params", cyqlone::qpalm::problems::PlatooningParams{}, "PlatooningParams()"));
}

void register_settings(py::module_ &m) {
    py::enum_<cyqlone::qpalm::SolverStatus>(m, "SolverStatus")
        .value("Busy", cyqlone::qpalm::SolverStatus::Busy)
        .value("Converged", cyqlone::qpalm::SolverStatus::Converged)
        .value("MaxTime", cyqlone::qpalm::SolverStatus::MaxTime)
        .value("MaxIter", cyqlone::qpalm::SolverStatus::MaxIter)
        .value("NotFinite", cyqlone::qpalm::SolverStatus::NotFinite)
        .value("NoProgress", cyqlone::qpalm::SolverStatus::NoProgress)
        .value("Interrupted", cyqlone::qpalm::SolverStatus::Interrupted)
        .value("Exception", cyqlone::qpalm::SolverStatus::Exception);
    using DetailedStats = cyqlone::qpalm::DetailedStats;
    py::class_<DetailedStats> detailed_stats(m, "DetailedStats");
    py::enum_<DetailedStats::ExitReason>(detailed_stats, "ExitReason")
        .value("Busy", DetailedStats::ExitReason::Busy)
        .value("Converged", DetailedStats::ExitReason::Converged)
        .value("NoActiveSetChange", DetailedStats::ExitReason::NoActiveSetChange)
        .value("Fail", DetailedStats::ExitReason::Fail);
    py::class_<DetailedStats::Entry>(detailed_stats, "Entry")
        .def_readwrite("outer_iter", &DetailedStats::Entry::outer_iter)
        .def_readwrite("inner_iter", &DetailedStats::Entry::inner_iter)
        .def_readwrite("stationarity", &DetailedStats::Entry::stationarity)
        .def_readwrite("ineq_constr_viol", &DetailedStats::Entry::ineq_constr_viol)
        .def_readwrite("eq_constr_viol", &DetailedStats::Entry::eq_constr_viol)
        .def_readwrite("linesearch_step_size", &DetailedStats::Entry::linesearch_step_size)
        .def_readwrite("linesearch_breakpoint_index",
                       &DetailedStats::Entry::linesearch_breakpoint_index)
        .def_readwrite("num_active_constr", &DetailedStats::Entry::num_active_constr)
        .def_readwrite("num_changing_constr", &DetailedStats::Entry::num_changing_constr)
        .def_readwrite("exit_reason", &DetailedStats::Entry::exit_reason);
    detailed_stats.def_readonly("entries", &DetailedStats::entries);
#if BATMAT_WITH_CPU_TIME
    py::class_<guanaqo::TimingsCPU> timings_cpu(m, "TimingsCPU");
    timings_cpu.def(py::init())
        .def("__copy__", [](const guanaqo::TimingsCPU &self) { return self; })
        .def(py::pickle(
            [](const guanaqo::TimingsCPU &p) { // __getstate__
                return py::make_tuple(
                    // clang-format off
                    p.num_invocations,
                    p.wall_time,
                    p.cpu_time)
                    // clang-format on
                    ;
            },
            [](py::tuple t) { // __setstate__
                if (t.size() != 3)
                    throw std::runtime_error("Invalid state!");
                using T = guanaqo::TimingsCPU;
                return T{
                    // clang-format off
                    .num_invocations = py::cast<decltype(T::num_invocations)>(t[0]),
                    .wall_time = py::cast<decltype(T::wall_time)>(t[1]),
                    .cpu_time = py::cast<decltype(T::cpu_time)>(t[2]),
                    // clang-format on
                };
            }))
        .def_readwrite("num_invocations", &guanaqo::TimingsCPU::num_invocations)
        .def_readwrite("wall_time", &guanaqo::TimingsCPU::wall_time)
        .def_readwrite("cpu_time", &guanaqo::TimingsCPU::cpu_time)
        .def("__str__", [](const guanaqo::TimingsCPU &self) {
            std::ostringstream ss;
            ss << self;
            return std::move(ss).str();
        });
    m.attr("DefaultTimings") = timings_cpu;
#else
    py::class_<batmat::DefaultTimings>(m, "DefaultTimings")
        .def(py::init())
        .def("__copy__", [](const batmat::DefaultTimings &self) { return self; })
        .def(py::pickle(
            [](const batmat::DefaultTimings &p) { // __getstate__
                return py::make_tuple(
                    // clang-format off
                    p.num_invocations,
                    p.wall_time)
                    // clang-format on
                    ;
            },
            [](py::tuple t) { // __setstate__
                if (t.size() != 2)
                    throw std::runtime_error("Invalid state!");
                using T = batmat::DefaultTimings;
                return T{
                    // clang-format off
                    .num_invocations = py::cast<decltype(T::num_invocations)>(t[0]),
                    .wall_time = py::cast<decltype(T::wall_time)>(t[1]),
                    // clang-format on
                };
            }))
        .def_readwrite("num_invocations", &batmat::DefaultTimings::num_invocations)
        .def_readwrite("wall_time", &batmat::DefaultTimings::wall_time)
        .def("__str__", [](const batmat::DefaultTimings &self) {
            std::ostringstream ss;
            ss << self;
            return std::move(ss).str();
        });
#endif
    py::class_<cyqlone::qpalm::SolverTimings>(m, "SolverTimings")
        .def(py::init())
        .def("__copy__", [](const cyqlone::qpalm::SolverTimings &self) { return self; })
        .def(py::pickle(
            [](const cyqlone::qpalm::SolverTimings &p) { // __getstate__
                return py::make_tuple(
                    // clang-format off
                    p.total,
                    p.scaling,
                    p.line_search,
                    p.recompute_inner,
                    p.recompute_outer,
                    p.mat_vec_M,
                    p.mat_vec_MT,
                    p.mat_vec_A,
                    p.mat_vec_AT,
                    p.mat_vec_Q,
                    p.active_set_change,
                    p.update_penalty,
                    p.update_regularization,
                    p.boost_regularization,
                    p.solve,
                    p.backend)
                    // clang-format on
                    ;
            },
            [](py::tuple t) { // __setstate__
                if (t.size() != 16)
                    throw std::runtime_error("Invalid state!");
                using T = cyqlone::qpalm::SolverTimings;
                return T{
                    // clang-format off
                    .total = py::cast<decltype(T::total)>(t[0]),
                    .scaling = py::cast<decltype(T::scaling)>(t[1]),
                    .line_search = py::cast<decltype(T::line_search)>(t[2]),
                    .recompute_inner = py::cast<decltype(T::recompute_inner)>(t[3]),
                    .recompute_outer = py::cast<decltype(T::recompute_outer)>(t[4]),
                    .mat_vec_M = py::cast<decltype(T::mat_vec_M)>(t[5]),
                    .mat_vec_MT = py::cast<decltype(T::mat_vec_MT)>(t[6]),
                    .mat_vec_A = py::cast<decltype(T::mat_vec_A)>(t[7]),
                    .mat_vec_AT = py::cast<decltype(T::mat_vec_AT)>(t[8]),
                    .mat_vec_Q = py::cast<decltype(T::mat_vec_Q)>(t[9]),
                    .active_set_change = py::cast<decltype(T::active_set_change)>(t[10]),
                    .update_penalty = py::cast<decltype(T::update_penalty)>(t[11]),
                    .update_regularization = py::cast<decltype(T::update_regularization)>(t[12]),
                    .boost_regularization = py::cast<decltype(T::boost_regularization)>(t[13]),
                    .solve = py::cast<decltype(T::solve)>(t[14]),
                    .backend = py::cast<decltype(T::backend)>(t[15]),
                    // clang-format on
                };
            }))
        .def_readwrite("total", &cyqlone::qpalm::SolverTimings::total)
        .def_readwrite("scaling", &cyqlone::qpalm::SolverTimings::scaling)
        .def_readwrite("line_search", &cyqlone::qpalm::SolverTimings::line_search)
        .def_readwrite("recompute_inner", &cyqlone::qpalm::SolverTimings::recompute_inner)
        .def_readwrite("recompute_outer", &cyqlone::qpalm::SolverTimings::recompute_outer)
        .def_readwrite("mat_vec_M", &cyqlone::qpalm::SolverTimings::mat_vec_M)
        .def_readwrite("mat_vec_MT", &cyqlone::qpalm::SolverTimings::mat_vec_MT)
        .def_readwrite("mat_vec_A", &cyqlone::qpalm::SolverTimings::mat_vec_A)
        .def_readwrite("mat_vec_AT", &cyqlone::qpalm::SolverTimings::mat_vec_AT)
        .def_readwrite("mat_vec_Q", &cyqlone::qpalm::SolverTimings::mat_vec_Q)
        .def_readwrite("active_set_change", &cyqlone::qpalm::SolverTimings::active_set_change)
        .def_readwrite("update_penalty", &cyqlone::qpalm::SolverTimings::update_penalty)
        .def_readwrite("update_regularization",
                       &cyqlone::qpalm::SolverTimings::update_regularization)
        .def_readwrite("boost_regularization", &cyqlone::qpalm::SolverTimings::boost_regularization)
        .def_readwrite("solve", &cyqlone::qpalm::SolverTimings::solve)
        .def_readwrite("backend", &cyqlone::qpalm::SolverTimings::backend);
    py::class_<cyqlone::qpalm::SolverStats>(m, "SolverStats")
        .def(py::init())
        .def("__copy__", [](const cyqlone::qpalm::SolverStats &self) { return self; })
        .def_readwrite("inner_iter", &cyqlone::qpalm::SolverStats::inner_iter)
        .def_readwrite("outer_iter", &cyqlone::qpalm::SolverStats::outer_iter)
        .def_readwrite("stationarity", &cyqlone::qpalm::SolverStats::stationarity)
        .def_readwrite("primal_residual_norm", &cyqlone::qpalm::SolverStats::primal_residual_norm)
        .def_readwrite("max_penalty", &cyqlone::qpalm::SolverStats::max_penalty)
        .def_readwrite("timings", &cyqlone::qpalm::SolverStats::timings)
        .def_readwrite("detail", &cyqlone::qpalm::SolverStats::detail);
    py::enum_<cyqlone::qpalm::WarmStartingStrategy>(m, "WarmStartingStrategy")
        .value("Zeros", cyqlone::qpalm::WarmStartingStrategy::Zeros)
        .value("Copy", cyqlone::qpalm::WarmStartingStrategy::Copy)
        .value("Shift", cyqlone::qpalm::WarmStartingStrategy::Shift)
        .value("ShiftNoInequality", cyqlone::qpalm::WarmStartingStrategy::ShiftNoInequality);
    py::class_<cyqlone::qpalm::CyqloneBackendSettings>(m, "CyqloneBackendSettings")
        .def("__copy__", [](const cyqlone::qpalm::CyqloneBackendSettings &self) { return self; })
        .def(py::init())
        .def_readwrite("log_processors", &cyqlone::qpalm::CyqloneBackendSettings::log_processors)
        .def_readwrite("print_residuals", &cyqlone::qpalm::CyqloneBackendSettings::print_residuals)
        .def_readwrite("print_precision", &cyqlone::qpalm::CyqloneBackendSettings::print_precision)
        .def_readwrite("factor_alt", &cyqlone::qpalm::CyqloneBackendSettings::factor_alt)
        .def_readwrite("changing_constr_factor",
                       &cyqlone::qpalm::CyqloneBackendSettings::changing_constr_factor)
        .def_readwrite("max_update_count",
                       &cyqlone::qpalm::CyqloneBackendSettings::max_update_count)
        .def_readwrite("detailed_timings",
                       &cyqlone::qpalm::CyqloneBackendSettings::detailed_timings)
        .def_readwrite("pcg_max_iter", &cyqlone::qpalm::CyqloneBackendSettings::pcg_max_iter)
        .def_readwrite("pcg_tolerance", &cyqlone::qpalm::CyqloneBackendSettings::pcg_tolerance)
        .def_readwrite("pcg_print_resid", &cyqlone::qpalm::CyqloneBackendSettings::pcg_print_resid)
        .def_readwrite("use_stair_preconditioner",
                       &cyqlone::qpalm::CyqloneBackendSettings::use_stair_preconditioner)
        .def_readwrite("strategy", &cyqlone::qpalm::CyqloneBackendSettings::strategy);
    py::class_<cyqlone::qpalm::Settings>(m, "Settings")
        .def(py::init())
        .def("__copy__", [](const cyqlone::qpalm::Settings &self) { return self; })
        .def_readwrite("max_outer_iter", &cyqlone::qpalm::Settings::max_outer_iter)
        .def_readwrite("max_inner_iter", &cyqlone::qpalm::Settings::max_inner_iter)
        .def_readwrite("max_total_inner_iter", &cyqlone::qpalm::Settings::max_total_inner_iter)
        .def_readwrite("max_time", &cyqlone::qpalm::Settings::max_time)
        .def_readwrite("tolerance", &cyqlone::qpalm::Settings::tolerance)
        .def_readwrite("dual_tolerance", &cyqlone::qpalm::Settings::dual_tolerance)
        .def_readwrite("eq_constr_tolerance", &cyqlone::qpalm::Settings::eq_constr_tolerance)
        .def_readwrite("initial_inner_tolerance",
                       &cyqlone::qpalm::Settings::initial_inner_tolerance)
        .def_readwrite("ρ", &cyqlone::qpalm::Settings::ρ)
        .def_readwrite("θ", &cyqlone::qpalm::Settings::θ)
        .def_readwrite("Δy", &cyqlone::qpalm::Settings::Δy)
        .def_readwrite("max_penalty_y", &cyqlone::qpalm::Settings::max_penalty_y)
        .def_readwrite("initial_penalty_y", &cyqlone::qpalm::Settings::initial_penalty_y)
        .def_readwrite("Δx", &cyqlone::qpalm::Settings::Δx)
        .def_readwrite("max_penalty_x", &cyqlone::qpalm::Settings::max_penalty_x)
        .def_readwrite("boost_penalty_x", &cyqlone::qpalm::Settings::boost_penalty_x)
        .def_readwrite("initial_penalty_x", &cyqlone::qpalm::Settings::initial_penalty_x)
        .def_readwrite("proximal", &cyqlone::qpalm::Settings::proximal)
        .def_readwrite("recompute_eq_res", &cyqlone::qpalm::Settings::recompute_eq_res)
        .def_readwrite("recompute_inner", &cyqlone::qpalm::Settings::recompute_inner)
        .def_readwrite("recompute", &cyqlone::qpalm::Settings::recompute)
        .def_readwrite("verbose", &cyqlone::qpalm::Settings::verbose)
        .def_readwrite("max_no_changes_active_set",
                       &cyqlone::qpalm::Settings::max_no_changes_active_set)
        .def_readwrite("linesearch_include_multipliers",
                       &cyqlone::qpalm::Settings::linesearch_include_multipliers)
        .def_readwrite("force_linesearch_if_no_set_change",
                       &cyqlone::qpalm::Settings::force_linesearch_if_no_set_change)
        .def_readwrite("force_linesearch_if_dir_deriv_pos",
                       &cyqlone::qpalm::Settings::force_linesearch_if_dir_deriv_pos)
        .def_readwrite("detailed_stats", &cyqlone::qpalm::Settings::detailed_stats)
        .def_readwrite("scale_newton_step", &cyqlone::qpalm::Settings::scale_newton_step)
        .def_readwrite("print_directional_deriv",
                       &cyqlone::qpalm::Settings::print_directional_deriv)
        .def_readwrite("print_linesearch_inputs",
                       &cyqlone::qpalm::Settings::print_linesearch_inputs);
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
                     cyqlone::CyqloneStorage<>::build(ocp.ocp, ocp.qr, ocp.rhs_eq, ocp.rhs_lb,
                                                      ocp.rhs_ub),
                     {.initial_variables              = ocp.x_guess,
                      .initial_inequality_multipliers = ocp.y_guess,
                      .initial_equality_multipliers   = ocp.p_guess},
                     backend_settings),
                 qpalm_settings} {}
};

template <class Solver, class BackendSettings>
void register_qpalm_solver(py::module_ &m, const char *name) {
    py::class_<Solver> solver(m, name);
    solver //
        .def(py::init<const PythonOCP &, BackendSettings, cyqlone::qpalm::Settings>(), "ocp"_a,
             "backend_settings"_a, "qpalm_settings"_a)
        .def("__call__", [](Solver &self) { return self.solver(); })
        .def_property_readonly("solution",
                               [](Solver &self) -> py::object {
                                   if (!self.solver.has_result())
                                       return py::none();
                                   return py::cast(self.solver.get_solution());
                               })
        .def_property_readonly("equality_multipliers",
                               [](Solver &self) -> py::object {
                                   if (!self.solver.has_result())
                                       return py::none();
                                   return py::cast(self.solver.get_equality_multipliers());
                               })
        .def_property_readonly("inequality_multipliers",
                               [](Solver &self) -> py::object {
                                   if (!self.solver.has_result())
                                       return py::none();
                                   return py::cast(self.solver.get_inequality_multipliers());
                               })
        .def_property_readonly(
            "stats", py::cpp_function([](Solver &self) -> auto & { return self.solver.stats; },
                                      py::return_value_policy::reference_internal))
        .def("warm_start_solution", [](Solver &self) { return self.solver.warm_start_solution(); })
        .def("update_data",
             [](Solver &self, const PythonOCP &ocp) {
                 BATMAT_ASSERT(self.solver.backend);
                 return update_qpalm_cyqlone_backend(*self.solver.backend, ocp.ocp, ocp.qr,
                                                     ocp.rhs_eq, ocp.rhs_lb, ocp.rhs_ub);
             })
        .def("set_b_eq",
             [](Solver &self, crvec b_eq) { return self.solver.set_b_eq(guanaqo::as_span(b_eq)); })
        .def("set_b_lb",
             [](Solver &self, crvec b_lb) { return self.solver.set_b_lb(guanaqo::as_span(b_lb)); })
        .def("set_b_ub",
             [](Solver &self, crvec b_ub) { return self.solver.set_b_ub(guanaqo::as_span(b_ub)); });
}

template <index_t VL>
void register_qpalm_cyqlone(py::module_ &m) {
    register_qpalm_solver<PythonCyqloneSolver<VL>, cyqlone::qpalm::CyqloneBackendSettings>(
        m, "QPALM_Cyqlone");
}

template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

} // namespace cyqlone

PYBIND11_MODULE(MODULE_NAME, m) {
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
    py::class_<guanaqo::TraceLogger::Log>(m, "TraceLog")
        .def_readonly("name", &guanaqo::TraceLogger::Log::name)
        .def_readonly("instance", &guanaqo::TraceLogger::Log::instance)
        .def_property_readonly(
            "start_time",
            [](const guanaqo::TraceLogger::Log &self) { return self.start_time.count(); })
        .def_property_readonly(
            "duration", [](const guanaqo::TraceLogger::Log &self) { return self.duration.count(); })
        .def_readonly("thread_id", &guanaqo::TraceLogger::Log::thread_id)
        .def_readonly("flop_count", &guanaqo::TraceLogger::Log::flop_count);
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
            cyqlone::foreach_thread([](index_t i, index_t) { GUANAQO_TRACE("thread_id", i); });
            guanaqo::TraceLogger::write_column_headings(csv) << '\n';
            for (const auto &log : guanaqo::trace_logger.get_logs())
                csv << log << '\n';
        },
        "filename"_a);
#endif
}
