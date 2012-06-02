# Authors: Frank Stappers, Aad Mathijssen and Rimco Boudewijns
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(mcrl2-gui-qt)
cmake_minimum_required(VERSION 2.6)

if(MCRL2_ENABLE_GUI_TOOLS)
find_package(Qt4)

if(QT4_FOUND)
set(QT_USE_QTXML TRUE)

include(${QT_USE_FILE})

set(MCRL2_GUI_RCC_SRCS mcrl2-gui.qrc)

qt4_add_resources(MCRL2_GUI_RC_FILES ${MCRL2_GUI_RCC_SRCS})

set(MCRL2_GUI_UI_FILES
  mainwindow.ui
)

qt4_wrap_ui(MCRL2_GUI_UI_HEADERS ${MCRL2_GUI_UI_FILES})

set(MCRL2_GUI_MOC_HEADERS
  mainwindow.h

  toolaction.h
  toolcatalog.h
  toolinformation.h
)
qt4_wrap_cpp(MCRL2_GUI_MOC_SOURCES ${MCRL2_GUI_MOC_HEADERS})

set(MCRL2_GUI_SRC_FILES
  ${MCRL2_GUI_RC_FILES}
  ${MCRL2_GUI_UI_HEADERS}
  ${MCRL2_GUI_MOC_SOURCES}
  main.cpp
  mainwindow.cpp

  toolaction.cpp
  toolcatalog.cpp
  toolinformation.cpp
)

# ######### Copy extra files ##########
if(NOT APPLE)
  configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/share/mcrl2/tool_catalog.xml" "${CMAKE_CURRENT_BINARY_DIR}/share/mcrl2/tool_catalog.xml" COPYONLY )
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/share/mcrl2/tool_catalog.xml DESTINATION share/mcrl2)

  if( UNIX )
    # Install desktop icons
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/share/applications/mcrl2-gui.desktop DESTINATION share/applications)
  endif( UNIX )

else(NOT APPLE)
  # Copy directly into application
  configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/share/mcrl2/tool_catalog.xml" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.app/share/mcrl2/tool_catalog.xml" COPYONLY )
endif(NOT APPLE)

# ########## Create MacOSX Bundle Information #########
include(MCRL2MacOSXBundleInformation)

# ######### Create Windows Icon ##########
if(WIN32)
  include_directories(${CMAKE_SOURCE_DIR}/build/msvc)
  set(MCRL2_GUI_SRC_FILES ${MCRL2_GUI_SRC_FILES} mcrl2-gui-qt.rc)
endif(WIN32)

add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${MACOSX_BUNDLE_ICON_FILE} ${MCRL2_GUI_SRC_FILES})

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  ${QT_QTMAIN_LIBRARY}
  mcrl2_core
  mcrl2_utilities
  mcrl2_utilities_qt
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})
# For promoted widgets
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/aterm/include)
include_directories(${CMAKE_SOURCE_DIR}/build/workarounds)
include_directories(${CMAKE_SOURCE_DIR}/build/precompile)

# install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_BIN_DIR})

# Relocate install tree
include(${CMAKE_SOURCE_DIR}/scripts/RelocateInstallTree.cmake)

# Generate man pages
include(${CMAKE_SOURCE_DIR}/scripts/GenerateManPages.cmake)

else(QT4_FOUND)
  message(STATUS "Could NOT find Qt4, not building mcrl2-gui-qt.")
endif(QT4_FOUND)
endif ( MCRL2_ENABLE_GUI_TOOLS )
