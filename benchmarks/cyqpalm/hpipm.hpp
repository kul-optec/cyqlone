#pragma once

#include <cyqlone/ocp.hpp>
#include <chrono>
#include <memory>

struct HPIPMQPData;
struct HPIPMSolverData;

struct HPIPMStats {
    int status;
    int iter;
    double max_res_stat;
    double max_res_eq;
    double max_res_ineq;
    double max_res_comp;
};

std::shared_ptr<HPIPMQPData> build_hpipm_qp(const cyqlone::LinearOCPStorage &ocp);
std::shared_ptr<HPIPMSolverData> create_hpipm_solver(std::shared_ptr<HPIPMQPData> qp_data,
                                                     int warm_start = 0);
void warm_start_hpipm(HPIPMSolverData &solver_data, std::span<const cyqlone::real_t> solution);
std::chrono::nanoseconds solve_hpipm(const HPIPMQPData &qp_data, HPIPMSolverData &solver_data);
HPIPMStats get_stats_hpipm(HPIPMSolverData &solver_data);
cyqlone::LinearOCPStorage::Solution get_solution_hpipm(HPIPMSolverData &solver_data);
void update_x0_hpipm(cyqlone::LinearOCPStorage &ocp, HPIPMQPData &qp_data,
                     std::span<const cyqlone::real_t> solution);
void shift_solution_hpipm(HPIPMQPData &qp_data, std::span<cyqlone::real_t> solution);
