# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(ltsgraph)
cmake_minimum_required(VERSION 2.6)

if ( MCRL2_BUILD_GUI_TOOLS )
add_definitions(-DTIXML_USE_TICPP)

set(MACOSX_BUNDLE_ICON_FILE ltsgraph.icns )
# ######### General setup ##########

# actual target:
add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE
  visualizer.cpp
  glcanvas.cpp
  ltsgraph.cpp
  ltsimporter.cpp
  state.cpp
  export_svg.cpp
  infodialog.cpp
  export_xml.cpp
  xmlimporter.cpp
  utils.cpp
  graph.cpp
  transition.cpp
  export_latex.cpp
  settingsdialog.cpp
  springlayout.cpp
  mainframe.cpp
  algodialog.cpp
  exporter.cpp
  importer.cpp

  ${MACOSX_BUNDLE_ICON_FILE}
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
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/aterm/include/aterm)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/svc/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/ticpp/include)
include_directories(${CMAKE_SOURCE_DIR}/build/precompile)
include_directories(${CMAKE_SOURCE_DIR}/build/workarounds)

if( MCRL2_MONO_LIB )
  target_link_libraries(${PROJECT_NAME}
    mcrl2_monolib
  )

  # ######### required for squadt connectivity ##########
  if(MCRL2_SQUADT_CONNECTIVITY)
    add_definitions("-DENABLE_SQUADT_CONNECTIVITY")
    include_directories(${CMAKE_SOURCE_DIR}/tools/squadt/libraries/tipi/include)
  endif(MCRL2_SQUADT_CONNECTIVITY)
  # #####################################################

else( MCRL2_MONO_LIB )
  target_link_libraries(${PROJECT_NAME}
    ${wxWidgets_LIBRARIES}
    mcrl2_lts
    mcrl2_utilities_command_line
    mcrl2_utilities_wx
    ticpp
  )

  #Add Squadt Connectivity when required
  include(${CMAKE_SOURCE_DIR}/scripts/SquadtConnectivity.cmake)

endif( MCRL2_MONO_LIB )

# install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_BIN_DIR})

# Relocate install tree
include(${CMAKE_SOURCE_DIR}/scripts/RelocateInstallTree.cmake)

# Generate man pages
include(${CMAKE_SOURCE_DIR}/scripts/GenerateManPages.cmake)

endif ( MCRL2_BUILD_GUI_TOOLS )
