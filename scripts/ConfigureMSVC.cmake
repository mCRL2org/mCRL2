# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

include_directories( build/workarounds/msvc )
set(BUILD_SHARED_LIBS OFF)

##---------------------------------------------------
## Set MSVC specific compiler flags
##---------------------------------------------------
set(COMPILE_FLAGS "${COMPILE_FLAGS} /EHs")

## Compile with big-objects for 64-bit
if(CMAKE_CL_64)
  set (COMPILE_FLAGS "${COMPILE_FLAGS} /bigobj")
endif(CMAKE_CL_64)

## Compile multi-core  
option(MCRL2_ENABLE_MULTICORE_COMPILATION "Enable/disable multi-core compilation" ON)
if( MCRL2_ENABLE_MULTICORE_COMPILATION )
  set(CMAKE_CXX_FLAGS "/MP ${CMAKE_CXX_FLAGS}")
endif( MCRL2_ENABLE_MULTICORE_COMPILATION )
