[settings]
arch.microarch=avx2
[conf]
tools.build:cflags+=["-march=haswell"]
tools.build:cxxflags+=["-march=haswell"]
[options]
openblas/*:target=HASWELL
blasfeo/*:target=X64_INTEL_HASWELL
