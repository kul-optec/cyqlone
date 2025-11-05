#include <cyqlone/qpalm/status.hpp>
#include <ostream>

namespace CYQLONE_NAMESPACE::qpalm {

std::ostream &operator<<(std::ostream &os, SolverStatus s) { return os << enum_name(s); }

} // namespace CYQLONE_NAMESPACE::qpalm
