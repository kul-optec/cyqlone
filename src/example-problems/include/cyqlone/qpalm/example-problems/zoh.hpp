#include <cyqlone/config.hpp>

#include <Eigen/Core>
#include <unsupported/Eigen/MatrixFunctions>

namespace CYQLONE_NAMESPACE::qpalm::problems {

using eigen_mat = Eigen::MatrixX<real_t>;

std::tuple<eigen_mat, eigen_mat> discretize_zoh(const Eigen::Ref<const eigen_mat> &A,
                                                const Eigen::Ref<const eigen_mat> &B, real_t Ts) {
    assert(A.rows() == B.rows());
    auto nx = A.rows(), nu = B.cols();
    eigen_mat ABOO              = eigen_mat::Zero(nx + nu, nx + nu);
    ABOO.topLeftCorner(nx, nx)  = A;
    ABOO.topRightCorner(nx, nu) = B;
    eigen_mat ABOId             = (Ts * ABOO).exp();
    return {ABOId.topLeftCorner(nx, nx), ABOId.topRightCorner(nx, nu)};
}

} // namespace CYQLONE_NAMESPACE::qpalm::problems
