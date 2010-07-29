# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(examples_lps)
cmake_minimum_required(VERSION 2.6)

# ######### General setup ##########
set(PREFIX "example_lps")

include_directories(${CMAKE_SOURCE_DIR}/libraries/lps/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include/ )
include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include )
include_directories(${CMAKE_SOURCE_DIR}/libraries/process/include/)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/build/workarounds/)
include_directories(${CMAKE_SOURCE_DIR}/build/precompile)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/aterm/include/aterm/)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/boost/)

##---------------------------------------------------
## Example lpsparse 
##--------------------------------------------------- 

# set(PROJECT_NAME "${PREFIX}_lpsparse")
# project( ${PROJECT_NAME} )

# add_executable("${PROJECT_NAME}" 
#       lpsparse.cpp 
#       mcrl2parser.cpp 
#       mcrl2lexer.cpp
# )

# if( MCRL2_MONO_LIB )
#   target_link_libraries(${PROJECT_NAME}
#     mcrl2_monolib
#   )
# else( MCRL2_MONO_LIB )  
# target_link_libraries(${PROJECT_NAME}
#    mcrl2_lps
#    mcrl2_utilities_command_line
# )
# endif( MCRL2_MONO_LIB )

##---------------------------------------------------
## Example lpsrewr 
##--------------------------------------------------- 

set(PROJECT_NAME "${PREFIX}_lpsrewr")
project( ${PROJECT_NAME} )

add_executable("${PROJECT_NAME}" 
      lpsrewr.cpp 
)

if( MCRL2_MONO_LIB )
  target_link_libraries(${PROJECT_NAME}
    mcrl2_monolib
  )
else( MCRL2_MONO_LIB )  
target_link_libraries(${PROJECT_NAME}
   mcrl2_lps
   mcrl2_utilities_command_line
)
endif( MCRL2_MONO_LIB )
