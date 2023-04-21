add_mcrl2_library(pres
  INSTALL_HEADERS TRUE
  SOURCES
    tools.cpp
    tools2.cpp
  DEPENDS
    mcrl2_core
    mcrl2_data
    mcrl2_lps
    mcrl2_modal_formula
    mcrl2_lts
    mcrl2_symbolic
    mcrl2_pbes
  INCLUDE
)

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
