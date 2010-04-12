# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(examples_utilities)
cmake_minimum_required(VERSION 2.6)

# ######### General setup ##########
set(PREFIX "example_utilities")

include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include/ )
include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include )
include_directories(${CMAKE_SOURCE_DIR}/build/workarounds/)
include_directories(${CMAKE_SOURCE_DIR}/build/precompile)

##---------------------------------------------------
## Example lpsrewr 
##--------------------------------------------------- 

set(PROJECT_NAME "${PREFIX}_cmdline1")
project( ${PROJECT_NAME} )

add_executable("${PROJECT_NAME}" 
      cmdline1.cpp 
)

if( MONO_LIB )
  target_link_libraries(${PROJECT_NAME}
    mcrl2_monolib
  )
else( MONO_LIB )  
target_link_libraries(${PROJECT_NAME}
   mcrl2_utilities_command_line
   mcrl2_core
)
endif( MONO_LIB )
