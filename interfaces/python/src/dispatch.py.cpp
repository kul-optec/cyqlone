#include <nanobind/nanobind.h>
#include <cpu_features_macros.h>
#if defined(CPU_FEATURES_ARCH_X86)
#include <cpuinfo_x86.h>

static const char *get_dispatch_name() {
    using namespace cpu_features;
    const X86Features features = GetX86Info().features;
    if (features.avx2)
        return "avx2";
    if (features.avx512f)
        return "avx512";
    return "generic";
}
#else
static const char *get_dispatch_name() { return "generic"; }
#endif

namespace nb = nanobind;
using namespace nb::literals;

NB_MODULE(MODULE_NAME, m) { m.def("get_dispatch_name", &get_dispatch_name); }
