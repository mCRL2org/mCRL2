add_mcrl2_library(atermpp
  INSTALL_HEADERS TRUE
  SOURCES
    aterm_implementation.cpp
    aterm_io_binary.cpp
    aterm_io_text.cpp
    function_symbol.cpp
    indexed_set.cpp
    table.cpp
  DEPENDS
    mcrl2_utilities
)
