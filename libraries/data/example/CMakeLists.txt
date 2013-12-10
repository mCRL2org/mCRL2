# Authors: Frank Stappers 
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(examples_data)
cmake_minimum_required(VERSION 2.6)

# ######### General setup ##########
set(PREFIX "example_data")

##---------------------------------------------------
## Example ABC 
##--------------------------------------------------- 

# set(PROJECT_NAME "${PREFIX}_ABC")
# project( ${PROJECT_NAME} )
# 
# add_executable("${PROJECT_NAME}" 
#   ABC.cpp 
#   ABC_parser.cpp 
#   ABC_lexer.cpp
# )
# 
# target_link_libraries(${PROJECT_NAME}
#    mcrl2_lps
#    mcrl2_utilities
# )

##---------------------------------------------------
## Example enumerator 
##--------------------------------------------------- 

set(PROJECT_NAME "${PREFIX}_enumerator")
project( ${PROJECT_NAME} )

add_executable("${PROJECT_NAME}" EXCLUDE_FROM_ALL
      enumerator.cpp
)

target_link_libraries(${PROJECT_NAME}
   mcrl2_data
)

build_and_run_test_example_target( ${PROJECT_NAME} )

##---------------------------------------------------
## Example example[1..5]
##--------------------------------------------------- 

file(GLOB TERM_OBJS "example*.cpp")

foreach( OBJ ${TERM_OBJS} )
  get_filename_component(result "${OBJ}" NAME_WE)
  set(PROJECT_NAME "${PREFIX}_${result}")
  project( ${PROJECT_NAME} )

  add_executable("${PROJECT_NAME}" EXCLUDE_FROM_ALL 
        ${OBJ} 
  )
  
  target_link_libraries(${PROJECT_NAME}
     mcrl2_data
  )

  build_and_run_test_example_target( ${PROJECT_NAME} )
  
endforeach( OBJ )

##---------------------------------------------------
## Example parse_data 
##--------------------------------------------------- 

set(PROJECT_NAME "${PREFIX}_parse_data")
project( ${PROJECT_NAME} )

add_executable("${PROJECT_NAME}" EXCLUDE_FROM_ALL 
      parse_data.cpp 
)

target_link_libraries(${PROJECT_NAME}
   mcrl2_data
)

build_and_run_test_example_target( ${PROJECT_NAME} )

