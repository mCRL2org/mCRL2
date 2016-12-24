set(MCRL2_TOOLS
  mcrl2parse
)

foreach(TOOL ${MCRL2_TOOLS})
  add_subdirectory(${TOOL})
endforeach()
