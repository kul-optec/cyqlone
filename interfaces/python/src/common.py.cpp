#include <cyqlone/cyqlone-params.hpp>
#include <cyqlone/qpalm/backends/ocp-backend-cyqlone.hpp>
#include <cyqlone/qpalm/detailed-stats.hpp>
#include <cyqlone/qpalm/example-problems/conversion.hpp>
#include <cyqlone/qpalm/example-problems/platooning.hpp>
#include <cyqlone/qpalm/example-problems/spring-mass.hpp>
#include <cyqlone/qpalm/solver.hpp>
#include <cyqlone/qpalm/status.hpp>
#include <cyqlone/timing.hpp>
#include <cyqlone/tracing.hpp>
#include <batmat-version.h>
#include <cyqlone-version.h>

#include <batmat/loop.hpp>
#include <batmat/openmp.h>
#if BATMAT_WITH_OPENMP
#include <omp.h>
#else
#include <batmat/thread-pool.hpp>
#endif
#if GUANAQO_WITH_TRACING
#include <guanaqo/trace.hpp>
#include <nanobind/stl/filesystem.h>
#include <filesystem>
#include <fstream>
#endif

#include "common.py.hpp"

namespace cyqlone {

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
                         return std::make_tuple(self.ocp.dim.N_horiz, self.ocp.dim.nx,
                                                self.ocp.dim.nu, self.ocp.dim.ny,
                                                self.ocp.dim.ny_N);
                     })
        .def(
            "A", [](PythonOCP &self, index_t i) { return np_view(self.ocp.A(i)); },
            nb::rv_policy::reference_internal, "i"_a)
        .def(
            "B", [](PythonOCP &self, index_t i) { return np_view(self.ocp.B(i)); },
            nb::rv_policy::reference_internal, "i"_a)
        .def(
            "AB", [](PythonOCP &self, index_t i) { return np_view(self.ocp.AB(i)); },
            nb::rv_policy::reference_internal, "i"_a)
        .def(
            "b", [](PythonOCP &self, index_t i) { return np_view_vec(self.ocp.b(i)); },
            nb::rv_policy::reference_internal, "i"_a)
        .def(
            "C", [](PythonOCP &self, index_t i) { return np_view(self.ocp.C(i)); },
            nb::rv_policy::reference_internal, "i"_a)
        .def(
            "D", [](PythonOCP &self, index_t i) { return np_view(self.ocp.D(i)); },
            nb::rv_policy::reference_internal, "i"_a)
        .def(
            "CD", [](PythonOCP &self, index_t i) { return np_view(self.ocp.CD(i)); },
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
        .def(
            "H", [](PythonOCP &self, index_t i) { return np_view(self.ocp.H(i)); },
            nb::rv_policy::reference_internal, "i"_a)
        .def(
            "q", [](PythonOCP &self, index_t i) { return np_view_vec(self.ocp.q(i)); },
            nb::rv_policy::reference_internal, "i"_a)
        .def(
            "r", [](PythonOCP &self, index_t i) { return np_view_vec(self.ocp.r(i)); },
            nb::rv_policy::reference_internal, "i"_a)
        .def_prop_ro("N_horiz", [](const PythonOCP &self) { return self.ocp.dim.N_horiz; })
        .def_prop_ro("nx", [](const PythonOCP &self) { return self.ocp.dim.nx; })
        .def_prop_ro("nu", [](const PythonOCP &self) { return self.ocp.dim.nu; })
        .def_prop_ro("ny", [](const PythonOCP &self) { return self.ocp.dim.ny; })
        .def_prop_ro("ny_N", [](const PythonOCP &self) { return self.ocp.dim.ny_N; })
        .def("dump_mat", &PythonOCP::dump_mat)
        .def("load_mat", &PythonOCP::load_mat);

    using Solution = LinearOCPStorage::Solution;
    nb::class_<Solution>(m, "Solution")
        .def_prop_ro("solution", [](Solution &self) { return np_view_vec(self.solution); })
        .def_prop_ro("inequality_multipliers",
                     [](Solution &self) { return np_view_vec(self.inequality_multipliers); })
        .def_prop_ro("equality_multipliers",
                     [](Solution &self) { return np_view_vec(self.equality_multipliers); });

    using KKTError = LinearOCPStorage::KKTError;
    nb::class_<KKTError>(m, "KKTError")
        .def_ro("stationarity", &KKTError::stationarity)
        .def_ro("inequality_residual", &KKTError::inequality_residual)
        .def_ro("equality_residual", &KKTError::equality_residual)
        .def_ro("complementarity", &KKTError::complementarity);

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
    using CyOCP = CyqloneStorage<>;
    nb::class_<CyOCP>(m, "CyqloneOCP")
        .def(
            "__init__",
            [](CyOCP &self, const PythonOCP &ocp, index_t ny_0) {
                new (&self) CyOCP{CyOCP::build(ocp.ocp, ny_0)};
            },
            "ocp"_a, "ny_0"_a = -1)
        .def(
            "update", [](CyOCP &self, const PythonOCP &ocp) { self.update(ocp.ocp); }, "ocp"_a)
        .def_prop_ro("N_horiz", [](const CyOCP &self) { return self.N_horiz; })
        .def_prop_ro("nx", [](const CyOCP &self) { return self.nx; })
        .def_prop_ro("nu", [](const CyOCP &self) { return self.nu; })
        .def_prop_ro("ny", [](const CyOCP &self) { return self.ny; })
        .def_prop_ro("ny_0", [](const CyOCP &self) { return self.ny_0; })
        .def_prop_ro("ny_N", [](const CyOCP &self) { return self.ny_N; })
        .def_prop_ro("indices_G0", [](CyOCP &self) { return np_view_vec(self.indices_G0); })
        .def_prop_ro("data_H", [](CyOCP &self) { return np_view(self.data_H.view()); })
        .def_prop_ro("data_F", [](CyOCP &self) { return np_view(self.data_F.view()); })
        .def_prop_ro("data_G", [](CyOCP &self) { return np_view(self.data_G.view()); })
        .def_prop_ro("data_G0N", [](CyOCP &self) { return np_view(self.data_G0N.view()); })
        .def_prop_ro("data_rq", [](CyOCP &self) { return np_view(self.data_rq.view()); })
        .def_prop_ro("data_c", [](CyOCP &self) { return np_view(self.data_c.view()); })
        .def_prop_ro("data_lb", [](CyOCP &self) { return np_view(self.data_lb.view()); })
        .def_prop_ro("data_lb0N", [](CyOCP &self) { return np_view(self.data_lb0N.view()); })
        .def_prop_ro("data_ub", [](CyOCP &self) { return np_view(self.data_ub.view()); })
        .def_prop_ro("data_ub0N", [](CyOCP &self) { return np_view(self.data_ub0N.view()); })
        .def(
            "reconstruct_ineq_multipliers",
            [](const CyOCP &self, np_vector<> y_compressed) {
                return np_copy(self.reconstruct_ineq_multipliers(as_span(y_compressed)));
            },
            "y_compressed"_a)
        .def(
            "reconstruct_solution",
            [](const CyOCP &self, const PythonOCP &ocp, np_vector<> ux_compressed,
               np_vector<> y_compressed, np_vector<> λ_compressed) {
                return self.reconstruct_solution(ocp.ocp, as_span(ux_compressed),
                                                 as_span(y_compressed), as_span(λ_compressed));
            },
            "ocp"_a, "ux_compressed"_a, "y_compressed"_a, "λ_compressed"_a)
        .def(
            "compute_kkt_error",
            [](const CyOCP &self, const PythonOCP &ocp, np_vector<> ux_compressed,
               np_vector<> y_compressed, np_vector<> λ_compressed) {
                return self.compute_kkt_error(ocp.ocp, as_span(ux_compressed),
                                              as_span(y_compressed), as_span(λ_compressed));
            },
            "ocp"_a, "ux_compressed"_a, "y_compressed"_a, "λ_compressed"_a);
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
    nb::class_<cyqlone::qpalm::problems::SpringMassParams> spring_mass_params(m,
                                                                              "SpringMassParams");
    using ActuatorPlacement = cyqlone::qpalm::problems::SpringMassParams::ActuatorPlacement;
    nb::enum_<ActuatorPlacement>(spring_mass_params, "ActuatorPlacement")
        .value("IndividualActuators", ActuatorPlacement::IndividualActuators)
        .value("RandomActuators", ActuatorPlacement::RandomActuators)
        .value("RandomPairsOfActuators", ActuatorPlacement::RandomPairsOfActuators)
        .value("WangBoydActuators", ActuatorPlacement::WangBoydActuators)
        .export_values();
    spring_mass_params.def(nb::init<>())
        .def_rw("friction", &cyqlone::qpalm::problems::SpringMassParams::friction)
        .def_rw("k_spring", &cyqlone::qpalm::problems::SpringMassParams::k_spring)
        .def_rw("F_max", &cyqlone::qpalm::problems::SpringMassParams::F_max)
        .def_rw("p_min", &cyqlone::qpalm::problems::SpringMassParams::p_min)
        .def_rw("p_max", &cyqlone::qpalm::problems::SpringMassParams::p_max)
        .def_rw("p_min_f", &cyqlone::qpalm::problems::SpringMassParams::p_min_f)
        .def_rw("p_max_f", &cyqlone::qpalm::problems::SpringMassParams::p_max_f)
        .def_rw("v_max", &cyqlone::qpalm::problems::SpringMassParams::v_max)
        .def_rw("v_max_f", &cyqlone::qpalm::problems::SpringMassParams::v_max_f)
        .def_rw("width", &cyqlone::qpalm::problems::SpringMassParams::width)
        .def_rw("N_horiz", &cyqlone::qpalm::problems::SpringMassParams::N_horiz)
        .def_rw("T_horiz", &cyqlone::qpalm::problems::SpringMassParams::T_horiz)
        .def_rw("q_vel", &cyqlone::qpalm::problems::SpringMassParams::q_vel)
        .def_rw("q_pos", &cyqlone::qpalm::problems::SpringMassParams::q_pos)
        .def_rw("q_vel_f", &cyqlone::qpalm::problems::SpringMassParams::q_vel_f)
        .def_rw("q_pos_f", &cyqlone::qpalm::problems::SpringMassParams::q_pos_f)
        .def_rw("r_act", &cyqlone::qpalm::problems::SpringMassParams::r_act)
        .def_rw("masses", &cyqlone::qpalm::problems::SpringMassParams::masses)
        .def_rw("n_actuators", &cyqlone::qpalm::problems::SpringMassParams::n_actuators)
        .def_rw("actuator_placement",
                &cyqlone::qpalm::problems::SpringMassParams::actuator_placement)
        .def_rw("seed", &cyqlone::qpalm::problems::SpringMassParams::seed)
        .def_static("wang_boyd_2008", &cyqlone::qpalm::problems::SpringMassParams::wang_boyd_2008,
                    "n_masses"_a, "N_horiz"_a = 30, "seed"_a = 0)
        .def_static("domahidi_2012", &cyqlone::qpalm::problems::SpringMassParams::domahidi_2012,
                    "n_masses"_a, "N_horiz"_a, "seed"_a = 0);
    m.def(
        "create_spring_mass_problem",
        [](const cyqlone::qpalm::problems::SpringMassParams &params) {
            auto p = spring_mass(params);
            return PythonOCP{std::move(p.ocp)};
        },
        "params"_a.sig("SpringMassParams()") = cyqlone::qpalm::problems::SpringMassParams{});
}

void register_settings(nb::module_ &m) {
    nb::class_<cyqlone::TricyqleParams<>>(m, "TricyqleParams")
        .def(nb::init<>())
        .def_rw("enable_prefetching", &cyqlone::TricyqleParams<>::enable_prefetching)
        .def_rw("pcg_max_iter", &cyqlone::TricyqleParams<>::pcg_max_iter)
        .def_rw("pcg_tolerance", &cyqlone::TricyqleParams<>::pcg_tolerance)
        .def_rw("pcg_print_resid", &cyqlone::TricyqleParams<>::pcg_print_resid)
        .def_rw("solve_method", &cyqlone::TricyqleParams<>::solve_method)
        .def_rw("pcr_max_update_fraction", &cyqlone::TricyqleParams<>::pcr_max_update_fraction)
        .def_rw("cr_max_update_fraction_Y0", &cyqlone::TricyqleParams<>::cr_max_update_fraction_Y0)
        .def_rw("parallel_solve_cr_threshold",
                &cyqlone::TricyqleParams<>::parallel_solve_cr_threshold)
        .def_rw("parallel_factor_pcr_threshold",
                &cyqlone::TricyqleParams<>::parallel_factor_pcr_threshold);
    nb::class_<cyqlone::CyqloneParams<>>(m, "CyqloneParams").def(nb::init<>());
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
    nb::enum_<cyqlone::SolveMethod>(m, "SolveMethod")
        .value("StairPCG", cyqlone::SolveMethod::StairPCG)
        .value("JacobiPCG", cyqlone::SolveMethod::JacobiPCG)
        .value("PCR", cyqlone::SolveMethod::PCR);
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
                 new (self) T{
                     // clang-format off
                    .num_invocations = nb::cast<decltype(T::num_invocations)>(t[0]),
                    .wall_time = nb::cast<decltype(T::wall_time)>(t[1]),
                     // clang-format on
                 };
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
        .def_rw("processors", &cyqlone::qpalm::CyqloneBackendSettings::processors)
        .def_rw("print_residuals", &cyqlone::qpalm::CyqloneBackendSettings::print_residuals)
        .def_rw("print_precision", &cyqlone::qpalm::CyqloneBackendSettings::print_precision)
        .def_rw("changing_constr_factor",
                &cyqlone::qpalm::CyqloneBackendSettings::changing_constr_factor)
        .def_rw("max_update_count", &cyqlone::qpalm::CyqloneBackendSettings::max_update_count)
        .def_rw("detailed_timings", &cyqlone::qpalm::CyqloneBackendSettings::detailed_timings)
        .def_rw("tricyqle_params", &cyqlone::qpalm::CyqloneBackendSettings::tricyqle_params)
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
        .def_rw("recompute_penalty_gradient", &cyqlone::qpalm::Settings::recompute_penalty_gradient)
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

} // namespace cyqlone

#ifdef CYQLONE_WITH_PYTHON_DISPATCH
#include <cpu_features_macros.h>
#if defined(CPU_FEATURES_ARCH_X86)
#include <cpuinfo_x86.h>

static const char *get_dispatch_name() {
    using namespace cpu_features;
    const X86Features features = GetX86Info().features;
    if (features.avx512f)
        return "avx512";
    if (features.avx2)
        return "avx2";
    return "generic";
}
#else
static const char *get_dispatch_name() { return "generic"; }
#endif
#endif

NB_MODULE(MODULE_NAME, m) {
    using namespace cyqlone;
    m.doc()                      = "Python interface to cyqlone's C++ implementation.";
    m.attr("__version__")        = CYQLONE_VERSION_FULL;
    m.attr("build_time")         = CYQLONE_BUILD_TIME;
    m.attr("commit_hash")        = CYQLONE_COMMIT_HASH;
    m.attr("batmat_version")     = BATMAT_VERSION_FULL;
    m.attr("batmat_build_time")  = BATMAT_BUILD_TIME;
    m.attr("batmat_commit_hash") = BATMAT_COMMIT_HASH;
#if CYQLONE_WITH_TRACING
    m.attr("with_tracing") = true;
#else
    m.attr("with_tracing") = false;
#endif
#if CYQLONE_WITH_ZLIB
    m.attr("with_zlib") = true;
#else
    m.attr("with_zlib") = false;
#endif

    register_ocp(m);
    register_settings(m);

#ifdef CYQLONE_WITH_PYTHON_DISPATCH
    m.def("get_dispatch_name", &get_dispatch_name);
#endif
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
        auto l = guanaqo::get_trace_logger().get_logs();
        return std::vector<guanaqo::TraceLogger::Log>{l.begin(), l.end()};
    });
    m.def("reset_trace_log", [] { guanaqo::get_trace_logger().reset(); });
    m.def(
        "dump_trace_log",
        [](const std::filesystem::path &filename) {
            if (filename.has_parent_path())
                std::filesystem::create_directories(filename.parent_path());
            std::ofstream csv{filename};
            guanaqo::TraceLogger::write_column_headings(csv) << '\n';
            for (const auto &log : guanaqo::get_trace_logger().get_logs())
                csv << log << '\n';
        },
        "filename"_a);
    m.def(
        "dump_trace_log_chrome",
        [](const std::filesystem::path &filename) {
            if (filename.has_parent_path())
                std::filesystem::create_directories(filename.parent_path());
            cyqlone::write_chrome_trace(filename, guanaqo::get_trace_logger().get_logs());
        },
        "filename"_a);
#endif
}
