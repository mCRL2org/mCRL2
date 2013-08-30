# Authors: Frank Stappers, Aad Mathijssen, Ruud Koolen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(diagraphica)
cmake_minimum_required(VERSION 2.6)

if(APPLE)
  include_directories(/System/Library/Frameworks)
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
set(DIAGRAPHICA_RCC_SRCS icons.qrc)

qt4_add_resources(DIAGRAPHICA_RC_FILES ${DIAGRAPHICA_RCC_SRCS})

set(DIAGRAPHICA_UI_FILES
  dofdialog.ui
  mainwindow.ui
  settingsdialog.ui
)
qt4_wrap_ui(DIAGRAPHICA_UI_HEADERS ${DIAGRAPHICA_UI_FILES})

set(DIAGRAPHICA_MOC_HEADERS
  arcdiagram.h
  attribute.h
  colorchooser.h
  combnplot.h
  corrlplot.h
  diagram.h
  diagrameditor.h
  distrplot.h
  dofdialog.h
  examiner.h
  graph.h
  mainwindow.h
  movabletablewidget.h
  parser.h
  settingsdialog.h
  shape.h
  simulator.h
  timeseries.h
  visualizer.h
)
qt4_wrap_cpp(DIAGRAPHICA_MOC_SOURCES ${DIAGRAPHICA_MOC_HEADERS})

set(DIAGRAPHICA_SRC_FILES
  ${DIAGRAPHICA_RC_FILES}
  ${DIAGRAPHICA_UI_HEADERS}
  ${DIAGRAPHICA_MOC_SOURCES}
  arcdiagram.cpp
  attrdiscr.cpp
  attribute.cpp
  bundle.cpp
  cluster.cpp
  colorchooser.cpp
  combnplot.cpp
  corrlplot.cpp
  diagram.cpp
  diagrameditor.cpp
  distrplot.cpp
  dof.cpp
  dofdialog.cpp
  examiner.cpp
  graph.cpp
  main.cpp
  mainwindow.cpp
  movabletablewidget.cpp
  node.cpp
  parser.cpp
  settingsdialog.cpp
  shape.cpp
  simulator.cpp
  timeseries.cpp
  utils.cpp
  visualizer.cpp
  visutils.cpp
)

# ######### Create Windows Icon ##########
if(WIN32)
  include_directories(${CMAKE_SOURCE_DIR}/build/msvc)
  set(DIAGRAPHICA_SRC_FILES ${DIAGRAPHICA_SRC_FILES} diagraphica.rc)
endif(WIN32)

add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${MACOSX_BUNDLE_ICON_FILE} ${DIAGRAPHICA_SRC_FILES})

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  mcrl2_lts
  mcrl2_syntax
  mcrl2_utilities
  mcrl2_utilities_qt
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
include_directories(${CMAKE_SOURCE_DIR}/libraries/process/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/svc/include)
include_directories(${CMAKE_SOURCE_DIR}/build/precompile)
include_directories(${CMAKE_SOURCE_DIR}/build/workarounds)

# install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_BIN_DIR} COMPONENT Applications)

# Relocate install tree
include(${CMAKE_SOURCE_DIR}/scripts/RelocateInstallTree.cmake)

# Generate man pages
include(${CMAKE_SOURCE_DIR}/scripts/GenerateManPages.cmake)
