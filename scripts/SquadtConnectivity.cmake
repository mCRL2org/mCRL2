# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# This script adds squadt connectivity for a tool when required.

if(SQUADT_CONNECTIVITY)
  add_definitions("-DENABLE_SQUADT_CONNECTIVITY")
  include_directories(${CMAKE_SOURCE_DIR}/tools/squadt/libraries/tipi/include)
  target_link_libraries(${PROJECT_NAME}
    mcrl2_utilities_squadt
  )
endif(SQUADT_CONNECTIVITY)
