# Author: Ruud Koolen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(lpsxsim-qt)
cmake_minimum_required(VERSION 2.6)

if (MCRL2_ENABLE_GUI_TOOLS)

find_package(Qt4 4.6 COMPONENTS QtCore QtGui)
if (QT4_FOUND)
include(${QT_USE_FILE})

# ########## Create MacOSX Bundle Information ######### 
include(MCRL2MacOSXBundleInformation)

# ######### General setup ##########
IF(WIN32)
  include_directories(${CMAKE_SOURCE_DIR}/build/msvc)
  SET(SRCS ${SRCS} lpsxsim.rc)
ENDIF(WIN32)

set(UI_FILES
  mainwindow.ui
)
qt4_wrap_ui(UI_HEADERS ${UI_FILES})

set(MOC_HEADERS
  mainwindow.h
  simulation.h
)
qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})

set(SRC_FILES
  ${UI_HEADERS}
  ${MOC_SOURCES}
  main.cpp
  mainwindow.cpp
  simulation.cpp
  ${MACOSX_BUNDLE_ICON_FILE}
)

IF (APPLE)
 SET_SOURCE_FILES_PROPERTIES(${MACOSX_BUNDLE_ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
ENDIF (APPLE)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/libraries/aterm/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lps/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lts/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/process/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/trace/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
include_directories(${CMAKE_SOURCE_DIR}/build/workarounds)

add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${SRC_FILES})

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  mcrl2_aterm
  mcrl2_core
  mcrl2_data
  mcrl2_lps
  mcrl2_lts
  mcrl2_utilities
  mcrl2_utilities_qt
)

# install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_BIN_DIR})

# Relocate install tree
include(${CMAKE_SOURCE_DIR}/scripts/RelocateInstallTree.cmake)

# Generate man pages
include(${CMAKE_SOURCE_DIR}/scripts/GenerateManPages.cmake)

else(QT4_FOUND)
  message(STATUS "Could NOT find Qt4, not building lpsxsim-qt.")
endif(QT4_FOUND)
endif ( MCRL2_ENABLE_GUI_TOOLS )
