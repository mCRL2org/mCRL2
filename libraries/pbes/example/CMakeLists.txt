# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(examples_pbes)
cmake_minimum_required(VERSION 2.6)

# ######### General setup ##########
set(PREFIX "example_pbes")

##---------------------------------------------------
## Example *.cpp 
##--------------------------------------------------- 

file(GLOB TERM_OBJS "*.cpp")

foreach( OBJ ${TERM_OBJS} )

  get_filename_component(result "${OBJ}" NAME_WE)
  set(PROJECT_NAME "${PREFIX}_${result}")
  project( ${PROJECT_NAME} )

  add_executable("${PROJECT_NAME}" EXCLUDE_FROM_ALL 
        ${OBJ} 
  )
  
  target_link_libraries(${PROJECT_NAME}
     mcrl2_lps
     mcrl2_lts
     mcrl2_utilities
     mcrl2_pbes
  )

  build_and_run_test_example_target( ${PROJECT_NAME} )

endforeach( OBJ )

