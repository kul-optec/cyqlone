#pragma once

#include <cyqlone/config.hpp>

namespace cyqlone::qpalm {

struct Breakpoint {
    // t = α/δ   <=>   α = t δ
    real_t t, δ;
    [[gnu::always_inline, nodiscard]] real_t α() const { return t * δ; }
};

} // namespace cyqlone::qpalm
