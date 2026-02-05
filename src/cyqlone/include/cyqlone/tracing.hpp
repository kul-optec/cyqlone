#pragma once

#include <guanaqo/trace.hpp>
#include <cstdint>

#if GUANAQO_WITH_TRACING
#include <filesystem>
#include <span>

namespace cyqlone {

namespace fs = std::filesystem;

struct TracingOptions {
    bool no_barrier = true; ///< Do not log barrier events.
};

void write_chrome_trace(const fs::path &filename, std::span<const guanaqo::TraceLogger::Log> logs,
                        const TracingOptions &opts = {});

enum class TraceDataNames {
    M,
    L,
    Y,
    U,
    Kf,
    Kb,
    Upf,
    Upb,
    Q,
    T,
    Sentinel,
};

inline uint32_t tracing_data_ids[static_cast<size_t>(TraceDataNames::Sentinel)][256][8]{};

} // namespace cyqlone

#define CYQ_TRACE_WRITE(name, index, lr)                                                           \
    do {                                                                                           \
        using ::cyqlone::TraceDataNames;                                                           \
        uint64_t id = ++tracing_data_ids[static_cast<size_t>(TraceDataNames::name)][index][lr];    \
        id += static_cast<uint64_t>(TraceDataNames::name) << 32;                                   \
        id += static_cast<uint64_t>(index) << 48;                                                  \
        id += static_cast<uint64_t>(lr) << 56;                                                     \
        auto s = ::guanaqo::get_trace_logger().trace("w:" #name, id);                              \
        s.log  = nullptr;                                                                          \
    } while (false)
#define CYQ_TRACE_READ(name, index, lr)                                                            \
    do {                                                                                           \
        using ::cyqlone::TraceDataNames;                                                           \
        uint64_t id = tracing_data_ids[static_cast<size_t>(TraceDataNames::name)][index][lr];      \
        id += static_cast<uint64_t>(TraceDataNames::name) << 32;                                   \
        id += static_cast<uint64_t>(index) << 48;                                                  \
        id += static_cast<uint64_t>(lr) << 56;                                                     \
        auto s = ::guanaqo::get_trace_logger().trace("r:" #name, id);                              \
        s.log  = nullptr;                                                                          \
    } while (false)

#else

#define CYQ_TRACE_WRITE(...) GUANAQO_NOOP()
#define CYQ_TRACE_READ(...) GUANAQO_NOOP()

#endif
