<p align="center">
<img src="docs/images/logo-text.svg" width=300>
</p>

# Cyqlone

Fast, **parallel and vectorized solver** for linear systems with **optimal control structure**.

Also includes an implementation of the **QPALM-OCP** solver that uses the cyqlone linear solver as a
backend, with parallel factorization update routines to handle active set changes.

## Installation

Requirements: CMake, Conan 2, plenty of RAM for parallel builds.

```sh
# Prepare environment
python3 -m pip install -U conan
export CONAN_HOME="$PWD/.conan2"
conan profile detect ||:
git clone https://github.com/tttapa/conan-recipes
conan remote add tttapa-conan-recipes "$PWD/conan-recipes"
```

```sh
# Install dependencies (including GCC) and build from source
conan build . --build=missing -pr scripts/dev/profiles/laptop -o \&:with_benchmarks=True -c tools.build:jobs=4
```

Replace `laptop` by `desktop` if your hardware supports AVX-512.

## Related projects

- [QPALM](https://github.com/kul-optec/QPALM): general-purpose quadratic programming solver on which QPALM-OCP is based
- [batmat](https://github.com/tttapa/batmat): high-performance batched linear algebra routines (used by cyqlone)
- [hyhound](https://github.com/kul-optec/hyhound): low-rank Cholesky factorization up- and downdate routines based on hyperbolic Householder transformations (with applications to e.g. Riccati recursion)
- [alpaqa](https://github.com/kul-optec/alpaqa): matrix-free, nonlinear programming solver
