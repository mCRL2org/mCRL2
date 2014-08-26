# Authors: Frank Stappers 
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

function(macosx_set_packaging_info ICONFILE)
  # Get copyright text from file
  file(READ "${CMAKE_SOURCE_DIR}/COPYING" MCRL2_COPYRIGHT_TEXT)

  # Set default bundle information 
  SET(MACOSX_ICON_PATH ${CMAKE_SOURCE_DIR}/build/packaging/icons/${ICONFILE}.icns PARENT_SCOPE)
  SET(MACOSX_BUNDLE_NAME ${PROJECT_NAME} PARENT_SCOPE)
  SET(MACOSX_BUNDLE_ICON_FILE ${ICONFILE}.icns PARENT_SCOPE)
  SET(MACOSX_BUNDLE_COPYRIGHT ${MCRL2_COPYRIGHT_TEXT} PARENT_SCOPE)
  SET(MACOSX_BUNDLE_BUNDLE_VERSION ${MCRL2_VERSION} PARENT_SCOPE)
endfunction(macosx_set_packaging_info)
