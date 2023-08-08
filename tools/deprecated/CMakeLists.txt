set(MCRL2_TOOLS
  formulacheck
  lpsrealzone
  pbesabstract
  pbes2booldeprecated
)

if(MCRL2_ENABLE_DEPRECATED)
  foreach(TOOL ${MCRL2_TOOLS})
    add_subdirectory(${TOOL})
  endforeach()
endif()
