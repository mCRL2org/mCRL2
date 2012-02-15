# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 2.6)

# ######################################
# ######## mcrl2_simulator_gui #########
# ######################################

if (MCRL2_ENABLE_GUI_TOOLS )

project(mcrl2_simulator_gui)

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

add_library(${PROJECT_NAME}
  simulatorgui.cpp
)

target_link_libraries(${PROJECT_NAME}
  mcrl2_lps
  ${wxWidgets_LIBRARIES}
)

# install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR})

endif (MCRL2_ENABLE_GUI_TOOLS )


# #####################################
# ########## Project lpsxsim ##########
# #####################################

if (MCRL2_ENABLE_GUI_TOOLS)

project(lpsxsim)

IF(WIN32)
  include_directories(${CMAKE_SOURCE_DIR}/build/msvc)
  SET(SRCS ${SRCS} lpsxsim.rc)
ENDIF(WIN32)

# ########## Create MacOSX Bundle Information ######### 
include(MCRL2MacOSXBundleInformation)

# ######### General setup ##########

add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE
  lpsxsim.cpp
  lpsxsimmain.cpp
  lpsxsimtrace.cpp
  tooltipListView.cpp

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
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${CMAKE_SOURCE_DIR}/build/precompile)
include_directories(${CMAKE_SOURCE_DIR}/build/workarounds)

#Add PLUGINDIR define
set_source_files_properties(lpsxsimmain.cpp
  PROPERTIES COMPILE_DEFINITIONS PLUGINDIR="${CMAKE_INSTALL_PREFIX}/lib/mcrl2/plugins"
)

#Allow xsim plugins to link against the xsim executable
set_target_properties(${PROJECT_NAME} PROPERTIES ENABLE_EXPORTS true)

target_link_libraries(${PROJECT_NAME}
  ${wxWidgets_LIBRARIES}
  mcrl2_simulator_gui
  mcrl2_utilities
)

# install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_BIN_DIR})

# Relocate install tree
include(${CMAKE_SOURCE_DIR}/scripts/RelocateInstallTree.cmake)

# Generate man pages
include(${CMAKE_SOURCE_DIR}/scripts/GenerateManPages.cmake)

if( NOT WIN32 )
  add_subdirectory ( plugins/trace )
endif( NOT WIN32 )

endif (MCRL2_ENABLE_GUI_TOOLS)
