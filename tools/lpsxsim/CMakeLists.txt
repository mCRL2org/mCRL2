# Author: Ruud Koolen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(lpsxsim)
cmake_minimum_required(VERSION 2.6)

# ########## Create MacOSX Bundle Information ######### 
include(MCRL2MacOSXBundleInformation)
IF (APPLE)
 SET_SOURCE_FILES_PROPERTIES(${MACOSX_BUNDLE_ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
ENDIF (APPLE)

# ######### General setup ##########
set(LPSXSIM_RCC_SRCS lpsxsim.qrc)

qt4_add_resources(LPSXSIM_RC_FILES ${LPSXSIM_RCC_SRCS})

set(LPSXSIM_UI_FILES
  mainwindow.ui
)
qt4_wrap_ui(LPSXSIM_UI_HEADERS ${LPSXSIM_UI_FILES})

set(LPSXSIM_MOC_HEADERS
  mainwindow.h
  simulation.h
)
qt4_wrap_cpp(LPSXSIM_MOC_SOURCES ${LPSXSIM_MOC_HEADERS})

set(LPSXSIM_SRC_FILES
  ${LPSXSIM_RC_FILES}
  ${LPSXSIM_UI_HEADERS}
  ${LPSXSIM_MOC_SOURCES}
  main.cpp
  mainwindow.cpp
  simulation.cpp
)

# ######### Create Windows Icon ##########
IF(WIN32)
  include_directories(${CMAKE_SOURCE_DIR}/build/msvc)
  SET(LPSXSIM_SRC_FILES ${LPSXSIM_SRC_FILES} lpsxsim.rc)
ENDIF(WIN32)

add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${MACOSX_BUNDLE_ICON_FILE} ${LPSXSIM_SRC_FILES})

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

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})
# For promoted widgets
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/libraries/aterm/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lps/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lts/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/process/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/trace/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)

# install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_BIN_DIR} COMPONENT Runtime)

# Relocate install tree
include(${CMAKE_SOURCE_DIR}/scripts/RelocateInstallTree.cmake)

# Generate man pages
include(${CMAKE_SOURCE_DIR}/scripts/GenerateManPages.cmake)

