mcrl2_add_library(mcrl2_pres
  SOURCES
    source/algorithms.cpp
    source/io.cpp
    source/pres.cpp
  DEPENDS
    mcrl2_core
    mcrl2_data
    mcrl2_lps
    mcrl2_modal_formula
    mcrl2_lts
    mcrl2_symbolic
    mcrl2_pbes
    mcrl2_utilities
)

# This is a hack to get rid of circular dependencies somehow
# TODO: Get rid of dependency on RES library
target_include_directories(mcrl2_pres PUBLIC ${CMAKE_SOURCE_DIR}/libraries/res/include)

# Copied the code below from AddMCRL2Binary.cmake so as to also link all tests
# against the RES library
if(MCRL2_ENABLE_TESTS)
  file(GLOB all_tests "test/*.cpp")
  foreach(test ${all_tests})
    get_filename_component(base ${test} NAME_WE)
    target_link_libraries(librarytest_mcrl2_pres_${base} mcrl2_pbes)
  endforeach()
endif()


add_subdirectory(example)
