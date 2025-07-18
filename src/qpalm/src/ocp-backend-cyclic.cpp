#include <cyqlone/qpalm/backends/ocp-backend-cyclic.tpp>
#include <cyqlone/qpalm/implementation/solver.tpp>

namespace cyqlone::qpalm {

#define KOQKATOO_INSTANTIATE_QPALM_CyclOCP(vl)                                                     \
                                                                                                   \
    template class Solver<CyclOCPBackend<vl> *>;                                                   \
    template class Solver<unique_CyclOCPBackend<vl>>;                                              \
    template struct unique_CyclOCPBackend<vl>;                                                     \
                                                                                                   \
    template unique_CyclOCPBackend<vl> make_qpalm_cyclocp_backend<vl>(                             \
        const CyqloneStorage<> &ocp, CyclOCPData data, const CyclOCPBackendSettings &settings)

KOQKATOO_INSTANTIATE_QPALM_CyclOCP(8);
KOQKATOO_INSTANTIATE_QPALM_CyclOCP(4);
KOQKATOO_INSTANTIATE_QPALM_CyclOCP(2);
KOQKATOO_INSTANTIATE_QPALM_CyclOCP(1);

} // namespace cyqlone::qpalm
