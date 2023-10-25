mcrl2_add_library(mcrl2_atermpp
  SOURCES
    source/aterm_implementation.cpp
    source/aterm_io_binary.cpp
    source/aterm_io_text.cpp
    source/function_symbol.cpp
    source/function_symbol_pool.cpp
  DEPENDS
    mcrl2_utilities
    Threads::Threads
)

if (${MCRL2_ENABLE_BENCHMARKS})
  add_subdirectory(benchmark/)
endif()

add_subdirectory(example)
