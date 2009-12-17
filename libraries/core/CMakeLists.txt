# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(mcrl2_core)
cmake_minimum_required(VERSION 2.6)

# ######### Set policies ############

# ################################### 
# "core" links to some transitive 
# libraries for which the linker must
# search:
#   wxWidgets
# Therefore CMP0003 is set to NEW
# If not set, a BUILD_SHARED_LIB=TRUE
# combined with a wxWidgets build 
# raises a warning
# ###################################

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# ######### General setup ##########

# actual target:
add_library(${PROJECT_NAME}
  source/typecheck.cpp
  source/parse.cpp
  source/struct_core.cpp
  source/mcrl2parser.cpp
  source/mcrl2lexer.cpp
  source/regfrmtrans.cpp
  source/print.cpp
  source/messaging.cpp
  source/aterm_ext.cpp
  source/numeric_string.cpp
  source/alpha.cpp
  source/print_c.cpp
  source/struct.cpp
  source/text_utility.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include )
include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include )
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/aterm/include/aterm/)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
include_directories(${CMAKE_SOURCE_DIR}/build/workarounds/)

target_link_libraries(${PROJECT_NAME}
  aterm
)

# add install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR})

