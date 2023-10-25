if(MCRL2_ENABLE_EXPERIMENTAL)
  set(EXPERIMENTAL_TOOLS
  source/experimental_tools.cpp
  source/experimental_tools2.cpp)
endif()

mcrl2_add_library(mcrl2_pbes
  SOURCES
    source/algorithms.cpp
    source/pbes.cpp
    source/io.cpp
    source/tools.cpp
    source/tools2.cpp
    source/pbes_explorer.cpp
    ${EXPERIMENTAL_TOOLS}
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

# This is a hack to get rid of circular dependencies somehow
# TODO: Get rid of dependency on BES library
target_include_directories(mcrl2_pbes PUBLIC ${CMAKE_SOURCE_DIR}/libraries/bes/include)

# Copied the code below from AddMCRL2Binary.cmake so as to also link all tests
# against the BES library
if(${MCRL2_ENABLE_TESTS})
  file(GLOB all_tests "test/*.cpp")
  foreach(test ${all_tests})
    get_filename_component(base ${test} NAME_WE)
    target_link_libraries(librarytest_mcrl2_pbes_${base} mcrl2_pbes mcrl2_bes)
  endforeach()
endif()

add_subdirectory(example)
