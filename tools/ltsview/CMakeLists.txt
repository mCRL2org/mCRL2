# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(ltsview)
cmake_minimum_required(VERSION 2.6)

if ( MCRL2_ENABLE_GUI_TOOLS )

# ########## Create MacOSX Bundle Information ######### 
include(MCRL2MacOSXBundleInformation)

# ######### General setup ##########
IF(WIN32)
  include_directories(${CMAKE_SOURCE_DIR}/build/msvc)
  SET(SRCS ${SRCS} ltsview.rc)
ENDIF(WIN32)

# Sources:
add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE
  cluster.cpp
  color_interpolator.cpp
  conedb.cpp
  fsm_state_positioner.cpp
  glcanvas.cpp
  infodialog.cpp
  lts.cpp
  ltsview.cpp
  mainframe.cpp
  markdialog.cpp
  markmanager.cpp
  markstateruledialog.cpp
  mathutils.cpp
  primitivefactory.cpp
  primitives.cpp
  rgb_color.cpp
  rtree.cpp
  savepicdialog.cpp
  savevecdialog.cpp
  settings.cpp
  settingsdialog.cpp
  simdialog.cpp
  simreader.cpp
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
include_directories(${CMAKE_SOURCE_DIR}/build/workarounds)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/dparser)
if(GL2PS_FOUND)
  include_directories(${GL2PS_INCLUDE_DIRS})
else(GL2PS_FOUND)
  include_directories(${CMAKE_SOURCE_DIR}/3rd-party/gl2ps/include/gl2ps)
endif(GL2PS_FOUND)

# actual target:
target_link_libraries(${PROJECT_NAME}
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

endif ( MCRL2_ENABLE_GUI_TOOLS )
