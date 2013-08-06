# Authors: Frank Stappers, Aad Mathijssen, Ruud Koolen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(ltsview)
cmake_minimum_required(VERSION 2.6)

# ########## Create MacOSX Bundle Information #########
include(MCRL2MacOSXBundleInformation)
IF (APPLE)
 SET_SOURCE_FILES_PROPERTIES(${MACOSX_BUNDLE_ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
ENDIF (APPLE)

# ######### General setup ##########
set(LTSVIEW_RCC_SRCS ltsview.qrc)

qt4_add_resources(LTSVIEW_RC_FILES ${LTSVIEW_RCC_SRCS})

set(LTSVIEW_UI_FILES
  infodock.ui
  mainwindow.ui
  markdock.ui
  markstateruledialog.ui
  savepicturedialog.ui
  savevectordialog.ui
  settingsdialog.ui
  settingsdock.ui
  simdock.ui
)
qt4_wrap_ui(LTSVIEW_UI_HEADERS ${LTSVIEW_UI_FILES})

set(LTSVIEW_MOC_HEADERS
  infodock.h
  ltscanvas.h
  ltsmanager.h
  markdock.h
  mainwindow.h
  markmanager.h
  markstateruledialog.h
  primitivefactory.h
  savepicturedialog.h
  savevectordialog.h
  settingsdialog.h
  settingsdock.h
  simdock.h
  simulation.h
  visualizer.h
)
qt4_wrap_cpp(LTSVIEW_MOC_SOURCES ${LTSVIEW_MOC_HEADERS})

set(LTSVIEW_SRC_FILES
  ${LTSVIEW_RC_FILES}
  ${LTSVIEW_UI_HEADERS}
  ${LTSVIEW_MOC_SOURCES}
  cluster.cpp
  conedb.cpp
  fsm_state_positioner.cpp
  infodock.cpp
  lts.cpp
  ltscanvas.cpp
  ltsmanager.cpp
  main.cpp
  mainwindow.cpp
  markdock.cpp
  markmanager.cpp
  markstateruledialog.cpp
  primitivefactory.cpp
  primitives.cpp
  rtree.cpp
  savepicturedialog.cpp
  savevectordialog.cpp
  settingsdialog.cpp
  settingsdock.cpp
  simdock.cpp
  simulation.cpp
  sp_state_positioner.cpp
  state.cpp
  transition.cpp
  vectors.cpp
  visobjectfactory.cpp
  visualizer.cpp
)

# ######### Create Windows Icon ##########
IF(WIN32)
  include_directories(${CMAKE_SOURCE_DIR}/build/msvc)
  SET(LTSVIEW_SRC_FILES ${LTSVIEW_SRC_FILES} ltsview.rc)
ENDIF(WIN32)

add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${MACOSX_BUNDLE_ICON_FILE} ${LTSVIEW_SRC_FILES})

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  mcrl2_lts
  mcrl2_utilities
  mcrl2_utilities_qt
  mcrl2_syntax
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


# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})
#include_directories(${CMAKE_SOURCE_DIR}/libraries/aterm/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lps/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lts/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/process/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/trace/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)

include_directories(${CMAKE_SOURCE_DIR}/3rd-party/svc/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/tr/include)
if(GL2PS_FOUND)
  include_directories(${GL2PS_INCLUDE_DIRS})
else(GL2PS_FOUND)
  include_directories(${CMAKE_SOURCE_DIR}/3rd-party/gl2ps/include/gl2ps)
endif(GL2PS_FOUND)

# install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_BIN_DIR} COMPONENT Applications)

# Relocate install tree
include(${CMAKE_SOURCE_DIR}/scripts/RelocateInstallTree.cmake)

# Generate man pages
include(${CMAKE_SOURCE_DIR}/scripts/GenerateManPages.cmake)

