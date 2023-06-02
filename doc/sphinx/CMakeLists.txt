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

if(MCRL2_ENABLE_DOC_DOXYGEN)
  # Replace variables inside @@ with the current values
  set(DOXYGEN_OUTPUT_PATH ${CMAKE_BINARY_DIR}/sphinx/source/_doxygen/)
  set(DOXYGEN_INPUT_PATH ${CMAKE_SOURCE_DIR}/libraries/)

  configure_file(${CMAKE_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_BINARY_DIR}/doc/Doxyfile @ONLY)

  # Configure the doxygen command
  add_custom_target(doxygen
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/libraries/
    COMMENT "Extracting source code documentation using Doxygen"
    # ensure the build directory exists, because Doxygen does not create it
    # automatically
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DOXYGEN_OUTPUT_PATH}
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/doc/Doxyfile
    VERBATIM
    USES_TERMINAL)
endif()

# Get the list of tools that are built by the current configuration
get_property(MCRL2_TOOLS GLOBAL PROPERTY MCRL2_TOOLS)

function(add_build_doc_target NAME DESC INCLUDE_CLEAN)
  set(SPHINX_TAGS "")
  if(MCRL2_ENABLE_DOC_DOXYGEN)
    list(APPEND SPHINX_TAGS -t build_doxygen)
  endif()
  if(MCRL2_ENABLE_DOC_PDFLATEX)
    list(APPEND SPHINX_TAGS -t build_pdflatex)
  endif()
  if(MCRL2_ENABLE_DOC_MANUAL)
    list(APPEND SPHINX_TAGS -t build_manual)
  endif()

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
    COMMAND ${CMAKE_COMMAND} -E env CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR} MCRL2_TOOL_PATH=${MCRL2_TOOL_PATH} MCRL2_TOOLS=${MCRL2_TOOLS} DOXYGEN_EXECUTABLE=${DOXYGEN_EXECUTABLE}
      ${SPHINX_BUILD_CMD}
      -b html 
      -d "${SPHINX_BUILD_TEMP_DIR}/doctrees"
      -T
      ${SPHINX_TAGS}
      ${SPHINX_PARALLEL}
      ${SPHINX_BUILD_SRC_DIR} 
      ${SPHINX_BUILD_OUT_DIR}
    VERBATIM
    USES_TERMINAL
  )

  if (MCRL2_ENABLE_DOC_DOXYGEN)
    add_dependencies(${NAME} doxygen)
  endif()

  if (INCLUDE_CLEAN)
    add_dependencies(${NAME} cleandoc)
    # Ensure that cleandoc is executed before doxygen is ran
    add_dependencies(doxygen cleandoc)
  endif()
endfunction()

add_build_doc_target(fastdoc "Generating mCRL2 documentation (does not clean up old files)" FALSE)
add_build_doc_target(doc "Generating mCRL2 documentation" TRUE)

if(MCRL2_ENABLE_DOCUMENTATION_MANUAL)
  if(NOT MCRL2_ENABLE_EXPERIMENTAL or NOT MCRL2_ENABLE_STABLE)
    message(FATAL_ERROR "The option MCRL2_ENABLE_DOCUMENTATION_MANUAL requires both the stable and experimental tools to be built")
  endif()
endif()