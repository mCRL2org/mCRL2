add_mcrl2_library(lps
  INSTALL_HEADERS TRUE
  NOHEADERTEST
    # These headers require C++17. Reenable when compiling with C++17.
    mcrl2/lps/explorer-cpp17.h
    
  SOURCES
    lps.cpp
    tools.cpp
    linearise.cpp
    lpsparunfoldlib.cpp
    next_state_generator.cpp
    simulation.cpp
  DEPENDS
    mcrl2_core
    mcrl2_data
    mcrl2_process
  INCLUDE
    # TODO: get rid of the trace header dependency
    ${CMAKE_SOURCE_DIR}/libraries/trace/include
    ${Boost_INCLUDE_DIRS}
)
