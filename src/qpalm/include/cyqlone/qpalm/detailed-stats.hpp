#pragma once

#include <cyqlone/config.hpp>
#include <vector>

namespace CYQLONE_NAMESPACE::qpalm {

struct DetailedStats {
    enum ExitReason {
        Busy,
        Converged,
        NoActiveSetChange,
        Fail,
    };
    struct Entry {
        unsigned outer_iter, inner_iter;
        real_t stationarity, ineq_constr_viol, eq_constr_viol;
        real_t linesearch_step_size;
        index_t linesearch_breakpoint_index;
        index_t num_active_constr, num_changing_constr;
        ExitReason exit_reason;
    };
    std::vector<Entry> entries;
};

} // namespace CYQLONE_NAMESPACE::qpalm
