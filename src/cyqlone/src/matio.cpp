#include <cyqlone/matio.hpp>
#include <cyqlone/ocp.hpp>
#include <batmat/linalg/copy.hpp>
#include <matio.h>

#include <guanaqo/string-util.hpp>
#include <algorithm>
#include <filesystem>
#include <format>
#include <memory>
#include <span>
#include <stdexcept>
#include <vector>

namespace CYQLONE_NAMESPACE {

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
void write_tensor(mat_t *matfp, const char *name, std::span<const real_t> buffer,
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

MatFilePtr open_mat(const std::filesystem::path &filename, MatioOpenMode mode) {
    int imode = mode == MatioOpenMode::Write ? MAT_ACC_RDWR : MAT_ACC_RDONLY;
    MatFilePtr matfp(Mat_Open(filename.c_str(), imode), Mat_Close);
    if (!matfp)
        throw std::runtime_error(std::format("Failed to create .mat file {}", filename.string()));
    return matfp;
}

MatFilePtr create_mat(const std::filesystem::path &filename) {
    MatFilePtr matfp(Mat_CreateVer(filename.c_str(), nullptr, MAT_FT_MAT5), Mat_Close);
    if (!matfp)
        throw std::runtime_error(std::format("Failed to create .mat file {}", filename.string()));
    return matfp;
}

void add_to_mat(mat_t *mat, const std::string &varname,
                guanaqo::MatrixView<const real_t, index_t> data) {
    const auto r = static_cast<size_t>(data.rows), c = static_cast<size_t>(data.cols);
    if (data.rows == data.outer_stride) {
        write_tensor<2>(mat, varname.c_str(), std::span{data.data, r * c}, {data.rows, data.cols});
    } else {
        std::vector<real_t> buffer(r * c);
        guanaqo::MatrixView<real_t, index_t>{{
            .data = buffer.data(),
            .rows = data.rows,
            .cols = data.cols,
        }} = data;
        write_tensor<2>(mat, varname.c_str(), std::span{buffer}, {data.rows, data.cols});
    }
}

void add_to_mat(mat_t *mat, const std::string &varname,
                batmat::matrix::View<const real_t, index_t> data) {
    const auto r = static_cast<size_t>(data.rows()), c = static_cast<size_t>(data.cols()),
               d = static_cast<size_t>(data.depth());
    if (data.rows() == data.outer_stride() && data.layer_stride() == data.rows() * data.cols()) {
        write_tensor<3>(mat, varname.c_str(), std::span{data.data, r * c * d},
                        {data.rows(), data.cols(), data.depth()});
    } else {
        batmat::matrix::Matrix<real_t, index_t> buffer{{
            .depth = data.depth(),
            .rows  = data.rows(),
            .cols  = data.cols(),
        }};
        for (index_t l = 0; l < data.depth(); ++l)
            batmat::linalg::copy(data(l), buffer(l));
        add_to_mat(mat, varname, buffer.view());
    }
}

void add_to_mat(mat_t *mat, const LinearOCPStorage &ocp) {
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
    write_tensor<3>(mat, "H", buf, {nxu, nxu, N + 1});

    // CD: (ny, nx+nu, N)
    buf.resize(N * ny * nxu);
    for (index_t i = 0; i < N; ++i)
        batmat::linalg::copy(
            ocp.CD(i),
            Mat{{.data = &buf[i * ny * nxu], .rows = ny, .cols = nxu, .outer_stride = ny}});
    write_tensor<3>(mat, "CD", buf, {ny, nxu, N});
    // C(N): (ny_N, nx+nu)
    buf.resize(ny_N * nx);
    batmat::linalg::copy(ocp.C(N),
                         Mat{{.data = buf.data(), .rows = ny_N, .cols = nx, .outer_stride = ny_N}});
    write_tensor<2>(mat, "CN", buf, {ny_N, nx});

    // AB: (nx, nx+nu, N)
    buf.resize(N * nx * nxu);
    for (index_t i = 0; i < N; ++i)
        batmat::linalg::copy(
            ocp.AB(i),
            Mat{{.data = &buf[i * nx * nxu], .rows = nx, .cols = nxu, .outer_stride = nx}});
    write_tensor<3>(mat, "AB", buf, {nx, nxu, N});

    write_tensor<2>(mat, "qr", std::span(ocp.qr().data, ocp.qr().rows), {ocp.qr().rows, 1});
    write_tensor<2>(mat, "b", std::span(ocp.b().data, ocp.b().rows), {ocp.b().rows, 1});
    write_tensor<2>(mat, "b_min", std::span(ocp.b_min().data, ocp.b_min().rows),
                    {ocp.b_min().rows, 1});
    write_tensor<2>(mat, "b_max", std::span(ocp.b_max().data, ocp.b_max().rows),
                    {ocp.b_max().rows, 1});
}

void read_from_mat(mat_t *mat, LinearOCPStorage &ocp) {
    MatVarPtr ABvar(Mat_VarRead(mat, "AB"), Mat_VarFree);
    MatVarPtr CDvar(Mat_VarRead(mat, "CD"), Mat_VarFree);
    MatVarPtr CNvar(Mat_VarRead(mat, "CN"), Mat_VarFree);
    MatVarPtr Hvar(Mat_VarRead(mat, "H"), Mat_VarFree);
    MatVarPtr qrvar(Mat_VarRead(mat, "qr"), Mat_VarFree);
    MatVarPtr bvar(Mat_VarRead(mat, "b"), Mat_VarFree);
    MatVarPtr b_minvar(Mat_VarRead(mat, "b_min"), Mat_VarFree);
    MatVarPtr b_maxvar(Mat_VarRead(mat, "b_max"), Mat_VarFree);
    std::vector<std::string_view> missing;
    if (!ABvar)
        missing.emplace_back("AB");
    if (!CDvar)
        missing.emplace_back("CD");
    if (!CNvar)
        missing.emplace_back("CN");
    if (!Hvar)
        missing.emplace_back("H");
    if (!qrvar)
        missing.emplace_back("qr");
    if (!bvar)
        missing.emplace_back("b");
    if (!b_minvar)
        missing.emplace_back("b_min");
    if (!b_maxvar)
        missing.emplace_back("b_max");
    if (!missing.empty())
        throw std::runtime_error("Missing variables: " + guanaqo::join(missing));
    if (ABvar->rank != 3 || CDvar->rank != 3 || CNvar->rank != 2 || Hvar->rank != 3 ||
        qrvar->rank != 2 || bvar->rank != 2 || b_minvar->rank != 2 || b_maxvar->rank != 2)
        throw std::runtime_error("Invalid rank");
    if (ABvar->isComplex || CDvar->isComplex || CNvar->isComplex || Hvar->isComplex ||
        qrvar->isComplex || bvar->isComplex || b_minvar->isComplex || b_maxvar->isComplex)
        throw std::runtime_error("Should be real");
    if (ABvar->class_type != matio_traits<real_t>::class_ ||
        CDvar->class_type != matio_traits<real_t>::class_ ||
        CNvar->class_type != matio_traits<real_t>::class_ ||
        Hvar->class_type != matio_traits<real_t>::class_ ||
        qrvar->class_type != matio_traits<real_t>::class_ ||
        bvar->class_type != matio_traits<real_t>::class_ ||
        b_minvar->class_type != matio_traits<real_t>::class_ ||
        b_maxvar->class_type != matio_traits<real_t>::class_)
        throw std::runtime_error("Invalid class type");
    if (ABvar->data_type != matio_traits<real_t>::type ||
        CDvar->data_type != matio_traits<real_t>::type ||
        CNvar->data_type != matio_traits<real_t>::type ||
        Hvar->data_type != matio_traits<real_t>::type ||
        qrvar->data_type != matio_traits<real_t>::type ||
        bvar->data_type != matio_traits<real_t>::type ||
        b_minvar->data_type != matio_traits<real_t>::type ||
        b_maxvar->data_type != matio_traits<real_t>::type)
        throw std::runtime_error("Invalid data type");
    auto nx = static_cast<index_t>(ABvar->dims[0]), nxu = static_cast<index_t>(ABvar->dims[1]),
         N = static_cast<index_t>(ABvar->dims[2]), nu = nxu - nx;
    auto ny = static_cast<index_t>(CDvar->dims[0]), ny_N = static_cast<index_t>(CNvar->dims[0]);
    BATMAT_ASSERT(nxu >= nx);
    BATMAT_ASSERT(static_cast<index_t>(Hvar->dims[0]) == nxu);
    BATMAT_ASSERT(static_cast<index_t>(Hvar->dims[1]) == nxu);
    BATMAT_ASSERT(static_cast<index_t>(Hvar->dims[2]) == N + 1);
    BATMAT_ASSERT(static_cast<index_t>(CDvar->dims[1]) == nxu);
    BATMAT_ASSERT(static_cast<index_t>(CDvar->dims[2]) == N);
    BATMAT_ASSERT(static_cast<index_t>(CNvar->dims[1]) == nx);
    BATMAT_ASSERT(static_cast<index_t>(qrvar->dims[0]) == N * nxu + nx);
    BATMAT_ASSERT(static_cast<index_t>(qrvar->dims[1]) == 1);
    BATMAT_ASSERT(static_cast<index_t>(bvar->dims[0]) == (N + 1) * nx);
    BATMAT_ASSERT(static_cast<index_t>(bvar->dims[1]) == 1);
    BATMAT_ASSERT(static_cast<index_t>(b_minvar->dims[0]) == N * ny + ny_N);
    BATMAT_ASSERT(static_cast<index_t>(b_minvar->dims[1]) == 1);
    BATMAT_ASSERT(static_cast<index_t>(b_maxvar->dims[0]) == N * ny + ny_N);
    BATMAT_ASSERT(static_cast<index_t>(b_maxvar->dims[1]) == 1);

    using batmat::matrix::View;
    View<const real_t, index_t> AB{
        {.data = static_cast<const real_t *>(ABvar->data), .depth = N, .rows = nx, .cols = nxu}};
    View<const real_t, index_t> CD{
        {.data = static_cast<const real_t *>(CDvar->data), .depth = N, .rows = ny, .cols = nxu}};
    View<const real_t, index_t> CN{
        {.data = static_cast<const real_t *>(CNvar->data), .depth = 1, .rows = ny_N, .cols = nx}};
    View<const real_t, index_t> H{{.data  = static_cast<const real_t *>(Hvar->data),
                                   .depth = N + 1,
                                   .rows  = nxu,
                                   .cols  = nxu}};
    View<const real_t, index_t> qr{{.data  = static_cast<const real_t *>(qrvar->data),
                                    .depth = 1,
                                    .rows  = N * nxu + nx,
                                    .cols  = 1}};
    View<const real_t, index_t> b{{.data  = static_cast<const real_t *>(bvar->data),
                                   .depth = 1,
                                   .rows  = (N + 1) * nx,
                                   .cols  = 1}};
    View<const real_t, index_t> b_min{{.data  = static_cast<const real_t *>(b_minvar->data),
                                       .depth = 1,
                                       .rows  = N * ny + ny_N,
                                       .cols  = 1}};
    View<const real_t, index_t> b_max{{.data  = static_cast<const real_t *>(b_maxvar->data),
                                       .depth = 1,
                                       .rows  = N * ny + ny_N,
                                       .cols  = 1}};

    ocp = {.dim = {.N_horiz = N, .nx = nx, .nu = nu, .ny = ny, .ny_N = ny_N}};

    // H: (nx+nu, nx+nu, N+1)
    for (index_t i = 0; i < N; ++i)
        batmat::linalg::copy(H(i), ocp.H(i));
    // Q(N): (nx, nx), padded by zeros
    batmat::linalg::copy(H(N).top_left(nx, nx), ocp.Q(N));

    // CD: (ny, nx+nu, N)
    for (index_t i = 0; i < N; ++i)
        batmat::linalg::copy(CD(i), ocp.CD(i));
    // C(N): (ny_N, nx+nu)
    batmat::linalg::copy(CN(0), ocp.C(N));

    // AB: (nx, nx+nu, N)
    for (index_t i = 0; i < N; ++i)
        batmat::linalg::copy(AB(i), ocp.AB(i));

    // Vectors
    ocp.qr()    = qr(0);
    ocp.b()     = b(0);
    ocp.b_min() = b_min(0);
    ocp.b_max() = b_max(0);
}

auto open_vector_var(mat_t *mat, const std::string &varname) {
    MatVarPtr var(Mat_VarRead(mat, varname.c_str()), Mat_VarFree);
    if (!var)
        throw std::runtime_error("Missing variable: " + varname);
    if (var->rank != 2)
        throw std::runtime_error("Invalid rank");
    if (var->isComplex)
        throw std::runtime_error("Should be real");
    if (var->class_type != matio_traits<real_t>::class_)
        throw std::runtime_error("Invalid class type");
    if (var->data_type != matio_traits<real_t>::type)
        throw std::runtime_error("Invalid data type");
    if (var->dims[1] != 1)
        throw std::runtime_error("Should have one column");
    return var;
}

void read_from_mat(mat_t *mat, const std::string &varname, std::span<real_t> data) {
    auto var = open_vector_var(mat, varname);
    auto n   = var->dims[0];
    BATMAT_ASSERT(n == data.size());
    const auto *var_data = static_cast<const real_t *>(var->data);
    std::copy_n(var_data, n, data.begin());
}

void read_from_mat(mat_t *mat, const std::string &varname, std::vector<real_t> &data) {
    auto var = open_vector_var(mat, varname);
    auto n   = var->dims[0];
    data.resize(n);
    const auto *var_data = static_cast<const real_t *>(var->data);
    std::copy_n(var_data, n, data.begin());
}

void ocp_dump_mat(const std::filesystem::path &filename, const LinearOCPStorage &ocp) {
    auto matfp = create_mat(filename);
    add_to_mat(matfp.get(), ocp);
}

} // namespace CYQLONE_NAMESPACE
