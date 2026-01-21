#pragma once

#include <guanaqo/trace.hpp>

#if GUANAQO_WITH_TRACING
#include <filesystem>
#include <span>

namespace cyqlone {

namespace fs = std::filesystem;

void write_chrome_trace(const fs::path &filename,
                           std::span<const guanaqo::TraceLogger::Log> logs);

} // namespace cyqlone

#endif
