# Add a target to measure the time to generate the statespace of a given mcrl2 specification.
function(add_benchmark NAME INPUT_MCRL2 INTERMEDIATE_LPS LPS2LTS_ARGUMENTS)
  set(MCRL22LPS_TARGET "benchmark_target_mcrl22lps_${NAME}")
  set(LPS2LTS_TARGET "benchmark_target_lps2lts_${NAME}")

  add_custom_target(${MCRL22LPS_TARGET}
    COMMAND mcrl22lps "${INPUT_MCRL2}" "${INTERMEDIATE_LPS}"
  )

  add_custom_target(${LPS2LTS_TARGET}
    COMMAND lps2lts "${LPS2LTS_ARGUMENTS}" "${INTERMEDIATE_LPS}"
    DEPENDS "${MCRL22LPS_TARGET}"
  )

  add_test(NAME benchmark_lps2lts_${NAME}
    COMMAND ${CMAKE_COMMAND} "--build" ${CMAKE_BINARY_DIR} "--target" "${LPS2LTS_TARGET}")
endfunction()

set(BENCHMARK_WORKSPACE ${CMAKE_BINARY_DIR}/benchmarks)
set(STATESPACE_BENCHMARKS
  "examples/academic/abp/abp.mcrl2"
  "examples/academic/allow/allow.mcrl2"
  "examples/academic/cabp/cabp.mcrl2"
  "examples/academic/dining/dining8.mcrl2"
  "examples/games/clobber/clobber.mcrl2"
  "examples/games/domineering/domineering.mcrl2"
  "examples/games/magic_square/magic_square.mcrl2"
  "examples/games/othello/othello.mcrl2"
  "examples/industrial/1394/1394-fin.mcrl2"
  "examples/industrial/brp/brp.mcrl2"
  "examples/industrial/chatbox/chatbox.mcrl2"
  "examples/industrial/lift/lift3-final.mcrl2"
  "examples/industrial/lift/lift3-init.mcrl2"
  )

foreach(benchmark ${STATESPACE_BENCHMARKS})
  # Obtain just <name>.mcrl2, split off <name> for the benchmark name and output lps <name>.lps
  get_filename_component(MCRL2_FILENAME ${benchmark} NAME)

  string(REPLACE ".mcrl2" "" NAME ${MCRL2_FILENAME})
  
  # Add a benchmark target for each statespace benchmark.
  add_benchmark("${NAME}" "${CMAKE_SOURCE_DIR}/${benchmark}" "${BENCHMARK_WORKSPACE}/${NAME}.lps" "")
  add_benchmark("${NAME}_cached" "${CMAKE_SOURCE_DIR}/${benchmark}" "${BENCHMARK_WORKSPACE}/${NAME}.lps" "--cached")
  # add_benchmark("${NAME}_jittyc" "${CMAKE_SOURCE_DIR}/${benchmark}" "${BENCHMARK_WORKSPACE}/${NAME}.lps" "--cached --rjittyc --prune")
endforeach()
