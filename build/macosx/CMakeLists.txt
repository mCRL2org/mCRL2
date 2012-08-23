# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(mac_installer)
cmake_minimum_required(VERSION 2.8)


# ########## Create MacOSX Bundle Information ######### 
include(MCRL2MacOSXBundleInformation)
# overrule default MACOSX_BUNDLE_ICON_FILE location
SET(MACOSX_BUNDLE_ICON_FILE mcrl2-logo.icns)

# ######### General setup ##########
set(MAC_INSTALLER_UI_FILES
  mac_installer.ui
)
qt4_wrap_ui(MAC_INSTALLER_UI_HEADERS ${MAC_INSTALLER_UI_FILES})

set(MAC_INSTALLER_MOC_HEADERS
  mac_installer.h
)
qt4_wrap_cpp(MAC_INSTALLER_MOC_SOURCES ${MAC_INSTALLER_MOC_HEADERS})

set(MAC_INSTALLER_SRC_FILES
  ${MAC_INSTALLER_RC_FILES}
  ${MAC_INSTALLER_UI_HEADERS}
  ${MAC_INSTALLER_MOC_SOURCES}
  mac_installer.cpp
)

if ( MCRL2_ENABLE_GUI_TOOLS )
add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE
  ${MAC_INSTALLER_SRC_FILES}
  ${MACOSX_BUNDLE_ICON_FILE}
)

IF (APPLE)
 SET_SOURCE_FILES_PROPERTIES(${MACOSX_BUNDLE_ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
ENDIF (APPLE)


# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})
# For promoted widgets
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME}
    ${QT_LIBRARIES}
)

# install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_BIN_DIR})

else ( MCRL2_ENABLE_GUI_TOOLS )
  message( STATUS "MCRL2_BUILD_GUI_TOOLS needs to be set to TRUE to create installer")
endif ( MCRL2_ENABLE_GUI_TOOLS )
