# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(ltsgraph-qt)
cmake_minimum_required(VERSION 2.6)

find_package(Qt4)

if (MCRL2_ENABLE_GUI_TOOLS)
if (QT4_FOUND)

set(QT_USE_QTOPENGL TRUE)
include(${QT_USE_FILE})

set(UI_FILES
  mainwindow.ui
  springlayout.ui
)
qt4_wrap_ui(UI_HEADERS ${UI_FILES})

set(MOC_HEADERS
  mainwindow.h
  glwidget.h
  springlayout.h
)
qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})

set(SRC_FILES
  ${UI_HEADERS}
  ${MOC_SOURCES}
  glscene.cpp
  glwidget.cpp
  graph.cpp
  main.cpp
  mainwindow.cpp
  springlayout.cpp
)

add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} 
  ${QT_LIBRARIES}
  mcrl2_lts
  mcrl2_core
  mcrl2_aterm
  mcrl2_data
  mcrl2_utilities
  mcrl2_lps
  svc
  GLU
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})
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

else(QT4_FOUND)
  message(STATUS "Could NOT find Qt4, not building ltsgraph-qt.")
endif(QT4_FOUND)
endif ( MCRL2_ENABLE_GUI_TOOLS )
