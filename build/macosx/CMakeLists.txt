# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(mac_installer)
cmake_minimum_required(VERSION 2.6)

# ######### General setup ##########

SET(MACOSX_BUNDLE_ICON_FILE mcrl2-logo.icns)

if ( MCRL2_BUILD_GUI_TOOLS )
add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE
  mac_installer.cpp
  ${MACOSX_BUNDLE_ICON_FILE}
)

IF (APPLE)
 SET_SOURCE_FILES_PROPERTIES(${MACOSX_BUNDLE_ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
ENDIF (APPLE)


include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)

if( MONO_LIB )
  target_link_libraries(${PROJECT_NAME}
    mcrl2_monolib
  )

else( MONO_LIB )
target_link_libraries(${PROJECT_NAME}
  ${wxWidgets_LIBRARIES}
)
endif( MONO_LIB )

# install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_BIN_DIR})

else ( MCRL2_BUILD_GUI_TOOLS)
  message( FATAL "MCRL2_BUILD_GUI_TOOLS needs to be set to TRUE to create installer")

endif ( MCRL2_BUILD_GUI_TOOLS )
