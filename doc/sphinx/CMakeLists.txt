# Authors: Jeroen Keiren
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# This script generates toolset documentation

configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/source/user_manual/downloads-nightly.inc.template" "${CMAKE_CURRENT_SOURCE_DIR}/source/user_manual/downloads-nightly.inc" @ONLY )
configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/source/user_manual/downloads-release.inc.template" "${CMAKE_CURRENT_SOURCE_DIR}/source/user_manual/downloads-release.inc" @ONLY )

set( MCRL2_GENERATE_DOC_RELEASE_FLAG "" )
if(MCRL2_PACKAGE_RELEASE)
  set( MCRL2_GENERATE_DOC_RELEASE_FLAG "-r" )
endif(MCRL2_PACKAGE_RELEASE)

if(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set( BIN_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
else()
  set( BIN_PATH ${CMAKE_BINARY_DIR})
endif()

set( MCRL2_GENERATE_DOC_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/generate.py )
set( MCRL2_GENERATE_DOC_COMMAND ${MCRL2_GENERATE_DOC_SCRIPT} ${MCRL2_GENERATE_DOC_RELEASE_FLAG} -v=${MCRL2_VERSION} --path=${BIN_PATH} --temp=${CMAKE_CURRENT_BINARY_DIR}/_temp --out=${CMAKE_CURRENT_BINARY_DIR}/html )

message(STATUS "Path: ${BIN_PATH}")

add_custom_target(doc
  COMMAND ${MCRL2_GENERATE_DOC_COMMAND} -ddc
  COMMENT "(Re)-generating mCRL2 documentation (regenerates all cached documentation)" VERBATIM
)

add_custom_target(fastdoc 
  COMMAND ${MCRL2_GENERATE_DOC_COMMAND} -dd
  COMMENT "Generating mCRL2 documentation (does not regenerate cached PDF, manual pages and Doxygen documentation)" VERBATIM
)
