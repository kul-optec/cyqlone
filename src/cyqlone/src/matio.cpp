#include <cyqlone/matio.hpp>
#include <cyqlone/ocp.hpp>
#include <batmat/linalg/copy.hpp>
#include <guanaqo/string-util.hpp>
#include <matio.h>

#include <algorithm>
#include <climits>
#include <concepts>
#include <cstring>
#include <filesystem>
#include <format>
#include <memory>
#include <span>
#include <stdexcept>
#include <vector>

namespace cyqlone {

template <class T>
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
template <std::unsigned_integral I>
struct matio_traits<I> {
    static constexpr auto type   = sizeof(I) == 1   ? MAT_T_UINT8
                                   : sizeof(I) == 2 ? MAT_T_UINT16
                                   : sizeof(I) == 4 ? MAT_T_UINT32
                                   : sizeof(I) == 8 ? MAT_T_UINT64
                                                    : MAT_T_UNKNOWN;
    static constexpr auto class_ = sizeof(I) == 1   ? MAT_C_UINT8
                                   : sizeof(I) == 2 ? MAT_C_UINT16
                                   : sizeof(I) == 4 ? MAT_C_UINT32
                                   : sizeof(I) == 8 ? MAT_C_UINT64
                                                    : MAT_C_EMPTY;
    static_assert(CHAR_BIT == 8, "Unsupported char size");
    static_assert(type != MAT_T_UNKNOWN, "Unsupported unsigned integer type");
    static_assert(class_ != MAT_C_EMPTY, "Unsupported unsigned integer type");
};
template <std::signed_integral I>
struct matio_traits<I> {
    static constexpr auto type   = sizeof(I) == 1   ? MAT_T_INT8
                                   : sizeof(I) == 2 ? MAT_T_INT16
                                   : sizeof(I) == 4 ? MAT_T_INT32
                                   : sizeof(I) == 8 ? MAT_T_INT64
                                                    : MAT_T_UNKNOWN;
    static constexpr auto class_ = sizeof(I) == 1   ? MAT_C_INT8
                                   : sizeof(I) == 2 ? MAT_C_INT16
                                   : sizeof(I) == 4 ? MAT_C_INT32
                                   : sizeof(I) == 8 ? MAT_C_INT64
                                                    : MAT_C_EMPTY;
    static_assert(CHAR_BIT == 8, "Unsupported char size");
    static_assert(type != MAT_T_UNKNOWN, "Unsupported signed integer type");
    static_assert(class_ != MAT_C_EMPTY, "Unsupported signed integer type");
};

// RAII wrappers
using MatFilePtr = std::unique_ptr<mat_t, decltype(&Mat_Close)>;
using MatVarPtr  = std::unique_ptr<matvar_t, decltype(&Mat_VarFree)>;

template <class T, size_t N>
MatVarPtr create_tensor_var(const char *name, std::span<const T> buffer,
                            std::array<index_t, N> dims) {
    std::array<size_t, N> dimsu;
    std::ranges::copy(dims, dimsu.begin());
    return MatVarPtr(Mat_VarCreate(name, matio_traits<T>::class_, matio_traits<T>::type,
                                   dimsu.size(), dimsu.data(), const_cast<T *>(buffer.data()), 0),
                     Mat_VarFree);
}

template <class T, size_t N>
void write_tensor(mat_t *matfp, const char *name, std::span<const T> buffer,
                  std::array<index_t, N> dims) {
    MatVarPtr var = create_tensor_var(name, buffer, dims);
    if (!var)
        throw std::runtime_error(std::format("Failed to create var {}", name));
    if (auto e = Mat_VarWrite(matfp, var.get(), MAT_COMPRESSION_ZLIB); e)
        throw std::runtime_error(std::format("Failed to write var {} ({})", name, e));
}

MatFilePtr open_mat(const std::filesystem::path &filename, MatioOpenMode mode) {
    int imode = mode == MatioOpenMode::Write ? MAT_ACC_RDWR : MAT_ACC_RDONLY;
    MatFilePtr matfp(Mat_Open(filename.c_str(), imode), Mat_Close);
    if (!matfp)
        throw std::runtime_error(std::format("Failed to open .mat file {}", filename.string()));
    return matfp;
}

MatFilePtr create_mat(const std::filesystem::path &filename) {
    MatFilePtr matfp(Mat_CreateVer(filename.c_str(), nullptr, MAT_FT_MAT5), Mat_Close);
    if (!matfp)
        throw std::runtime_error(std::format("Failed to create .mat file {}", filename.string()));
    return matfp;
}

template <class T>
void add_to_mat_impl(mat_t *mat, const std::string &varname,
                     guanaqo::MatrixView<const T, index_t> data) {
    const auto r = static_cast<size_t>(data.rows), c = static_cast<size_t>(data.cols);
    if (data.rows == data.outer_stride) {
        write_tensor<T, 2>(mat, varname.c_str(), std::span{data.data, r * c},
                           {data.rows, data.cols});
    } else {
        std::vector<T> buffer(r * c);
        guanaqo::MatrixView<T, index_t>{{
            .data = buffer.data(),
            .rows = data.rows,
            .cols = data.cols,
        }} = data;
        write_tensor<T, 2>(mat, varname.c_str(), std::span{buffer}, {data.rows, data.cols});
    }
}

void add_to_mat(mat_t *mat, const std::string &varname, float value) {
    write_tensor<float, 1>(mat, varname.c_str(), std::span{&value, 1}, {1});
}

void add_to_mat(mat_t *mat, const std::string &varname, double value) {
    write_tensor<double, 1>(mat, varname.c_str(), std::span{&value, 1}, {1});
}

void add_to_mat(mat_t *mat, const std::string &varname, unsigned short value) {
    write_tensor<unsigned short, 1>(mat, varname.c_str(), std::span{&value, 1}, {1});
}

void add_to_mat(mat_t *mat, const std::string &varname, unsigned int value) {
    write_tensor<unsigned int, 1>(mat, varname.c_str(), std::span{&value, 1}, {1});
}

void add_to_mat(mat_t *mat, const std::string &varname, unsigned long value) {
    write_tensor<unsigned long, 1>(mat, varname.c_str(), std::span{&value, 1}, {1});
}

void add_to_mat(mat_t *mat, const std::string &varname, unsigned long long value) {
    write_tensor<unsigned long long, 1>(mat, varname.c_str(), std::span{&value, 1}, {1});
}

void add_to_mat(mat_t *mat, const std::string &varname, short value) {
    write_tensor<short, 1>(mat, varname.c_str(), std::span{&value, 1}, {1});
}

void add_to_mat(mat_t *mat, const std::string &varname, int value) {
    write_tensor<int, 1>(mat, varname.c_str(), std::span{&value, 1}, {1});
}

void add_to_mat(mat_t *mat, const std::string &varname, long value) {
    write_tensor<long, 1>(mat, varname.c_str(), std::span{&value, 1}, {1});
}

void add_to_mat(mat_t *mat, const std::string &varname, long long value) {
    write_tensor<long long, 1>(mat, varname.c_str(), std::span{&value, 1}, {1});
}

void add_to_mat(mat_t *mat, const std::string &varname,
                guanaqo::MatrixView<const double, index_t> data) {
    add_to_mat_impl(mat, varname, data);
}

void add_to_mat(mat_t *mat, const std::string &varname,
                guanaqo::MatrixView<const float, index_t> data) {
    add_to_mat_impl(mat, varname, data);
}

template <class T>
void add_to_mat_impl(mat_t *mat, const std::string &varname,
                     batmat::matrix::View<const T, index_t> data) {
    const auto r = static_cast<size_t>(data.rows()), c = static_cast<size_t>(data.cols()),
               d = static_cast<size_t>(data.depth());
    if (data.rows() == data.outer_stride() && data.layer_stride() == data.rows() * data.cols()) {
        write_tensor<T, 3>(mat, varname.c_str(), std::span{data.data(), r * c * d},
                           {data.rows(), data.cols(), data.depth()});
    } else {
        batmat::matrix::Matrix<T, index_t> buffer{{
            .depth = data.depth(),
            .rows  = data.rows(),
            .cols  = data.cols(),
        }};
        for (index_t l = 0; l < data.depth(); ++l)
            batmat::linalg::copy(data(l), buffer(l));
        add_to_mat(mat, varname, buffer.view());
    }
}

void add_to_mat(mat_t *mat, const std::string &varname,
                batmat::matrix::View<const double, index_t> data) {
    add_to_mat_impl(mat, varname, data);
}

void add_to_mat(mat_t *mat, const std::string &varname,
                batmat::matrix::View<const float, index_t> data) {
    add_to_mat_impl(mat, varname, data);
}

void add_to_mat(mat_t *mat, const std::string &varname, std::span<const float> data) {
    write_tensor<float, 1>(mat, varname.c_str(), data, {static_cast<index_t>(data.size())});
}

void add_to_mat(mat_t *mat, const std::string &varname, std::span<const double> data) {
    write_tensor<double, 1>(mat, varname.c_str(), data, {static_cast<index_t>(data.size())});
}

void add_to_mat(mat_t *mat, const std::string &varname, const SparseMatrix &matrix) {
    std::array<const char *, 5> fieldnames{"num_rows", "num_cols", "row_indices", "col_indices",
                                           "values"};
    std::array<size_t, 2> struct_dims{1, 1};
    MatVarPtr struct_{Mat_VarCreateStruct(varname.c_str(), struct_dims.size(), struct_dims.data(),
                                          fieldnames.data(), fieldnames.size()),
                      Mat_VarFree};
    if (!struct_)
        throw std::runtime_error(std::format("Failed to create struct {}", varname));
    using index_traits = matio_traits<decltype(SparseMatrix::row_indices)::value_type>;
    std::array<size_t, 2> scalar_dims{1, 1}; // MATLAB scalars are 1x1 matrices
    Mat_VarSetStructFieldByIndex(struct_.get(), 0, 0,
                                 Mat_VarCreate("num_rows", index_traits::class_, index_traits::type,
                                               scalar_dims.size(), scalar_dims.data(),
                                               &matrix.sparsity.rows, 0));
    Mat_VarSetStructFieldByIndex(struct_.get(), 1, 0,
                                 Mat_VarCreate("num_cols", index_traits::class_, index_traits::type,
                                               scalar_dims.size(), scalar_dims.data(),
                                               &matrix.sparsity.cols, 0));
    const size_t n_row_indices = matrix.sparsity.row_indices.size();
    Mat_VarSetStructFieldByIndex(struct_.get(), 2, 0,
                                 Mat_VarCreate("row_indices", index_traits::class_,
                                               index_traits::type, 1, &n_row_indices,
                                               matrix.sparsity.row_indices.data(), 0));
    const size_t n_col_indices = matrix.sparsity.col_indices.size();
    Mat_VarSetStructFieldByIndex(struct_.get(), 3, 0,
                                 Mat_VarCreate("col_indices", index_traits::class_,
                                               index_traits::type, 1, &n_col_indices,
                                               matrix.sparsity.col_indices.data(), 0));
    using value_traits    = matio_traits<decltype(SparseMatrix::values)::value_type>;
    const size_t n_values = matrix.values.size();
    Mat_VarSetStructFieldByIndex(struct_.get(), 4, 0,
                                 Mat_VarCreate("values", value_traits::class_, value_traits::type,
                                               1, &n_values, matrix.values.data(), 0));
    if (auto e = Mat_VarWrite(mat, struct_.get(), MAT_COMPRESSION_ZLIB); e)
        throw std::runtime_error(std::format("Failed to write struct {} ({})", varname, e));
}

void add_to_mat(mat_t *mat, const std::string &varname, const LinearOCPStorage &ocp) {
    const auto [N, nx, nu, ny, ny_N] = ocp.dim;
    const auto nxu                   = nx + nu;
    using Mat                        = guanaqo::MatrixView<real_t, index_t>;
    std::array<const char *, 8> fieldnames{"H", "CD", "CN", "AB", "qr", "b", "b_min", "b_max"};
    std::array<size_t, 2> struct_dims{1, 1};
    MatVarPtr struct_{Mat_VarCreateStruct(varname.c_str(), struct_dims.size(), struct_dims.data(),
                                          fieldnames.data(), fieldnames.size()),
                      Mat_VarFree};
    if (!struct_)
        throw std::runtime_error(std::format("Failed to create struct {}", varname));
    const auto set_struct_field = [&](size_t field_index, MatVarPtr field) {
        if (std::strcmp(field->name, fieldnames[field_index]) != 0)
            throw std::runtime_error(std::format("Field name mismatch: expected {}, got {}",
                                                 fieldnames[field_index], field->name));
        if (!field)
            throw std::runtime_error(std::format("Failed to create field {} in struct {}",
                                                 fieldnames[field_index], varname));
        Mat_VarSetStructFieldByIndex(struct_.get(), field_index, 0, field.release());
    };

    // H: (nx+nu, nx+nu, N+1)
    std::vector<real_t> buf((N + 1) * nxu * nxu, 0.0);
    for (index_t i = 0; i < N; ++i)
        batmat::linalg::copy(
            ocp.H(i),
            Mat{{.data = &buf[i * nxu * nxu], .rows = nxu, .cols = nxu, .outer_stride = nxu}});
    // Q(N): (nx, nx), padded by zeros
    batmat::linalg::copy(
        ocp.Q(N), Mat{{.data = &buf[N * nxu * nxu], .rows = nx, .cols = nx, .outer_stride = nxu}});
    set_struct_field(0, create_tensor_var<real_t, 3>("H", buf, {nxu, nxu, N + 1}));

    // CD: (ny, nx+nu, N)
    buf.resize(N * ny * nxu);
    for (index_t i = 0; i < N; ++i)
        batmat::linalg::copy(
            ocp.CD(i),
            Mat{{.data = &buf[i * ny * nxu], .rows = ny, .cols = nxu, .outer_stride = ny}});
    set_struct_field(1, create_tensor_var<real_t, 3>("CD", buf, {ny, nxu, N}));
    // C(N): (ny_N, nx+nu)
    buf.resize(ny_N * nx);
    batmat::linalg::copy(ocp.C(N),
                         Mat{{.data = buf.data(), .rows = ny_N, .cols = nx, .outer_stride = ny_N}});
    set_struct_field(2, create_tensor_var<real_t, 2>("CN", buf, {ny_N, nx}));

    // AB: (nx, nx+nu, N)
    buf.resize(N * nx * nxu);
    for (index_t i = 0; i < N; ++i)
        batmat::linalg::copy(
            ocp.AB(i),
            Mat{{.data = &buf[i * nx * nxu], .rows = nx, .cols = nxu, .outer_stride = nx}});
    set_struct_field(3, create_tensor_var<real_t, 3>("AB", buf, {nx, nxu, N}));

    // Vectors
    set_struct_field(4, create_tensor_var<real_t, 2>("qr", std::span(ocp.qr().data, ocp.qr().rows),
                                                     {ocp.qr().rows, 1}));
    set_struct_field(5, create_tensor_var<real_t, 2>("b", std::span(ocp.b().data, ocp.b().rows),
                                                     {ocp.b().rows, 1}));
    set_struct_field(6, create_tensor_var<real_t, 2>("b_min",
                                                     std::span(ocp.b_min().data, ocp.b_min().rows),
                                                     {ocp.b_min().rows, 1}));
    set_struct_field(7, create_tensor_var<real_t, 2>("b_max",
                                                     std::span(ocp.b_max().data, ocp.b_max().rows),
                                                     {ocp.b_max().rows, 1}));

    if (auto e = Mat_VarWrite(mat, struct_.get(), MAT_COMPRESSION_ZLIB); e)
        throw std::runtime_error(std::format("Failed to write struct {} ({})", varname, e));
}

void validate_mat_var(const matvar_t *var, const std::string &name, int expected_rank) {
    if (!var)
        throw std::runtime_error(std::format("Variable {} is missing", name));
    if (var->rank != expected_rank)
        throw std::runtime_error(std::format("Variable {}: invalid rank {} (expected {})", name,
                                             var->rank, expected_rank));
    if (var->isComplex)
        throw std::runtime_error(std::format("Variable {}: should be real", name));
    if (var->class_type != matio_traits<real_t>::class_)
        throw std::runtime_error(std::format("Variable {}: invalid class type", name));
    if (var->data_type != matio_traits<real_t>::type)
        throw std::runtime_error(std::format("Variable {}: invalid data type", name));
}

void read_from_mat(mat_t *mat, const std::string &varname, LinearOCPStorage &ocp) {
    MatVarPtr ocpvar(Mat_VarRead(mat, varname.c_str()), Mat_VarFree);
    if (!ocpvar)
        throw std::runtime_error(std::format("Missing variable: {}", varname));
    if (ocpvar->class_type != MAT_C_STRUCT)
        throw std::runtime_error(std::format("Variable {} should be a struct", varname));

    const matvar_t *ABmat    = Mat_VarGetStructFieldByName(ocpvar.get(), "AB", 0);
    const matvar_t *CDmat    = Mat_VarGetStructFieldByName(ocpvar.get(), "CD", 0);
    const matvar_t *CNmat    = Mat_VarGetStructFieldByName(ocpvar.get(), "CN", 0);
    const matvar_t *Hmat     = Mat_VarGetStructFieldByName(ocpvar.get(), "H", 0);
    const matvar_t *qrmat    = Mat_VarGetStructFieldByName(ocpvar.get(), "qr", 0);
    const matvar_t *bmat     = Mat_VarGetStructFieldByName(ocpvar.get(), "b", 0);
    const matvar_t *b_minmat = Mat_VarGetStructFieldByName(ocpvar.get(), "b_min", 0);
    const matvar_t *b_maxmat = Mat_VarGetStructFieldByName(ocpvar.get(), "b_max", 0);

    std::vector<std::string_view> missing;
    if (!ABmat)
        missing.emplace_back("AB");
    if (!CDmat)
        missing.emplace_back("CD");
    if (!CNmat)
        missing.emplace_back("CN");
    if (!Hmat)
        missing.emplace_back("H");
    if (!qrmat)
        missing.emplace_back("qr");
    if (!bmat)
        missing.emplace_back("b");
    if (!b_minmat)
        missing.emplace_back("b_min");
    if (!b_maxmat)
        missing.emplace_back("b_max");
    if (!missing.empty())
        throw std::runtime_error(
            std::format("Variable {} is missing fields: {}", varname, guanaqo::join(missing)));
    validate_mat_var(ABmat, "AB", 3);
    validate_mat_var(CDmat, "CD", 3);
    validate_mat_var(CNmat, "CN", 2);
    validate_mat_var(Hmat, "H", 3);
    validate_mat_var(qrmat, "qr", 2);
    validate_mat_var(bmat, "b", 2);
    validate_mat_var(b_minmat, "b_min", 2);
    validate_mat_var(b_maxmat, "b_max", 2);
    auto nx = static_cast<index_t>(ABmat->dims[0]), nxu = static_cast<index_t>(ABmat->dims[1]),
         N = static_cast<index_t>(ABmat->dims[2]), nu = nxu - nx;
    auto ny = static_cast<index_t>(CDmat->dims[0]), ny_N = static_cast<index_t>(CNmat->dims[0]);
    BATMAT_ASSERT(nxu >= nx);
    BATMAT_ASSERT(static_cast<index_t>(Hmat->dims[0]) == nxu);
    BATMAT_ASSERT(static_cast<index_t>(Hmat->dims[1]) == nxu);
    BATMAT_ASSERT(static_cast<index_t>(Hmat->dims[2]) == N + 1);
    BATMAT_ASSERT(static_cast<index_t>(CDmat->dims[1]) == nxu);
    BATMAT_ASSERT(static_cast<index_t>(CDmat->dims[2]) == N);
    BATMAT_ASSERT(static_cast<index_t>(CNmat->dims[1]) == nx);
    BATMAT_ASSERT(static_cast<index_t>(qrmat->dims[0]) == N * nxu + nx);
    BATMAT_ASSERT(static_cast<index_t>(qrmat->dims[1]) == 1);
    BATMAT_ASSERT(static_cast<index_t>(bmat->dims[0]) == (N + 1) * nx);
    BATMAT_ASSERT(static_cast<index_t>(bmat->dims[1]) == 1);
    BATMAT_ASSERT(static_cast<index_t>(b_minmat->dims[0]) == N * ny + ny_N);
    BATMAT_ASSERT(static_cast<index_t>(b_minmat->dims[1]) == 1);
    BATMAT_ASSERT(static_cast<index_t>(b_maxmat->dims[0]) == N * ny + ny_N);
    BATMAT_ASSERT(static_cast<index_t>(b_maxmat->dims[1]) == 1);

    using batmat::matrix::View;
    View<const real_t, index_t> ABview{
        {.data = static_cast<const real_t *>(ABmat->data), .depth = N, .rows = nx, .cols = nxu}};
    View<const real_t, index_t> CDview{
        {.data = static_cast<const real_t *>(CDmat->data), .depth = N, .rows = ny, .cols = nxu}};
    View<const real_t, index_t> CNview{
        {.data = static_cast<const real_t *>(CNmat->data), .depth = 1, .rows = ny_N, .cols = nx}};
    View<const real_t, index_t> Hview{{.data  = static_cast<const real_t *>(Hmat->data),
                                       .depth = N + 1,
                                       .rows  = nxu,
                                       .cols  = nxu}};
    View<const real_t, index_t> qrview{{.data  = static_cast<const real_t *>(qrmat->data),
                                        .depth = 1,
                                        .rows  = N * nxu + nx,
                                        .cols  = 1}};
    View<const real_t, index_t> bview{{.data  = static_cast<const real_t *>(bmat->data),
                                       .depth = 1,
                                       .rows  = (N + 1) * nx,
                                       .cols  = 1}};
    View<const real_t, index_t> b_minview{{.data  = static_cast<const real_t *>(b_minmat->data),
                                           .depth = 1,
                                           .rows  = N * ny + ny_N,
                                           .cols  = 1}};
    View<const real_t, index_t> b_maxview{{.data  = static_cast<const real_t *>(b_maxmat->data),
                                           .depth = 1,
                                           .rows  = N * ny + ny_N,
                                           .cols  = 1}};

    ocp = {.dim = {.N_horiz = N, .nx = nx, .nu = nu, .ny = ny, .ny_N = ny_N}};

    // H: (nx+nu, nx+nu, N+1)
    for (index_t i = 0; i < N; ++i)
        batmat::linalg::copy(Hview(i), ocp.H(i));
    // Q(N): (nx, nx), padded by zeros
    batmat::linalg::copy(Hview(N).top_left(nx, nx), ocp.Q(N));

    // CD: (ny, nx+nu, N)
    for (index_t i = 0; i < N; ++i)
        batmat::linalg::copy(CDview(i), ocp.CD(i));
    // C(N): (ny_N, nx+nu)
    batmat::linalg::copy(CNview(0), ocp.C(N));

    // AB: (nx, nx+nu, N)
    for (index_t i = 0; i < N; ++i)
        batmat::linalg::copy(ABview(i), ocp.AB(i));

    // Vectors
    ocp.qr()    = qrview(0);
    ocp.b()     = bview(0);
    ocp.b_min() = b_minview(0);
    ocp.b_max() = b_maxview(0);
}

auto open_vector_var(mat_t *mat, const std::string &varname) {
    MatVarPtr var(Mat_VarRead(mat, varname.c_str()), Mat_VarFree);
    if (!var)
        throw std::runtime_error(std::format("Missing variable: {}", varname));
    validate_mat_var(var.get(), varname, 2);
    if (var->dims[1] != 1)
        throw std::runtime_error(std::format("Variable {}: should have one column", varname));
    return var;
}

void read_from_mat(mat_t *mat, const std::string &varname, std::span<float> data) {
    auto var = open_vector_var(mat, varname);
    auto n   = var->dims[0];
    BATMAT_ASSERT(n == data.size());
    const auto *var_data = static_cast<const float *>(var->data);
    std::copy_n(var_data, n, data.begin());
}

void read_from_mat(mat_t *mat, const std::string &varname, std::span<double> data) {
    auto var = open_vector_var(mat, varname);
    auto n   = var->dims[0];
    BATMAT_ASSERT(n == data.size());
    const auto *var_data = static_cast<const double *>(var->data);
    std::copy_n(var_data, n, data.begin());
}

void read_from_mat(mat_t *mat, const std::string &varname, std::vector<float> &data) {
    auto var = open_vector_var(mat, varname);
    auto n   = var->dims[0];
    data.resize(n);
    const auto *var_data = static_cast<const float *>(var->data);
    std::copy_n(var_data, n, data.begin());
}

void read_from_mat(mat_t *mat, const std::string &varname, std::vector<double> &data) {
    auto var = open_vector_var(mat, varname);
    auto n   = var->dims[0];
    data.resize(n);
    const auto *var_data = static_cast<const double *>(var->data);
    std::copy_n(var_data, n, data.begin());
}

void ocp_dump_mat(const std::filesystem::path &filename, const LinearOCPStorage &ocp) {
    auto matfp = create_mat(filename);
    add_to_mat(matfp.get(), "ocp", ocp);
}

} // namespace cyqlone
