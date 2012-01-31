# Authors: Jeroen Keiren
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# This script generates toolset documentation

set( MCRL2_GENERATE_DOC_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/generate.py )

add_custom_target(doc
  ${MCRL2_GENERATE_DOC_SCRIPT} -c -v=${MCRL2_MAJOR_VERSION}.${MCRL2_MINOR_VERSION} --temp=${CMAKE_CURRENT_BINARY_DIR}/_temp --out=${CMAKE_CURRENT_BINARY_DIR}/html
  COMMAND ${MCRL2_GENERATE_DOC_SCRIPT} -v=${MCRL2_MAJOR_VERSION}.${MCRL2_MINOR_VERSION} --temp=${CMAKE_CURRENT_BINARY_DIR}/_temp --out=${CMAKE_CURRENT_BINARY_DIR}/html
  COMMENT "(Re)-generating mCRL2 documentation (regenerates all cached documentation)" VERBATIM
)

add_custom_target(fastdoc ${MCRL2_GENERATE_DOC_SCRIPT} --temp=${CMAKE_CURRENT_BINARY_DIR}/_temp --out=${CMAKE_CURRENT_BINARY_DIR}/html
  COMMENT "Generating mCRL2 documentation (does not regenerate cached PDF, manual pages and Doxygen documentation)" VERBATIM
)
