if(${CMAKE_VERSION} VERSION_LESS 3.1)
  return()
endif()

# The Threads module provides the Threads::Threads target since 3.1
cmake_minimum_required(VERSION 3.1)

find_package(Threads)
function(add_benchmark NAME SOURCE)
  set(BENCHMARK_TARGET benchmark_target_atermpp_${NAME})
  add_executable(${BENCHMARK_TARGET} ${SOURCE})

  target_link_libraries(${BENCHMARK_TARGET} mcrl2_atermpp Threads::Threads)

  set(BENCHMARK benchmark_atermpp_${NAME})
  add_test(NAME ${BENCHMARK} COMMAND ${CMAKE_CTEST_COMMAND}
     --build-and-test
     "${CMAKE_SOURCE_DIR}"
     "${CMAKE_BINARY_DIR}"
     --build-noclean
     --build-nocmake
     --build-generator "${CMAKE_GENERATOR}"
     --build-target "${BENCHMARK_TARGET}"
     --build-makeprogram "${CMAKE_MAKE_PROGRAM}"
     --test-command "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${BENCHMARK_TARGET}" 
     ${ARGN}
  )
endfunction()

# Generate one target for each generic benchmark
file(GLOB BENCHMARKS *.cpp)
foreach (benchmark ${BENCHMARKS})
  get_filename_component(filename ${benchmark} NAME_WE)
  add_benchmark(${filename} ${benchmark})
endforeach()

# Generate one target for function application benchmarks with n number of arguments.
set(FUNCTION_APPLICATION_BENCHMARKS "function_application_creation" "function_application_with_converter_creation")
set(NUMBER_OF_ARGUMENTS 1 2 4 7 8 12 16 24 32)

foreach (benchmark ${FUNCTION_APPLICATION_BENCHMARKS})
  foreach(argument ${NUMBER_OF_ARGUMENTS})
    add_benchmark("${benchmark}_${argument}" "${benchmark}.cpp" ${argument})
  endforeach()
endforeach()
