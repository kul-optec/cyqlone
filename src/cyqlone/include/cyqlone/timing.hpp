#pragma once

#include <cyqlone/config.hpp>
#include <batmat/timing.hpp>

namespace CYQLONE_NAMESPACE {
struct DefaultTimings : batmat::DefaultTimings {};
} // namespace CYQLONE_NAMESPACE

// Deduction guide for guanaqo::Timed<cyqlone::DefaultTiming>
namespace guanaqo {
Timed(::cyqlone::DefaultTimings &) -> Timed<::batmat::DefaultTimings>;
}
