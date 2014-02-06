# Authors: Jeroen Keiren 
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

include(FindPackageHandleStandardArgs)

find_library(GL2PS_LIBRARY NAMES gl2ps)
find_path(GL2PS_INCLUDE "gl2ps.h")

if(NOT (GL2PS_LIBRARY AND GL2PS_INCLUDE_DIR))
  add_subdirectory(${CMAKE_SOURCE_DIR}/3rd-party/gl2ps)
  set(GL2PS_LIBRARY "gl2ps")
  set(GL2PS_INCLUDE ${CMAKE_SOURCE_DIR}/3rd-party/gl2ps/include)
endif()

set(GL2PS_LIBRARIES ${GL2PS_LIBRARY} )
set(GL2PS_INCLUDE_DIR ${GL2PS_INCLUDE} )

find_package_handle_standard_args(gl2ps DEFAULT_MSG GL2PS_LIBRARY GL2PS_INCLUDE)

mark_as_advanced( GL2PS_LIBRARY GL2PS_INCLUDE )

