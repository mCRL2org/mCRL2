# Print a short message on how to perform the benchmarks.
message(STATUS "To generate the necessary files for benchmarking, build the benchmarks target")

# Add a script to execute the benchmarks for additional profiling, e.g. to measure memory usage.
set(BENCHMARK_SCRIPT "" CACHE STRING "Pass the benchmark tool (example: mcrl22lps x y) as the first parameter of this script or binary")
mark_as_advanced(BENCHMARK_SCRIPT)

# Add a benchmark named NAME that uses TOOL on the given INPUT and written to OUTPUT. Additional arguments after OUTPUT are passed to TOOL.
function(add_tool_benchmark NAME TOOL INPUT OUTPUT)
  set(TARGET "benchmark_target_${TOOL}_${NAME}")
  set(TEST "benchmark_${TOOL}_${NAME}")

  add_test(NAME ${TEST} 
    COMMAND ${BENCHMARK_SCRIPT} $<TARGET_FILE:${TOOL}> ${ARGN} ${INPUT} ${OUTPUT})
  set_property(TEST ${TEST} PROPERTY LABELS "benchmark_tool")
endfunction()

set(BENCHMARK_WORKSPACE ${CMAKE_BINARY_DIR}/benchmarks)
set(STATESPACE_BENCHMARKS
  "examples/academic/abp/abp.mcrl2"
  "examples/academic/allow/allow.mcrl2"
  "examples/academic/cabp/cabp.mcrl2"
  "examples/academic/dining/dining8.mcrl2"
  "examples/games/magic_square/magic_square.mcrl2"
  "examples/industrial/1394/1394-fin.mcrl2"
  "examples/industrial/brp/brp.mcrl2"
  "examples/industrial/chatbox/chatbox.mcrl2"
  "examples/industrial/lift/lift3-final.mcrl2"
  "examples/industrial/lift/lift3-init.mcrl2"
  )

# These specifications use lists of lists to represent the board of a game. The
# symbolic tools cannot deal with this single parameter and require additional
# preprocessing of the LPS (using lpsparunfold).
set(GAME_BENCHMARKS "")

# This target is used to generate all intermediate files required for benchmarks. 
add_custom_target(benchmarks)
add_dependencies(benchmarks lps2lts pbes2bool ltsconvert)

foreach(benchmark ${STATESPACE_BENCHMARKS} ${GAME_BENCHMARKS})
  # Obtain just <name>.mcrl2, split off <name> for the benchmark name and output lps <name>.lps
  get_filename_component(MCRL2_FILENAME ${benchmark} NAME)
  string(REPLACE ".mcrl2" "" NAME ${MCRL2_FILENAME})

  set(LPS_FILENAME "${BENCHMARK_WORKSPACE}/${NAME}.lps")
  set(LTS_FILENAME "${BENCHMARK_WORKSPACE}/${NAME}.lts")
  set(NODEADLOCK_PBES_FILENAME "${BENCHMARK_WORKSPACE}/${NAME}.nodeadlock.pbes")

  # Generate the lps for this benchmark.
  add_custom_command(TARGET benchmarks 
    COMMAND mcrl22lps "${CMAKE_SOURCE_DIR}/${benchmark}" "${LPS_FILENAME}"
    USES_TERMINAL
    )

  # Generate a pbes for the nodeadlock property for this benchmark
  add_custom_command(TARGET benchmarks
    COMMAND lps2pbes -f "${CMAKE_SOURCE_DIR}/examples/modal-formulas/nodeadlock.mcf" "${LPS_FILENAME}" "${NODEADLOCK_PBES_FILENAME}"
    USES_TERMINAL
    )

  # Benchmark linearisation.
  add_tool_benchmark("${NAME}" mcrl22lps "${CMAKE_SOURCE_DIR}/${benchmark}" "")

  # Benchmark statespace generation.
  add_tool_benchmark("${NAME}" lps2lts "${LPS_FILENAME}" "")
  add_tool_benchmark("${NAME}_parallel" lps2lts "${LPS_FILENAME}" "" "--threads=4")

  if(MCRL2_ENABLE_JITTY)
    add_tool_benchmark("${NAME}_jittyc" lps2lts "${LPS_FILENAME}" "" "-rjittyc")
    add_tool_benchmark("${NAME}_jittyc_parallel" lps2lts "${LPS_FILENAME}" "" "-rjittyc" "--threads=4")
  endif()

  # Benchmark statespace reduction techniques, the first target generates the statespaces.
  if(NOT WIN32)
    #add_tool_benchmark("${NAME}_exploration_jittyc" lps2lts "${LPS_FILENAME}" "${LTS_FILENAME}" "-rjittyc")
    add_tool_benchmark("${NAME}_exploration" lps2lts "${LPS_FILENAME}" "${LTS_FILENAME}")
  else()
    add_tool_benchmark("${NAME}_exploration" lps2lts "${LPS_FILENAME}" "${LTS_FILENAME}")
  endif()

  add_tool_benchmark("${NAME}_bisim" ltsconvert "${LTS_FILENAME}" "" "-ebisim")
  add_tool_benchmark("${NAME}_bisim-gjkw" ltsconvert "${LTS_FILENAME}" "" "-ebisim-gjkw")

  add_tool_benchmark("${NAME}_branching-bisim" ltsconvert "${LTS_FILENAME}" "" "-ebranching-bisim")
  add_tool_benchmark("${NAME}_branching-bisim-gjkw" ltsconvert "${LTS_FILENAME}" "" "-ebranching-bisim-gjkw")

  # The ltsconvert benchmarks depend on the statespace written by the
  # exploration benchmarks. The benchmark names are hardcoded and depend on the
  # names generated in add_tool_benchmark.
  set_tests_properties(
    "benchmark_ltsconvert_${NAME}_bisim" 
    "benchmark_ltsconvert_${NAME}_bisim-gjkw" 
    "benchmark_ltsconvert_${NAME}_branching-bisim" 
    "benchmark_ltsconvert_${NAME}_branching-bisim-gjkw" 
    PROPERTIES DEPENDS "benchmark_lps2lts_${NAME}_exploration")

  # Benchmark solving PBES
  add_tool_benchmark("${NAME}" pbes2bool "${NODEADLOCK_PBES_FILENAME}" "")
  add_tool_benchmark("${NAME}_parallel" pbes2bool "${NODEADLOCK_PBES_FILENAME}" "" "--threads=4")
  
  if(MCRL2_ENABLE_JITTY)
    add_tool_benchmark("${NAME}_jittyc" pbes2bool "${NODEADLOCK_PBES_FILENAME}" "" "-rjittyc")
    add_tool_benchmark("${NAME}_jittyc_parallel" pbes2bool "${NODEADLOCK_PBES_FILENAME}" "" "-rjittyc" "--threads=4")
  endif()

endforeach()

file(GLOB_RECURSE LTS_BENCHMARKS ${CMAKE_CURRENT_SOURCE_DIR} "*.aut")
foreach(benchmark ${LTS_BENCHMARKS})
  get_filename_component(LTS_FILENAME ${benchmark} NAME)
  string(REPLACE ".aut" "" NAME ${LTS_FILENAME})
  
  add_tool_benchmark("${NAME}_bisim" ltsconvert "${LTS_FILENAME}" "" "-ebisim")
  add_tool_benchmark("${NAME}_bisim-gjkw" ltsconvert "${LTS_FILENAME}" "" "-ebisim-gjkw")

  add_tool_benchmark("${NAME}_branching-bisim" ltsconvert "${LTS_FILENAME}" "" "-ebranching-bisim")
  add_tool_benchmark("${NAME}_branching-bisim-gjkw" ltsconvert "${LTS_FILENAME}" "" "-ebranching-bisim-gjkw")
endforeach()

# Only add the symbolic benchmarks when the tools are part of the build, i.e., experimental tools enabled and Sylvan can be compiled.
if (MCRL2_ENABLE_EXPERIMENTAL AND MCRL2_ENABLE_SYLVAN)

  add_dependencies(benchmarks lpsreach pbessolvesymbolic)

  foreach(benchmark ${STATESPACE_BENCHMARKS})
    # Obtain just <name>.mcrl2, split off <name> for the benchmark name and output lps <name>.lps
    get_filename_component(MCRL2_FILENAME ${benchmark} NAME)
    string(REPLACE ".mcrl2" "" NAME ${MCRL2_FILENAME})

    set(LPS_FILENAME "${BENCHMARK_WORKSPACE}/${NAME}.lps")
    set(NODEADLOCK_PBES_FILENAME "${BENCHMARK_WORKSPACE}/${NAME}.nodeadlock.pbes")

    # Benchmark symbolic statespace generation.
    add_tool_benchmark("${NAME}" lpsreach "${LPS_FILENAME}" "")

    # Benchmark solving PBES symbolically.
    add_tool_benchmark("${NAME}" pbessolvesymbolic "${NODEADLOCK_PBES_FILENAME}" "" "-Q0")

    if(MCRL2_ENABLE_JITTYC)
      add_tool_benchmark("${NAME}_jittyc" lpsreach "${LPS_FILENAME}" "" "-rjittyc")
      add_tool_benchmark("${NAME}_jittyc" pbessolvesymbolic "${NODEADLOCK_PBES_FILENAME}" "" "-Q0" "-rjittyc")
    endif()
  endforeach()
endif()

if (MCRL2_ENABLE_DEVELOPER)

  add_dependencies(benchmarks mcrl2rewrite)

  # Find all the benchmark files
  file(GLOB BENCHMARK_FILES ${CMAKE_CURRENT_SOURCE_DIR}/REC/*.dataspec)

  foreach(benchmark ${BENCHMARK_FILES})
    get_filename_component(benchmark ${benchmark} NAME)
    string(REPLACE ".dataspec" "" NAME ${benchmark})

    add_tool_benchmark("${NAME}_jitty" mcrl2rewrite 
      "${CMAKE_CURRENT_SOURCE_DIR}/REC/${NAME}.dataspec" 
      "${CMAKE_CURRENT_SOURCE_DIR}/REC/${NAME}.expressions" 
      "-rjitty")
      
    if(MCRL2_ENABLE_JITTYC)
      add_tool_benchmark("${NAME}_jittyc" mcrl2rewrite 
        "${CMAKE_CURRENT_SOURCE_DIR}/REC/${NAME}.dataspec" 
        "${CMAKE_CURRENT_SOURCE_DIR}/REC/${NAME}.expressions" 
        "-rjittyc")
    endif()
  endforeach()
endif()
