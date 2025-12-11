# CyQPALM benchmarks

To reproduce the results from the paper, build the project using Conan, and run the benchmark
executable using the following options:

```sh
./build/Release/spring-mass \
    -p wang-boyd-2008 --warm -I 150 --cm \
    -N 32 -N 64 -N 96 -N 128 -N 160 -N 192 -N 224 -N 256 \
    -M 6 -M 12 -M 18 -M 24 -M 30 \
    --benchmark_repetitions=3 --benchmark_report_aggregates_only \
    --benchmark_min_time=0.05s --benchmark_out=wang-boyd-2008.json
```
