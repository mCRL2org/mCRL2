# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ##################################
# ## mcrl2_utilities_command_line ##
# ##################################

project(mcrl2_utilities_command_line)
cmake_minimum_required(VERSION 2.6)

add_library(${PROJECT_NAME}
  source/command_line_interface.cpp
  source/toolset_revision.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)

set_source_files_properties(source/toolset_revision.cpp
  PROPERTIES COMPILE_DEFINITIONS MCRL2_REVISION="${SVN_REV} - ${CMAKE_BUILD_TYPE}"
)

if(USE_BOOST_INTERNAL)
  target_link_libraries(${PROJECT_NAME}
    boost
  ) 
else(USE_BOOST_INTERNAL)
  target_link_libraries(${PROJECT_NAME}
    ${Boost_LIBRARIES}
  )
endif(USE_BOOST_INTERNAL)

install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR})


# ##############################
# ##  mcrl2_utilities_squadt  ##
# ##############################

if(SQUADT_CONNECTIVITY)
  
project(mcrl2_utilities_squadt)
cmake_minimum_required(VERSION 2.6)

add_library(${PROJECT_NAME}
  source/squadt_interface.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/tools/squadt/libraries/tipi/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)

add_definitions("-DENABLE_SQUADT_CONNECTIVITY")

target_link_libraries(${PROJECT_NAME}
  squadt_tipi
)

if(MINGW)
  target_link_libraries(${PROJECT_NAME}
    wsock32
    ws2_32
  )
endif(MINGW)   

if ( APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "-framework Carbon ${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS "-framework Carbon ${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS "-framework Carbon ${CMAKE_SHARED_LINKER_FLAGS}")
endif (APPLE)

# add install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR})

endif(SQUADT_CONNECTIVITY)


# ##############################
# ##    mcrl2_utilities_wx    ##
# ##############################

if(MCRL2_BUILD_GUI_TOOLS)

project(mcrl2_utilities_wx)
cmake_minimum_required(VERSION 2.6)

add_library(${PROJECT_NAME}
  source/spinctrlfloat.cpp
  source/colorbutton.cpp
  source/font_renderer.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
include_directories(${CMAKE_SOURCE_DIR}/build/precompile)

target_link_libraries(${PROJECT_NAME}
  ${wxWidgets_LIBRARIES}
  ${OPENGL_LIBRARY}
)

# add install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR})

endif(MCRL2_BUILD_GUI_TOOLS)
