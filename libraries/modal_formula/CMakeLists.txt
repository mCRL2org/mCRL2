# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(mcrl2_modal_formula)
cmake_minimum_required(VERSION 2.6)

# Trick to add header files to projects in IDEs like Visual Studio and XCode
FILE(GLOB_RECURSE HEADERS "include" "*.h")
add_custom_target(${PROJECT_NAME}_headers
  SOURCES ${HEADERS})

# ######### General setup ##########

set(mcrl2_modal_formula_src
  ${CMAKE_CURRENT_SOURCE_DIR}/source/modal_formula.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/regfrmtrans.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/typecheck.cpp
)
set(mcrl2_modal_formula_src ${mcrl2_modal_formula_src} PARENT_SCOPE)

add_library(${PROJECT_NAME}
  ${mcrl2_modal_formula_src}
)

target_link_libraries(${PROJECT_NAME}
  mcrl2_data
  mcrl2_lps
  mcrl2_process
  mcrl2_core
)

# add install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR} COMPONENT Libraries)
