if(MCRL2_ENABLE_EXPERIMENTAL)
  set(EXPERIMENTAL_TOOLS
  experimental_tools.cpp
  experimental_tools2.cpp)
endif()

add_mcrl2_library(pbes
  INSTALL_HEADERS TRUE
  SOURCES
    algorithms.cpp
    pbes.cpp
    tools.cpp
    tools2.cpp
    pbes_explorer.cpp
    ${EXPERIMENTAL_TOOLS}
  DEPENDS
    mcrl2_core
    mcrl2_data
    mcrl2_lps
    mcrl2_modal_formula
    mcrl2_lts
  INCLUDE
    # TODO: Get rid of dependency on BES library
    ${CMAKE_SOURCE_DIR}/libraries/bes/include
)

# Copied the code below from AddMCRL2Binary.cmake so as to also link all tests
# against the BES library
if(${MCRL2_ENABLE_TESTS})
  file(GLOB all_tests "test/*.cpp")
  foreach(test ${all_tests})
    get_filename_component(base ${test} NAME_WE)
    target_link_libraries(librarytest_mcrl2_pbes_${base} mcrl2_pbes mcrl2_bes)
  endforeach()
  file(GLOB_RECURSE headers "include/*.h")
  foreach(header ${headers})
    file(RELATIVE_PATH cppname ${CMAKE_CURRENT_SOURCE_DIR}/include ${header})
    file(TO_CMAKE_PATH ${cppname} cppname)
    string(REGEX REPLACE "\\.|/" "_" testname "${cppname}")
    set(testname headertest_${testname})
    if(NOT TARGET ${testname})
        target_link_libraries(${testname} mcrl2_pbes mcrl2_bes)
    endif()
  endforeach()
endif()
