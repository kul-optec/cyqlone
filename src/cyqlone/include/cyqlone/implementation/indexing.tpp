#include <cyqlone/cyqlone.hpp>

#include <batmat/assume.hpp>

namespace CYQLONE_NS(cyqlone) {

template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::add_wrap_N(index_t a, index_t b) const -> index_t {
    const index_t N = ceil_N;
    BATMAT_ASSUME(a >= 0);
    BATMAT_ASSUME(b >= 0);
    BATMAT_ASSUME(a < N);
    a += b;
    return a >= N ? a - N : a;
}
template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::sub_wrap_N(index_t a, index_t b) const -> index_t {
    const index_t N = ceil_N;
    BATMAT_ASSUME(a >= 0);
    BATMAT_ASSUME(b >= 0);
    BATMAT_ASSUME(a < N);
    a -= b;
    return a < 0 ? a + N : a;
}
template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::sub_wrap_PmV(index_t a, index_t b) const -> index_t {
    BATMAT_ASSUME(a >= 0);
    BATMAT_ASSUME(b >= 0);
    BATMAT_ASSUME(a < (1 << (lP - lvl)));
    a -= b;
    return a < 0 ? a + (1 << (lP - lvl)) : a;
}
template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::add_wrap_PmV(index_t a, index_t b) const -> index_t {
    BATMAT_ASSUME(a >= 0);
    BATMAT_ASSUME(b >= 0);
    BATMAT_ASSUME(a < (1 << (lP - lvl)));
    a += b;
    return a >= (1 << (lP - lvl)) ? a - (1 << (lP - lvl)) : a;
}
template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::sub_wrap_P(index_t a, index_t b) const -> index_t {
    BATMAT_ASSUME(a >= 0);
    BATMAT_ASSUME(b >= 0);
    BATMAT_ASSUME(a < (1 << lP));
    a -= b;
    return a < 0 ? a + (1 << lP) : a;
}
template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::get_linear_batch_offset(index_t biA) const -> index_t {
    const auto levA = biA > 0 ? get_level(biA) : lP;
    const auto levP = lP - lvl;
    if (levA >= levP)
        return (((1 << levP) - 1) << (lP - levP)) + (biA >> levP);
    return (((1 << levA) - 1) << (lP - levA)) + get_index_in_level(biA);
}
template <index_t VL, class T, StorageOrder DefaultOrder>
[[nodiscard]] bool CyqloneSolver<VL, T, DefaultOrder>::is_active(index_t l, index_t bi) const {
    const index_t lbi   = bi > 0 ? get_level(bi) : lP - lvl;
    const bool inactive = lbi < l;
    return ((bi >> l) & 1) == 1 && !inactive;
}
template <index_t VL, class T, StorageOrder DefaultOrder>
[[nodiscard]] index_t CyqloneSolver<VL, T, DefaultOrder>::ν2p(index_t bi) const {
    BATMAT_ASSUME(bi >= 0);
    return bi == 0 ? lP - lvl : get_level(bi);
}
template <index_t VL, class T, StorageOrder DefaultOrder>
[[nodiscard]] bool CyqloneSolver<VL, T, DefaultOrder>::is_U_below_Y(index_t l, index_t bi) const {
    return ((bi >> l) & 3) == 1 && l + 1 != lP - lvl;
}

} // namespace CYQLONE_NS(cyqlone)
