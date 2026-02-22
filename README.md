[![arXiv Preprint](https://img.shields.io/badge/arXiv-Preprint-b31b1b)](https://arxiv.org/abs/2512.09058)

<p align="center">
<img src="docs/source/images/cyqlone-logo-text.svg" width=300>
</p>

# Cyqlone

Fast, **parallel and vectorized solver** for linear systems with **optimal control structure**.

Also includes an implementation of the **CyQPALM** solver that uses the Cyqlone linear solver as a
backend, with parallel factorization update routines to handle active set changes and a parallel
exact line search.

## Preprint

The paper describing the algorithms in this repository can be found on arXiv: [**arxiv.org/abs/2512.09058**](https://arxiv.org/abs/2512.09058)

```bibtex
@misc{pas_cyqlone_2025,
	title = {Cyqlone: {A} {Parallel}, {High}-{Performance} {Linear} {Solver} for {Optimal} {Control}},
	url = {http://arxiv.org/abs/2512.09058},
	doi = {10.48550/arXiv.2512.09058},
	publisher = {arXiv},
	author = {Pas, Pieter and Patrinos, Panagiotis},
	month = dec,
	year = {2025},
}
```

## Reproduction of benchmark results from the paper

A script to reproduce the benchmark results from the paper can be found in `benchmarks/cyqpalm`.
See [`benchmarks/cyqpalm/README.md`](benchmarks/cyqpalm/README.md) for instructions.

## Installation

Requirements: CMake, Conan 2.

```sh
# Prepare environment and dependencies
python3 -m pip install -U conan
export CONAN_HOME="$PWD/.conan2"  # Use a local directory for Conan data
conan profile detect --exist-ok
conan remote add tttapa-conan-recipes "$PWD/scripts/ci/conan-recipes"
conan config install scripts/ci/conan-profiles/settings_user.yml
```

### C++ project

```sh
# Install dependencies (including GCC) and build from source
conan build . --build=missing -pr:h scripts/dev/profiles/dev -o \&:with_benchmarks=True -c tools.build:jobs=4
```

To build using Clang instead of GCC, add `-pr:h scripts/ci/conan-profiles/profiles/toolchain/clang-linux.profile` to the command above (in addition to the `dev` profile). To use the Intel compiler, add `-pr:h scripts/ci/conan-profiles/profiles/toolchain/icx-linux.profile` instead.

If you have sufficient RAM available, the number of parallel build jobs can be increased.

To re-build after code changes, simply run `cmake --build --preset conan-release` (or rebuild the
`conan-release` preset from your IDE).

Manual installation using CMake (without the Conan package manager) is also supported, provided that the necessary dependencies are installed.

### Python bindings (development)

To install the Python bindings for local development,
use the `install-linux-dev.sh` script (without arguments).

Requirements: Python 3.8+, uv, Conan 2 (as configured above).

```sh
./scripts/dev/install-linux-dev.sh
```

By default, this script builds the package for both AVX2 and AVX-512 targets, which may be wasteful if you only need AVX2. You can remove `avx512` from the `archs` array in the script to only build for AVX2.

To run the tests, use `pytest`.

To re-build the AVX2 version after code changes, run `cmake --build --preset conan-cp312-abi3-linux_x86_64-1-release` (replace `cp312-abi3` with your Python version and ABI, and replace `-1-` with `-2-` to re-build the AVX-512 version). You will need to restart the Python interpreter to load the new version of the package.

### Python bindings (distribution)

To build the Python package for distribution, use the scripts in `scripts/ci`.

```sh
# Build the wheel for distribution, and generate the stubs
./scripts/ci/build-linux-cross.sh 3.12 x86_64-bionic-linux-gnu . dist true
```

If you are cross-compiling, you will need to generate the stubs using a native build first, since this requires running the built extension module.

```sh
# Native build to generate the stub files
./scripts/ci/build-linux-native.sh . dist python
# Build the wheel for distribution
./scripts/ci/build-linux-cross.sh 3.12 x86_64-bionic-linux-gnu . dist
```

Either way, generated Wheel files will be placed in the `dist` directory.

## Related projects

- [QPALM](https://github.com/kul-optec/QPALM): general-purpose quadratic programming solver on which CyQPALM is based
- [batmat](https://github.com/tttapa/batmat): high-performance batched linear algebra routines (used by cyqlone)
- [hyhound](https://github.com/kul-optec/hyhound): low-rank Cholesky factorization up- and downdate routines based on hyperbolic Householder transformations (with applications to e.g. Riccati recursion)
- [alpaqa](https://github.com/kul-optec/alpaqa): matrix-free, nonlinear programming solver
