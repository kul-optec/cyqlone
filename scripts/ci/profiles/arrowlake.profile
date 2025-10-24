[settings]
arch.microarch=arrowlake
[conf]
tools.build:cflags+=["-march=arrowlake"]
tools.build:cxxflags+=["-march=arrowlake"]
[options]
openblas/*:target=HASWELL
blasfeo/*:target=X64_INTEL_HASWELL
