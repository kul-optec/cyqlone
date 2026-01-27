#include <cyqlone/cyqlone.hpp>

#include <batmat/assume.hpp>

namespace CYQLONE_NS(cyqlone) {

template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::add_wrap_N(index_t a, index_t b) const -> index_t {
    const index_t N = ceil_N();
    BATMAT_ASSUME(N >= 0);
    BATMAT_ASSUME(a >= 0);
    BATMAT_ASSUME(b >= 0);
    BATMAT_ASSUME(a < N);
    a += b;
    return a >= N ? a - N : a;
}
template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::sub_wrap_N(index_t a, index_t b) const -> index_t {
    const index_t N = ceil_N();
    BATMAT_ASSUME(N >= 0);
    BATMAT_ASSUME(a >= 0);
    BATMAT_ASSUME(b >= 0);
    BATMAT_ASSUME(a < N);
    a -= b;
    return a < 0 ? a + N : a;
}
template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::sub_wrap_p(index_t a, index_t b) const -> index_t {
    BATMAT_ASSUME(p > 0);
    BATMAT_ASSUME(a >= 0);
    BATMAT_ASSUME(b >= 0);
    BATMAT_ASSUME(a < p);
    a -= b;
    return a < 0 ? a + p : a;
}
template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::add_wrap_p(index_t a, index_t b) const -> index_t {
    BATMAT_ASSUME(p > 0);
    BATMAT_ASSUME(a >= 0);
    BATMAT_ASSUME(b >= 0);
    BATMAT_ASSUME(a < p);
    a += b;
    return a >= p ? a - p : a;
}
template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::sub_wrap_ceil_p(index_t a, index_t b) const -> index_t {
    const index_t ceil_p = 1 << lp();
    BATMAT_ASSUME(ceil_p > 0);
    BATMAT_ASSUME(a >= 0);
    BATMAT_ASSUME(b >= 0);
    BATMAT_ASSUME(a < ceil_p);
    a -= b;
    return a < 0 ? a + ceil_p : a;
}
template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::add_wrap_ceil_p(index_t a, index_t b) const -> index_t {
    const index_t ceil_p = 1 << lp();
    BATMAT_ASSUME(ceil_p > 0);
    BATMAT_ASSUME(a >= 0);
    BATMAT_ASSUME(b >= 0);
    BATMAT_ASSUME(a < ceil_p);
    a += b;
    return a >= ceil_p ? a - ceil_p : a;
}
template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::sub_wrap_ceil_P(index_t a, index_t b) const -> index_t {
    const index_t ceil_P = (1 << lP);
    BATMAT_ASSUME(a >= 0);
    BATMAT_ASSUME(b >= 0);
    BATMAT_ASSUME(a < ceil_P);
    a -= b;
    return a < 0 ? a + ceil_P : a;
}
template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::add_wrap_ceil_P(index_t a, index_t b) const -> index_t {
    const index_t ceil_P = (1 << lP);
    BATMAT_ASSUME(a >= 0);
    BATMAT_ASSUME(b >= 0);
    BATMAT_ASSUME(a < ceil_P);
    a += b;
    return a >= ceil_P ? a - ceil_P : a;
}
template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::get_linear_batch_offset(index_t biA) const -> index_t {
    const auto levA = biA > 0 ? get_level(biA) : lP;
    const auto levP = lp();
    if (levA >= levP)
        return (((1 << levP) - 1) << (lP - levP)) + (biA >> levP);
    return (((1 << levA) - 1) << (lP - levA)) + get_index_in_level(biA);
}
template <index_t VL, class T, StorageOrder DefaultOrder>
[[nodiscard]] index_t CyqloneSolver<VL, T, DefaultOrder>::ν2(index_t bi) const {
    BATMAT_ASSUME(bi > 0);
    auto ui = static_cast<std::make_unsigned_t<index_t>>(bi);
    return static_cast<index_t>(std::countr_zero(ui));
}
template <index_t VL, class T, StorageOrder DefaultOrder>
[[nodiscard]] index_t CyqloneSolver<VL, T, DefaultOrder>::ν2p(index_t bi) const {
    BATMAT_ASSUME(bi >= 0);
    return bi == 0 ? lp() : ν2(bi);
}
template <index_t VL, class T, StorageOrder DefaultOrder>
[[nodiscard]] index_t CyqloneSolver<VL, T, DefaultOrder>::ν2P(index_t bi) const {
    BATMAT_ASSUME(bi >= 0);
    return bi == 0 ? lP : ν2(bi);
}

} // namespace CYQLONE_NS(cyqlone)
