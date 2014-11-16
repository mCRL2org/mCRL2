# Authors: Frank Stappers 
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

FIND_LIBRARY(MLIB_LIBRARY
  NAMES
  m mlib
)
			
set( mlib_FOUND ${MLIB_LIBRARY} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(mlib  DEFAULT_MSG
                                  MLIB_LIBRARY )

mark_as_advanced(MLIB_LIBRARY )
