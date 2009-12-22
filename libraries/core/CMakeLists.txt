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

set(mcrl2_core_src
  ${CMAKE_CURRENT_SOURCE_DIR}/source/typecheck.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/parse.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/struct_core.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/mcrl2parser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/mcrl2lexer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/regfrmtrans.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/print.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/messaging.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/aterm_ext.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/alpha.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/print_c.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/text_utility.cpp
)

set(mcrl2_core_src ${mcrl2_core_src} PARENT_SCOPE)

if( NOT MONO_LIB)
  include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include )
  include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include )
  include_directories(${CMAKE_SOURCE_DIR}/3rd-party/aterm/include/aterm/)
  include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
  include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
  include_directories(${CMAKE_SOURCE_DIR}/build/workarounds/)
  
  # actual target:
  add_library(${PROJECT_NAME}
    ${mcrl2_core_src}
  )
  
  target_link_libraries(${PROJECT_NAME}
    aterm
  )
  
  # add install target:
  install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR})

endif( NOT MONO_LIB)
