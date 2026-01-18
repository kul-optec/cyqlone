#!/usr/bin/env bash
set -euo pipefail

# Paths and environment setup
CYQLONE_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
export CONAN_HOME="${CYQLONE_ROOT}/.conan2"
# User-configurable variables with defaults
: "${TASKSET_CPU:=taskset -c 0-7}"  # Default to binding to CPU cores 0-7
: "${NPROC:=8}" # Default to using 8 processors (p parameter in the paper)

# Set up Conan and install the dependencies
setup_deps() {
    local dev_profile="${CYQLONE_ROOT}/scripts/dev/profiles/dev"
    local clang_profile="${CYQLONE_ROOT}/scripts/ci/conan-profiles/profiles/toolchain/clang-linux.profile"
    local profiles=("-pr:h" "${dev_profile}")
    local with_mkl=False
    local editable=0
    while [[ $# -gt 0 ]]; do
        case $1 in
            --gcc) ;;
            --clang*) export TTTAPA_CONAN_PROFILES_CLANG_SUFFIX="${1#--clang}"
                      profiles=("-pr:h" "${dev_profile}" "-pr:h" "${clang_profile}") ;;
            --with-mkl) with_mkl=True ;;
            --editable|-e) editable=1 ;;
            *) echo "Unknown compiler option '$1'" >&2; exit 1 ;;
        esac
        shift
    done
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
    )
    if [[ $editable -eq 1 ]]; then
        find "${CYQLONE_ROOT}/build" -name CMakeCache.txt -delete ||:
        conan build "${CYQLONE_ROOT}" "${conan_args[@]}"
        conan editable add "${CYQLONE_ROOT}"
    else
        conan editable remove "${CYQLONE_ROOT}" ||:
        conan export "${CYQLONE_ROOT}"
    fi
    conan install . "${conan_args[@]}" --lockfile-partial --format=json > conan.json
}

# Build the benchmark project
build() {
    if [[ ! -f CMakeUserPresets.json ]] || [[ ! -f conan.json ]]; then
        echo "Conan files not found, please run deps first." >&2; exit 1
    fi
    generators_folder="$(jq -r '.graph.nodes."0".generators_folder' conan.json)"
    set -x +u
    source "${generators_folder}/conanbuild.sh"
    set -u
    cmake --fresh --preset conan-release
    cmake --build --preset conan-release
}

# Run the quick benchmark as a sanity check
benchmark_quick() {
    build_folder="$(jq -r '.graph.nodes."0".build_folder' conan.json)"
    generators_folder="$(jq -r '.graph.nodes."0".generators_folder' conan.json)"
    set -x +u
    source "${generators_folder}/conanrun.sh"
    set -u
    ${TASKSET_CPU} "${build_folder}/spring-mass" \
        --problem wang-boyd-2008 --warm -I 1 -p "${NPROC}" --cm \
        -N 256 -M 12 \
        --benchmark_repetitions=11 --benchmark_report_aggregates_only \
        --benchmark_min_time=0.1s
}

# Run the horizon scaling benchmark
benchmark_scaling() {
    build_folder="$(jq -r '.graph.nodes."0".build_folder' conan.json)"
    generators_folder="$(jq -r '.graph.nodes."0".generators_folder' conan.json)"
    set -x +u
    source "${generators_folder}/conanrun.sh"
    set -u
    ${TASKSET_CPU} "${build_folder}/spring-mass" \
        --problem wang-boyd-2008 --warm -I 20 -p "${NPROC}" --cm --no-updates \
        -N 32  -N 64  -N 96  -N 128 -N 160 -N 192 -N 224 -N 256 \
        -N 288 -N 320 -N 352 -N 384 -N 416 -N 448 -N 480 -N 512 \
        -M 12 \
        --benchmark_repetitions=3 --benchmark_report_aggregates_only \
        --benchmark_min_time=0.05s --benchmark_out=benchmark-scaling.json
}

# Run the M×N grid benchmark
benchmark_grid() {
    build_folder="$(jq -r '.graph.nodes."0".build_folder' conan.json)"
    generators_folder="$(jq -r '.graph.nodes."0".generators_folder' conan.json)"
    set -x +u
    source "${generators_folder}/conanrun.sh"
    set -u
    ${TASKSET_CPU} "${build_folder}/spring-mass" \
        --problem wang-boyd-2008 --warm -I 150 -p "${NPROC}" --cm \
        -N 32 -N 64 -N 96 -N 128 -N 160 -N 192 -N 224 -N 256 \
        -M 6 -M 12 -M 18 -M 24 -M 30 \
        --benchmark_repetitions=3 --benchmark_report_aggregates_only \
        --benchmark_min_time=0.05s --benchmark_out=benchmark-grid.json
}

# Clean up build files and benchmark results
clean() {
    set -x
    rm -rf conan.json build CMakeUserPresets.json
    rm -f benchmark-grid.json benchmark-scaling.json
}

main() {
    local cmd="${1:-help}"
    shift
    case "${cmd}" in
        deps)
            setup_deps "$@"
            ;;
        build)
            build
            ;;
        benchmark-quick)
            benchmark_quick
            ;;
        benchmark-scaling)
            benchmark_scaling
            ;;
        benchmark-grid)
            benchmark_grid
            ;;
        all)
            setup_deps "$@"
            build
            benchmark_quick
            benchmark_scaling
            benchmark_grid
            ;;
        clean)
            clean
            ;;
        help|*)
            echo "Usage: $0 {deps|build|benchmark-quick|benchmark-grid|benchmark-scaling|all}"       >&2
            echo ""                                                                                  >&2
            echo "Commands:"                                                                         >&2
            echo "  deps                    - Set up Conan dependencies"                             >&2
            echo "  build                   - Build the benchmark project"                           >&2
            echo "  benchmark-quick         - Run quick benchmark (sanity check)"                    >&2
            echo "  benchmark-scaling       - Run scaling benchmark (takes a couple of minutes)"     >&2
            echo "  benchmark-grid          - Run grid benchmark (takes a couple of hours)"          >&2
            echo "  all                     - Run all commands above in sequence"                    >&2
            echo "  clean                   - Remove all build files and benchmark results"          >&2
            echo ""                                                                                  >&2
            echo "Environment variables:"                                                            >&2
            echo "  NPROC        - number of processors to use for the benchmark (default: 8)"       >&2
            echo "  TASKSET_CPU  - taskset command to bind CPU cores (default: taskset -c 0-7)"      >&2
            exit 1
            ;;
    esac
}

main "$@"
