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

#define CYQLONE_INSTANTIATE_SOLVER(T, VL)                                                          \
    template class CyqloneSolver<VL, T, StorageOrder::ColMajor>;                                   \
    template class CyqloneSolver<VL, T, StorageOrder::RowMajor>;
BATMAT_FOREACH_DTYPE_VL(CYQLONE_INSTANTIATE_SOLVER)

} // namespace CYQLONE_NS(cyqlone)
