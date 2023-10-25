if(MCRL2_ENABLE_STABLE)
  add_subdirectory(release)
endif()

if(MCRL2_ENABLE_EXPERIMENTAL)
  add_subdirectory(experimental)
endif()

if(MCRL2_ENABLE_DEPRECATED)
  add_subdirectory(deprecated)
endif()

if(MCRL2_ENABLE_DEVELOPER)
  add_subdirectory(developer)
endif()