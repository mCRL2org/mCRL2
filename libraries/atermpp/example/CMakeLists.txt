# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(examples_atermpp_)
cmake_minimum_required(VERSION 2.6)

# ######### General setup ##########
set(PREFIX "example_atermpp")

include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/aterm/include/)
include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)

##---------------------------------------------------
## Example algorithm 
##--------------------------------------------------- 

set(PROJECT_NAME "${PREFIX}_algorithm")
project( ${PROJECT_NAME} )

add_executable("${PROJECT_NAME}" EXCLUDE_FROM_ALL 
      algorithm.cpp 
)

target_link_libraries(${PROJECT_NAME}
   mcrl2_aterm 
)

build_and_run_test_example_target( ${PROJECT_NAME} )
