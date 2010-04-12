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

include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include/ )
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/aterm/include/aterm/)

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
# if( MONO_LIB )
#   target_link_libraries(${PROJECT_NAME}
#     mcrl2_monolib
#   )
# else( MONO_LIB )  
# target_link_libraries(${PROJECT_NAME}
#    mcrl2_lps
#    mcrl2_utilities_command_line
# )
# endif( MONO_LIB )

##---------------------------------------------------
## Example enumerator 
##--------------------------------------------------- 

set(PROJECT_NAME "${PREFIX}_enumerator")
project( ${PROJECT_NAME} )

add_executable("${PROJECT_NAME}" 
      enumerator.cpp 
)

if( MONO_LIB )
  target_link_libraries(${PROJECT_NAME}
    mcrl2_monolib
  )
else( MONO_LIB )  
target_link_libraries(${PROJECT_NAME}
   mcrl2_data
)

endif( MONO_LIB )

##---------------------------------------------------
## Example example[1..5]
##--------------------------------------------------- 

file(GLOB TERM_OBJS "example*.cpp")

foreach( OBJ ${TERM_OBJS} )
  get_filename_component(result "${OBJ}" NAME_WE)
  set(PROJECT_NAME "${PREFIX}_${result}")
  project( ${PROJECT_NAME} )

  add_executable("${PROJECT_NAME}" 
        ${OBJ} 
  )
  
  if( MONO_LIB )
    target_link_libraries(${PROJECT_NAME}
      mcrl2_monolib
    )
  else( MONO_LIB )  
  target_link_libraries(${PROJECT_NAME}
     mcrl2_data
  )
  
  endif( MONO_LIB )
endforeach( OBJ )

##---------------------------------------------------
## Example parse_data 
##--------------------------------------------------- 

set(PROJECT_NAME "${PREFIX}_parse_data")
project( ${PROJECT_NAME} )

add_executable("${PROJECT_NAME}" 
      parse_data.cpp 
)

if( MONO_LIB )
  target_link_libraries(${PROJECT_NAME}
    mcrl2_monolib
  )
else( MONO_LIB )  
target_link_libraries(${PROJECT_NAME}
   mcrl2_data
)

endif( MONO_LIB )
