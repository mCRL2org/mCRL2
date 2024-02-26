if(MCRL2_ENABLE_JITTYC)
  set(COMPILING_REWRITER_SRC source/detail/rewrite/jittyc.cpp)
endif()

mcrl2_add_library(mcrl2_data
  SOURCES
    source/data.cpp
    source/data_io.cpp
    source/data_specification.cpp
    source/machine_word.cpp
    source/typecheck.cpp
    source/detail/prover/smt_lib_solver.cpp
    source/detail/rewrite/jitty.cpp
    source/detail/rewrite/rewrite.cpp
    source/detail/rewrite/strategy.cpp
    ${COMPILING_REWRITER_SRC}
  EXCLUDE_HEADERTEST
    mcrl2/data/detail/rewrite/jittycpreamble.h
  DEPENDS
    mcrl2_core
    mcrl2_utilities
    ${COMPILING_REWRITER_DEPS}
)

if(MCRL2_ENABLE_JITTYC)
  target_link_libraries(mcrl2_data PUBLIC dl)
  include(CompilingRewriter.cmake)
endif()

add_subdirectory(example)
