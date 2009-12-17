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

add_library(${PROJECT_NAME}
  #the following files are all included by the file implementations.cpp:
  #source/basic_datatype.cpp
  #source/command_line_interface.cpp
  #source/common.cpp
  #source/configuration.cpp
  #source/display.cpp
  #source/mime_type.cpp
  #source/restore_visitor.cpp
  #source/store_visitor.cpp
  #source/detail/event_handlers.cpp
  #source/transport/socket_listener.cpp
  #source/transport/socket_transceiver.cpp
  #source/transport/transporter.cpp
  #source/controller/capabilities.cpp
  #source/tool/capabilities.cpp
  source/implementations.cpp
  source/controller/communicator.cpp
  source/tool/communicator.cpp

)

include_directories(${CMAKE_SOURCE_DIR}/3rd-party/ticpp/include)
include_directories(${CMAKE_SOURCE_DIR}/build/precompile)
include_directories(${CMAKE_SOURCE_DIR}/build/workarounds)
include_directories(${CMAKE_SOURCE_DIR}/tools/squadt/libraries/tipi/include)

target_link_libraries(${PROJECT_NAME}
  ticpp
)

if(MINGW OR CYGWIN OR MSYS)
  target_link_libraries(${PROJECT_NAME}
    wsock32
    ws2_32)
endif(MINGW OR CYGWIN OR MSYS)

if(USE_BOOST_INTERNAL)
  target_link_libraries(${PROJECT_NAME}
    boost
  ) 
else(USE_BOOST_INTERNAL)
  target_link_libraries(${PROJECT_NAME}
    ${Boost_LIBRARIES}     
  ) 
endif(USE_BOOST_INTERNAL)

# add install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR})
