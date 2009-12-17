# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(gl2ps)
cmake_minimum_required(VERSION 2.6)

# ######### General setup ##########
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/gl2ps/include/gl2ps)

# actual target:
add_library(${PROJECT_NAME}
  source/gl2ps.c  
)

target_link_libraries(${PROJECT_NAME}
  ${OPENGL_LIBRARY}
)

# add install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR})
