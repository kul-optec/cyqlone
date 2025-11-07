#include <cyqlone/config.hpp>
#include <cyqlone/qpalm/backends/ocp-backend-cyqlone.hpp>
#include <cyqlone/qpalm/solver.hpp>
#include <batmat/assume.hpp>
#include <guanaqo/eigen/span.hpp> // TODO: remove
#include <guanaqo/eigen/view.hpp>

#include <stdexcept>

#include "common.py.hpp"

namespace CYQLONE_NS(cyqlone) {

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

} // namespace CYQLONE_NS(cyqlone)

NB_MODULE(MODULE_NAME, m) {
    auto simd8  = m.def_submodule("simd8");
    auto simd4  = m.def_submodule("simd4");
    auto scalar = m.def_submodule("scalar");
    cyqlone::register_qpalm_cyqlone<8>(simd8);
    cyqlone::register_qpalm_cyqlone<4>(simd4);
    cyqlone::register_qpalm_cyqlone<1>(scalar);
}
