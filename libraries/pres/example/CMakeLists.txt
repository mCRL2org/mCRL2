# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://github.com/mCRL2org/mCRL2/blob/master/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(examples_pres)

# ######### General setup ##########
set(PREFIX "example_pres")

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
     mcrl2_pres
  )

endforeach( OBJ )
