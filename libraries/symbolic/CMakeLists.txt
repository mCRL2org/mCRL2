if (UNIX)
  add_mcrl2_library(symbolic
    SOURCES
      ldd_stream.cpp
    DEPENDS
      mcrl2_data
    INCLUDE
      ${Boost_INCLUDE_DIRS}
  )

  target_link_libraries(mcrl2_symbolic sylvan)
endif()
