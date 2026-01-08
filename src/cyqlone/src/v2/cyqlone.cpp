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

template struct CyqloneSolver<1, real_t, StorageOrder::ColMajor>;
template struct CyqloneSolver<4, real_t, StorageOrder::ColMajor>;
template struct CyqloneSolver<8, real_t, StorageOrder::ColMajor>;

template struct CyqloneSolver<1, real_t, StorageOrder::RowMajor>;
template struct CyqloneSolver<4, real_t, StorageOrder::RowMajor>;
template struct CyqloneSolver<8, real_t, StorageOrder::RowMajor>;

#if BATMAT_WITH_SINGLE
template struct CyqloneSolver<16, float, StorageOrder::ColMajor>;
template struct CyqloneSolver<16, float, StorageOrder::RowMajor>;
#endif

} // namespace CYQLONE_NS(cyqlone)::v2
