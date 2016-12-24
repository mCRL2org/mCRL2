set(MCRL2_TOOLS
  formulacheck
)

foreach(TOOL ${MCRL2_TOOLS})
  add_subdirectory(${TOOL})
endforeach()
