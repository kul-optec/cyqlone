#pragma once

#include <batmat/config.hpp>
#include <guanaqo/mat-view.hpp>
#include <vector>

namespace cyqlone {

using batmat::index_t;
using batmat::real_t;

struct OCPDim {
    index_t N_horiz;
    index_t nx, nu, ny, ny_N = ny;
    friend constexpr bool operator==(OCPDim, OCPDim) = default;
    friend constexpr bool operator!=(OCPDim, OCPDim) = default;
};

struct LinearOCPStorage {
    OCPDim dim;
    /// Storage layout:     size        offset
    /// N × [ Q Sᵀ] = H     (nx+nu)²    0
    ///     [ S R ]
    /// 1 × [ Q ]           nx²         N (nx+nu)²
    /// N × [ C D ]         ny(nx+nu)   N (nx+nu)² + nx²
    /// 1 × [ C ]           ny_N nx     N (nx+nu+ny)(nx+nu) + nx²
    /// N × [ A B ]         nx(nx+nu)   N (nx+nu+ny)(nx+nu) + (nx+ny_N)nx
    ///                                 N (2nx+nu+ny)(nx+nu) + (nx+ny_N)nx
    std::vector<real_t> storage = create_storage(dim);
    static std::vector<real_t> create_storage(OCPDim dim) {
        auto [N, nx, nu, ny, ny_N] = dim;
        auto size                  = N * (2 * nx + nu + ny) * (nx + nu) + (nx + ny_N) * nx;
        return std::vector<real_t>(size);
    }
    [[nodiscard]] guanaqo::MatrixView<real_t, index_t> H(index_t i) {
        auto [N, nx, nu, ny, ny_N] = dim;
        index_t offset             = 0;
        index_t size               = (nx + nu) * (nx + nu);
        return {{
            .data = &storage[offset + i * size],
            .rows = i < N ? nx + nu : nx,
            .cols = i < N ? nx + nu : nx,
        }};
    }
    [[nodiscard]] guanaqo::MatrixView<real_t, index_t> Q(index_t i) {
        auto [N, nx, nu, ny, ny_N] = dim;
        assert(i <= N);
        return H(i).top_left(nx, nx);
    }
    [[nodiscard]] guanaqo::MatrixView<real_t, index_t> R(index_t i) {
        auto [N, nx, nu, ny, ny_N] = dim;
        assert(i < N);
        return H(i).bottom_right(nu, nu);
    }
    [[nodiscard]] guanaqo::MatrixView<real_t, index_t> S(index_t i) {
        auto [N, nx, nu, ny, ny_N] = dim;
        assert(i < N);
        return H(i).bottom_left(nu, nx);
    }
    [[nodiscard]] guanaqo::MatrixView<real_t, index_t> S_trans(index_t i) {
        auto [N, nx, nu, ny, ny_N] = dim;
        assert(i < N);
        return H(i).top_right(nx, nu);
    }
    [[nodiscard]] guanaqo::MatrixView<real_t, index_t> CD(index_t i) {
        auto [N, nx, nu, ny, ny_N] = dim;
        assert(i <= N);
        index_t offset = N * (nx + nu) * (nx + nu) + nx * nx;
        index_t size   = ny * (nx + nu);
        return {{
            .data = &storage[offset + i * size],
            .rows = i < N ? ny : ny_N,
            .cols = i < N ? nx + nu : nx,
        }};
    }
    [[nodiscard]] guanaqo::MatrixView<real_t, index_t> C(index_t i) {
        auto [N, nx, nu, ny, ny_N] = dim;
        assert(i <= N);
        return CD(i).left_cols(nx);
    }
    [[nodiscard]] guanaqo::MatrixView<real_t, index_t> D(index_t i) {
        auto [N, nx, nu, ny, ny_N] = dim;
        assert(i < N);
        return CD(i).right_cols(nu);
    }
    [[nodiscard]] guanaqo::MatrixView<real_t, index_t> AB(index_t i) {
        auto [N, nx, nu, ny, ny_N] = dim;
        assert(i < N);
        index_t offset = N * (nx + nu + ny) * (nx + nu) + (nx + ny_N) * nx;
        index_t size   = nx * (nx + nu);
        return {{
            .data = &storage[offset + i * size],
            .rows = nx,
            .cols = nx + nu,
        }};
    }
    [[nodiscard]] guanaqo::MatrixView<real_t, index_t> A(index_t i) {
        auto [N, nx, nu, ny, ny_N] = dim;
        assert(i < N);
        return AB(i).left_cols(nx);
    }
    [[nodiscard]] guanaqo::MatrixView<real_t, index_t> B(index_t i) {
        auto [N, nx, nu, ny, ny_N] = dim;
        assert(i < N);
        return AB(i).right_cols(nu);
    }

    [[nodiscard]] guanaqo::MatrixView<const real_t, index_t> H(index_t i) const {
        auto [N, nx, nu, ny, ny_N] = dim;
        index_t offset             = 0;
        index_t size               = (nx + nu) * (nx + nu);
        return {{
            .data = &storage[offset + i * size],
            .rows = i < N ? nx + nu : nx,
            .cols = i < N ? nx + nu : nx,
        }};
    }
    [[nodiscard]] guanaqo::MatrixView<const real_t, index_t> Q(index_t i) const {
        auto [N, nx, nu, ny, ny_N] = dim;
        assert(i <= N);
        return H(i).top_left(nx, nx);
    }
    [[nodiscard]] guanaqo::MatrixView<const real_t, index_t> R(index_t i) const {
        auto [N, nx, nu, ny, ny_N] = dim;
        assert(i < N);
        return H(i).bottom_right(nu, nu);
    }
    [[nodiscard]] guanaqo::MatrixView<const real_t, index_t> S(index_t i) const {
        auto [N, nx, nu, ny, ny_N] = dim;
        assert(i < N);
        return H(i).bottom_left(nu, nx);
    }
    [[nodiscard]] guanaqo::MatrixView<const real_t, index_t> S_trans(index_t i) const {
        auto [N, nx, nu, ny, ny_N] = dim;
        assert(i < N);
        return H(i).top_right(nx, nu);
    }
    [[nodiscard]] guanaqo::MatrixView<const real_t, index_t> CD(index_t i) const {
        auto [N, nx, nu, ny, ny_N] = dim;
        assert(i <= N);
        index_t offset = N * (nx + nu) * (nx + nu) + nx * nx;
        index_t size   = ny * (nx + nu);
        return {{
            .data = &storage[offset + i * size],
            .rows = i < N ? ny : ny_N,
            .cols = i < N ? nx + nu : nx,
        }};
    }
    [[nodiscard]] guanaqo::MatrixView<const real_t, index_t> C(index_t i) const {
        auto [N, nx, nu, ny, ny_N] = dim;
        assert(i <= N);
        return CD(i).left_cols(nx);
    }
    [[nodiscard]] guanaqo::MatrixView<const real_t, index_t> D(index_t i) const {
        auto [N, nx, nu, ny, ny_N] = dim;
        assert(i < N);
        return CD(i).right_cols(nu);
    }
    [[nodiscard]] guanaqo::MatrixView<const real_t, index_t> AB(index_t i) const {
        auto [N, nx, nu, ny, ny_N] = dim;
        assert(i < N);
        index_t offset = N * (nx + nu + ny) * (nx + nu) + (nx + ny_N) * nx;
        index_t size   = nx * (nx + nu);
        return {{
            .data = &storage[offset + i * size],
            .rows = nx,
            .cols = nx + nu,
        }};
    }
    [[nodiscard]] guanaqo::MatrixView<const real_t, index_t> A(index_t i) const {
        auto [N, nx, nu, ny, ny_N] = dim;
        assert(i < N);
        return AB(i).left_cols(nx);
    }
    [[nodiscard]] guanaqo::MatrixView<const real_t, index_t> B(index_t i) const {
        auto [N, nx, nu, ny, ny_N] = dim;
        assert(i < N);
        return AB(i).right_cols(nu);
    }

    [[nodiscard]] index_t num_variables() const {
        auto [N, nx, nu, ny, ny_N] = dim;
        return N * (nx + nu) + nx;
    }
    [[nodiscard]] index_t num_constraints() const {
        auto [N, nx, nu, ny, ny_N] = dim;
        return N * ny + ny_N;
    }
    [[nodiscard]] index_t num_dynamics_constraints() const {
        auto [N, nx, nu, ny, ny_N] = dim;
        return (N + 1) * nx;
    }
};

} // namespace cyqlone
