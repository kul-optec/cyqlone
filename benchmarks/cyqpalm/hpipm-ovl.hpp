#pragma once

#include <hpipm_common.h>
#include <hpipm_d_ocp_qp.h>
#include <hpipm_d_ocp_qp_dim.h>
#include <hpipm_d_ocp_qp_ipm.h>
#include <hpipm_d_ocp_qp_sol.h>
#include <hpipm_s_ocp_qp.h>
#include <hpipm_s_ocp_qp_dim.h>
#include <hpipm_s_ocp_qp_ipm.h>
#include <hpipm_s_ocp_qp_sol.h>

namespace hpipm {

// ================================================================================================
// Alias templates for structs
// ================================================================================================

template <class Scalar>
struct ocp_qp_types;

template <>
struct ocp_qp_types<double> {
    using dim     = ::d_ocp_qp_dim;
    using qp      = ::d_ocp_qp;
    using sol     = ::d_ocp_qp_sol;
    using ipm_arg = ::d_ocp_qp_ipm_arg;
    using ipm_ws  = ::d_ocp_qp_ipm_ws;
};

template <>
struct ocp_qp_types<float> {
    using dim     = ::s_ocp_qp_dim;
    using qp      = ::s_ocp_qp;
    using sol     = ::s_ocp_qp_sol;
    using ipm_arg = ::s_ocp_qp_ipm_arg;
    using ipm_ws  = ::s_ocp_qp_ipm_ws;
};

template <class Scalar>
using ocp_qp_dim = typename ocp_qp_types<Scalar>::dim;
template <class Scalar>
using ocp_qp = typename ocp_qp_types<Scalar>::qp;
template <class Scalar>
using ocp_qp_sol = typename ocp_qp_types<Scalar>::sol;
template <class Scalar>
using ocp_qp_ipm_arg = typename ocp_qp_types<Scalar>::ipm_arg;
template <class Scalar>
using ocp_qp_ipm_ws = typename ocp_qp_types<Scalar>::ipm_ws;

template <typename Scalar>
hpipm_size_t ocp_qp_dim_memsize(int N) = delete;

// ================================================================================================
// d_ocp_qp_dim functions
// ================================================================================================

template <>
inline hpipm_size_t ocp_qp_dim_memsize<double>(int N) {
    return ::d_ocp_qp_dim_memsize(N);
}

inline void ocp_qp_dim_create(int N, ::d_ocp_qp_dim *qp_dim, void *memory) {
    ::d_ocp_qp_dim_create(N, qp_dim, memory);
}

inline void ocp_qp_dim_set_nx(int stage, int value, ::d_ocp_qp_dim *dim) {
    ::d_ocp_qp_dim_set_nx(stage, value, dim);
}

inline void ocp_qp_dim_set_nu(int stage, int value, ::d_ocp_qp_dim *dim) {
    ::d_ocp_qp_dim_set_nu(stage, value, dim);
}

inline void ocp_qp_dim_set_nbx(int stage, int value, ::d_ocp_qp_dim *dim) {
    ::d_ocp_qp_dim_set_nbx(stage, value, dim);
}

inline void ocp_qp_dim_set_nbxe(int stage, int value, ::d_ocp_qp_dim *dim) {
    ::d_ocp_qp_dim_set_nbxe(stage, value, dim);
}

inline void ocp_qp_dim_set_ng(int stage, int value, ::d_ocp_qp_dim *dim) {
    ::d_ocp_qp_dim_set_ng(stage, value, dim);
}

// ================================================================================================
// d_ocp_qp functions
// ================================================================================================

inline hpipm_size_t ocp_qp_memsize(::d_ocp_qp_dim *dim) { return ::d_ocp_qp_memsize(dim); }

inline void ocp_qp_create(::d_ocp_qp_dim *dim, ::d_ocp_qp *qp, void *memory) {
    ::d_ocp_qp_create(dim, qp, memory);
}

inline void ocp_qp_set_A(int stage, double *mat, ::d_ocp_qp *qp) {
    ::d_ocp_qp_set_A(stage, mat, qp);
}

inline void ocp_qp_set_B(int stage, double *mat, ::d_ocp_qp *qp) {
    ::d_ocp_qp_set_B(stage, mat, qp);
}

inline void ocp_qp_set_b(int stage, double *vec, ::d_ocp_qp *qp) {
    ::d_ocp_qp_set_b(stage, vec, qp);
}

inline void ocp_qp_set_Q(int stage, double *mat, ::d_ocp_qp *qp) {
    ::d_ocp_qp_set_Q(stage, mat, qp);
}

inline void ocp_qp_set_S(int stage, double *mat, ::d_ocp_qp *qp) {
    ::d_ocp_qp_set_S(stage, mat, qp);
}

inline void ocp_qp_set_R(int stage, double *mat, ::d_ocp_qp *qp) {
    ::d_ocp_qp_set_R(stage, mat, qp);
}

inline void ocp_qp_set_q(int stage, double *vec, ::d_ocp_qp *qp) {
    ::d_ocp_qp_set_q(stage, vec, qp);
}

inline void ocp_qp_set_r(int stage, double *vec, ::d_ocp_qp *qp) {
    ::d_ocp_qp_set_r(stage, vec, qp);
}

inline void ocp_qp_set_lbx(int stage, double *vec, ::d_ocp_qp *qp) {
    ::d_ocp_qp_set_lbx(stage, vec, qp);
}

inline void ocp_qp_set_ubx(int stage, double *vec, ::d_ocp_qp *qp) {
    ::d_ocp_qp_set_ubx(stage, vec, qp);
}

inline void ocp_qp_set_Jbx(int stage, double *vec, ::d_ocp_qp *qp) {
    ::d_ocp_qp_set_Jbx(stage, vec, qp);
}

inline void ocp_qp_set_C(int stage, double *mat, ::d_ocp_qp *qp) {
    ::d_ocp_qp_set_C(stage, mat, qp);
}

inline void ocp_qp_set_D(int stage, double *mat, ::d_ocp_qp *qp) {
    ::d_ocp_qp_set_D(stage, mat, qp);
}

inline void ocp_qp_set_lg(int stage, double *vec, ::d_ocp_qp *qp) {
    ::d_ocp_qp_set_lg(stage, vec, qp);
}

inline void ocp_qp_set_ug(int stage, double *vec, ::d_ocp_qp *qp) {
    ::d_ocp_qp_set_ug(stage, vec, qp);
}

// ================================================================================================
// d_ocp_qp_sol functions
// ================================================================================================

inline hpipm_size_t ocp_qp_sol_memsize(::d_ocp_qp_dim *dim) { return ::d_ocp_qp_sol_memsize(dim); }

inline void ocp_qp_sol_create(::d_ocp_qp_dim *dim, ::d_ocp_qp_sol *qp_sol, void *memory) {
    ::d_ocp_qp_sol_create(dim, qp_sol, memory);
}

inline void ocp_qp_sol_get_u(int stage, ::d_ocp_qp_sol *qp_sol, double *vec) {
    ::d_ocp_qp_sol_get_u(stage, qp_sol, vec);
}

inline void ocp_qp_sol_get_x(int stage, ::d_ocp_qp_sol *qp_sol, double *vec) {
    ::d_ocp_qp_sol_get_x(stage, qp_sol, vec);
}

inline void ocp_qp_sol_get_pi(int stage, ::d_ocp_qp_sol *qp_sol, double *vec) {
    ::d_ocp_qp_sol_get_pi(stage, qp_sol, vec);
}

inline void ocp_qp_sol_get_lam_lbx(int stage, ::d_ocp_qp_sol *qp_sol, double *vec) {
    ::d_ocp_qp_sol_get_lam_lbx(stage, qp_sol, vec);
}

inline void ocp_qp_sol_get_lam_ubx(int stage, ::d_ocp_qp_sol *qp_sol, double *vec) {
    ::d_ocp_qp_sol_get_lam_ubx(stage, qp_sol, vec);
}

inline void ocp_qp_sol_get_lam_lg(int stage, ::d_ocp_qp_sol *qp_sol, double *vec) {
    ::d_ocp_qp_sol_get_lam_lg(stage, qp_sol, vec);
}

inline void ocp_qp_sol_get_lam_ug(int stage, ::d_ocp_qp_sol *qp_sol, double *vec) {
    ::d_ocp_qp_sol_get_lam_ug(stage, qp_sol, vec);
}

inline void ocp_qp_sol_set_u(int stage, double *vec, ::d_ocp_qp_sol *qp_sol) {
    ::d_ocp_qp_sol_set_u(stage, vec, qp_sol);
}

inline void ocp_qp_sol_set_x(int stage, double *vec, ::d_ocp_qp_sol *qp_sol) {
    ::d_ocp_qp_sol_set_x(stage, vec, qp_sol);
}

// ================================================================================================
// d_ocp_qp_ipm_arg functions
// ================================================================================================

inline hpipm_size_t ocp_qp_ipm_arg_memsize(::d_ocp_qp_dim *ocp_dim) {
    return ::d_ocp_qp_ipm_arg_memsize(ocp_dim);
}

inline void ocp_qp_ipm_arg_create(::d_ocp_qp_dim *ocp_dim, ::d_ocp_qp_ipm_arg *arg, void *mem) {
    ::d_ocp_qp_ipm_arg_create(ocp_dim, arg, mem);
}

inline void ocp_qp_ipm_arg_set_default(enum hpipm_mode mode, ::d_ocp_qp_ipm_arg *arg) {
    ::d_ocp_qp_ipm_arg_set_default(mode, arg);
}

inline void ocp_qp_ipm_arg_set_iter_max(int *iter_max, ::d_ocp_qp_ipm_arg *arg) {
    ::d_ocp_qp_ipm_arg_set_iter_max(iter_max, arg);
}

inline void ocp_qp_ipm_arg_set_tol_stat(double *tol_stat, ::d_ocp_qp_ipm_arg *arg) {
    ::d_ocp_qp_ipm_arg_set_tol_stat(tol_stat, arg);
}

inline void ocp_qp_ipm_arg_set_tol_eq(double *tol_eq, ::d_ocp_qp_ipm_arg *arg) {
    ::d_ocp_qp_ipm_arg_set_tol_eq(tol_eq, arg);
}

inline void ocp_qp_ipm_arg_set_tol_ineq(double *tol_ineq, ::d_ocp_qp_ipm_arg *arg) {
    ::d_ocp_qp_ipm_arg_set_tol_ineq(tol_ineq, arg);
}

inline void ocp_qp_ipm_arg_set_tol_comp(double *tol_comp, ::d_ocp_qp_ipm_arg *arg) {
    ::d_ocp_qp_ipm_arg_set_tol_comp(tol_comp, arg);
}

inline void ocp_qp_ipm_arg_set_warm_start(int *warm_start, ::d_ocp_qp_ipm_arg *arg) {
    ::d_ocp_qp_ipm_arg_set_warm_start(warm_start, arg);
}

inline void ocp_qp_ipm_arg_set_mu0(double *mu0, ::d_ocp_qp_ipm_arg *arg) {
    ::d_ocp_qp_ipm_arg_set_mu0(mu0, arg);
}

inline void ocp_qp_ipm_arg_set_alpha_min(double *alpha_min, ::d_ocp_qp_ipm_arg *arg) {
    ::d_ocp_qp_ipm_arg_set_alpha_min(alpha_min, arg);
}

inline void ocp_qp_ipm_arg_set_reg_prim(double *reg_prim, ::d_ocp_qp_ipm_arg *arg) {
    ::d_ocp_qp_ipm_arg_set_reg_prim(reg_prim, arg);
}

inline void ocp_qp_ipm_arg_set_lam_min(double *lam_min, ::d_ocp_qp_ipm_arg *arg) {
    ::d_ocp_qp_ipm_arg_set_lam_min(lam_min, arg);
}

inline void ocp_qp_ipm_arg_set_t_min(double *t_min, ::d_ocp_qp_ipm_arg *arg) {
    ::d_ocp_qp_ipm_arg_set_t_min(t_min, arg);
}

inline void ocp_qp_ipm_arg_set_tau_min(double *tau_min, ::d_ocp_qp_ipm_arg *arg) {
    ::d_ocp_qp_ipm_arg_set_tau_min(tau_min, arg);
}

// ================================================================================================
// d_ocp_qp_ipm_ws functions
// ================================================================================================

inline hpipm_size_t ocp_qp_ipm_ws_memsize(::d_ocp_qp_dim *ocp_dim, ::d_ocp_qp_ipm_arg *arg) {
    return ::d_ocp_qp_ipm_ws_memsize(ocp_dim, arg);
}

inline void ocp_qp_ipm_ws_create(::d_ocp_qp_dim *ocp_dim, ::d_ocp_qp_ipm_arg *arg,
                                 ::d_ocp_qp_ipm_ws *ws, void *mem) {
    ::d_ocp_qp_ipm_ws_create(ocp_dim, arg, ws, mem);
}

inline void ocp_qp_ipm_get_status(::d_ocp_qp_ipm_ws *ws, int *status) {
    ::d_ocp_qp_ipm_get_status(ws, status);
}

inline void ocp_qp_ipm_get_iter(::d_ocp_qp_ipm_ws *ws, int *iter) {
    ::d_ocp_qp_ipm_get_iter(ws, iter);
}

inline void ocp_qp_ipm_get_max_res_stat(::d_ocp_qp_ipm_ws *ws, double *res_stat) {
    ::d_ocp_qp_ipm_get_max_res_stat(ws, res_stat);
}

inline void ocp_qp_ipm_get_max_res_eq(::d_ocp_qp_ipm_ws *ws, double *res_eq) {
    ::d_ocp_qp_ipm_get_max_res_eq(ws, res_eq);
}

inline void ocp_qp_ipm_get_max_res_ineq(::d_ocp_qp_ipm_ws *ws, double *res_ineq) {
    ::d_ocp_qp_ipm_get_max_res_ineq(ws, res_ineq);
}

inline void ocp_qp_ipm_get_max_res_comp(::d_ocp_qp_ipm_ws *ws, double *res_comp) {
    ::d_ocp_qp_ipm_get_max_res_comp(ws, res_comp);
}

// ================================================================================================
// d_ocp_qp_ipm_solve
// ================================================================================================

inline void ocp_qp_ipm_solve(::d_ocp_qp *qp, ::d_ocp_qp_sol *qp_sol, ::d_ocp_qp_ipm_arg *arg,
                             ::d_ocp_qp_ipm_ws *ws) {
    ::d_ocp_qp_ipm_solve(qp, qp_sol, arg, ws);
}

// ================================================================================================
// s_ocp_qp_dim functions (float)
// ================================================================================================

template <>
inline hpipm_size_t ocp_qp_dim_memsize<float>(int N) {
    return ::s_ocp_qp_dim_memsize(N);
}

inline void ocp_qp_dim_create(int N, ::s_ocp_qp_dim *qp_dim, void *memory) {
    ::s_ocp_qp_dim_create(N, qp_dim, memory);
}

inline void ocp_qp_dim_set_nx(int stage, int value, ::s_ocp_qp_dim *dim) {
    ::s_ocp_qp_dim_set_nx(stage, value, dim);
}

inline void ocp_qp_dim_set_nu(int stage, int value, ::s_ocp_qp_dim *dim) {
    ::s_ocp_qp_dim_set_nu(stage, value, dim);
}

inline void ocp_qp_dim_set_nbx(int stage, int value, ::s_ocp_qp_dim *dim) {
    ::s_ocp_qp_dim_set_nbx(stage, value, dim);
}

inline void ocp_qp_dim_set_nbxe(int stage, int value, ::s_ocp_qp_dim *dim) {
    ::s_ocp_qp_dim_set_nbxe(stage, value, dim);
}

inline void ocp_qp_dim_set_ng(int stage, int value, ::s_ocp_qp_dim *dim) {
    ::s_ocp_qp_dim_set_ng(stage, value, dim);
}

// ================================================================================================
// s_ocp_qp functions (float)
// ================================================================================================

inline hpipm_size_t ocp_qp_memsize(::s_ocp_qp_dim *dim) { return ::s_ocp_qp_memsize(dim); }

inline void ocp_qp_create(::s_ocp_qp_dim *dim, ::s_ocp_qp *qp, void *memory) {
    ::s_ocp_qp_create(dim, qp, memory);
}

inline void ocp_qp_set_A(int stage, float *mat, ::s_ocp_qp *qp) {
    ::s_ocp_qp_set_A(stage, mat, qp);
}

inline void ocp_qp_set_B(int stage, float *mat, ::s_ocp_qp *qp) {
    ::s_ocp_qp_set_B(stage, mat, qp);
}

inline void ocp_qp_set_b(int stage, float *vec, ::s_ocp_qp *qp) {
    ::s_ocp_qp_set_b(stage, vec, qp);
}

inline void ocp_qp_set_Q(int stage, float *mat, ::s_ocp_qp *qp) {
    ::s_ocp_qp_set_Q(stage, mat, qp);
}

inline void ocp_qp_set_S(int stage, float *mat, ::s_ocp_qp *qp) {
    ::s_ocp_qp_set_S(stage, mat, qp);
}

inline void ocp_qp_set_R(int stage, float *mat, ::s_ocp_qp *qp) {
    ::s_ocp_qp_set_R(stage, mat, qp);
}

inline void ocp_qp_set_q(int stage, float *vec, ::s_ocp_qp *qp) {
    ::s_ocp_qp_set_q(stage, vec, qp);
}

inline void ocp_qp_set_r(int stage, float *vec, ::s_ocp_qp *qp) {
    ::s_ocp_qp_set_r(stage, vec, qp);
}

inline void ocp_qp_set_lbx(int stage, float *vec, ::s_ocp_qp *qp) {
    ::s_ocp_qp_set_lbx(stage, vec, qp);
}

inline void ocp_qp_set_ubx(int stage, float *vec, ::s_ocp_qp *qp) {
    ::s_ocp_qp_set_ubx(stage, vec, qp);
}

inline void ocp_qp_set_Jbx(int stage, float *vec, ::s_ocp_qp *qp) {
    ::s_ocp_qp_set_Jbx(stage, vec, qp);
}

inline void ocp_qp_set_C(int stage, float *mat, ::s_ocp_qp *qp) {
    ::s_ocp_qp_set_C(stage, mat, qp);
}

inline void ocp_qp_set_D(int stage, float *mat, ::s_ocp_qp *qp) {
    ::s_ocp_qp_set_D(stage, mat, qp);
}

inline void ocp_qp_set_lg(int stage, float *vec, ::s_ocp_qp *qp) {
    ::s_ocp_qp_set_lg(stage, vec, qp);
}

inline void ocp_qp_set_ug(int stage, float *vec, ::s_ocp_qp *qp) {
    ::s_ocp_qp_set_ug(stage, vec, qp);
}

// ================================================================================================
// s_ocp_qp_sol functions (float)
// ================================================================================================

inline hpipm_size_t ocp_qp_sol_memsize(::s_ocp_qp_dim *dim) { return ::s_ocp_qp_sol_memsize(dim); }

inline void ocp_qp_sol_create(::s_ocp_qp_dim *dim, ::s_ocp_qp_sol *qp_sol, void *memory) {
    ::s_ocp_qp_sol_create(dim, qp_sol, memory);
}

inline void ocp_qp_sol_get_u(int stage, ::s_ocp_qp_sol *qp_sol, float *vec) {
    ::s_ocp_qp_sol_get_u(stage, qp_sol, vec);
}

inline void ocp_qp_sol_get_x(int stage, ::s_ocp_qp_sol *qp_sol, float *vec) {
    ::s_ocp_qp_sol_get_x(stage, qp_sol, vec);
}

inline void ocp_qp_sol_get_pi(int stage, ::s_ocp_qp_sol *qp_sol, float *vec) {
    ::s_ocp_qp_sol_get_pi(stage, qp_sol, vec);
}

inline void ocp_qp_sol_get_lam_lbx(int stage, ::s_ocp_qp_sol *qp_sol, float *vec) {
    ::s_ocp_qp_sol_get_lam_lbx(stage, qp_sol, vec);
}

inline void ocp_qp_sol_get_lam_ubx(int stage, ::s_ocp_qp_sol *qp_sol, float *vec) {
    ::s_ocp_qp_sol_get_lam_ubx(stage, qp_sol, vec);
}

inline void ocp_qp_sol_get_lam_lg(int stage, ::s_ocp_qp_sol *qp_sol, float *vec) {
    ::s_ocp_qp_sol_get_lam_lg(stage, qp_sol, vec);
}

inline void ocp_qp_sol_get_lam_ug(int stage, ::s_ocp_qp_sol *qp_sol, float *vec) {
    ::s_ocp_qp_sol_get_lam_ug(stage, qp_sol, vec);
}

inline void ocp_qp_sol_set_u(int stage, float *vec, ::s_ocp_qp_sol *qp_sol) {
    ::s_ocp_qp_sol_set_u(stage, vec, qp_sol);
}

inline void ocp_qp_sol_set_x(int stage, float *vec, ::s_ocp_qp_sol *qp_sol) {
    ::s_ocp_qp_sol_set_x(stage, vec, qp_sol);
}

// ================================================================================================
// s_ocp_qp_ipm_arg functions (float)
// ================================================================================================

inline hpipm_size_t ocp_qp_ipm_arg_memsize(::s_ocp_qp_dim *ocp_dim) {
    return ::s_ocp_qp_ipm_arg_memsize(ocp_dim);
}

inline void ocp_qp_ipm_arg_create(::s_ocp_qp_dim *ocp_dim, ::s_ocp_qp_ipm_arg *arg, void *mem) {
    ::s_ocp_qp_ipm_arg_create(ocp_dim, arg, mem);
}

inline void ocp_qp_ipm_arg_set_default(enum hpipm_mode mode, ::s_ocp_qp_ipm_arg *arg) {
    ::s_ocp_qp_ipm_arg_set_default(mode, arg);
}

inline void ocp_qp_ipm_arg_set_iter_max(int *iter_max, ::s_ocp_qp_ipm_arg *arg) {
    ::s_ocp_qp_ipm_arg_set_iter_max(iter_max, arg);
}

inline void ocp_qp_ipm_arg_set_tol_stat(float *tol_stat, ::s_ocp_qp_ipm_arg *arg) {
    ::s_ocp_qp_ipm_arg_set_tol_stat(tol_stat, arg);
}

inline void ocp_qp_ipm_arg_set_tol_eq(float *tol_eq, ::s_ocp_qp_ipm_arg *arg) {
    ::s_ocp_qp_ipm_arg_set_tol_eq(tol_eq, arg);
}

inline void ocp_qp_ipm_arg_set_tol_ineq(float *tol_ineq, ::s_ocp_qp_ipm_arg *arg) {
    ::s_ocp_qp_ipm_arg_set_tol_ineq(tol_ineq, arg);
}

inline void ocp_qp_ipm_arg_set_tol_comp(float *tol_comp, ::s_ocp_qp_ipm_arg *arg) {
    ::s_ocp_qp_ipm_arg_set_tol_comp(tol_comp, arg);
}

inline void ocp_qp_ipm_arg_set_warm_start(int *warm_start, ::s_ocp_qp_ipm_arg *arg) {
    ::s_ocp_qp_ipm_arg_set_warm_start(warm_start, arg);
}

inline void ocp_qp_ipm_arg_set_mu0(float *mu0, ::s_ocp_qp_ipm_arg *arg) {
    ::s_ocp_qp_ipm_arg_set_mu0(mu0, arg);
}

inline void ocp_qp_ipm_arg_set_alpha_min(float *alpha_min, ::s_ocp_qp_ipm_arg *arg) {
    ::s_ocp_qp_ipm_arg_set_alpha_min(alpha_min, arg);
}

inline void ocp_qp_ipm_arg_set_reg_prim(float *reg_prim, ::s_ocp_qp_ipm_arg *arg) {
    ::s_ocp_qp_ipm_arg_set_reg_prim(reg_prim, arg);
}

inline void ocp_qp_ipm_arg_set_lam_min(float *lam_min, ::s_ocp_qp_ipm_arg *arg) {
    ::s_ocp_qp_ipm_arg_set_lam_min(lam_min, arg);
}

inline void ocp_qp_ipm_arg_set_t_min(float *t_min, ::s_ocp_qp_ipm_arg *arg) {
    ::s_ocp_qp_ipm_arg_set_t_min(t_min, arg);
}

inline void ocp_qp_ipm_arg_set_tau_min(float *tau_min, ::s_ocp_qp_ipm_arg *arg) {
    ::s_ocp_qp_ipm_arg_set_tau_min(tau_min, arg);
}

// ================================================================================================
// s_ocp_qp_ipm_ws functions (float)
// ================================================================================================

inline hpipm_size_t ocp_qp_ipm_ws_memsize(::s_ocp_qp_dim *ocp_dim, ::s_ocp_qp_ipm_arg *arg) {
    return ::s_ocp_qp_ipm_ws_memsize(ocp_dim, arg);
}

inline void ocp_qp_ipm_ws_create(::s_ocp_qp_dim *ocp_dim, ::s_ocp_qp_ipm_arg *arg,
                                 ::s_ocp_qp_ipm_ws *ws, void *mem) {
    ::s_ocp_qp_ipm_ws_create(ocp_dim, arg, ws, mem);
}

inline void ocp_qp_ipm_get_status(::s_ocp_qp_ipm_ws *ws, int *status) {
    ::s_ocp_qp_ipm_get_status(ws, status);
}

inline void ocp_qp_ipm_get_iter(::s_ocp_qp_ipm_ws *ws, int *iter) {
    ::s_ocp_qp_ipm_get_iter(ws, iter);
}

inline void ocp_qp_ipm_get_max_res_stat(::s_ocp_qp_ipm_ws *ws, float *res_stat) {
    ::s_ocp_qp_ipm_get_max_res_stat(ws, res_stat);
}

inline void ocp_qp_ipm_get_max_res_eq(::s_ocp_qp_ipm_ws *ws, float *res_eq) {
    ::s_ocp_qp_ipm_get_max_res_eq(ws, res_eq);
}

inline void ocp_qp_ipm_get_max_res_ineq(::s_ocp_qp_ipm_ws *ws, float *res_ineq) {
    ::s_ocp_qp_ipm_get_max_res_ineq(ws, res_ineq);
}

inline void ocp_qp_ipm_get_max_res_comp(::s_ocp_qp_ipm_ws *ws, float *res_comp) {
    ::s_ocp_qp_ipm_get_max_res_comp(ws, res_comp);
}

// ================================================================================================
// s_ocp_qp_ipm_solve (float)
// ================================================================================================

inline void ocp_qp_ipm_solve(::s_ocp_qp *qp, ::s_ocp_qp_sol *qp_sol, ::s_ocp_qp_ipm_arg *arg,
                             ::s_ocp_qp_ipm_ws *ws) {
    ::s_ocp_qp_ipm_solve(qp, qp_sol, arg, ws);
}

} // namespace hpipm
