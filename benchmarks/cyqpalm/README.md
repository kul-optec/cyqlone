# CyQPALM benchmarks

To reproduce the results from the paper, you will need the [Conan](https://conan.io/) package
manager. It can be installed via `python3 -m pip install conan`. You'll also need the following
standard tools: Git, wget, make, GCC and libgfortran (`apt install git wget gcc g++ make libgfortran5`).
Currently, only Linux is supported (tested on Ubuntu 22.04 and 24.04, and Rocky Linux 9.7).
Then download, build and install the dependencies:
```sh
./benchmark.sh deps --gcc
# -- or --
./benchmark.sh deps --clang   # or use e.g. --clang-21 for a specific version
# -- or --
./benchmark.sh deps --icx
```
By default, compiler flags are selected automatically depending on the build machine, but this can
be overridden by setting the `DEV_ARCH` variable. For example, to enable AVX2 but not AVX-512, set
`DEV_ARCH=x86-64-v3`; to enable both AVX2 and AVX-512, set `DEV_ARCH=x86-64-v4`.

Next, build the benchmark project:
```sh
./benchmark.sh build
```

By default, the benchmarks run on the first eight CPUs available on your system. To change this
(e.g. if fewer CPUs are available or if the first eight logical CPUs are hyperthreads on four
physical cores), configure the following environment variables:
```sh
export OMP_NUM_THREADS=4 # number of CPUs to use (p parameter in the paper)
export OMP_PLACES="0:4"  # which CPUs to use (here: first four)
```
Using `OMP_PLACES` to pin the benchmark processes to specific CPUs helps reduce variability in the
results, and is crucial when using a system with both performance and efficiency cores (you want
to select the performance cores). See https://www.openmp.org/spec-html/5.0/openmpse53.html for more
details on the `OMP_PLACES` syntax.

You can now run the benchmarks. For a quick sanity check, run:
```sh
./benchmark.sh benchmark-quick
```
To run the full scaling benchmark (Figure 15 in the paper), run:
```sh
./benchmark.sh benchmark-scaling # takes a couple of minutes
```
To run the full grid benchmark (Tables 1-3, Figures 13-14, Tables 5-7 in the paper), run:
```sh
./benchmark.sh benchmark-grid # takes a couple of hours
```
To visualize the results of the scaling benchmark, run:
```sh
./benchmark.sh plot-scaling
```
For a full list of the available commands, run:
```sh
./benchmark.sh --help
```
For more details on the available benchmark parameters, run:
```sh
./benchmark.sh benchmark --help
```

## Docker

If building locally is not an option, Docker files are provided for both GCC and Clang.
To build the Docker image, run:
```sh
rm -rf benchmarks/cyqpalm/build # Clean up any previous builds just in case
docker build -f Dockerfile.gcc -t cyqlone-benchmarks:latest .
docker run -it cyqlone-benchmarks:latest
./benchmark.sh benchmark-quick
```
