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

##---------------------------------------------------
## Example lpsrewr 
##--------------------------------------------------- 

set(PROJECT_NAME "${PREFIX}_cmdline1")
project( ${PROJECT_NAME} )

add_executable("${PROJECT_NAME}" EXCLUDE_FROM_ALL 
      cmdline1.cpp
)

target_link_libraries(${PROJECT_NAME}
   mcrl2_utilities
   mcrl2_core
)

build_and_run_test_example_target( ${PROJECT_NAME} )

##--------------------------------------------------
## Logger example, used in quickbook documentation
##--------------------------------------------------
set(PROJECT_NAME "${PREFIX}_logger")
project( ${PROJECT_NAME} )

add_executable("${PROJECT_NAME}" EXCLUDE_FROM_ALL 
      logger.cpp
)

target_link_libraries(${PROJECT_NAME}
   mcrl2_core
)

build_and_run_test_example_target( ${PROJECT_NAME} )

