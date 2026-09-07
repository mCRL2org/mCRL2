# Replace variables @@ with the current values
# Prefer the sphinx-build executable belonging to the Python interpreter found
# by FindPythonRequirements (e.g. inside a virtual environment), with a fallback
# to whatever sphinx-build is on the PATH.
get_filename_component(SPHINX_PYTHON_BIN_DIR "${Python_EXECUTABLE}" DIRECTORY)
if(MSVC)
  set(SPHINX_BUILD_NAMES sphinx-build.bat sphinx-build)
else()
  set(SPHINX_BUILD_NAMES sphinx-build)
endif()
find_program(SPHINX_BUILD_CMD NAMES ${SPHINX_BUILD_NAMES} HINTS "${SPHINX_PYTHON_BIN_DIR}")
if(NOT SPHINX_BUILD_CMD)
  message(FATAL_ERROR "Could not find sphinx-build next to ${Python_EXECUTABLE} or on the PATH. \
Install the Python dependencies (see the developer manual), e.g. 'pip install -r <mCRL2>/requirements.txt'.")
endif()
set(SPHINX_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})
set(SPHINX_BUILD_ROOT_DIR "${CMAKE_BINARY_DIR}/sphinx")
set(SPHINX_BUILD_OUT_DIR "${SPHINX_BUILD_ROOT_DIR}/build")
set(SPHINX_BUILD_TEMP_DIR "${SPHINX_BUILD_ROOT_DIR}/temp")


set(DOXYGEN_BASE ${SPHINX_BUILD_ROOT_DIR}/_doxygen/)
set(DOXYGEN_OUTPUT_PATH ${DOXYGEN_BASE}/output/)
set(DOXYGEN_TAG_PATH ${DOXYGEN_BASE}/mcrl2.tag)
set(DOXYGEN_INPUT_PATH ${CMAKE_SOURCE_DIR}/libraries/)

set(SPHINX_PARALLEL "-jauto")
if(CMAKE_BUILD_PARALLEL_LEVEL)
  set(SPHINX_PARALLEL "-j${CMAKE_BUILD_PARALLEL_LEVEL}")
endif()

if(MCRL2_ENABLE_DOC_DOXYGEN)
  configure_file(${CMAKE_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_BINARY_DIR}/doc/Doxyfile @ONLY)

  # Configure two doxygen commands (one will have the cleandoc as dependency)
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

# Set additional options for sphinx-build
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

# Replace the cmake variables by their values (we do not use the environment since we want to be able to call sphinx_build from the command line)
configure_file(${SPHINX_SOURCE_DIR}/conf.in.py ${SPHINX_SOURCE_DIR}/conf.py @ONLY)

add_custom_target("doc"
  COMMENT "Generating mCRL2 documentation"
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  # run the Sphinx build command
  COMMAND ${SPHINX_BUILD_CMD}
    -b html 
    -d "${SPHINX_BUILD_TEMP_DIR}/doctrees"
    -T
#     -W --keep-going
    ${SPHINX_TAGS}
    ${SPHINX_PARALLEL}
    ${SPHINX_SOURCE_DIR} 
    ${SPHINX_BUILD_OUT_DIR}      
  VERBATIM
  USES_TERMINAL
)

add_custom_target(cleandoc
  COMMENT "Delete all cached documentation"
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${DOXYGEN_BASE}
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${SPHINX_BUILD_OUT_DIR}
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${SPHINX_BUILD_TEMP_DIR}
  VERBATIM
)
