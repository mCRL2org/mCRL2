add_mcrl2_library(lps
  INSTALL_HEADERS TRUE
  SOURCES
    lps.cpp
    lps_io.cpp
    tools.cpp
    linearise.cpp
    lpsparunfoldlib.cpp
    next_state_generator.cpp
  DEPENDS
    mcrl2_core
    mcrl2_data
    mcrl2_process
    mcrl2_smt
  INCLUDE
    ${Boost_INCLUDE_DIRS}
)

if (UNIX)
  target_link_libraries(mcrl2_lps sylvan)
endif()

add_subdirectory(example)
