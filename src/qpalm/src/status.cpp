#include <cyqlone/qpalm/status.hpp>
#include <ostream>

namespace cyqlone::qpalm {

std::ostream &operator<<(std::ostream &os, SolverStatus s) { return os << enum_name(s); }

} // namespace cyqlone::qpalm
