#!/usr/bin/env bash
set -euo pipefail

# Paths and environment setup
CYQLONE_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
export CONAN_HOME="${CYQLONE_ROOT}/.conan2"
# User-configurable variables with defaults
: "${TASKSET_CPU:=taskset -c 0-7}"
: "${NPROC:=8}"

# Set up Conan and install the dependencies
setup_deps() {
    set -x
    mkdir -p "${CONAN_HOME}"
    conan profile detect -e
    conan remote add --force cyqlone "${CYQLONE_ROOT}/scripts/ci/conan-recipes"
    conan config install "${CYQLONE_ROOT}/scripts/ci/conan-profiles/settings_user.yml"
    conan export "${CYQLONE_ROOT}"
    conan install . -pr:h "${CYQLONE_ROOT}/scripts/dev/profiles/dev" \
        -c tools.build:skip_test=True -o guanaqo/\*:with_mkl=False \
        -s:b compiler.cppstd=20 --build=missing --format=json > conan.json
}

# Build the benchmark project
build() {
    if [[ ! -f conan.json ]]; then setup_deps; fi
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

# Clean up build files and benchmark results
clean() {
    set -x
    rm -rf conan.json build CMakeUserPresets.json
    rm -f benchmark-grid.json benchmark-scaling.json
}

main() {
    local cmd="${1:-help}"
    case "${cmd}" in
        deps)
            setup_deps
            ;;
        build)
            build
            ;;
        benchmark-quick)
            benchmark_quick
            ;;
        benchmark-grid)
            benchmark_grid
            ;;
        benchmark-scaling)
            benchmark_scaling
            ;;
        all)
            build
            benchmark_quick
            benchmark_grid
            benchmark_scaling
            ;;
        clean)
            clean
            ;;
        help|*)
            echo "Usage: $0 {deps|build|benchmark-quick|benchmark-grid|benchmark-scaling|all}"
            echo ""
            echo "Commands:"
            echo "  deps                    - Set up Conan dependencies"
            echo "  build                   - Build the benchmark project"
            echo "  benchmark-quick         - Run quick benchmark (sanity check)"
            echo "  benchmark-grid          - Run grid benchmark (takes a couple of hours)"
            echo "  benchmark-scaling       - Run scaling benchmark (takes a couple of minutes)"
            echo "  all                     - Run all commands above in sequence"
            echo "  clean                   - Remove all build files and benchmark results"
            echo ""
            echo "Environment variables:"
            echo "  NPROC        - number of processors to use for the benchmark (default: 8)"
            echo "  TASKSET_CPU  - taskset command to bind CPU cores (default: taskset -c 0-7)"
            exit 1
            ;;
    esac
}

main "$@"
