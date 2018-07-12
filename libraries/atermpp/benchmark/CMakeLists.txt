# The Threads module provides the Threads::Threads target since 3.1
cmake_minimum_required(VERSION 3.1)

find_package(Threads)

file(GLOB BENCHMARKS *.cpp)

foreach (benchmark ${BENCHMARKS})
  get_filename_component(filename ${benchmark} NAME_WE)

  set(BENCHMARK_TARGET benchmark_atermpp_${filename})
  add_executable(${BENCHMARK_TARGET} ${benchmark})

  target_link_libraries(${BENCHMARK_TARGET} mcrl2_atermpp Threads::Threads)
endforeach()
