# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://github.com/mCRL2org/mCRL2/blob/master/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(examples_pg)

# ######### General setup ##########
set(PREFIX "example_pg")

set(PROJECT_NAME "${PREFIX}_solve_pg")
project( ${PROJECT_NAME} )

add_executable("${PROJECT_NAME}"
  pg_solving.cpp
)

target_link_libraries(${PROJECT_NAME}
  mcrl2_pg
)
