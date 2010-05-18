# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(mcrl2_monolib)
cmake_minimum_required(VERSION 2.6)

# ######### General setup ##########
include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include )
include_directories(${CMAKE_SOURCE_DIR}/libraries/trace/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lps/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/pbes/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lts/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include/ )
include_directories(${CMAKE_SOURCE_DIR}/libraries/process/include/)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/build/workarounds/)
include_directories(${CMAKE_SOURCE_DIR}/build/precompile)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/aterm/include/aterm/)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/ticpp/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/svc/include)
include_directories(${CMAKE_SOURCE_DIR}/tools/squadt/libraries/tipi/include/)
#The following include directory is required by out-of-source builds for finding setup.h
include_directories(${CMAKE_BINARY_DIR}/libraries/utilities/include)

if(MCRL2_SQUADT_CONNECTIVITY)
  add_definitions("-DENABLE_MCRL2_SQUADT_CONNECTIVITY")

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
endif(MCRL2_SQUADT_CONNECTIVITY)

if(MCRL2_BUILD_GUI_TOOLS)
  set(MCRL2_SIMULATOR_GUI
    ${CMAKE_SOURCE_DIR}/tools/lpsxsim/simulatorgui.cpp
  )
endif(MCRL2_BUILD_GUI_TOOLS)

add_library( ${PROJECT_NAME}
  ${mcrl2_core_src}
  ${mcrl2_data_src}
  ${mcrl2_utilities_squadt_src}
  ${mcrl2_utilities_wx_src}
  ${mcrl2_lps_src}
  ${mcrl2_lts_src}
  ${mcrl2_trace_src}
  ${mcrl2_process_src}
  ${squadt_tipi_src}
  ${mcrl2_utilities_command_line_src}
  ${CMAKE_SOURCE_DIR}/tools/lpsxsim/simulator.cpp
  ${CMAKE_SOURCE_DIR}/tools/lpsxsim/simbase.cpp
  ${MCRL2_SIMULATOR_GUI}
)

set_source_files_properties(${CMAKE_SOURCE_DIR}/libraries/utilities/source/toolset_revision.cpp
  PROPERTIES COMPILE_DEFINITIONS MCRL2_REVISION="${SVN_REV} - ${CMAKE_BUILD_TYPE}"
)

target_link_libraries(${PROJECT_NAME}
  svc 
  ticpp
  aterm
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


if(MCRL2_BUILD_GUI_TOOLS)
  target_link_libraries(${PROJECT_NAME}
    ${wxWidgets_LIBRARIES}
    ${OPENGL_LIBRARY}
  )
endif(MCRL2_BUILD_GUI_TOOLS)


install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR})
