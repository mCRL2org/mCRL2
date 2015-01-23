# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

project(gl2ps)
cmake_minimum_required(VERSION 2.6)

find_package(OpenGL QUIET REQUIRED)
find_package(mlib   QUIET)

include_directories(include)

add_custom_target(${PROJECT_NAME}_headers SOURCES include/gl2ps.h)
add_library(${PROJECT_NAME} STATIC source/gl2ps.c)

target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARY})
if(mlib_FOUND)
  target_link_libraries(${PROJECT_NAME} ${MLIB_LIBRARY})
endif(mlib_FOUND)

