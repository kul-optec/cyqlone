#include <cyqlone/qpalm/backends/ocp-backend-cyqlone.tpp>
#include <cyqlone/qpalm/implementation/solver.tpp>
#include <batmat/config.hpp>

namespace CYQLONE_NS(cyqlone::qpalm) {

#define CYQLONE_INSTANTIATE_QPALM_Cyqlone(v, order)                                                \
                                                                                                   \
    template class Solver<CyqloneBackend<v, order> *>;                                             \
    template class Solver<unique_CyqloneBackend<v, order>>;                                        \
    template struct unique_CyqloneBackend<v, order>;                                               \
                                                                                                   \
    template unique_CyqloneBackend<v, order> make_qpalm_cyqlone_backend<v, order>(                 \
        const CyqloneStorage<> &ocp, CyqloneData data, const CyqloneBackendSettings &settings);    \
    template void update_qpalm_cyqlone_backend<v, order>(CyqloneBackend<v, order> &,               \
                                                         const CyqloneStorage<> &ocp);             \
    template void update_qpalm_cyqlone_backend<v, order>(CyqloneBackend<v, order> &,               \
                                                         const LinearOCPStorage &ocp);

BATMAT_FOREACH_VL_DOUBLE(CYQLONE_INSTANTIATE_QPALM_Cyqlone, StorageOrder::ColMajor)
BATMAT_FOREACH_VL_DOUBLE(CYQLONE_INSTANTIATE_QPALM_Cyqlone, StorageOrder::RowMajor)

} // namespace CYQLONE_NS(cyqlone::qpalm)
