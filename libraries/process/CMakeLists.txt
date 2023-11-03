mcrl2_add_library(mcrl2_process
  SOURCES
    source/process.cpp
  DEPENDS
    mcrl2_core
    mcrl2_data
)

target_include_directories(mcrl2_process PUBLIC ${CMAKE_SOURCE_DIR}/libraries/lps/include)