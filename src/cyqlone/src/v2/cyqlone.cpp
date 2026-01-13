#include <cyqlone/v2/cyqlone.hpp>
#include <cyqlone/v2/implementation/cr.tpp>
#include <cyqlone/v2/implementation/data.tpp>
#include <cyqlone/v2/implementation/factor.tpp>
#include <cyqlone/v2/implementation/indexing.tpp>
#include <cyqlone/v2/implementation/mat-vec.tpp>
#include <cyqlone/v2/implementation/pcg.tpp>
#include <cyqlone/v2/implementation/pcr.tpp>
#include <cyqlone/v2/implementation/riccati.tpp>
#include <cyqlone/v2/implementation/schur.tpp>
#include <cyqlone/v2/implementation/sparse.tpp>
#include <cyqlone/v2/implementation/update.tpp>
#include <batmat/config.hpp>

namespace CYQLONE_NS(cyqlone)::v2 {

#define CYQLONE_INSTANTIATE_SOLVER(T, VL)                                                          \
    template class CyqloneSolver<VL, T, StorageOrder::ColMajor>;                                   \
    template class CyqloneSolver<VL, T, StorageOrder::RowMajor>;
BATMAT_FOREACH_DTYPE_VL(CYQLONE_INSTANTIATE_SOLVER)

} // namespace CYQLONE_NS(cyqlone)::v2
