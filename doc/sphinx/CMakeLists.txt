set(SPHINX_BUILD_CMD "sphinx-build")
set(SPHINX_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})
set(SPHINX_BUILD_DIR "${CMAKE_BINARY_DIR}/sphinx")

set(SPHINX_PARALLEL "-jauto")
if(CMAKE_BUILD_PARALLEL_LEVEL)
  set(SPHINX_PARALLEL "-j${CMAKE_BUILD_PARALLEL_LEVEL}")
endif()


add_custom_target(cleandoc
  COMMENT "Delete all cached documentation"
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${SPHINX_BUILD_DIR}
  VERBATIM
)

function(add_build_doc_target NAME DESC INCLUDE_CLEAN)
  set(CLEAN_DEPENCY "")
  if(INCLUDE_CLEAN)
    list(APPEND CLEAN_DEPENCY DEPENDS cleandoc)
  endif()
  add_custom_target(${NAME}
    COMMENT ${DESC}
    ${CLEAN_DEPENCY}
    WORKING_DIRECTORY ${SPHINX_SOURCE_DIR}
    # ensure the build directory exists, because Sphinx does not create it automatically
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SPHINX_BUILD_DIR}
    # run the Sphinx build command
    COMMAND ${SPHINX_BUILD_CMD} -b html -d "${SPHINX_BUILD_DIR}/doctrees" ${SPHINX_PARALLEL} ${SPHINX_SOURCE_DIR} ${SPHINX_BUILD_DIR} 2> ${SPHINX_BUILD_DIR}/sphinx-build-warnings.log
    VERBATIM
  )
  add_custom_command(
    TARGET ${NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND}
      -DIncludePath=${CMAKE_MODULE_PATH}
      -DLogPath=${SPHINX_BUILD_DIR}/sphinx-build-warnings.log
      -P ${CMAKE_SOURCE_DIR}/build/cmake/sphinx/WarnIfNeeded.cmake
  )
endfunction(add_build_doc_target)

add_build_doc_target(fastdoc "Generating mCRL2 documentation (does not regenerate cached PDF, manual pages and Doxygen documentation)" FALSE)
add_build_doc_target(doc "(Re)-generating mCRL2 documentation (regenerates all cached documentation)" TRUE)
