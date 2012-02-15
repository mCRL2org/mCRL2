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

if ( MCRL2_ENABLE_GUI_TOOLS )

# ########## Create MacOSX Bundle Information ######### 
include(MCRL2MacOSXBundleInformation)

# ######### General setup ##########
if(NOT APPLE)
  configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/share/mcrl2/tool_catalog.xml" "${CMAKE_CURRENT_BINARY_DIR}/share/mcrl2/tool_catalog.xml" COPYONLY )
  configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/share/mcrl2/mcrl2.png" "${CMAKE_CURRENT_BINARY_DIR}/share/mcrl2/mcrl2.png" COPYONLY )
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/share/mcrl2/tool_catalog.xml DESTINATION share/mcrl2)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/share/mcrl2/mcrl2.png DESTINATION share/mcrl2)

  if( UNIX )
		# Install desktop icons			
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/share/icons/mcrl2-gui.png DESTINATION share/icons)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/share/applications/mcrl2-gui.desktop DESTINATION share/applications)
  endif( UNIX )

else(NOT APPLE)
  # Copy directly into application
  configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/share/mcrl2/tool_catalog.xml" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.app/share/mcrl2/tool_catalog.xml" COPYONLY )
  configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/share/mcrl2/mcrl2.png" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.app/share/mcrl2/mcrl2.png" COPYONLY )
endif(NOT APPLE)

# ########## mcrl2_tools_mcrl2 executable ##########

IF(WIN32)
  include_directories(${CMAKE_SOURCE_DIR}/build/msvc)
  SET(SRCS ${SRCS} mcrl2-gui.rc)
ENDIF(WIN32)

add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE
  mcrl2-gui.cpp
  initialization.cpp
  #gui/mainframe.cpp

  ${MACOSX_BUNDLE_ICON_FILE}
  ${SRCS}
)

IF (APPLE)
 SET_SOURCE_FILES_PROPERTIES(${MACOSX_BUNDLE_ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
ENDIF (APPLE)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)

include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include)

target_link_libraries(${PROJECT_NAME}
  ${wxWidgets_LIBRARIES}
  mcrl2_utilities
  mcrl2_utilities_wx
  mcrl2_core
)

# install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_BIN_DIR})

# Relocate install tree
include(${CMAKE_SOURCE_DIR}/scripts/RelocateInstallTree.cmake)

# Generate man pages
include(${CMAKE_SOURCE_DIR}/scripts/GenerateManPages.cmake)

endif ( MCRL2_ENABLE_GUI_TOOLS )
