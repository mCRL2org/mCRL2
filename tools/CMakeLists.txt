add_subdirectory(release)

if(MCRL2_ENABLE_EXPERIMENTAL)
  add_subdirectory(experimental)
endif()

if(MCRL2_ENABLE_DEPRECATED)
  add_subdirectory(deprecated)
endif()

if(MCRL2_ENABLE_DEVELOPER)
  add_subdirectory(developer)
endif()

if(MCRL2_ENABLE_TESTS AND NOT MCRL2_ENABLE_DEVELOPER)
  # For testing we need several developer tools anyway
  add_subdirectory(developer/lpstransform)
  add_subdirectory(developer/pbestransform)
  add_subdirectory(developer/ltsconvertsymbolic)
endif()

if(MCRL2_ENABLE_TESTS AND NOT MCRL2_ENABLE_DEPRECATED)
  # The pbessolve tests use pbespgsolve as a reference solver
  add_subdirectory(deprecated/pbespgsolve)
endif()