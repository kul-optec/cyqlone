[settings]
arch.microarch=sse4
[conf]
tools.build:cflags=["-march=nehalem"]
tools.build:cxxflags=["-march=nehalem"]
[options]
openblas/*:target=NEHALEM
blasfeo/*:target=X64_INTEL_CORE
