#include <cyqlone/qpalm/backends/ocp-backend-cyqlone.tpp>
#include <cyqlone/qpalm/implementation/solver.tpp>
#include <batmat/config.hpp>

namespace CYQLONE_NS(cyqlone::qpalm) {

#define CYQLONE_INSTANTIATE_QPALM_Cyqlone(vl, order)                                               \
                                                                                                   \
    template class Solver<CyqloneBackend<vl, order> *>;                                            \
    template class Solver<unique_CyqloneBackend<vl, order>>;                                       \
    template struct unique_CyqloneBackend<vl, order>;                                              \
                                                                                                   \
    template unique_CyqloneBackend<vl, order> make_qpalm_cyqlone_backend<vl, order>(               \
        const CyqloneStorage<> &ocp, CyqloneData data, const CyqloneBackendSettings &settings);    \
    template void update_qpalm_cyqlone_backend<vl, order>(CyqloneBackend<vl, order> &,             \
                                                          const CyqloneStorage<> &ocp);            \
    template void update_qpalm_cyqlone_backend<vl, order>(CyqloneBackend<vl, order> &,             \
                                                          const LinearOCPStorage &ocp)

#define CYQLONE_INSTANTIATE_SOLVER(VL)                                                             \
    CYQLONE_INSTANTIATE_QPALM_Cyqlone(VL, StorageOrder::ColMajor);                                 \
    CYQLONE_INSTANTIATE_QPALM_Cyqlone(VL, StorageOrder::RowMajor);
BATMAT_FOREACH_VL_DOUBLE(CYQLONE_INSTANTIATE_SOLVER)

} // namespace CYQLONE_NS(cyqlone::qpalm)
