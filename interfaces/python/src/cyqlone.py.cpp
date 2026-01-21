#include <cyqlone/config.hpp>
#include <cyqlone/cyqlone-storage.hpp>
#include <cyqlone/cyqlone.hpp>
#include <cyqlone/qpalm/backends/ocp-backend-cyqlone.hpp>
#include <cyqlone/qpalm/solver.hpp>
#include <cyqlone/v2/cyqlone.hpp>
#include <batmat/assume.hpp>
#include <batmat/config.hpp>
#include <batmat/linalg/simdify.hpp> // TODO: remove
#include <guanaqo/eigen/span.hpp>    // TODO: remove
#include <guanaqo/eigen/view.hpp>

#include <format>
#include <stdexcept>

#include "common.py.hpp"

namespace CYQLONE_NS(cyqlone) {

template <index_t VL>
struct PythonCyqloneSolver {
    using Backend = cyqlone::qpalm::unique_CyqloneBackend<VL>;
    using Solver  = cyqlone::qpalm::Solver<Backend>;
    Solver solver;
    PythonCyqloneSolver(const CyqloneStorage<> &ocp,
                        cyqlone::qpalm::CyqloneBackendSettings backend_settings,
                        cyqlone::qpalm::Settings qpalm_settings)
        : solver{cyqlone::qpalm::make_qpalm_cyqlone_backend<VL>(ocp, {}, backend_settings),
                 qpalm_settings} {}
    PythonCyqloneSolver(const PythonOCP &ocp,
                        cyqlone::qpalm::CyqloneBackendSettings backend_settings,
                        cyqlone::qpalm::Settings qpalm_settings)
        : solver{cyqlone::qpalm::make_qpalm_cyqlone_backend<VL>(
                     cyqlone::CyqloneStorage<>::build(ocp.ocp), {}, backend_settings),
                 qpalm_settings} {}
};

template <class Solver>
void register_cyqlone_solver(nb::module_ &m) {
    static constexpr index_t VL           = Solver::vl;
    static constexpr auto view_as_batched = []<class T>(const np_batched_view<VL, T> &t) {
        using View = batmat::matrix::View<T, index_t, std::integral_constant<index_t, VL>, index_t,
                                          index_t, StorageOrder::ColMajor>;
        constexpr size_t alignment = VL * alignof(T);
        if (reinterpret_cast<uintptr_t>(t.data()) % alignment != 0)
            throw std::invalid_argument(std::format("Data pointer not aligned to {} bytes for "
                                                    "batched view, got {}",
                                                    alignment,
                                                    static_cast<const void *>(t.data())));
        if (t.shape(0) > 1 && t.stride(0) != 1)
            throw std::invalid_argument(std::format("Innermost stride should be unit for batched "
                                                    "view, got ({}, {}, {}, {}):({}, {}, {}, {})",
                                                    t.shape(0), t.shape(1), t.shape(2), t.shape(3),
                                                    t.stride(0), t.stride(1), t.stride(2),
                                                    t.stride(3)));
        if (t.shape(1) > 1 && t.stride(1) != VL)
            throw std::invalid_argument(std::format("Row stride should be VL for batched view, "
                                                    "got ({}, {}, {}, {}):({}, {}, {}, {})",
                                                    t.shape(0), t.shape(1), t.shape(2), t.shape(3),
                                                    t.stride(0), t.stride(1), t.stride(2),
                                                    t.stride(3)));
        if (t.shape(2) > 1 && t.stride(2) % VL != 0)
            throw std::invalid_argument(std::format("VL should divide column stride for batched "
                                                    "view, got ({}, {}, {}, {}):({}, {}, {}, {})",
                                                    t.shape(0), t.shape(1), t.shape(2), t.shape(3),
                                                    t.stride(0), t.stride(1), t.stride(2),
                                                    t.stride(3)));
        if (t.shape(3) > 1 && t.stride(3) % VL != 0)
            throw std::invalid_argument(std::format("VL should divide layer stride for batched "
                                                    "view, got ({}, {}, {}, {}):({}, {}, {}, {})",
                                                    t.shape(0), t.shape(1), t.shape(2), t.shape(3),
                                                    t.stride(0), t.stride(1), t.stride(2),
                                                    t.stride(3)));
        return View{{
            .data         = t.data(),
            .depth        = static_cast<index_t>(t.shape(0) * t.shape(3)),
            .rows         = static_cast<index_t>(t.shape(1)),
            .cols         = static_cast<index_t>(t.shape(2)),
            .outer_stride = static_cast<index_t>(t.stride(2) / VL),
            .layer_stride = static_cast<index_t>(t.stride(3) / VL),
        }};
    };

    nb::class_<Solver> solver(m, "CyqloneSolver");
    solver //
        .def(
            "__init__",
            [](Solver &self, const CyqloneStorage<> &ocp, index_t lP) {
                new (&self) Solver(Solver::build(ocp, lP));
            },
            "ocp"_a, "lP"_a)
        .def(
            "__init__",
            [](Solver &self, const PythonOCP &ocp, index_t lP) {
                new (&self) Solver(Solver::build(CyqloneStorage<>::build(ocp.ocp), lP));
            },
            "ocp"_a, "lP"_a)
        .def_prop_ro("params_string", [](const Solver &self) { return self.get_params_string(); })
        .def_prop_ro("num_variables", &Solver::num_variables)
        .def_prop_ro("num_dynamics_constraints", &Solver::num_dynamics_constraints)
        .def_prop_ro("num_general_constraints", &Solver::num_general_constraints)
        .def_rw("enable_prefetching", &Solver::enable_prefetching)
        .def_rw("pcg_max_iter", &Solver::pcg_max_iter)
        .def_rw("pcg_tolerance", &Solver::pcg_tolerance)
        .def_rw("pcg_print_resid", &Solver::pcg_print_resid)
        .def_rw("solve_method", &Solver::solve_method)
        .def_rw("pcr_max_update_fraction", &Solver::pcr_max_update_fraction)
        .def_rw("cr_max_update_fraction", &Solver::cr_max_update_fraction)
        .def(
            "initialize_rhs",
            [](Solver &self, const CyqloneStorage<> &ocp, np_batched_view<VL, real_t> rhs) {
                auto rhs_vw = view_as_batched(rhs);
                self.initialize_rhs(ocp, rhs_vw);
            },
            "ocp"_a, "rhs"_a.noconvert())
        .def(
            "initialize_rhs",
            [](Solver &self, const CyqloneStorage<> &ocp) {
                return np_copy(self.initialize_rhs(ocp));
            },
            "ocp"_a)
        .def(
            "initialize_gradient",
            [](Solver &self, const CyqloneStorage<> &ocp, np_batched_view<VL, real_t> grad) {
                auto grad_vw = view_as_batched(grad);
                self.initialize_gradient(ocp, grad_vw);
            },
            "ocp"_a, "grad"_a.noconvert())
        .def(
            "initialize_gradient",
            [](Solver &self, const CyqloneStorage<> &ocp) {
                return np_copy(self.initialize_gradient(ocp));
            },
            "ocp"_a)
        .def(
            "initialize_bounds",
            [](Solver &self, const CyqloneStorage<> &ocp, np_batched_view<VL, real_t> b_min,
               np_batched_view<VL, real_t> b_max) {
                auto b_min_vw = view_as_batched(b_min);
                auto b_max_vw = view_as_batched(b_max);
                self.initialize_bounds(ocp, b_min_vw, b_max_vw);
            },
            "ocp"_a, "b_min"_a.noconvert(), "b_max"_a.noconvert())
        .def(
            "initialize_bounds",
            [](Solver &self, const CyqloneStorage<> &ocp) {
                auto [b_min, b_max] = self.initialize_bounds(ocp);
                return std::make_pair(np_copy(std::move(b_min)), np_copy(std::move(b_max)));
            },
            "ocp"_a)
        .def(
            "pack_variables",
            [](Solver &self, np_vector<const real_t> ux_lin, np_batched_view<VL, real_t> ux) {
                auto ux_vw = view_as_batched(ux);
                self.pack_variables(as_span(ux_lin), ux_vw);
            },
            "ux_lin"_a, "ux"_a.noconvert())
        .def(
            "pack_variables",
            [](Solver &self, np_vector<const real_t> ux_lin) {
                return np_copy(self.pack_variables(as_span(ux_lin)));
            },
            "ux_lin"_a)
        .def(
            "unpack_variables",
            [](Solver &self, np_batched_view<VL, const real_t> ux) {
                auto ux_vw = view_as_batched(ux);
                return np_copy(self.unpack_variables(ux_vw));
            },
            "ux"_a.noconvert())
        .def(
            "pack_dynamics",
            [](Solver &self, np_vector<const real_t> λ_lin, np_batched_view<VL, real_t> λ) {
                auto λ_vw = view_as_batched(λ);
                self.pack_dynamics(as_span(λ_lin), λ_vw);
            },
            "λ_lin"_a, "λ"_a.noconvert())
        .def(
            "pack_dynamics",
            [](Solver &self, np_vector<const real_t> λ_lin) {
                return np_copy(self.pack_dynamics(as_span(λ_lin)));
            },
            "λ_lin"_a)
        .def(
            "unpack_dynamics",
            [](Solver &self, np_batched_view<VL, const real_t> λ) {
                auto λ_vw = view_as_batched(λ);
                return np_copy(self.unpack_dynamics(λ_vw));
            },
            "λ"_a.noconvert())
        .def(
            "pack_constraints",
            [](Solver &self, np_vector<const real_t> y_lin, np_batched_view<VL, real_t> y,
               real_t fill) {
                auto y_vw = view_as_batched(y);
                self.pack_constraints(as_span(y_lin), y_vw, fill);
            },
            "y_lin"_a, "y"_a.noconvert(), "fill"_a = 0)
        .def(
            "pack_constraints",
            [](Solver &self, np_vector<const real_t> y_lin, real_t fill) {
                return np_copy(self.pack_constraints(as_span(y_lin), fill));
            },
            "y_lin"_a, "fill"_a = 0)
        .def(
            "unpack_constraints",
            [](Solver &self, np_batched_view<VL, const real_t> y) {
                auto y_vw = view_as_batched(y);
                return np_copy(self.unpack_constraints(y_vw));
            },
            "y"_a.noconvert())
        .def(
            "factor",
            [](Solver &self, real_t S, np_batched_view<VL, const real_t> Σ) {
                auto Σ_vw = view_as_batched(Σ);
                self.parallel_ctx->run([&](auto &ctx) { self.factor(ctx, S, Σ_vw); });
            },
            "S"_a, "Σ"_a.noconvert())
        .def(
            "factor_solve",
            [](Solver &self, real_t S, np_batched_view<VL, const real_t> Σ,
               np_batched_view<VL, real_t> ux, np_batched_view<VL, real_t> λ) {
                auto Σ_vw  = view_as_batched(Σ);
                auto ux_vw = view_as_batched(ux);
                auto λ_vw  = view_as_batched(λ);
                self.parallel_ctx->run(
                    [&](auto &ctx) { self.factor_solve(ctx, S, Σ_vw, ux_vw, λ_vw); });
            },
            "S"_a, "Σ"_a.noconvert(), "ux"_a.noconvert(), "λ"_a.noconvert())
        .def(
            "factor_solve",
            [](Solver &self, real_t S, np_batched_view<VL, const real_t> Σ,
               const CyqloneStorage<> &ocp) {
                auto Σ_vw = view_as_batched(Σ);
                auto ux   = self.initialize_gradient(ocp);
                Solver::compact_blas::xneg(batmat::linalg::simdify(ux)); // TODO: remove
                auto λ = self.initialize_rhs(ocp);
                self.parallel_ctx->run([&](auto &ctx) { self.factor_solve(ctx, S, Σ_vw, ux, λ); });
                return std::make_tuple(np_copy(std::move(ux)), np_copy(std::move(λ)));
            },
            "S"_a, "Σ"_a.noconvert(), "ocp"_a)
        .def(
            "residual_dynamics_constr",
            [](Solver &self, np_batched_view<VL, const real_t> x,
               np_batched_view<VL, const real_t> b) {
                auto x_vw = view_as_batched(x);
                auto b_vw = view_as_batched(b);
                auto Mxb  = self.initialize_dynamics_constraints();
                self.parallel_ctx->run(
                    [&](auto &ctx) { self.residual_dynamics_constr(ctx, x_vw, b_vw, Mxb); });
                return np_copy(std::move(Mxb));
            },
            "ux"_a.noconvert(), "b"_a.noconvert())
        .def(
            "residual_dynamics_constr",
            [](Solver &self, np_batched_view<VL, const real_t> x, const CyqloneStorage<> &ocp) {
                auto x_vw = view_as_batched(x);
                auto b    = self.initialize_rhs(ocp);
                auto Mxb  = self.initialize_dynamics_constraints();
                self.parallel_ctx->run(
                    [&](auto &ctx) { self.residual_dynamics_constr(ctx, x_vw, b, Mxb); });
                return np_copy(std::move(Mxb));
            },
            "ux"_a.noconvert(), "b"_a.noconvert())
        .def(
            "transposed_dynamics_constr",
            [](Solver &self, np_batched_view<VL, const real_t> λ) {
                auto λ_vw = view_as_batched(λ);
                auto Mᵀλ  = self.initialize_variables();
                self.parallel_ctx->run(
                    [&](auto &ctx) { self.transposed_dynamics_constr(ctx, λ_vw, Mᵀλ); });
                return np_copy(std::move(Mᵀλ));
            },
            "λ"_a.noconvert())
        .def(
            "solve_forward",
            [](Solver &self, np_batched_view<VL, real_t> ux, np_batched_view<VL, real_t> λ) {
                auto ux_vw = view_as_batched(ux);
                auto λ_vw  = view_as_batched(λ);
                self.parallel_ctx->run([&](auto &ctx) { self.solve_forward(ctx, ux_vw, λ_vw); });
            },
            "ux"_a.noconvert(), "λ"_a.noconvert())
        .def(
            "solve_reverse",
            [](Solver &self, np_batched_view<VL, real_t> ux, np_batched_view<VL, real_t> λ) {
                auto ux_vw = view_as_batched(ux);
                auto λ_vw  = view_as_batched(λ);
                self.parallel_ctx->run(
                    [&](auto &ctx) { self.solve_reverse(ctx, ux_vw, λ_vw, self.riccati_work); });
            },
            "ux"_a.noconvert(), "λ"_a.noconvert())
        .def("build_sparse",
             [](Solver &self, const PythonOCP &ocp, np_vector<const real_t> Σ) {
                 return self.build_sparse(CyqloneStorage<>::build(ocp.ocp), as_span(Σ));
             })
        .def("build_rhs",
             [](Solver &self, np_batched_view<VL, const real_t> ux,
                np_batched_view<VL, const real_t> λ) {
                 return np_copy(self.build_rhs(view_as_batched(ux), view_as_batched(λ)));
             })
        .def("build_sparse_factor", [](Solver &self) { return self.build_sparse_factor(); })
        .def("build_sparse_diag", [](Solver &self) { return self.build_sparse_diag(); });

#if GUANAQO_WITH_TRACING
    solver.def("log_thread_names", [](Solver &self) {
        self.parallel_ctx->run([](auto &ctx) { GUANAQO_TRACE("thread_id", ctx.index); });
    });
#endif

    if constexpr (requires { &Solver::solve; })
        solver.def(
            "solve",
            [](Solver &self, np_batched_view<VL, real_t> ux, np_batched_view<VL, real_t> λ) {
                auto ux_vw = view_as_batched(ux);
                auto λ_vw  = view_as_batched(λ);
                self.parallel_ctx->run([&](auto &ctx) { self.solve(ctx, ux_vw, λ_vw); });
            },
            "ux"_a.noconvert(), "λ"_a.noconvert());
    if constexpr (requires { &Solver::solve_forward_new; })
        solver.def(
            "solve_forward_new",
            [](Solver &self, np_batched_view<VL, real_t> ux, np_batched_view<VL, real_t> λ) {
                auto ux_vw = view_as_batched(ux);
                auto λ_vw  = view_as_batched(λ);
                self.parallel_ctx->run(
                    [&](auto &ctx) { self.solve_forward_new(ctx, ux_vw, λ_vw); });
            },
            "ux"_a.noconvert(), "λ"_a.noconvert());
    if constexpr (requires { &Solver::solve_reverse_new; })
        solver.def(
            "solve_reverse_new",
            [](Solver &self, np_batched_view<VL, real_t> ux, np_batched_view<VL, real_t> λ) {
                auto ux_vw = view_as_batched(ux);
                auto λ_vw  = view_as_batched(λ);
                self.parallel_ctx->run([&](auto &ctx) {
                    self.solve_reverse_new(ctx, ux_vw, λ_vw, self.riccati_work);
                });
            },
            "ux"_a.noconvert(), "λ"_a.noconvert());
}

template <class Solver, class BackendSettings>
void register_qpalm_solver(nb::module_ &m, const char *name) {
    nb::class_<Solver> solver(m, name);
    solver //
        .def(nb::init<const CyqloneStorage<> &, BackendSettings, cyqlone::qpalm::Settings>(),
             "ocp"_a, "backend_settings"_a, "qpalm_settings"_a)
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

#define CYQLONE_INSTANTIATE_MODULE(VL)                                                             \
    auto m##VL = m.def_submodule(VL == 1 ? "scalar" : "simd" #VL);                                 \
    cyqlone::register_qpalm_cyqlone<VL>(m##VL);                                                    \
    auto m##VL##_v2 = m##VL.def_submodule("v2");                                                   \
    cyqlone::register_cyqlone_solver<cyqlone::v2::CyqloneSolver<VL>>(m##VL##_v2);

NB_MODULE(MODULE_NAME, m) { BATMAT_FOREACH_VL_DOUBLE(CYQLONE_INSTANTIATE_MODULE) }
