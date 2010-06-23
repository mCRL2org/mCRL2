# Authors: Frank Stappers 
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(mcrl2-gui)
cmake_minimum_required(VERSION 2.6)

if ( MCRL2_BUILD_GUI_TOOLS )

add_definitions(-DTIXML_USE_TICPP)

# ######### General setup ##########

#set(MACOSX_BUNDLE_ICON_FILE mcrl2-gui.icns)

configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/share/mcrl2/tool_catalog.xml" "${CMAKE_CURRENT_BINARY_DIR}/share/mcrl2/tool_catalog.xml" COPYONLY )
if(MCRL2_SINGLE_BUNDLE)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/share/mcrl2/tool_catalog.xml DESTINATION MacOS/share/mcrl2)
else(MCRL2_SINGLE_BUNDLE)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/share/mcrl2/tool_catalog.xml DESTINATION share/mcrl2)
endif(MCRL2_SINGLE_BUNDLE)

configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/share/mcrl2/mcrl2.png" "${CMAKE_CURRENT_BINARY_DIR}/share/mcrl2/mcrl2.png" COPYONLY )
if(MCRL2_SINGLE_BUNDLE)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/share/mcrl2/mcrl2.png DESTINATION MacOS/share/mcrl2)
else(MCRL2_SINGLE_BUNDLE)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/share/mcrl2/mcrl2.png DESTINATION share/mcrl2)
endif(MCRL2_SINGLE_BUNDLE)

# ########## mcrl2_tools_mcrl2 executable ##########

add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE
	mcrl2-gui.cpp
    initialization.cpp
	#gui/mainframe.cpp

  ${MACOSX_BUNDLE_ICON_FILE}
)

IF (APPLE)
 SET_SOURCE_FILES_PROPERTIES(${MACOSX_BUNDLE_ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
ENDIF (APPLE)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/ticpp/include)

if( MCRL2_MONO_LIB )
  target_link_libraries(${PROJECT_NAME}
    mcrl2_monolib
  )
else( MCRL2_MONO_LIB )
target_link_libraries(${PROJECT_NAME}
  ${wxWidgets_LIBRARIES}
  ticpp
)
endif( MCRL2_MONO_LIB )

# install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_BIN_DIR})

# Relocate install tree
include(${CMAKE_SOURCE_DIR}/scripts/RelocateInstallTree.cmake)

# Generate man pages
include(${CMAKE_SOURCE_DIR}/scripts/GenerateManPages.cmake)

endif ( MCRL2_BUILD_GUI_TOOLS )
