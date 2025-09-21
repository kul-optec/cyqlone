#pragma once

#include <cyqlone/config.hpp>
#include <span>

namespace cyqlone::qpalm {

struct Breakpoint {
    // t = α/δ   <=>   α = t δ
    real_t t, δ;
    [[gnu::always_inline, nodiscard]] real_t α() const { return t * δ; }
};

struct PartitionedBreakpoints {
    std::span<Breakpoint> neg_bp, pos_bp;
};

} // namespace cyqlone::qpalm
