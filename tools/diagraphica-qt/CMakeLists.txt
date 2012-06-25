# Authors: Frank Stappers, Aad Mathijssen, Ruud Koolen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(diagraphica-qt)
cmake_minimum_required(VERSION 2.6)

if ( MCRL2_ENABLE_GUI_TOOLS )

find_package(Qt4 4.6 COMPONENTS QtCore QtGui QtOpenGL)
if (QT4_FOUND)
include(${QT_USE_FILE})

# ########## Create MacOSX Bundle Information ######### 
include(MCRL2MacOSXBundleInformation)

# ######### General setup ##########

IF(WIN32)
  include_directories(${CMAKE_SOURCE_DIR}/build/msvc)
  SET(SRCS ${SRCS} diagraphica.rc)
ENDIF(WIN32)

add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE
  arcdiagram.cpp
  attrdiscr.cpp
  attribute.cpp
  bitmappanel.cpp
  bundle.cpp
  cluster.cpp
  colleague.cpp
  colorchooser.cpp
  combnplot.cpp
  corrlplot.cpp
  diagram.cpp
  diagrameditor.cpp
  diagraphica.cpp
  distrplot.cpp
  dof.cpp
  droptarget.cpp
  edge.cpp
  examiner.cpp
  frame.cpp
  glcanvas.cpp
  graph.cpp
  node.cpp
  opacitychooser.cpp
  parser.cpp
  popupframe.cpp
  settingsframe.cpp
  shape.cpp
  simulator.cpp
  timeseries.cpp
  utils.cpp
  value.cpp
  visualizer.cpp
  visutils.cpp

  ${MACOSX_BUNDLE_ICON_FILE}
  ${SRCS}
)

IF (APPLE)
 SET_SOURCE_FILES_PROPERTIES(${MACOSX_BUNDLE_ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
ENDIF (APPLE)

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

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  ${wxWidgets_LIBRARIES}
  mcrl2_lts
  mcrl2_syntax
  dparser
  mcrl2_utilities
  mcrl2_utilities_qt
  mcrl2_utilities_wx
)

# install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_BIN_DIR})

# Relocate install tree
include(${CMAKE_SOURCE_DIR}/scripts/RelocateInstallTree.cmake)

# Generate man pages
include(${CMAKE_SOURCE_DIR}/scripts/GenerateManPages.cmake)

else(QT4_FOUND)
  message(STATUS "Could NOT find Qt4, not building ltsview-qt.")
endif(QT4_FOUND)
endif ( MCRL2_ENABLE_GUI_TOOLS )
