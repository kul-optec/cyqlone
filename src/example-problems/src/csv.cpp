#include <cyqlone/qpalm/example-problems/csv.hpp>
#include <guanaqo/eigen/view.hpp>
#include <guanaqo/io/csv.hpp>

#include <filesystem>
#include <fstream>

namespace CYQLONE_NS(cyqlone::qpalm::problems) {

using eigen_mat = Eigen::MatrixX<real_t>;

LinearOCPStorage load_from_csv(const fs::path &folder, const std::string &name) {
    using guanaqo::as_view;

    std::array<index_t, 5> dims_array;
    {
        std::ifstream dims_file = folder / ("dim-" + name + ".csv");
        guanaqo::io::csv_read_row(dims_file, std::span{dims_array});
    }
    auto [N, nx, nu, ny, ny_N] = dims_array;

    LinearOCPStorage ocp{.dim{
        .N_horiz = N,
        .nx      = nx,
        .nu      = nu,
        .ny      = ny,
        .ny_N    = ny_N,
    }};

    // Continuous-time dynamics
    eigen_mat A = eigen_mat::Zero(nx, nx), B = eigen_mat::Zero(nx, nu), C = eigen_mat::Zero(ny, nx),
              D = eigen_mat::Zero(ny, nu), CN = eigen_mat::Zero(ny_N, nx),
              QN = eigen_mat::Zero(nx, nx), Q = eigen_mat::Zero(nx, nx),
              R = eigen_mat::Zero(nu, nu);
    {
        std::ifstream f = folder / ("A-" + name + ".csv");
        guanaqo::io::csv_read(f, as_view(A));
    }
    {
        std::ifstream f = folder / ("B-" + name + ".csv");
        guanaqo::io::csv_read(f, as_view(B));
    }
    {
        std::ifstream f = folder / ("C-" + name + ".csv");
        guanaqo::io::csv_read(f, as_view(C));
    }
    {
        std::ifstream f = folder / ("D-" + name + ".csv");
        guanaqo::io::csv_read(f, as_view(D));
    }
    {
        std::ifstream f = folder / ("CN-" + name + ".csv");
        guanaqo::io::csv_read(f, as_view(CN));
    }
    {
        std::ifstream f = folder / ("QN-" + name + ".csv");
        guanaqo::io::csv_read(f, as_view(QN));
    }
    {
        std::ifstream f = folder / ("Q-" + name + ".csv");
        guanaqo::io::csv_read(f, as_view(Q));
    }
    {
        std::ifstream f = folder / ("R-" + name + ".csv");
        guanaqo::io::csv_read(f, as_view(R));
    }

    // Reference states and inputs
    std::vector<real_t> qr(N * (nx + nu) + nx);
    {
        std::ifstream f = folder / ("q-" + name + ".csv");
        guanaqo::io::csv_read_row(f, std::span{qr});
    }

    // Constraints rhs
    std::vector<real_t> eq((N + 1) * nx), lb(N * ny + ny_N), ub(N * ny + ny_N);
    {
        std::ifstream f = folder / ("eq-" + name + ".csv");
        guanaqo::io::csv_read_row(f, std::span{eq});
    }
    {
        std::ifstream f = folder / ("lb-" + name + ".csv");
        guanaqo::io::csv_read_row(f, std::span{lb});
    }
    {
        std::ifstream f = folder / ("ub-" + name + ".csv");
        guanaqo::io::csv_read_row(f, std::span{ub});
    }

    // Dynamics and constraint matrices
    for (index_t i = 0; i < N; ++i) {
        auto Ai = ocp.A(i), Bi = ocp.B(i), Ci = ocp.C(i), Di = ocp.D(i);
        auto Qi = ocp.Q(i), Ri = ocp.R(i);
        Ai = as_view(A);
        Bi = as_view(B);
        Ci = as_view(C);
        Di = as_view(D);
        Qi = as_view(Q);
        Ri = as_view(R);
    }
    auto Ci = ocp.C(N), Qi = ocp.Q(N);
    Ci = as_view(CN);
    Qi = as_view(QN);

    // TODO: unnecessary copy
    ocp.qr()    = decltype(ocp.qr())::as_column(std::span{qr});
    ocp.b()     = decltype(ocp.b())::as_column(std::span{eq});
    ocp.b_min() = decltype(ocp.b_min())::as_column(std::span{lb});
    ocp.b_max() = decltype(ocp.b_max())::as_column(std::span{ub});

    return ocp;
}

} // namespace CYQLONE_NS(cyqlone::qpalm::problems)
