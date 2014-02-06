# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

if(NOT WIN32)
  return()
endif()

##---------------------------------------------------
## Set Shared Build  
##---------------------------------------------------

set(BUILD_SHARED_LIBS OFF) 

##---------------------------------------------------
## Set run-time destination  
##---------------------------------------------------

set(MCRL2_BIN_DIR "bin")

add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-D_SCL_SECURE_NO_DEPRECATE)
add_definitions(-D_SCL_SECURE_NO_WARNINGS)
add_definitions(-DBOOST_ALL_NO_LIB=1)
add_definitions(-DWIN32)

# The following definition is mCRL2 specific.
# It prevents code being compiled for the innermost and jitty compiling rewriters
# TODO: Should be prefixed with MCRL2_
add_definitions(-DNO_DYNLOAD)

# Prevent windows.h from defining min and max.
add_definitions(-DNOMINMAX)
