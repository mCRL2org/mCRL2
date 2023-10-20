find_package(Threads)

# Add a benchmark with the name that executes the given target.
function(add_benchmark NAME TARGET)
  set(BENCHMARK benchmark_${NAME})
  add_test(NAME "${BENCHMARK}" COMMAND "benchmark_target_${TARGET}" 
     ${ARGN}
     )

  set_property(TEST ${BENCHMARK} PROPERTY LABELS "benchmark_atermpp")
endfunction()

# Add a benchmark target given the sources.
function(add_benchmark_target NAME SOURCE)
  set(BENCHMARK_TARGET benchmark_target_${NAME})
  add_executable(${BENCHMARK_TARGET} ${SOURCE})
  add_dependencies(benchmarks ${BENCHMARK_TARGET})

  target_link_libraries(${BENCHMARK_TARGET} mcrl2_atermpp Threads::Threads)
endfunction()

# Generate one target for each generic benchmark
file(GLOB BENCHMARKS *.cpp)
foreach (benchmark ${BENCHMARKS})
  get_filename_component(filename ${benchmark} NAME_WE)
  add_benchmark_target("atermpp_${filename}" ${benchmark})

  add_benchmark("atermpp_${filename}" "atermpp_${filename}" 2 1)
endforeach()