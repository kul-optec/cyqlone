#include <cyqlone/qpalm/backends/ocp-backend-cyqlone.tpp>
#include <cyqlone/qpalm/implementation/solver.tpp>

namespace CYQLONE_NAMESPACE::qpalm {

#define CYQLONE_INSTANTIATE_QPALM_Cyqlone(vl)                                                      \
                                                                                                   \
    template class Solver<CyqloneBackend<vl> *>;                                                   \
    template class Solver<unique_CyqloneBackend<vl>>;                                              \
    template struct unique_CyqloneBackend<vl>;                                                     \
                                                                                                   \
    template unique_CyqloneBackend<vl> make_qpalm_cyqlone_backend<vl>(                             \
        const CyqloneStorage<> &ocp, CyqloneData data, const CyqloneBackendSettings &settings);    \
    template void update_qpalm_cyqlone_backend<vl>(CyqloneBackend<vl> &,                           \
                                                   const CyqloneStorage<> &ocp);                   \
    template void update_qpalm_cyqlone_backend<vl>(CyqloneBackend<vl> &,                           \
                                                   const LinearOCPStorage &ocp)

CYQLONE_INSTANTIATE_QPALM_Cyqlone(8);
CYQLONE_INSTANTIATE_QPALM_Cyqlone(4);
CYQLONE_INSTANTIATE_QPALM_Cyqlone(1);

} // namespace CYQLONE_NAMESPACE::qpalm
