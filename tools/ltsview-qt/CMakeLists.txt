# Authors: Frank Stappers, Aad Mathijssen, Ruud Koolen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(ltsview-qt)
cmake_minimum_required(VERSION 2.6)

if ( MCRL2_ENABLE_GUI_TOOLS )

find_package(Qt4 4.6 COMPONENTS QtCore QtGui QtOpenGL)
if (QT4_FOUND)

# ########## Create MacOSX Bundle Information ######### 
include(MCRL2MacOSXBundleInformation)

# ######### General setup ##########
IF(WIN32)
  include_directories(${CMAKE_SOURCE_DIR}/build/msvc)
  SET(SRCS ${SRCS} ltsview.rc)
ENDIF(WIN32)

set(UI_FILES
  infodialog.ui
  markstateruledialog.ui
  savepicturedialog.ui
  savevectordialog.ui
  settingsdialog.ui
  simdialog.ui
)
qt4_wrap_ui(UI_HEADERS ${UI_FILES})

set(MOC_HEADERS
  glcanvas.h
  infodialog.h
  ltsview.h
  mainframe.h
  markmanager.h
  markstateruledialog.h
  primitivefactory.h
  savepicturedialog.h
  savevectordialog.h
  settings.h
  settingsdialog.h
  simdialog.h
  simulation.h
  visualizer.h
)
qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})

# Sources:
add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE
  ${UI_HEADERS}
  ${MOC_SOURCES}
  cluster.cpp
  conedb.cpp
  fsm_state_positioner.cpp
  glcanvas.cpp
  infodialog.cpp
  lts.cpp
  ltsview.cpp
  mainframe.cpp
  markdialog_old.cpp
  markmanager.cpp
  markstateruledialog.cpp
  primitivefactory.cpp
  primitives.cpp
  rtree.cpp
  savepicturedialog.cpp
  savevectordialog.cpp
  settingsdialog.cpp
  simdialog.cpp
  simulation.cpp
  sp_state_positioner.cpp
  state.cpp
  transition.cpp
  vectors.cpp
  visobjectfactory.cpp
  visualizer.cpp

  ${MACOSX_BUNDLE_ICON_FILE}
  ${SRCS}
)

IF (APPLE)
 SET_SOURCE_FILES_PROPERTIES(${MACOSX_BUNDLE_ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
ENDIF (APPLE)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/libraries/process/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lps/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lts/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/trace/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/aterm/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/svc/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/tr/include)
include_directories(${CMAKE_SOURCE_DIR}/build/precompile)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/dparser)
include_directories(${QT_INCLUDES})
if(GL2PS_FOUND)
  include_directories(${GL2PS_INCLUDE_DIRS})
else(GL2PS_FOUND)
  include_directories(${CMAKE_SOURCE_DIR}/3rd-party/gl2ps/include/gl2ps)
endif(GL2PS_FOUND)

# actual target:
target_link_libraries(${PROJECT_NAME}
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  ${QT_QTOPENGL_LIBRARY}
  ${wxWidgets_LIBRARIES}
  mcrl2_lts
  mcrl2_utilities
  mcrl2_utilities_wx
  mcrl2_syntax
  dparser
  tr
)

if(GL2PS_FOUND)
  target_link_libraries(${PROJECT_NAME}
    ${GL2PS_LIBRARIES}
  )
else (GL2PS_FOUND)
  target_link_libraries(${PROJECT_NAME}
    gl2ps
  )
endif(GL2PS_FOUND)

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
