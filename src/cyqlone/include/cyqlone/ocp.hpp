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
    OCPDim dim{};
    /// Storage layout:     size        offset
    /// N × [ Q Sᵀ] = H     (nx+nu)²    0
    ///     [ S R ]
    /// 1 × [ Q ]           nx²         N (nx+nu)²
    /// N × [ C D ]         ny(nx+nu)   N (nx+nu)² + nx²
    /// 1 × [ C ]           ny_N nx     N (nx+nu+ny)(nx+nu) + nx²
    /// N × [ A B ]         nx(nx+nu)   N (nx+nu+ny)(nx+nu) + (nx+ny_N)nx
    /// N × [ q r ]         nx+nu       N (2nx+nu+ny)(nx+nu) + (nx+ny_N)nx
    /// 1 × [ q ]           nx          N (2nx+nu+ny+1)(nx+nu) + (nx+ny_N)nx
    /// N+1 × [ b ]         nx          N (2nx+nu+ny+1)(nx+nu) + (nx+ny_N+1)nx
    /// N × [ l ]           ny          N (2nx+nu+ny+1)(nx+nu) + (nx+ny_N+2+N)nx
    /// 1 × [ l ]           ny_N        N (2nx+nu+ny+1)(nx+nu) + (nx+ny_N+2+N)nx + N ny
    /// N × [ u ]           ny          N (2nx+nu+ny+1)(nx+nu) + (nx+ny_N+2+N)nx + N ny + ny_N
    /// 1 × [ u ]           ny_N        N (2nx+nu+ny+1)(nx+nu) + (nx+ny_N+2+N)nx + N (2ny) + ny_N
    ///                                 N (2nx+nu+ny+1)(nx+nu) + (nx+ny_N+2+N)nx + N (2ny) + 2ny_N
    std::vector<real_t> storage = create_storage(dim);
    static std::vector<real_t> create_storage(OCPDim dim) {
        auto [N, nx, nu, ny, ny_N] = dim;
        index_t size = N * (2 * nx + nu + ny + 1) * (nx + nu) + (nx + ny_N + 2 + N) * nx +
                       2 * N * ny + 2 * ny_N;
        return std::vector<real_t>(size);
    }
    [[nodiscard]] guanaqo::MatrixView<real_t, index_t> H(index_t i) {
        auto [N, nx, nu, ny, ny_N] = dim;
        assert(i <= N);
        index_t offset = 0;
        index_t size   = (nx + nu) * (nx + nu);
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
        assert(i <= N);
        index_t offset = 0;
        index_t size   = (nx + nu) * (nx + nu);
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

    [[nodiscard]] guanaqo::MatrixView<real_t, index_t> qr() {
        auto [N, nx, nu, ny, ny_N] = dim;
        index_t offset             = N * (2 * nx + nu + ny) * (nx + nu) + (nx + ny_N) * nx;
        return {{
            .data = &storage[offset],
            .rows = N * (nx + nu) + nx,
            .cols = 1,
        }};
    }
    [[nodiscard]] guanaqo::MatrixView<real_t, index_t> qr(index_t i) {
        auto [N, nx, nu, ny, ny_N] = dim;
        assert(i <= N);
        index_t size = nx + nu;
        return qr().middle_rows(i * size, i < N ? size : nx);
    }
    [[nodiscard]] guanaqo::MatrixView<real_t, index_t> q(index_t i) {
        auto [N, nx, nu, ny, ny_N] = dim;
        assert(i <= N);
        return qr(i).top_rows(nx);
    }
    [[nodiscard]] guanaqo::MatrixView<real_t, index_t> r(index_t i) {
        auto [N, nx, nu, ny, ny_N] = dim;
        assert(i < N);
        return qr(i).bottom_rows(nu);
    }

    [[nodiscard]] guanaqo::MatrixView<real_t, index_t> b() {
        auto [N, nx, nu, ny, ny_N] = dim;
        index_t offset             = N * (2 * nx + nu + ny + 1) * (nx + nu) + (nx + ny_N + 1) * nx;
        return {{
            .data = &storage[offset],
            .rows = (N + 1) * nx,
            .cols = 1,
        }};
    }
    [[nodiscard]] guanaqo::MatrixView<real_t, index_t> b(index_t i) {
        auto [N, nx, nu, ny, ny_N] = dim;
        assert(i <= N);
        index_t size = nx;
        return b().middle_rows(i * size, size);
    }

    [[nodiscard]] guanaqo::MatrixView<real_t, index_t> b_min() {
        auto [N, nx, nu, ny, ny_N] = dim;
        index_t offset = N * (2 * nx + nu + ny + 1) * (nx + nu) + (nx + ny_N + 2 + N) * nx;
        return {{
            .data = &storage[offset],
            .rows = N * ny + ny_N,
            .cols = 1,
        }};
    }
    [[nodiscard]] guanaqo::MatrixView<real_t, index_t> b_min(index_t i) {
        auto [N, nx, nu, ny, ny_N] = dim;
        assert(i <= N);
        index_t size = ny;
        return b_min().middle_rows(i * size, i < N ? size : ny_N);
    }

    [[nodiscard]] guanaqo::MatrixView<real_t, index_t> b_max() {
        auto [N, nx, nu, ny, ny_N] = dim;
        index_t offset =
            N * (2 * nx + nu + ny + 1) * (nx + nu) + (nx + ny_N + 2 + N) * nx + N * ny + ny_N;
        return {{
            .data = &storage[offset],
            .rows = N * ny + ny_N,
            .cols = 1,
        }};
    }
    [[nodiscard]] guanaqo::MatrixView<real_t, index_t> b_max(index_t i) {
        auto [N, nx, nu, ny, ny_N] = dim;
        assert(i <= N);
        index_t size = ny;
        return b_max().middle_rows(i * size, i < N ? size : ny_N);
    }

    [[nodiscard]] guanaqo::MatrixView<const real_t, index_t> qr() const {
        auto [N, nx, nu, ny, ny_N] = dim;
        index_t offset             = N * (2 * nx + nu + ny) * (nx + nu) + (nx + ny_N) * nx;
        return {{
            .data = &storage[offset],
            .rows = N * (nx + nu) + nx,
            .cols = 1,
        }};
    }
    [[nodiscard]] guanaqo::MatrixView<const real_t, index_t> qr(index_t i) const {
        auto [N, nx, nu, ny, ny_N] = dim;
        assert(i <= N);
        index_t size = nx + nu;
        return qr().middle_rows(i * size, i < N ? size : nx);
    }
    [[nodiscard]] guanaqo::MatrixView<const real_t, index_t> q(index_t i) const {
        auto [N, nx, nu, ny, ny_N] = dim;
        assert(i <= N);
        return qr(i).top_rows(nx);
    }
    [[nodiscard]] guanaqo::MatrixView<const real_t, index_t> r(index_t i) const {
        auto [N, nx, nu, ny, ny_N] = dim;
        assert(i < N);
        return qr(i).bottom_rows(nu);
    }

    [[nodiscard]] guanaqo::MatrixView<const real_t, index_t> b() const {
        auto [N, nx, nu, ny, ny_N] = dim;
        index_t offset             = N * (2 * nx + nu + ny + 1) * (nx + nu) + (nx + ny_N + 1) * nx;
        return {{
            .data = &storage[offset],
            .rows = (N + 1) * nx,
            .cols = 1,
        }};
    }
    [[nodiscard]] guanaqo::MatrixView<const real_t, index_t> b(index_t i) const {
        auto [N, nx, nu, ny, ny_N] = dim;
        assert(i <= N);
        index_t size = nx;
        return b().middle_rows(i * size, size);
    }

    [[nodiscard]] guanaqo::MatrixView<const real_t, index_t> b_min() const {
        auto [N, nx, nu, ny, ny_N] = dim;
        index_t offset = N * (2 * nx + nu + ny + 1) * (nx + nu) + (nx + ny_N + 2 + N) * nx;
        return {{
            .data = &storage[offset],
            .rows = N * ny + ny_N,
            .cols = 1,
        }};
    }
    [[nodiscard]] guanaqo::MatrixView<const real_t, index_t> b_min(index_t i) const {
        auto [N, nx, nu, ny, ny_N] = dim;
        assert(i <= N);
        index_t size = ny;
        return b_min().middle_rows(i * size, i < N ? size : ny_N);
    }

    [[nodiscard]] guanaqo::MatrixView<const real_t, index_t> b_max() const {
        auto [N, nx, nu, ny, ny_N] = dim;
        index_t offset =
            N * (2 * nx + nu + ny + 1) * (nx + nu) + (nx + ny_N + 2 + N) * nx + N * ny + ny_N;
        return {{
            .data = &storage[offset],
            .rows = N * ny + ny_N,
            .cols = 1,
        }};
    }
    [[nodiscard]] guanaqo::MatrixView<const real_t, index_t> b_max(index_t i) const {
        auto [N, nx, nu, ny, ny_N] = dim;
        assert(i <= N);
        index_t size = ny;
        return b_max().middle_rows(i * size, i < N ? size : ny_N);
    }
};

} // namespace cyqlone
