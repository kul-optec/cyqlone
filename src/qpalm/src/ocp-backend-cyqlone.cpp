#include <cyqlone/qpalm/backends/ocp-backend-cyqlone.tpp>
#include <cyqlone/qpalm/implementation/solver.tpp>
#include <batmat/config.hpp>

namespace CYQLONE_NS(cyqlone::qpalm) {

#define CYQLONE_INSTANTIATE_QPALM_Cyqlone(v, order)                                                \
                                                                                                   \
    template class Solver<CyQPALMBackend<v, order> *>;                                             \
    template class Solver<unique_CyQPALMBackend<v, order>>;                                        \
    template struct unique_CyQPALMBackend<v, order>;                                               \
                                                                                                   \
    template unique_CyQPALMBackend<v, order> make_cyqpalm_backend<v, order>(                 \
        const CyqloneStorage<> &ocp, CyqloneData data, const CyQPALMBackendSettings &settings);    \
    template void update_cyqpalm_backend<v, order>(CyQPALMBackend<v, order> &,               \
                                                         const CyqloneStorage<> &ocp);             \
    template void update_cyqpalm_backend<v, order>(CyQPALMBackend<v, order> &,               \
                                                         const LinearOCPStorage &ocp);

BATMAT_FOREACH_VL_DOUBLE(CYQLONE_INSTANTIATE_QPALM_Cyqlone, StorageOrder::ColMajor)
BATMAT_FOREACH_VL_DOUBLE(CYQLONE_INSTANTIATE_QPALM_Cyqlone, StorageOrder::RowMajor)

} // namespace CYQLONE_NS(cyqlone::qpalm)
