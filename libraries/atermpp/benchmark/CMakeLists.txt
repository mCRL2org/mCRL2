if(${CMAKE_VERSION} VERSION_LESS 3.1)
  return()
endif()

# The Threads module provides the Threads::Threads target since 3.1
cmake_minimum_required(VERSION 3.1)
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
  add_dependencies(prepare_benchmarks ${BENCHMARK_TARGET})

  target_link_libraries(${BENCHMARK_TARGET} mcrl2_atermpp Threads::Threads)

  add_benchmark(${NAME} ${NAME})
endfunction()

# Generate one target for each generic benchmark
file(GLOB BENCHMARKS *.cpp)
foreach (benchmark ${BENCHMARKS})
  get_filename_component(filename ${benchmark} NAME_WE)
  add_benchmark_target("atermpp_${filename}" ${benchmark})
endforeach()

# Generate one target for the function application benchmarks with n number of arguments.
set(FUNCTION_APPLICATION_BENCHMARKS "function_application_creation" "function_application_with_converter_creation")
set(NUMBER_OF_ARGUMENTS 1 2 4 7 8 12 16 24 32)

foreach (benchmark ${FUNCTION_APPLICATION_BENCHMARKS})
  foreach(argument ${NUMBER_OF_ARGUMENTS})
    add_benchmark("atermpp_${benchmark}_${argument}" "atermpp_${benchmark}" ${argument})
  endforeach()
endforeach()
