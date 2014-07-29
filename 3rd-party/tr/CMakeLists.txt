# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(tr)
cmake_minimum_required(VERSION 2.6)

# ######### General setup ##########

# Trick to add header files to projects in IDEs like Visual Studio and XCode
FILE(GLOB HEADERS "include/tr/*.h")
add_custom_target(${PROJECT_NAME}_headers
  SOURCES ${HEADERS} ${DETAIL_HEADERS}
)

# actual target:
add_library(${PROJECT_NAME}
  source/tr.c
)

target_link_libraries(${PROJECT_NAME}
  ${OPENGL_LIBRARY}
)

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/3rd-party/tr/include/tr)

# add install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR} COMPONENT Libraries)
