mcrl2_add_library(mcrl2_pbes
  SOURCES
    source/algorithms.cpp
    source/pbes.cpp
    source/io.cpp
    source/pbes_explorer.cpp
    source/pgsolver.cpp
  DPARSER_SOURCES
    source/pg_syntax.g
  DEPENDS
    mcrl2_core
    mcrl2_data
    mcrl2_lps
    mcrl2_modal_formula
    mcrl2_lts
    mcrl2_symbolic 
  EXCLUDE_HEADERTEST
    # This test fails due to circular dependencies between the bes and
    # pbes libraries on a mac. Therefore it is disabled.
    mcrl2/pbes/pbes_solver_test.h
    # The headertest below fails as the sylvan library is required for it. 
    mcrl2/pbes/pbesbddsolve1.h   
)

add_subdirectory(example)
