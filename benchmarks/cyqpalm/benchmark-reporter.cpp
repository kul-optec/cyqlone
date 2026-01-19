#include <benchmark/benchmark.h>
#include <fmt/color.h>
#include <guanaqo/string-util.hpp>
#include <limits>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

// Custom Google Benchmark reporter that formats results in a table with:
// - Fixed-width columns for easy reading
// - Headers with problem name that print when the problem changes
// - Specific counters: success, iter, res_dual, res_eq, res_ineq, compl, outer_iter, num_factor, num_upd, rank_upd
// - Additional counters printed on a separate optional line in parentheses

class CustomReporter : public benchmark::BenchmarkReporter {
  public:
    explicit CustomReporter(size_t problem_name_width, size_t solver_name_width,
                            bool print_extra = true, bool with_color = true)
        : max_problem_name_width(problem_name_width), max_solver_name_width(solver_name_width),
          print_extra(print_extra), with_color(with_color) {}

    bool ReportContext(const Context &context) override { return true; }

    void ReportRuns(const std::vector<Run> &reports) override {
        for (const auto &run : reports) {
            auto [problem_name, solver_name] = ParseRunName(run.run_name.str());

            // Print header if problem name changed
            if (problem_name != current_problem_name_) {
                current_problem_name_   = problem_name;
                current_solver_name_    = solver_name;
                current_solver_counter_ = 0;
                PrintHeader(problem_name);
            } else if (solver_name != current_solver_name_) {
                current_solver_name_ = solver_name;
                if (std::exchange(current_solver_counter_, 0) > 1)
                    fmt::print("---\n");
            }
            ++current_solver_counter_;

            PrintRunData(run, solver_name);
        }
    }

  private:
    fmt::text_style $(fmt::text_style s) const { return with_color ? s : fmt::text_style{}; }

    struct Field {
        std::string name;
        int width;
        bool is_integer;
    };

    static constexpr Field fields[] = {
        {"success", 7, true},     {"iter", 4, true},        {"res_dual", 10, false},
        {"res_eq", 10, false},    {"res_ineq", 10, false},  {"compl", 10, false},
        {"outer_iter", 10, true}, {"num_factor", 11, true}, {"num_upd", 7, true},
        {"rank_upd", 8, true},
    };

    struct ParsedName {
        std::string problem_name;
        std::string solver_name;
    };

    ParsedName ParseRunName(const std::string &run_name) const {
        ParsedName result;
        tie(result.problem_name, result.solver_name) = guanaqo::split(run_name, "@");
        result.solver_name = get<0>(guanaqo::split(result.solver_name, "/"));
        return result;
    }

    size_t max_name_width() const {
        return std::max(max_problem_name_width, max_solver_name_width + 1 + aggregate_width);
    }

    void PrintHeader(const std::string &problem_name) const {
        // Calculate total width for separator
        size_t total_width = max_name_width() + 1 + time_width + 1 + time_width + 1 + 12;
        for (const auto &field : fields)
            total_width += 2 + field.width;

        fmt::print("{}\n", std::string(total_width, '='));
        fmt::print($(fg(fmt::color::dodger_blue)), "{:<{}}", problem_name, max_name_width());
        fmt::print(" {0:>{3}} {1:>{3}} {2:>12}", "Time", "CPU", "Iterations", time_width);
        for (const auto &field : fields)
            fmt::print("  {:>{}}", field.name, field.width);
        fmt::print("\n{}\n", std::string(total_width, '-'));
    }

    void PrintRunData(const Run &run, const std::string &solver_name) const {
        // Handle skipped/error runs
        if (run.skipped != benchmark::internal::NotSkipped) {
            fmt::print($(fg(fmt::color::red)), "{:>{}}", solver_name, max_solver_name_width);
            fmt::print(" skipped: {}\n", run.skip_message);
            return;
        }
        // Aggregate name
        bool aggregate             = (run.run_type == Run::RT_Aggregate);
        bool median                = aggregate && run.aggregate_name == "median";
        std::string aggregate_name = aggregate ? "[" + run.aggregate_name + "]" : "";
        // Format time and CPU time
        bool unit_time   = !aggregate || run.aggregate_unit == benchmark::StatisticUnit::kTime;
        const char *unit = unit_time ? GetTimeUnitString(run.time_unit) : "%";
        auto real_time   = unit_time ? run.GetAdjustedRealTime() : 100 * run.real_accumulated_time;
        auto cpu_time    = unit_time ? run.GetAdjustedCPUTime() : 100 * run.cpu_accumulated_time;
        std::string time_str = fmt::format("{:.2f} {:<2}", real_time, unit);
        std::string cpu_str  = fmt::format("{:.2f} {:<2}", cpu_time, unit);
        // Check if success counter exists and is not 1
        bool failed = false;
        if (auto s = run.counters.find("success"); s != run.counters.end())
            failed = s->second.value != 1.0;
        if (failed && aggregate && (run.aggregate_name == "stddev" || run.aggregate_name == "cv"))
            failed = false;
        auto name_color = failed ? fmt::color::red : fmt::color::lime_green;
        auto emph = median ? (fmt::emphasis::italic | fmt::emphasis::bold) : fmt::text_style{};
        // Name, times and iterations
        auto solver_name_width = max_name_width() - aggregate_width - 1;
        fmt::print("{:>{}} {:<{}}", styled(solver_name, $(emph | fg(name_color))),
                   solver_name_width, aggregate_name, aggregate_width);
        fmt::print(" ");
        fmt::print($(emph | fg(fmt::color::yellow)), "{0:>{2}} {1:>{2}}", time_str, cpu_str,
                   time_width);
        fmt::print(" ");
        fmt::print($(emph | fg(fmt::color::deep_sky_blue)), "{:>12}", run.iterations);

        // Print standard counters
        for (const auto &field : fields) {
            auto it = run.counters.find(field.name);
            it != run.counters.end() ? PrintCounter(field, it->second.value, emph)
                                     : PrintDash(field);
        }
        fmt::print("\n");

        // Collect and print additional counters
        if (print_extra)
            PrintExtraCounters(run);
    }

    void PrintExtraCounters(const Run &run) const {
        std::vector<std::string> extra_counters;
        for (const auto &[name, counter] : run.counters)
            if (!std::ranges::contains(fields, name, &Field::name))
                extra_counters.emplace_back(FormatExtraCounter(name, counter));
        if (!extra_counters.empty()) {
            fmt::print("{:<{}}                                ", "", max_solver_name_width);
            fmt::print($(fg(fmt::color::light_gray)), "(");
            for (size_t i = 0; i < extra_counters.size(); ++i) {
                if (i > 0)
                    fmt::print($(fg(fmt::color::light_gray)), ", ");
                fmt::print($(fg(fmt::color::gray)), "{}", extra_counters[i]);
            }
            fmt::print($(fg(fmt::color::light_gray)), ")");
            fmt::print("\n");
        }
    }

    std::string FormatExtraCounter(std::string_view name, const benchmark::Counter &counter) const {
        bool is_int = static_cast<double>(std::numeric_limits<int>::min()) < counter.value &&
                      counter.value < static_cast<double>(std::numeric_limits<int>::max()) &&
                      static_cast<double>(static_cast<int>(counter.value)) == counter.value;
        return is_int ? fmt::format("{}={}", name, static_cast<int>(counter.value))
                      : fmt::format("{}={:e}", name, counter.value);
    }

    void PrintCounter(const Field &field, double value, fmt::text_style style) const {
        if (field.is_integer)
            fmt::print($(style), "  {:>{}}", static_cast<int>(value), field.width);
        else
            fmt::print($(style), "  {:>{}.3e}", value, field.width);
    }

    void PrintDash(const Field &field) const {
        fmt::print($(fg(fmt::color::gray)), "  {:>{}}", "-", field.width);
    }

    bool with_color;
    bool print_extra;
    size_t max_solver_name_width;
    size_t max_problem_name_width;
    size_t aggregate_width = 9; // Width for aggregate name column (including brackets)
    size_t time_width      = 12;
    std::string current_problem_name_;
    std::string current_solver_name_;
    size_t current_solver_counter_ = 0;
};

std::unique_ptr<benchmark::BenchmarkReporter> make_custom_reporter(size_t problem_name_width,
                                                                   size_t solver_name_width,
                                                                   bool print_extra,
                                                                   bool with_color) {
    return std::make_unique<CustomReporter>(problem_name_width, solver_name_width, print_extra,
                                            with_color);
}
