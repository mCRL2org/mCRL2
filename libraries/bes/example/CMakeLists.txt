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
set(PREFIX "example_bes")

include_directories(${CMAKE_SOURCE_DIR}/libraries/bes/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/pbes/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lps/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lts/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/trace/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include/ )
include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include )
include_directories(${CMAKE_SOURCE_DIR}/libraries/process/include/)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/build/workarounds/)
include_directories(${CMAKE_SOURCE_DIR}/build/precompile)
include_directories(${CMAKE_SOURCE_DIR}/libraries/aterm/include/)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/dparser)

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

