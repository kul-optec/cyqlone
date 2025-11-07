#include <cyqlone/config.hpp>

#include <Eigen/Core>
#include <unsupported/Eigen/MatrixFunctions>
#include <cassert>

namespace CYQLONE_NAMESPACE::qpalm::problems {

using eigen_mat = Eigen::MatrixX<real_t>;

inline std::tuple<eigen_mat, eigen_mat> discretize_zoh(const Eigen::Ref<const eigen_mat> &A,
                                                       const Eigen::Ref<const eigen_mat> &B,
                                                       real_t Ts) {
    assert(A.rows() == B.rows());
    auto nx = A.rows(), nu = B.cols();
    eigen_mat ABOO              = eigen_mat::Zero(nx + nu, nx + nu);
    ABOO.topLeftCorner(nx, nx)  = A;
    ABOO.topRightCorner(nx, nu) = B;
    eigen_mat ABOId             = (Ts * ABOO).exp();
    return {ABOId.topLeftCorner(nx, nx), ABOId.topRightCorner(nx, nu)};
}

inline std::tuple<eigen_mat, eigen_mat, eigen_mat>
discretize_zoh(const Eigen::Ref<const eigen_mat> &A, const Eigen::Ref<const eigen_mat> &B,
               const Eigen::Ref<const eigen_mat> &b, real_t Ts) {
    assert(A.rows() == B.rows());
    assert(A.rows() == b.rows());
    auto nx = A.rows(), nu = B.cols(), nb = b.cols();
    eigen_mat ABOO                 = eigen_mat::Zero(nx + nu + nb, nx + nu + nb);
    ABOO.block(0, 0, nx, nx)       = A;
    ABOO.block(0, nx, nx, nu)      = B;
    ABOO.block(0, nx + nu, nx, nb) = b;
    eigen_mat ABOId                = (Ts * ABOO).exp();
    return {ABOId.block(0, 0, nx, nx), ABOId.block(0, nx, nx, nu), ABOId.block(0, nx + nu, nx, nb)};
}

} // namespace CYQLONE_NAMESPACE::qpalm::problems
