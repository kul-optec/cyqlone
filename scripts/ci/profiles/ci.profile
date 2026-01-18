{% set my_profiles = os.path.join(profile_dir, "..", "..", "ci", "conan-profiles", "profiles") %}

# Determine machine architecture and select custom toolchain for the platform (or use default)
{% set machine = os.getenv("RUNNER_ARCH", os.popen("uname -m").read().strip()) %}
{% if machine in ["x86_64", "AMD64", "X64"] %}
include({{ os.path.join(my_profiles, "toolchain", "x86_64-bionic-linux-gnu.profile") }})
include({{ os.path.join(my_profiles, "arch", "linux", "x86-64-v3.profile") }})
{% elif machine in ["aarch64", "ARM64"] %}
include({{ os.path.join(my_profiles, "toolchain", "aarch64-rpi3-linux-gnu.profile") }})
include({{ os.path.join(my_profiles, "arch", "linux", "generic.profile") }})
{% else %}
include(default)
{% endif %}

# Common options
include({{ os.path.join(my_profiles, "sccache", "only-self.profile") }})
include({{ os.path.join(my_profiles, "color", "gcc.profile") }})
include({{ os.path.join(my_profiles, "link", "lto-auto.profile") }})
include({{ os.path.join(my_profiles, "link", "static.profile") }})
include({{ os.path.join(my_profiles, "link", "static-gfortran.profile") }})
include({{ os.path.join(my_profiles, "link", "mold.profile") }})
include({{ os.path.join(my_profiles, "test", "only-self.profile") }})
include({{ os.path.join(my_profiles, "visibility", "hidden.profile") }})
include({{ os.path.join(my_profiles, "tools", "ninja.profile") }})

# Inject additional tools
[tool_requires]
cmake/4.2.1
blasfeo/*: cmake/[<4]

[conf]
tools.build.cross_building:cross_build=True
tools.build.cross_building:can_run=True
# Work around bug in OpenBLAS recipe
tools.build:sharedlinkflags+=["-lgfortran"]
tools.build:exelinkflags+=["-lgfortran"]
&:tools.cmake.cmaketoolchain:extra_variables*={"BATMAT_WITH_ACCURATE_BUILD_TIME": "Off"}
&:tools.cmake.cmaketoolchain:extra_variables*={"CYQLONE_WITH_ACCURATE_BUILD_TIME": "Off"}
hyhound/*:tools.cmake.cmaketoolchain:extra_variables*={"CMAKE_UNITY_BUILD": "On"}
hpipm/*:tools.build:cflags+=["-fvisibility=default"]

[options]
guanaqo/*:with_blas=True
guanaqo/*:with_itt=False
guanaqo/*:with_tracing=False
guanaqo/*:with_openmp=True
batmat/*:with_openmp=True
batmat/*:with_benchmarks=True
batmat/*:with_blasfeo=True
cyqlone/*:with_benchmarks=True
cyqlone/*:with_blasfeo=True
