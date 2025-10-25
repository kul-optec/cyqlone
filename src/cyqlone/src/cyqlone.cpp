#include <cyqlone/cyqlone.hpp>
#include <cyqlone/implementation/data.tpp>
#include <cyqlone/implementation/factor.tpp>
#include <cyqlone/implementation/indexing.tpp>
#include <cyqlone/implementation/mat-vec.tpp>
#include <cyqlone/implementation/pcg.tpp>
#include <cyqlone/implementation/solve.tpp>
#include <cyqlone/implementation/sparse.tpp>
#include <cyqlone/implementation/update.tpp>
#include <batmat/config.hpp>

namespace cyqlone {

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

} // namespace cyqlone
