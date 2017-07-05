set(MCRL2_TOOL_CLASSES
  release                  # These tools are production tools in the toolset.
  experimental             # These tools are experiments with the intention to become production tools.
  deprecated               # These are outphased tools, to be deleted.
  developer                # This directory contains tools useful for local experiments by developers.
)

foreach(TOOL ${MCRL2_TOOL_CLASSES})
  add_subdirectory(${TOOL})
endforeach()
