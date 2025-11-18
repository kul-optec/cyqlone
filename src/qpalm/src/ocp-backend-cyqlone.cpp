#include <cyqlone/qpalm/backends/ocp-backend-cyqlone.tpp>
#include <cyqlone/qpalm/implementation/solver.tpp>

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

CYQLONE_INSTANTIATE_QPALM_Cyqlone(8, StorageOrder::ColMajor);
CYQLONE_INSTANTIATE_QPALM_Cyqlone(4, StorageOrder::ColMajor);
CYQLONE_INSTANTIATE_QPALM_Cyqlone(1, StorageOrder::ColMajor);
CYQLONE_INSTANTIATE_QPALM_Cyqlone(8, StorageOrder::RowMajor);
CYQLONE_INSTANTIATE_QPALM_Cyqlone(4, StorageOrder::RowMajor);
CYQLONE_INSTANTIATE_QPALM_Cyqlone(1, StorageOrder::RowMajor);

} // namespace CYQLONE_NS(cyqlone::qpalm)
