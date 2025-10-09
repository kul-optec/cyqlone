[settings]
arch.microarch=avx512
[conf]
tools.build:cflags=["-march=rocketlake", "-mavx512f", "-mavx512dq", "-mavx512ifma", "-mavx512cd", "-mavx512bw", "-mavx512vl", "-mavx512vbmi", "-mavx512vbmi2", "-mavx512vnni", "-mavx512bitalg", "-mavx512vpopcntdq", "-mfma"]
tools.build:cxxflags=["-march=rocketlake", "-mavx512f", "-mavx512dq", "-mavx512ifma", "-mavx512cd", "-mavx512bw", "-mavx512vl", "-mavx512vbmi", "-mavx512vbmi2", "-mavx512vnni", "-mavx512bitalg", "-mavx512vpopcntdq", "-mfma"]
tools.cmake.cmaketoolchain:extra_variables*={"HYHOUND_MAX_HYH_KERNEL_WIDTH": "16"}
[options]
openblas/*:target=SKYLAKEX
blasfeo/*:target=X64_INTEL_SKYLAKE_X
