# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(squadt_tipi)
cmake_minimum_required(VERSION 2.6)

# ######### General setup ##########
add_definitions(-DTIXML_USE_TICPP)

set(squadt_tipi_src
  ${CMAKE_CURRENT_SOURCE_DIR}/source/implementations.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/controller/communicator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/tool/communicator.cpp
)
set(squadt_tipi_src ${squadt_tipi_src} PARENT_SCOPE)

if (NOT MCRL2_MONO_LIB )

  include_directories(${CMAKE_SOURCE_DIR}/3rd-party/ticpp/include)
  include_directories(${CMAKE_SOURCE_DIR}/build/precompile)
  include_directories(${CMAKE_SOURCE_DIR}/build/workarounds)
  include_directories(${CMAKE_SOURCE_DIR}/tools/squadt/libraries/tipi/include)

  add_library(${PROJECT_NAME}
    ${squadt_tipi_src}  
  )

  target_link_libraries(${PROJECT_NAME}
    ticpp
  )
  
  if(MINGW OR CYGWIN OR MSYS)
    target_link_libraries(${PROJECT_NAME}
      wsock32
      ws2_32)
  endif(MINGW OR CYGWIN OR MSYS)
  
  if(MCRL2_USE_BOOST_INTERNAL)
    target_link_libraries(${PROJECT_NAME}
      boost
    ) 
  else(MCRL2_USE_BOOST_INTERNAL)
    target_link_libraries(${PROJECT_NAME}
      ${Boost_LIBRARIES}     
    ) 
  endif(MCRL2_USE_BOOST_INTERNAL)
  
  # add install target:
  install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR})

endif(NOT MCRL2_MONO_LIB)
