# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 2.6)

# #############################################
# ########## Project mcrl2_simulator ##########
# #############################################

if( NOT MONO_LIB )

project(mcrl2_simulator)

include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lps/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lts/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/trace/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/aterm/include/aterm)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/svc/include)

include_directories(${CMAKE_SOURCE_DIR}/build/precompile)
include_directories(${CMAKE_SOURCE_DIR}/build/workarounds)

add_library(${PROJECT_NAME}
  simulator.cpp
  simbase.cpp 
)

target_link_libraries(${PROJECT_NAME}
  mcrl2_lps
  mcrl2_trace
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR})

endif( NOT MONO_LIB )


# ####################################
# ########## Project lpssim ##########
# ####################################

project(lpssim)

add_executable(${PROJECT_NAME}
  sim.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lps/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/trace/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/aterm/include/aterm)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/svc/include)
include_directories(${CMAKE_SOURCE_DIR}/build/precompile)
include_directories(${CMAKE_SOURCE_DIR}/build/workarounds)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if( MONO_LIB )
target_link_libraries(${PROJECT_NAME}
  mcrl2_monolib
)
else( MONO_LIB )
target_link_libraries(${PROJECT_NAME}
  mcrl2_simulator
  mcrl2_utilities_command_line
  ${Boost_LIBRARIES}
)
endif( MONO_LIB )

# add install target:
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# Generate man pages
include(${CMAKE_SOURCE_DIR}/scripts/GenerateManPages.cmake)

# ######################################
# ######## mcrl2_simulator_gui #########
# ######################################

if (MCRL2_BUILD_GUI_TOOLS AND NOT MONO_LIB)

project(mcrl2_simulator_gui)

include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lps/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lts/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/trace/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/aterm/include/aterm)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/svc/include)

include_directories(${CMAKE_SOURCE_DIR}/build/precompile)
include_directories(${CMAKE_SOURCE_DIR}/build/workarounds)

add_library(${PROJECT_NAME}
  simulatorgui.cpp
)

target_link_libraries(${PROJECT_NAME}
  mcrl2_simulator
  ${wxWidgets_LIBRARIES}
)

# install target:
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# Relocate install tree
include(${CMAKE_SOURCE_DIR}/scripts/RelocateInstallTree.cmake)

endif (MCRL2_BUILD_GUI_TOOLS AND NOT MONO_LIB)


# #####################################
# ########## Project lpsxsim ##########
# #####################################

if (MCRL2_BUILD_GUI_TOOLS)

project(lpsxsim)

# ######### General setup ##########

add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE
  xsim.cpp
  xsimmain.cpp
  xsimtrace.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lps/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lts/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/trace/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/aterm/include/aterm)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/svc/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${CMAKE_SOURCE_DIR}/build/precompile)
include_directories(${CMAKE_SOURCE_DIR}/build/workarounds)

#Add PLUGINDIR define
set_source_files_properties(xsimmain.cpp
  PROPERTIES COMPILE_DEFINITIONS PLUGINDIR="${CMAKE_INSTALL_PREFIX}/lib/mcrl2/plugins"
)

#Allow xsim plugins to link against the xsim executable
set_target_properties(${PROJECT_NAME} PROPERTIES ENABLE_EXPORTS true)

if( MONO_LIB )
  target_link_libraries(${PROJECT_NAME}
    mcrl2_monolib
  )

  # ######### required for squadt connectivity ##########
  if(SQUADT_CONNECTIVITY)
    add_definitions("-DENABLE_SQUADT_CONNECTIVITY")
    include_directories(${CMAKE_SOURCE_DIR}/tools/squadt/libraries/tipi/include)
  endif(SQUADT_CONNECTIVITY)
  # #####################################################

else( MONO_LIB )
target_link_libraries(${PROJECT_NAME}
  ${wxWidgets_LIBRARIES}
  mcrl2_simulator_gui
  mcrl2_utilities_command_line
)

  #Add Squadt Connectivity when required
  include(${CMAKE_SOURCE_DIR}/scripts/SquadtConnectivity.cmake)

endif( MONO_LIB )

# install target:
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# Relocate install tree
include(${CMAKE_SOURCE_DIR}/scripts/RelocateInstallTree.cmake)

# Generate man pages
include(${CMAKE_SOURCE_DIR}/scripts/GenerateManPages.cmake)

if( NOT WIN32 )
  add_subdirectory ( plugins/garage )
  add_subdirectory ( plugins/trace )
endif( NOT WIN32 )

endif (MCRL2_BUILD_GUI_TOOLS)
