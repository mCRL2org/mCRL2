add_custom_target(doc
  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
  COMMAND make html
  COMMENT "(Re)-generating mCRL2 documentation (regenerates all cached documentation)" VERBATIM
)

add_custom_target(fastdoc
  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
  COMMAND make clean html
  COMMENT "Generating mCRL2 documentation (does not regenerate cached PDF, manual pages and Doxygen documentation)" VERBATIM
)
