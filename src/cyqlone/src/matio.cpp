#include <cyqlone/matio.hpp>
#include <cyqlone/ocp.hpp>
#include <batmat/linalg/copy.hpp>
#include <matio.h>

#include <algorithm>
#include <filesystem>
#include <format>
#include <memory>
#include <span>
#include <stdexcept>
#include <vector>

namespace cyqlone {

template <typename T>
struct matio_traits;
template <>
struct matio_traits<float> {
    static constexpr auto type   = MAT_T_SINGLE;
    static constexpr auto class_ = MAT_C_SINGLE;
};
template <>
struct matio_traits<double> {
    static constexpr auto type   = MAT_T_DOUBLE;
    static constexpr auto class_ = MAT_C_DOUBLE;
};

// RAII wrappers
using MatFilePtr = std::unique_ptr<mat_t, decltype(&Mat_Close)>;
using MatVarPtr  = std::unique_ptr<matvar_t, decltype(&Mat_VarFree)>;

template <size_t N>
void write_tensor(mat_t *matfp, const char *name, std::span<real_t> buffer,
                  std::array<index_t, N> dims) {
    std::array<size_t, N> dimsu;
    std::ranges::copy(dims, dimsu.begin());
    MatVarPtr var(Mat_VarCreate(name, matio_traits<real_t>::class_, matio_traits<real_t>::type,
                                dimsu.size(), dimsu.data(), const_cast<real_t *>(buffer.data()), 0),
                  Mat_VarFree);
    if (!var)
        throw std::runtime_error(std::format("Failed to create var {}", name));
    if (auto e = Mat_VarWrite(matfp, var.get(), MAT_COMPRESSION_ZLIB); e)
        throw std::runtime_error(std::format("Failed to write var {} ({})", name, e));
}

void ocp_dump_mat(const LinearOCPStorage &ocp, const std::filesystem::path &filename) {
    MatFilePtr matfp(Mat_CreateVer(filename.c_str(), nullptr, MAT_FT_MAT5), Mat_Close);
    if (!matfp)
        throw std::runtime_error(std::format("Failed to create .mat file {}", filename.string()));

    const auto [N, nx, nu, ny, ny_N] = ocp.dim;
    const auto nxu                   = nx + nu;
    using Mat                        = guanaqo::MatrixView<real_t, index_t>;

    // H: (nx+nu, nx+nu, N+1)
    std::vector<real_t> buf((N + 1) * nxu * nxu, 0.0);
    for (index_t i = 0; i < N; ++i)
        batmat::linalg::copy(
            ocp.H(i),
            Mat{{.data = &buf[i * nxu * nxu], .rows = nxu, .cols = nxu, .outer_stride = nxu}});
    // Q(N): (nx, nx), padded by zeros
    batmat::linalg::copy(
        ocp.Q(N), Mat{{.data = &buf[N * nxu * nxu], .rows = nx, .cols = nx, .outer_stride = nxu}});
    write_tensor<3>(matfp.get(), "H", buf, {nxu, nxu, N + 1});

    // CD: (ny, nx+nu, N)
    buf.resize(N * ny * nxu);
    for (index_t i = 0; i < N; ++i)
        batmat::linalg::copy(
            ocp.CD(i),
            Mat{{.data = &buf[i * ny * nxu], .rows = ny, .cols = nxu, .outer_stride = ny}});
    write_tensor<3>(matfp.get(), "CD", buf, {ny, nxu, N});
    // C(N): (ny_N, nx+nu)
    buf.resize(ny_N * nx);
    batmat::linalg::copy(ocp.C(N),
                         Mat{{.data = buf.data(), .rows = ny_N, .cols = nx, .outer_stride = ny_N}});
    write_tensor<2>(matfp.get(), "CN", buf, {ny_N, nx});

    // AB: (nx, nx+nu, N)
    buf.resize(N * nx * nxu);
    for (index_t i = 0; i < N; ++i)
        batmat::linalg::copy(
            ocp.AB(i),
            Mat{{.data = &buf[i * nx * nxu], .rows = nx, .cols = nxu, .outer_stride = nx}});
    write_tensor<3>(matfp.get(), "AB", buf, {nx, nxu, N});
}

} // namespace cyqlone
