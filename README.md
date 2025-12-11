[![arXiv Preprint](https://img.shields.io/badge/arXiv-Preprint-b31b1b)](https://arxiv.org/abs/2512.09058)

<p align="center">
<img src="docs/images/logo-text.svg" width=300>
</p>

# Cyqlone

Fast, **parallel and vectorized solver** for linear systems with **optimal control structure**.

Also includes an implementation of the **CyQPALM** solver that uses the Cyqlone linear solver as a
backend, with parallel factorization update routines to handle active set changes and a parallel
exact line search.

## Preprint

The paper describing the algorithms in this repository can be found on arXiv: **<https://arxiv.org/abs/2512.09058>**

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

## Installation

Requirements: CMake, Conan 2.

```sh
# Prepare environment and dependencies
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

Replace `laptop` by `desktop` if your hardware supports AVX-512. If sufficient RAM is available, the number of parallel build jobs can be increased.

## Related projects

- [QPALM](https://github.com/kul-optec/QPALM): general-purpose quadratic programming solver on which CyQPALM is based
- [batmat](https://github.com/tttapa/batmat): high-performance batched linear algebra routines (used by cyqlone)
- [hyhound](https://github.com/kul-optec/hyhound): low-rank Cholesky factorization up- and downdate routines based on hyperbolic Householder transformations (with applications to e.g. Riccati recursion)
- [alpaqa](https://github.com/kul-optec/alpaqa): matrix-free, nonlinear programming solver
