# Authors: Frank Stappers, Aad Mathijssen and Rimco Boudewijns
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(ltsgraph)
cmake_minimum_required(VERSION 2.6)

if(APPLE)
  INCLUDE_DIRECTORIES ( /System/Library/Frameworks )
  find_library(GLUT_LIBRARY GLUT )
  find_library(OpenGL_LIBRARY OpenGL )
  set(MCRL2gl ${GLUT_LIBRARY} ${OPENGL_LIBRARY})
else(APPLE)
  find_library(OpenGL_LIBRARY OpenGL )
  set(MCRL2gl ${OPENGL_glu_LIBRARY} ${OPENGL_gl_LIBRARY})
endif(APPLE)

# ########## Create MacOSX Bundle Information #########
include(MCRL2MacOSXBundleInformation)
IF (APPLE)
 SET_SOURCE_FILES_PROPERTIES(${MACOSX_BUNDLE_ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
ENDIF (APPLE)

# ######### General setup ##########
set(LTSGRAPH_RCC_SRCS ltsgraph.qrc)

qt4_add_resources(LTSGRAPH_RC_FILES ${LTSGRAPH_RCC_SRCS})

set(LTSGRAPH_UI_FILES
  mainwindow.ui
  glwidget.ui
  springlayout.ui
  information.ui
  dimensionsdialog.ui
)
qt4_wrap_ui(LTSGRAPH_UI_HEADERS ${LTSGRAPH_UI_FILES})

set(LTSGRAPH_MOC_HEADERS
  mainwindow.h
  glwidget.h
  springlayout.h
  information.h
  dimensionsdialog.h
)

qt4_wrap_cpp(LTSGRAPH_MOC_SOURCES ${LTSGRAPH_MOC_HEADERS})

set(LTSGRAPH_SRC_FILES
  ${LTSGRAPH_RC_FILES}
  ${LTSGRAPH_UI_HEADERS}
  ${LTSGRAPH_MOC_SOURCES}
  main.cpp
  mainwindow.cpp
  glwidget.cpp
  glscene.cpp
  graph.cpp
  springlayout.cpp
  information.cpp
  dimensionsdialog.cpp
)

# ######### Create Windows Icon ##########
if(WIN32)
  include_directories(${CMAKE_SOURCE_DIR}/build/msvc)
  set(LTSGRAPH_SRC_FILES ${LTSGRAPH_SRC_FILES} ltsgraph.rc)
endif(WIN32)

add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${MACOSX_BUNDLE_ICON_FILE} ${LTSGRAPH_SRC_FILES})

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  mcrl2_core
  mcrl2_data
  mcrl2_lps
  mcrl2_lts
  mcrl2_utilities
  mcrl2_utilities_qt
  svc
  gl2ps
  ${MCRL2gl}
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})
# For promoted widgets
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
# include_directories(${CMAKE_SOURCE_DIR}/libraries/aterm/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lps/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lts/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/trace/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)

include_directories(${CMAKE_SOURCE_DIR}/3rd-party/svc/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/dparser)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/gl2ps/include/gl2ps)

# install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_BIN_DIR} COMPONENT Applications)

# Relocate install tree
include(${CMAKE_SOURCE_DIR}/scripts/RelocateInstallTree.cmake)

# Generate man pages
include(${CMAKE_SOURCE_DIR}/scripts/GenerateManPages.cmake)
