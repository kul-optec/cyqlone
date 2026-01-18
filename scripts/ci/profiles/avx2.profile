# This is the AVX2 profile used for the Python builds. We support a wide range of AVX2-capable CPUs
# by targeting the x86-64-v3 microarchitecture level. For optimal performance on newer CPUs, we
# select the recent arrowlake microarchitecture for tuning.
{% set arch_dir = os.path.join(profile_dir, "..", "conan-profiles", "profiles", "arch") %}
include({{ os.path.join(arch_dir, "linux", "x86-64-v3.profile") }})
[settings]
arch.microarch=arrowlake
[conf]
tools.build:cflags+=["-mtune=arrowlake"]
tools.build:cxxflags+=["-mtune=arrowlake"]
