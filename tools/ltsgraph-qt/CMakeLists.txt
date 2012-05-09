# Authors: Frank Stappers, Aad Mathijssen and Rimco Boudewijns
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(ltsgraph-qt)
cmake_minimum_required(VERSION 2.6)

if (MCRL2_ENABLE_GUI_TOOLS)
find_package(Qt4)

IF(APPLE)
   INCLUDE_DIRECTORIES ( /System/Library/Frameworks )
   FIND_LIBRARY(GLUT_LIBRARY GLUT )
   FIND_LIBRARY(OpenGL_LIBRARY OpenGL )
   MARK_AS_ADVANCED (GLUT_LIBRARY
                     OpenGL_LIBRARY)
   SET(MCRL2gl ${GLUT_LIBRARY} ${OpenGL_LIBRARY})
ELSE(APPLE)
   SET(MCRL2gl GLU32 )
ENDIF (APPLE)

if (QT4_FOUND)
set(QT_USE_QTOPENGL TRUE)
include(${QT_USE_FILE})

set(LTSGRAPH_UI_FILES
  mainwindow.ui
)
qt4_wrap_ui(LTSGRAPH_UI_HEADERS ${LTSGRAPH_UI_FILES})

set(LTSGRAPH_MOC_HEADERS
  mainwindow.h
  glwidget.h
  glscene.h
  graph.h
)
qt4_wrap_cpp(LTSGRAPH_MOC_SOURCES ${LTSGRAPH_MOC_HEADERS})

set(LTSGRAPH_SRC_FILES
  ${LTSGRAPH_UI_HEADERS}
  ${LTSGRAPH_MOC_SOURCES}
  main.cpp
  mainwindow.cpp
  glwidget.cpp
  glscene.cpp
  graph.cpp
)

add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${LTSGRAPH_SRC_FILES})

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  ${QT_QTMAIN_LIBRARY}
  mcrl2_aterm
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
include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lps/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lts/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/trace/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/aterm/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/svc/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/dparser)
include_directories(${CMAKE_SOURCE_DIR}/build/precompile)
include_directories(${CMAKE_SOURCE_DIR}/build/workarounds)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/gl2ps/include/gl2ps)

else(QT4_FOUND)
  message(STATUS "Could NOT find Qt4, not building ltsgraph-qt.")
endif(QT4_FOUND)
endif ( MCRL2_ENABLE_GUI_TOOLS )
