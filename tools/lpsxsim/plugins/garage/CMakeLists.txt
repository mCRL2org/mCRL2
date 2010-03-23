# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 2.6)

# ########## Project xsim_garage ##########
project(xsim_garage_view)

# ######### General setup ##########

add_definitions(-DGARAGEFRAMEDLL)

add_library(${PROJECT_NAME}
  garagecanvas.cpp
  garageframe.cpp
  visutils.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/tools/lpsxsim/)
include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include)

if (MONO_LIB)
  target_link_libraries(${PROJECT_NAME}
    lpsxsim
    ${wxWidgets_LIBRARIES}
    mcrl2_monolib
  )
else (MONO_LIB)
  target_link_libraries(${PROJECT_NAME}
    lpsxsim
    ${wxWidgets_LIBRARIES}
    mcrl2_data
    mcrl2_simulator_gui
  )
endif (MONO_LIB)

# add install target:
install(TARGETS ${PROJECT_NAME} DESTINATION lib/mcrl2/plugins)
