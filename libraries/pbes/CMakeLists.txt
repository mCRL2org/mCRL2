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
  INCLUDE
    # TODO: Get rid of dependency on BES library
    ${CMAKE_SOURCE_DIR}/libraries/bes/include
)
