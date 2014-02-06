if(NOT WINDOWS)
  set(COMPILING_REWRITER_SRC detail/rewrite/jittyc.cpp)
  set(COMPILING_REWRITER_DEPS dl)
endif()

add_mcrl2_library(data
  INSTALL_HEADERS TRUE
  SOURCES
    data.cpp
    data_specification.cpp
    typecheck.cpp
    detail/enumerate/enum_standard.cpp
    detail/prover/smt_lib_solver.cpp
    detail/rewrite/with_prover.cpp
    detail/rewrite/jitty.cpp
    detail/rewrite/rewrite.cpp
    ${COMPILING_REWRITER_SRC}
  DEPENDS
    mcrl2_atermpp    HEADER_ONLY
    mcrl2_core
    mcrl2_utilities
    ${COMPILING_REWRITER_DEPS}
)
