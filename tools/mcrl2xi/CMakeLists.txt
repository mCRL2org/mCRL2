# Authors: Frank Stappers, Aad Mathijssen and Rimco Boudewijns
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(mcrl2xi)
cmake_minimum_required(VERSION 2.6)

# ########## Create MacOSX Bundle Information #########
include(MCRL2MacOSXBundleInformation)
IF (APPLE)
 SET_SOURCE_FILES_PROPERTIES(${MACOSX_BUNDLE_ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
ENDIF (APPLE)

# ######### General setup ##########
set(MCRL2XI_RCC_SRCS mcrl2xi.qrc)

qt4_add_resources(MCRL2XI_RC_FILES ${MCRL2XI_RCC_SRCS})

set(MCRL2XI_UI_FILES
  mainwindow.ui
  findreplacedialog.ui
)

qt4_wrap_ui(MCRL2XI_UI_HEADERS ${MCRL2XI_UI_FILES})

set(MCRL2XI_MOC_HEADERS
  mainwindow.h
  highlighter.h
  documentmanager.h
  documentwidget.h
  parser.h
  rewriter.h
  solver.h
  findreplacedialog.h
)
qt4_wrap_cpp(MCRL2XI_MOC_SOURCES ${MCRL2XI_MOC_HEADERS})

set(MCRL2XI_SRC_FILES
  ${MCRL2XI_RC_FILES}
  ${MCRL2XI_UI_HEADERS}
  ${MCRL2XI_MOC_SOURCES}
  main.cpp
  mainwindow.cpp
  highlighter.cpp
  documentmanager.cpp
  documentwidget.cpp
  parser.cpp
  rewriter.cpp
  solver.cpp
  findreplacedialog.cpp

)


# ######### Create Windows Icon ##########
if(WIN32)
  include_directories(${CMAKE_SOURCE_DIR}/build/msvc)
  set(MCRL2XI_SRC_FILES ${MCRL2XI_SRC_FILES} mcrl2xi.rc)
endif(WIN32)

add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${MACOSX_BUNDLE_ICON_FILE} ${MCRL2XI_SRC_FILES})

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
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
include_directories(${CMAKE_SOURCE_DIR}/libraries/aterm/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/bes/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lps/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/pbes/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/process/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/dparser)

include_directories(${CMAKE_SOURCE_DIR}/build/precompile)

# install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_BIN_DIR} COMPONENT Runtime)

# Relocate install tree
include(${CMAKE_SOURCE_DIR}/scripts/RelocateInstallTree.cmake)

# Generate man pages
include(${CMAKE_SOURCE_DIR}/scripts/GenerateManPages.cmake)

