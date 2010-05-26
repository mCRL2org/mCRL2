# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(mcrl2_trace)
cmake_minimum_required(VERSION 2.6)

# ######### General setup ##########

if( NOT MCRL2_MONO_LIB )
  include_directories(${CMAKE_SOURCE_DIR}/libraries/trace/include)
  include_directories(${CMAKE_SOURCE_DIR}/3rd-party/aterm/include/aterm)
  include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
  include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include)
  include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
  
  
  # add install target:
  install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR})
endif( NOT MCRL2_MONO_LIB )

