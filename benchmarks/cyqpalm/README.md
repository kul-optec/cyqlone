# CyQPALM benchmarks

To reproduce the results from the paper, you will need the [Conan](https://conan.io/) package
manager. It can be installed via `pip install conan`. Then download, build and install the
dependencies:
```sh
./benchmark.sh deps --gcc
# -- or --
./benchmark.sh deps --clang   # or use e.g. --clang-21 for a specific version
# -- or --
./benchmark.sh deps --icx
```
Next, build the benchmark project:
```sh
./benchmark.sh build
```

By default, the benchmarks run on the first eight CPUs available on your system. To change this
(e.g. if fewer CPUs are available or if the first eight logical CPUs are hyperthreads on four
physical cores), configure the following environment variables:
```sh
export NPROC=4                      # number of CPUs to use (p parameter in the paper)
export TASKSET_CPU="taskset -c 0-3" # which CPUs to use (here: first four)
```
Using `taskset` to pin the benchmark processes to specific CPUs helps reduce variability in the
results, and is crucial when using a system with both performance and efficiency cores (you want
to select the performance cores).

You can now run the benchmarks. For a quick sanity check, run:
```sh
./benchmark.sh benchmark-quick
```
To run the full scaling benchmark (takes a couple of minutes), run:
```sh
./benchmark.sh benchmark-scaling
```
To run the full grid benchmark (takes a couple of hours), run:
```sh
./benchmark.sh benchmark-grid
```
