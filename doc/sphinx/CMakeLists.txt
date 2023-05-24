set(SPHINX_BUILD_CMD "sphinx-build")
set(SPHINX_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})
set(SPHINX_BUILD_ROOT_DIR "${CMAKE_BINARY_DIR}/sphinx")
set(SPHINX_BUILD_SRC_DIR "${SPHINX_BUILD_ROOT_DIR}/source")
set(SPHINX_BUILD_OUT_DIR "${SPHINX_BUILD_ROOT_DIR}/build")
set(SPHINX_BUILD_TEMP_DIR "${SPHINX_BUILD_ROOT_DIR}/temp")

set(SPHINX_PARALLEL "-jauto")
if(CMAKE_BUILD_PARALLEL_LEVEL)
  set(SPHINX_PARALLEL "-j${CMAKE_BUILD_PARALLEL_LEVEL}")
endif()


add_custom_target(cleandoc
  COMMENT "Delete all cached documentation"
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${SPHINX_BUILD_SRC_DIR}
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${SPHINX_BUILD_OUT_DIR}
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${SPHINX_BUILD_TEMP_DIR}
  COMMAND ${CMAKE_COMMAND} -E remove "${SPHINX_BUILD_ROOT_DIR}/build-warnings.log"
  VERBATIM
)

function(add_build_doc_target NAME DESC INCLUDE_CLEAN)
  add_custom_target(${NAME}
    COMMENT ${DESC}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    # ensure the build directory exists, because Sphinx does not create it
    # automatically
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SPHINX_BUILD_OUT_DIR}
    # copy the Sphinx source to the build directory, because some of the
    # extensions modify the source and we don't want to pollute source dir
    COMMAND ${CMAKE_COMMAND}
      -DCOPY_SRC=${SPHINX_SOURCE_DIR}
      -DCOPY_DEST=${SPHINX_BUILD_SRC_DIR}
      -P "${CMAKE_SOURCE_DIR}/build/cmake/CopyIfDifferent.cmake"
    # run the Sphinx build command
    COMMAND ${CMAKE_COMMAND} -E env MCRL2_SRC_DIR=${CMAKE_SOURCE_DIR}
      ${SPHINX_BUILD_CMD}
      -b html -d "${SPHINX_BUILD_TEMP_DIR}/doctrees" ${SPHINX_PARALLEL}
      ${SPHINX_BUILD_SRC_DIR} ${SPHINX_BUILD_OUT_DIR}
      2> ${SPHINX_BUILD_ROOT_DIR}/build-warnings.log
    VERBATIM
    USES_TERMINAL
  )

  add_custom_command(
    TARGET ${NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red --bold
      "-- Please check ${SPHINX_BUILD_ROOT_DIR}/build-warnings.log for warnings and errors"
  )

  if (INCLUDE_CLEAN)
    add_dependencies(${NAME} cleandoc)
  endif()
endfunction()

add_build_doc_target(fastdoc "Generating mCRL2 documentation (does not \
regenerate cached PDF, manual pages and Doxygen documentation)" FALSE)
add_build_doc_target(doc "Generating mCRL2 documentation" TRUE)
