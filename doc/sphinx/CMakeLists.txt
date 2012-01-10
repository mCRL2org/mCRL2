# Authors: Jeroen Keiren
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# This script generates toolset documentation

set( MCRL2_GENERATE_DOC_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/generate.py )

add_custom_target(doc ${MCRL2_GENERATE_DOC_SCRIPT}
  COMMENT "Generating mCRL2 documentation" VERBATIM
)

add_custom_target(doc/fast ${MCRL2_GENERATE_DOC_SCRIPT}
  COMMENT "Generating mCRL2 documentation" VERBATIM
)

