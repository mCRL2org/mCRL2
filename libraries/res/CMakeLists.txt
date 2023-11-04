mcrl2_add_library(mcrl2_res
  SOURCES
    source/res.cpp
    source/io.cpp
  DEPENDS
    mcrl2_pres
)

add_subdirectory(example)
