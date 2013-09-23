# Authors: Frank Stappers 
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

function(macosx_set_packaging_info ICONFILE ICONLOCATION)
  # Get copyright text from file
  file(READ "${CMAKE_SOURCE_DIR}/COPYING" MCRL2_COPYRIGHT_TEXT)

  # Raise warning if MCRL2_VERSION is not set.
  if( NOT(MCRL2_VERSION) )
    message(WARNING "MCRL2_VERSION is not set." )
  endif( NOT(MCRL2_VERSION) )

  # Set default bundle information
  SET(MACOSX_BUNDLE_NAME ${PROJECT_NAME})
  SET(MACOSX_BUNDLE_ICON_FILE ${CMAKE_SOURCE_DIR}/build/packaging/icons/${ICONFILE}.icns)
  SET(MACOSX_BUNDLE_COPYRIGHT ${MCRL2_COPYRIGHT_TEXT})
  SET(MACOSX_BUNDLE_BUNDLE_VERSION ${MCRL2_VERSION}) 
  SET_SOURCE_FILES_PROPERTIES(${MACOSX_BUNDLE_ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION ${ICONLOCATION})
endfunction(macosx_set_packaging_info)
