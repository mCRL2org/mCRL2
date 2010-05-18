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

set(mcrl2_utilities_command_line_src
  ${CMAKE_CURRENT_SOURCE_DIR}/source/command_line_interface.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/toolset_revision.cpp
)

set(mcrl2_utilities_command_line_src ${mcrl2_utilities_command_line_src}  PARENT_SCOPE)

set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/source/toolset_revision.cpp
  PROPERTIES COMPILE_DEFINITIONS MCRL2_REVISION="${SVN_REV} - ${CMAKE_BUILD_TYPE}"
)

if( NOT MCRL2_MONO_LIB )
  include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)

  add_library(${PROJECT_NAME}
    ${mcrl2_utilities_command_line_src}
  )
  
  if(MCRL2_USE_BOOST_INTERNAL)
    target_link_libraries(${PROJECT_NAME}
      boost
    ) 
  else(MCRL2_USE_BOOST_INTERNAL)
    target_link_libraries(${PROJECT_NAME}
      ${Boost_LIBRARIES}
    )
  endif(MCRL2_USE_BOOST_INTERNAL)
  
  install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR})
endif( NOT MCRL2_MONO_LIB )

# ##############################
# ##  mcrl2_utilities_squadt  ##
# ##############################

if(MCRL2_SQUADT_CONNECTIVITY)
  
  project(mcrl2_utilities_squadt)
  cmake_minimum_required(VERSION 2.6)
  
  set(mcrl2_utilities_squadt_src
    ${CMAKE_CURRENT_SOURCE_DIR}/source/squadt_interface.cpp
  )
  
  set(mcrl2_utilities_squadt_src ${mcrl2_utilities_squadt_src} PARENT_SCOPE)  

  if( NOT MCRL2_MONO_LIB )
  
    include_directories(${CMAKE_SOURCE_DIR}/tools/squadt/libraries/tipi/include)
    include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
    
    add_library(${PROJECT_NAME}
      ${mcrl2_utilities_squadt_src}
    )
    
    add_definitions("-DENABLE_MCRL2_SQUADT_CONNECTIVITY")
    
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
  
  endif( NOT MCRL2_MONO_LIB )

endif(MCRL2_SQUADT_CONNECTIVITY)


# ##############################
# ##    mcrl2_utilities_wx    ##
# ##############################

if(MCRL2_BUILD_GUI_TOOLS)

project(mcrl2_utilities_wx)
cmake_minimum_required(VERSION 2.6)

set(mcrl2_utilities_wx_src
  ${CMAKE_CURRENT_SOURCE_DIR}/source/spinctrlfloat.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/colorbutton.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/font_renderer.cpp
)

set(mcrl2_utilities_wx_src ${mcrl2_utilities_wx_src} PARENT_SCOPE)

if( NOT MCRL2_MONO_LIB )

  include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
  include_directories(${CMAKE_SOURCE_DIR}/build/precompile)
  
  add_library(${PROJECT_NAME}
    ${mcrl2_utilities_wx_src}
  )
  
  target_link_libraries(${PROJECT_NAME}
    ${wxWidgets_LIBRARIES}
    ${OPENGL_LIBRARY}
  )
  
  # add install target:
  install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR})

endif( NOT MCRL2_MONO_LIB )

endif(MCRL2_BUILD_GUI_TOOLS)
