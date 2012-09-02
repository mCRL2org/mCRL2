# Authors: Jeroen Keiren 
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

FIND_LIBRARY(GL2PS_LIBRARY
  NAMES gl2ps
)

FIND_PATH(GL2PS_INCLUDE_DIR "gl2ps.h" )

set(GL2PS_LIBRARIES ${GL2PS_LIBRARY} )
set(GL2PS_INCLUDE_DIRS ${GL2PS_INCLUDE_DIR} )

INCLUDE(FindPackageHandleStandardArgs)

find_package_handle_standard_args(gl2ps DEFAULT_MSG GL2PS_LIBRARY GL2PS_INCLUDE_DIRS )

##---------------------------------------------------
## Find mlib / required by gl2ps to resolve undefined symbols
##---------------------------------------------------
if(UNIX)
  find_package( mlib )
  if(NOT mlib_FOUND )
    message( FATAL_ERROR "Package 'mlib' not found (required for using gl2ps).")
  endif(NOT mlib_FOUND )
endif(UNIX)

MARK_AS_ADVANCED( GL2PS_LIBRARY GL2PS_INCLUDE_DIRS )

