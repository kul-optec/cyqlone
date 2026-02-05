#include <cyqlone/tracing.hpp>

#if GUANAQO_WITH_TRACING

#include <batmat/assume.hpp>
#include <chrono>
#include <cstring>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <ratio>
#include <span>
#include <string>
#include <string_view>

#if CYQLONE_WITH_ZLIB
#include <zlib.h>
#endif

namespace cyqlone {

namespace fs = std::filesystem;

inline auto ns_to_us(std::chrono::nanoseconds ns) {
    return std::chrono::duration<double, std::micro>(ns).count();
}

inline auto gflops(const guanaqo::TraceLogger::Log &log) {
    bool valid    = log.duration.count() > 0 && log.flop_count >= 0;
    auto duration = std::chrono::duration<double, std::nano>(log.duration).count();
    return valid ? static_cast<double>(log.flop_count) / duration : 0.0;
}

inline bool is_barrier_event(const guanaqo::TraceLogger::Log &log) {
    static constexpr std::string_view prefix = "barrier-";
    return std::strncmp(log.name, prefix.data(), prefix.size()) == 0;
}

inline bool is_read_event(const guanaqo::TraceLogger::Log &log) {
    static constexpr std::string_view prefix = "r:";
    return std::strncmp(log.name, prefix.data(), prefix.size()) == 0;
}

inline bool is_write_event(const guanaqo::TraceLogger::Log &log) {
    static constexpr std::string_view prefix = "w:";
    return std::strncmp(log.name, prefix.data(), prefix.size()) == 0;
}

#if CYQLONE_WITH_ZLIB
struct gzfile_deleter {
    void operator()(gzFile f) const noexcept { gzclose(f); }
};
using gzfile_ptr = std::unique_ptr<std::remove_pointer_t<gzFile>, gzfile_deleter>;

inline void gzwrite(gzFile f, std::string_view sv) {
    gzwrite(f, sv.data(), static_cast<unsigned>(sv.size()));
}
#endif

inline std::function<void(std::string_view)> get_writer(const fs::path &path) {
    if (path.filename().string().ends_with(".json.gz")) {
#if CYQLONE_WITH_ZLIB
        gzfile_ptr file{gzopen(path.string().c_str(), "wb")};
        if (!file)
            throw std::runtime_error("Failed to open file " + path.string() + " for writing");
        return [file = std::shared_ptr{std::move(file)}](std::string_view sv) {
            gzwrite(file.get(), sv); // TODO: check return value
        };
#else
        throw std::runtime_error("Cannot write .gz file: zlib support not enabled");
#endif
    } else if (path.filename().string().ends_with(".json")) {
        auto file = std::make_shared<std::ofstream>(path);
        if (!file || !file->is_open())
            throw std::runtime_error("Failed to open file " + path.string() + " for writing");
        return [file = std::move(file)](std::string_view sv) {
            file->write(sv.data(), static_cast<ptrdiff_t>(sv.size())); // TODO: check bad bit
        };
    } else {
        throw std::runtime_error("Unsupported file extension: should be .json or .json.gz");
    }
}

void write_chrome_trace(const fs::path &path, std::span<const guanaqo::TraceLogger::Log> logs,
                        const TracingOptions &opts) {
    using namespace std::string_view_literals;
    auto writer = get_writer(path);
    std::string buf(4095, '\0');
    bool first = true;
    writer("{\"traceEvents\":["sv);
    std::map<std::size_t, std::size_t> thread_ids;
    std::map<std::size_t, uint64_t> thread_flops;
    std::size_t num_threads = 0;
    for (const auto &log : logs)
        if (log.name == "thread_id"sv && thread_ids.try_emplace(log.thread_id, log.instance).second)
            num_threads = std::max(num_threads, static_cast<std::size_t>(log.instance + 1));
    for (const auto &log : logs)
        if (thread_ids.try_emplace(log.thread_id, num_threads).second)
            ++num_threads;
    auto get_thread_id = [&](std::size_t orig_id) {
        auto it = thread_ids.find(orig_id);
        return it != thread_ids.end() ? it->second : orig_id;
    };
    for (const auto &log : logs) {
        if (log.name == "thread_id"sv)
            continue;
        if (opts.no_barrier && is_barrier_event(log))
            continue;
        BATMAT_ASSERT(!std::string_view{log.name}.contains('\"')); // TODO: proper escaping
        const auto ts_us  = ns_to_us(log.start_time);
        const auto dur_us = ns_to_us(log.duration);
        auto tid          = get_thread_id(log.thread_id);
        auto entry        = [&] {
            if (is_read_event(log)) {
                return std::format_to_n( //
                    buf.data(), static_cast<ptrdiff_t>(buf.size()),
                    "{}{{\"cat\":\"{}\",\"id\":{},\"ph\":\"f\",\"ts\":{:.3f},"
                           "\"pid\":0,\"tid\":{}}}",
                    first ? "\n" : ",\n", std::string_view{log.name}.substr(2), log.instance, ts_us,
                    tid);
            } else if (is_write_event(log)) {
                return std::format_to_n( //
                    buf.data(), static_cast<ptrdiff_t>(buf.size()),
                    "{}{{\"cat\":\"{}\",\"id\":{},\"ph\":\"s\",\"ts\":{:.3f},"
                           "\"pid\":0,\"tid\":{}}}",
                    first ? "\n" : ",\n", std::string_view{log.name}.substr(2), log.instance, ts_us,
                    tid);
            } else if (log.flop_count == -1) {
                std::string_view cat = is_barrier_event(log) ? "barrier" : "trace";
                if (log.duration.count() > 0)
                    return std::format_to_n( //
                        buf.data(), static_cast<ptrdiff_t>(buf.size()),
                        "{}{{\"name\":\"{}\",\"cat\":\"{}\",\"ph\":\"X\",\"ts\":{:.3f},"
                               "\"pid\":0,\"tid\":{},\"dur\":{:.3f},"
                               "\"args\":{{\"instance\":{}}}}}",
                        first ? "\n" : ",\n", log.name, cat, ts_us, tid, dur_us, log.instance);
                else
                    return std::format_to_n( //
                        buf.data(), static_cast<ptrdiff_t>(buf.size()),
                        "{}{{\"name\":\"{}\",\"cat\":\"{}\",\"ph\":\"i\",\"ts\":{:.3f},"
                               "\"pid\":0,\"tid\":{},"
                               "\"args\":{{\"instance\":{}}}}}",
                        first ? "\n" : ",\n", log.name, cat, ts_us, tid, log.instance);
            } else {
                uint64_t &total_flops = thread_flops[tid];
                auto old_flops        = 1e-9 * static_cast<double>(total_flops);
                auto new_flops        = 1e-9 * static_cast<double>(total_flops += log.flop_count);
                return std::format_to_n( //
                    buf.data(), static_cast<ptrdiff_t>(buf.size()),
                    "{0}{{\"name\":\"{1}\",\"cat\":\"gflops\",\"ph\":\"X\",\"ts\":{2:.3f},"
                           "\"pid\":0,\"tid\":{3},\"dur\":{4:.3f},\"args\":{{\"instance\":{5},"
                           "\"flop_count\":{6},\"gflops\":{7:.6f}}}}},\n"
                           "  {{\"name\":\"gflops_{3}\",\"cat\":\"{1}\",\"ph\":\"C\",\"ts\":{2:.3f},"
                           "\"pid\":0,\"tid\":{3},\"args\":{{\"gflop_count\":{8:.9f}}}}},\n"
                           "  {{\"name\":\"gflops_{3}\",\"cat\":\"{1}\",\"ph\":\"C\",\"ts\":{9:.3f},"
                           "\"pid\":0,\"tid\":{3},\"args\":{{\"gflop_count\":{10:.9f}}}}}",
                    first ? "\n" : ",\n", // 0
                    log.name,             // 1
                    ts_us,                // 2
                    tid,                  // 3
                    dur_us,               // 4
                    log.instance,         // 5
                    log.flop_count,       // 6
                    gflops(log),          // 7
                    old_flops,            // 8
                    ts_us + dur_us,       // 9
                    new_flops             // 10
                );
            }
        }();
        writer(std::string_view{buf.data(), static_cast<size_t>(entry.size)});
        first = false;
    }
    writer("\n]}"sv);
}

} // namespace cyqlone

#endif
