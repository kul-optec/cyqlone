#include <cyqlone/config.hpp>
#include <cyqlone/qpalm/implementation/breakpoint.hpp>
#include <guanaqo/trace.hpp>
#include <cmath>
#include <ranges>

namespace CYQLONE_NS(cyqlone::qpalm) {

template <class Vec>
std::span<Breakpoint>
compute_breakpoints_default(std::vector<Breakpoint> &breakpoints, const Vec &Σ, const Vec &y,
                            const Vec &Ad, const Vec &Ax, const Vec &b_min, const Vec &b_max) {
    GUANAQO_TRACE("linesearch breakpoints", 0);
    using std::sqrt;
    // Allocate memory
    auto m = static_cast<size_t>(y.size());
    breakpoints.resize(2 * m);
    // Compute break points t[i] and intermediate values α[i] and δ[i]
    auto indices = std::views::iota(index_t{});
    for (auto [Σi, yi, Adi, Axi, li, ui, i] :
         std::views::zip(Σ, y, Ad, Ax, b_min, b_max, indices)) {
        const auto s         = sqrt(Σi);
        breakpoints[m + i].δ = s * Adi;
        breakpoints[i].δ     = -breakpoints[m + i].δ;
        breakpoints[i].t     = (yi + Σi * (Axi - li)) / (s * breakpoints[i].δ);
        breakpoints[m + i].t = (Σi * (ui - Axi) - yi) / (s * breakpoints[m + i].δ);
    }
    return std::span{breakpoints};
}

} // namespace CYQLONE_NS(cyqlone::qpalm)
