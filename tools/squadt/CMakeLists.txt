# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(squadt)
cmake_minimum_required(VERSION 2.6)

if ( MCRL2_BUILD_GUI_TOOLS )

add_definitions(-DTIXML_USE_TICPP)

# ######### General setup ##########

configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/share/squadt/tool_catalog" "${CMAKE_CURRENT_BINARY_DIR}/share/squadt/tool_catalog" COPYONLY )
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/share/squadt/tool_catalog DESTINATION share/squadt)

# ########## mcrl2_tools_squadt executable ##########

add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE
  build_system.cpp
  command.cpp
  executor.cpp
  process.cpp
  processor.cpp
  project_manager.cpp
  restore_visitor.cpp
  settings_manager.cpp
  squadt.cpp
  store_visitor.cpp
  task_monitor.cpp
  tool_manager.cpp
  type_registry.cpp

  gui/main.cpp
  gui/project.cpp
  gui/resources.cpp
  gui/tool_display.cpp

  gui/dialog/base.cpp
  gui/dialog/preferences.cpp
  gui/dialog/processor_settings.cpp
)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
include_directories(${CMAKE_SOURCE_DIR}/tools/squadt/libraries/tipi/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/ticpp/include)
include_directories(${CMAKE_SOURCE_DIR}/build/precompile)
include_directories(${CMAKE_SOURCE_DIR}/build/workarounds)

if( MONO_LIB )
  target_link_libraries(${PROJECT_NAME}
    mcrl2_monolib
  )
else( MONO_LIB )
target_link_libraries(${PROJECT_NAME}
  ${wxWidgets_LIBRARIES}
  mcrl2_utilities_command_line
  mcrl2_utilities_wx
  squadt_tipi
  ticpp
)
endif( MONO_LIB )

# install target:
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# Relocate install tree
include(${CMAKE_SOURCE_DIR}/scripts/RelocateInstallTree.cmake)

# Generate man pages
include(${CMAKE_SOURCE_DIR}/scripts/GenerateManPages.cmake)

endif ( MCRL2_BUILD_GUI_TOOLS )
