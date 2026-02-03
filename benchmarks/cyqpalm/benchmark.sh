#!/usr/bin/env bash
set -euo pipefail

# Paths and environment setup
cd "$(dirname "${BASH_SOURCE[0]}")"
CYQLONE_ROOT="$(cd ../.. && pwd)"
export CONAN_HOME="${CYQLONE_ROOT}/.conan2"
# User-configurable variables with defaults
: "${CONF_PRESET:=conan-release}"
: "${BUILD_PRESET:=conan-release}"
: "${TASKSET_CPU:=taskset -c 0-7}"  # Default to binding to CPU cores 0-7
: "${NPROC:=8}" # Default to using 8 processors (p parameter in the paper)

# Set up Conan and install the dependencies
deps() {
    local dev_profile="${CYQLONE_ROOT}/scripts/dev/profiles/dev"
    local clang_profile="${CYQLONE_ROOT}/scripts/ci/conan-profiles/profiles/toolchain/clang-linux.profile"
    local icx_profile="${CYQLONE_ROOT}/scripts/ci/conan-profiles/profiles/toolchain/icx-linux.profile"
    local profiles=("-pr:h" "${dev_profile}")
    local with_mkl=False; local editable=0; local lock=0
    local extra_args=()
    while [[ $# -gt 0 ]]; do
        if [[ $1 = "--" ]]; then
            shift; extra_args+=("$@")
            break
        fi
        case $1 in
            --gcc) ;;
            --clang*) export TTTAPA_CONAN_PROFILES_CLANG_SUFFIX="${1#--clang}"
                      profiles=("-pr:h" "${dev_profile}" "-pr:h" "${clang_profile}") ;;
            --icx) profiles=("-pr:h" "${dev_profile}" "-pr:h" "${icx_profile}") ;;
            --with-mkl) with_mkl=True ;;
            --editable|-e) editable=1 ;;
            --lock) lock=1 ;;
            --help|-h)
                echo "Usage: $0 {deps|all} [--gcc|--clang|--icx] [--with-mkl] [--editable|-e] [--lock]" >&2;
                echo "  --gcc            Use GCC toolchain (default)"                                   >&2;
                echo "  --clang[-SUFFIX] Use Clang toolchain, optional SUFFIX for specific versions"    >&2;
                echo "  --icx            Use Intel oneAPI DPC++/C++ Compiler toolchain"                 >&2;
                echo "  --with-mkl       Build with MKL support (OpenBLAS is used by default)"          >&2;
                echo "  --editable,-e    Set up Cyqlone as editable Conan package (for development)"    >&2;
                echo "  --lock           Create or update the Conan lockfile"                           >&2;
                exit 0 ;;
            *) echo "Unknown option '$1'. Use $0 deps --help for usage information." >&2; exit 1 ;;
        esac
        shift
    done
    if ! which conan &> /dev/null; then
        echo "Conan not found in PATH. Please install Conan 2.x and try again. https://conan.io" >&2
        exit 1
    fi
    set -x
    mkdir -p "${CONAN_HOME}"
    conan profile detect -e
    conan remote add --force cyqlone "${CYQLONE_ROOT}/scripts/ci/conan-recipes"
    conan config install "${CYQLONE_ROOT}/scripts/ci/conan-profiles/settings_user.yml"
    local conan_args=(
        "${profiles[@]}"
        -c tools.build:skip_test=True
        -o guanaqo/\*:with_mkl=$with_mkl
        -s:b compiler.cppstd=20
        --build=missing
        --lockfile-partial
        "${extra_args[@]}"
    )
    if [[ $editable -eq 1 ]]; then
        find "${CYQLONE_ROOT}/build" -name CMakeCache.txt -delete ||:
        conan build "${CYQLONE_ROOT}" "${conan_args[@]}"
        conan editable add "${CYQLONE_ROOT}"
    else
        conan editable remove "${CYQLONE_ROOT}" ||:
        conan export "${CYQLONE_ROOT}"
    fi
    if [[ $lock -eq 1 ]]; then
        conan lock create . "${conan_args[@]}"
        conan lock remove --requires="cyqlone/*"
    fi
    conan install . "${conan_args[@]}" --format=json > conan.json
}

# Wrapper for jq that downloads a binary if not found
_jq() {
    local jq_url="https://github.com/jqlang/jq/releases/download/jq-1.8.1/"
    local dest="${CYQLONE_ROOT}/.local/bin"
    export PATH="$dest:${PATH}"
    if ! which jq &> /dev/null; then
        echo "jq not found in PATH. Downloading ..." >&2
        case "$(uname -s)-$(uname -m)" in
            Linux-x86_64) jq_url+="jq-linux-amd64" ;;
            Linux-aarch64) jq_url+="jq-linux-arm64" ;;
            Darwin-x86_64) jq_url+="jq-macos-amd64" ;;
            Darwin-arm64)  jq_url+="jq-macos-arm64" ;;
            *) echo "Unsupported platform. Please install jq manually." >&2; exit 1 ;;
        esac
        mkdir -p "$dest"; [ -f "$dest/.gitignore" ] || echo '*' > "$dest/.gitignore"
        wget -O "$dest/jq" "${jq_url}"; chmod +x "$dest/jq"
    fi
    jq "$@"
}

# Build the benchmark project
build() {
    if [[ ! -f CMakeUserPresets.json ]] || [[ ! -f conan.json ]]; then
        echo "Conan files not found, please run the deps step first." >&2; exit 1
    fi
    generators_folder="$(_jq -r '.graph.nodes."0".generators_folder' conan.json)"
    set +ux
    source "${generators_folder}/conanbuild.sh"
    set -ux
    cmake --fresh --preset ${CONF_PRESET}
    cmake --build --preset ${BUILD_PRESET}
}

# Run a benchmark command
benchmark() {
    if [[ ! -f CMakeUserPresets.json ]] || [[ ! -f conan.json ]]; then
        echo "Conan files not found, please run the deps and build steps first." >&2; exit 1
    fi
    build_folder="$(_jq -r '.graph.nodes."0".build_folder' conan.json)"
    generators_folder="$(_jq -r '.graph.nodes."0".generators_folder' conan.json)"
    set +ux
    source "${generators_folder}/conanrun.sh"
    set -ux
    if [[ ! -x "${build_folder}/spring-mass" ]]; then
        echo "Benchmark executable not found, please run the build step first." >&2; exit 1
    fi
    echo $0
    exec ${TASKSET_CPU} "${build_folder}/spring-mass" "$@"
}

# Run the quick benchmark as a sanity check
benchmark_quick() {
    benchmark \
        --problem wang-boyd-2008 --cold --warm-shift -I 1 -p "${NPROC}" --cm \
        -N 256 -M 12 \
        --benchmark_repetitions=11 --benchmark_report_aggregates_only \
        --benchmark_min_time=0.1s "$@"
}

# Run the horizon scaling benchmark
benchmark_scaling() {
    benchmark \
        --problem wang-boyd-2008 --cold --warm-shift -I 20 -p "${NPROC}" --cm --no-updates \
        -N 32  -N 64  -N 96  -N 128 -N 160 -N 192 -N 224 -N 256 \
        -N 288 -N 320 -N 352 -N 384 -N 416 -N 448 -N 480 -N 512 \
        -M 12 \
        --benchmark_repetitions=3 --benchmark_report_aggregates_only \
        --benchmark_min_time=0.05s --benchmark_out=benchmark-scaling.json "$@"
}

# Run the M×N grid benchmark
benchmark_grid() {
    benchmark \
        --problem wang-boyd-2008 --cold --warm-shift -I 150 -p "${NPROC}" --cm \
        -N 32 -N 64 -N 96 -N 128 -N 160 -N 192 -N 224 -N 256 \
        -M 6 -M 12 -M 18 -M 24 -M 30 \
        --benchmark_repetitions=3 --benchmark_report_aggregates_only \
        --benchmark_min_time=0.05s --benchmark_out=benchmark-grid.json "$@"
}

# Clean up build files and benchmark results
clean() {
    set -x
    rm -rf conan.json build CMakeUserPresets.json
    rm -f benchmark-grid.json benchmark-scaling.json
}

main() {
    local cmd="${1:-help}"
    set +e
    shift; exit=$?
    set -e
    case "${cmd}" in
        deps)
            deps "$@" ;;
        build)
            build ;;
        benchmark-quick)
            benchmark_quick "$@" ;;
        benchmark-scaling)
            benchmark_scaling "$@" ;;
        benchmark-grid)
            benchmark_grid "$@" ;;
        all)
            deps "$@"
            build
            benchmark_quick
            benchmark_scaling
            benchmark_grid ;;
        benchmark)
            benchmark "$@" ;;
        clean)
            clean ;;
        help|*)
            echo "Usage: $0 {deps|build|benchmark-quick|benchmark-scaling|benchmark-grid|all}"      >&2
            echo ""                                                                                 >&2
            echo "Commands:"                                                                        >&2
            echo "  deps [...]              - Install the dependencies (using Conan)"               >&2
            echo "  build                   - Build the benchmark project"                          >&2
            echo "  benchmark-quick         - Run quick benchmark (sanity check)"                   >&2
            echo "  benchmark-scaling       - Run scaling benchmark (takes a couple of minutes)"    >&2
            echo "  benchmark-grid          - Run grid benchmark (takes a couple of hours)"         >&2
            echo "  all                     - Run all commands above in sequence"                   >&2
            echo "  benchmark [...]         - Run a custom benchmark"                               >&2
            echo "  clean                   - Remove all build files and benchmark results"         >&2
            echo ""                                                                                 >&2
            echo "Use $0 deps --help for more information on dependency installation options."      >&2
            echo "Use $0 benchmark --help for the available benchmark parameters."                  >&2
            echo ""                                                                                 >&2
            echo "Environment variables:"                                                           >&2
            echo "  CONF_PRESET  - name of the CMake configure preset (default: conan-release)"     >&2
            echo "  BUILD_PRESET - name of the CMake build preset (default: conan-release)"         >&2
            echo "  NPROC        - number of processors to use for the benchmark (default: 8)"      >&2
            echo "  TASKSET_CPU  - taskset command to bind CPU cores (default: taskset -c 0-7)"     >&2
            exit $exit ;;
    esac
}

main "$@"

# To cross-compile for e.g. a Raspberry Pi 5, use:
# DEV_MACHINE=aarch64 DEV_ARCH=cortex-a76 ./benchmark.sh deps -- -c tools.cmake.cmake_layout:build_folder_vars+='["const.rpi5"]'
# CONF_PRESET=conan-rpi5-release BUILD_PRESET=conan-rpi5-release ./benchmark.sh build
# file build/rpi5/Release/spring-mass
#
# When using Clang, you can link libomp.so statically using:
# ./benchmark.sh deps --clang-21 -- -o llvm-openmp/\*:shared=False
# Since the shared option does not affect the package ID of dependent packages, you may have to
# rebuild the downstream packages as well:
# ./benchmark.sh deps --clang-21 -- -o llvm-openmp/\*:shared=False --build=llvm-openmp/\* --build=cascade
