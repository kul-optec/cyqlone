# This is the AVX-512 profile used for the Python builds. We target the x86-64-v4 microarchitecture
# level. We assume that consumer-level AVX-512-capable CPUs are more common than server-level ones,
# so we select the rocketlake microarchitecture for tuning. HPC users should consider building from
# source with a profile better suited to their hardware.
include({{ os.path.join(profile_dir, "x86-64-v4.profile") }})
[settings]
arch.microarch=rocketlake
[conf]
tools.build:cflags+=["-mtune=rocketlake"]
tools.build:cxxflags+=["-mtune=rocketlake"]
