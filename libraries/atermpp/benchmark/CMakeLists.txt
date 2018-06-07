file(GLOB BENCHMARKS *.cpp)

foreach (benchmark ${BENCHMARKS})
  get_filename_component(filename ${benchmark} NAME_WE)

  set(BENCHMARK_TARGET benchmark_atermpp_${filename})
  add_executable(${BENCHMARK_TARGET} ${benchmark})

  target_link_libraries(${BENCHMARK_TARGET} mcrl2_atermpp)
endforeach()
