#include <gtest/gtest.h>
#include <filesystem>

#include <cyqlone/matio.hpp>
#include <cyqlone/ocp.hpp>

using cyqlone::index_t;
using cyqlone::real_t;

TEST(Matio, ocp) {
    std::filesystem::path pth = std::filesystem::temp_directory_path();
    auto mat                  = cyqlone::create_mat(pth / "ocp.mat");
    cyqlone::LinearOCPStorage ocp{.dim = {.N_horiz = 31, .nx = 5, .nu = 3, .ny = 7, .ny_N = 2}};
    const auto N = ocp.dim.N_horiz;
    for (index_t i = 0; i < N; ++i) {
        for (index_t r = 0; r < ocp.dim.nx; ++r)
            for (index_t c = 0; c < ocp.dim.nx; ++c)
                ocp.A(i)(r, c) = static_cast<real_t>(91000 + i * 100 + r + 10 * c);
        for (index_t r = 0; r < ocp.dim.nx; ++r)
            for (index_t c = 0; c < ocp.dim.nu; ++c)
                ocp.B(i)(r, c) = static_cast<real_t>(92000 + i * 100 + r + 10 * c);
        for (index_t r = 0; r < ocp.dim.ny; ++r)
            for (index_t c = 0; c < ocp.dim.nx; ++c)
                ocp.C(i)(r, c) = static_cast<real_t>(93000 + i * 100 + r + 10 * c);
        for (index_t r = 0; r < ocp.dim.ny; ++r)
            for (index_t c = 0; c < ocp.dim.nu; ++c)
                ocp.D(i)(r, c) = static_cast<real_t>(94000 + i * 100 + r + 10 * c);
        for (index_t r = 0; r < ocp.dim.nx + ocp.dim.nu; ++r)
            for (index_t c = 0; c < ocp.dim.nx + ocp.dim.nu; ++c)
                ocp.H(i)(r, c) = static_cast<real_t>(95000 + i * 100 + r + 10 * c);
    }
    for (index_t r = 0; r < ocp.dim.ny_N; ++r)
        for (index_t c = 0; c < ocp.dim.nx; ++c)
            ocp.C(N)(r, c) = static_cast<real_t>(93000 + N * 100 + r + 10 * c);
    for (index_t r = 0; r < ocp.dim.nx; ++r)
        for (index_t c = 0; c < ocp.dim.nx; ++c)
            ocp.Q(N)(r, c) = static_cast<real_t>(95000 + N * 100 + r + 10 * c);

    cyqlone::add_to_mat(mat.get(), ocp);
    mat.reset();

    auto mat_loaded = cyqlone::open_mat(pth / "ocp.mat");
    cyqlone::LinearOCPStorage ocp_loaded;
    cyqlone::read_from_mat(mat_loaded.get(), ocp_loaded);

    ASSERT_EQ(ocp.dim.N_horiz, ocp_loaded.dim.N_horiz);
    ASSERT_EQ(ocp.dim.nx, ocp_loaded.dim.nx);
    ASSERT_EQ(ocp.dim.nu, ocp_loaded.dim.nu);
    ASSERT_EQ(ocp.dim.ny, ocp_loaded.dim.ny);
    ASSERT_EQ(ocp.dim.ny_N, ocp_loaded.dim.ny_N);

    for (index_t i = 0; i < N; ++i) {
        for (index_t r = 0; r < ocp.dim.nx; ++r)
            for (index_t c = 0; c < ocp.dim.nx; ++c)
                EXPECT_EQ(ocp.A(i)(r, c), ocp_loaded.A(i)(r, c));
        for (index_t r = 0; r < ocp.dim.nx; ++r)
            for (index_t c = 0; c < ocp.dim.nu; ++c)
                EXPECT_EQ(ocp.B(i)(r, c), ocp_loaded.B(i)(r, c));
        for (index_t r = 0; r < ocp.dim.ny; ++r)
            for (index_t c = 0; c < ocp.dim.nx; ++c)
                EXPECT_EQ(ocp.C(i)(r, c), ocp_loaded.C(i)(r, c));
        for (index_t r = 0; r < ocp.dim.ny; ++r)
            for (index_t c = 0; c < ocp.dim.nu; ++c)
                EXPECT_EQ(ocp.D(i)(r, c), ocp_loaded.D(i)(r, c));
        for (index_t r = 0; r < ocp.dim.nx + ocp.dim.nu; ++r)
            for (index_t c = 0; c < ocp.dim.nx + ocp.dim.nu; ++c)
                EXPECT_EQ(ocp.H(i)(r, c), ocp_loaded.H(i)(r, c));
    }
    for (index_t r = 0; r < ocp.dim.ny_N; ++r)
        for (index_t c = 0; c < ocp.dim.nx; ++c)
            EXPECT_EQ(ocp.C(N)(r, c), ocp_loaded.C(N)(r, c));
    for (index_t r = 0; r < ocp.dim.nx; ++r)
        for (index_t c = 0; c < ocp.dim.nx; ++c)
            EXPECT_EQ(ocp.Q(N)(r, c), ocp_loaded.Q(N)(r, c));
}
