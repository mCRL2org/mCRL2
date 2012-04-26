# Authors: Frank Stappers, Aad Mathijssen and Rimco Boudewijns
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(mcrl2xi_qt)
cmake_minimum_required(VERSION 2.6)

if (MCRL2_ENABLE_GUI_TOOLS)
find_package(Qt4)

if (QT4_FOUND)
include(${QT_USE_FILE})

set(UI_FILES
  mainwindow.ui
  documentmanager.ui
  documentwidget.ui
)
qt4_wrap_ui(UI_HEADERS ${UI_FILES})

set(MOC_HEADERS
  mainwindow.h
  highlighter.h

  documentmanager.h
  documentwidget.h
  parser.h
  rewriter.h
)
qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})

set(SRC_FILES
  ${UI_HEADERS}
  ${MOC_SOURCES}
  main.cpp
  mainwindow.cpp
  highlighter.cpp

  documentmanager.cpp
  documentwidget.cpp
  parser.cpp
  rewriter.cpp
)

add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${SRC_FILES})

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  ${QT_QTMAIN_LIBRARY}
  mcrl2_aterm
  mcrl2_core
  mcrl2_data
  mcrl2_lps
  mcrl2_utilities
  mcrl2_utilities_qt
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})
# For promoted widgets
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lps/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/bes/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/pbes/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/process/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/aterm/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/dparser)
include_directories(${CMAKE_SOURCE_DIR}/build/precompile)

else(QT4_FOUND)
  message(STATUS "Could NOT find Qt4, not building mcrl2xi-qt.")
endif(QT4_FOUND)
endif ( MCRL2_ENABLE_GUI_TOOLS )
