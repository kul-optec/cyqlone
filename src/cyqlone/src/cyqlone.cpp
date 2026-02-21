#include <cyqlone/cyqlone.hpp>
#include <cyqlone/implementation/cr.tpp>
#include <cyqlone/implementation/data.tpp>
#include <cyqlone/implementation/factor.tpp>
#include <cyqlone/implementation/indexing.tpp>
#include <cyqlone/implementation/mat-vec.tpp>
#include <cyqlone/implementation/pcg.tpp>
#include <cyqlone/implementation/pcr.tpp>
#include <cyqlone/implementation/riccati.tpp>
#include <cyqlone/implementation/schur.tpp>
#include <cyqlone/implementation/sparse.tpp>
#include <cyqlone/implementation/update.tpp>
#include <batmat/config.hpp>

namespace CYQLONE_NS(cyqlone) {

#define CYQLONE_INSTANTIATE_TRICYQLE(T, VL, O) template struct TricyqleSolver<VL, T, O>;
BATMAT_FOREACH_DTYPE_VL(CYQLONE_INSTANTIATE_TRICYQLE, StorageOrder::ColMajor)
BATMAT_FOREACH_DTYPE_VL(CYQLONE_INSTANTIATE_TRICYQLE, StorageOrder::RowMajor)
#define CYQLONE_INSTANTIATE_CYQLONE(VL, O) template struct CyqloneSolver<VL, real_t, O>;
BATMAT_FOREACH_VL_DEFAULT(CYQLONE_INSTANTIATE_CYQLONE, StorageOrder::ColMajor)
BATMAT_FOREACH_VL_DEFAULT(CYQLONE_INSTANTIATE_CYQLONE, StorageOrder::RowMajor)

} // namespace CYQLONE_NS(cyqlone)
