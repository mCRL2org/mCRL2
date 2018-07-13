#!/bin/sh

echo "Running simple benchmarks"
perf stat benchmark_atermpp_integer_term_creation
perf stat benchmark_atermpp_list_creation
perf stat benchmark_atermpp_function_symbol_creation
perf stat benchmark_atermpp_garbage_collection_short

echo "Running nested function application benchmark from 0 to 32"
for i in 0 1 2 4 7 8 12 16 20 26 32; do
  perf stat benchmark_atermpp_function_application_creation $i
done;

echo "Running nested function application with trivial converter benchmark from 0 to 32"
for i in 0 1 2 4 7 8 12 16 20 26 32; do
  perf stat benchmark_atermpp_function_application_with_converter_creation $i
done;
