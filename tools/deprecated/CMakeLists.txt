set(MCRL2_TOOLS
  formulacheck
)

if(MCRL2_ENABLE_DEPRECATED)
  foreach(TOOL ${MCRL2_TOOLS})
    add_subdirectory(${TOOL})
  endforeach()
endif()
