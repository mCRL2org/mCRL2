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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHs")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /EHs")

## Compile with big-objects 
set(CMAKE_CXX_FLAGS "/bigobj ${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS "/bigobj ${CMAKE_CXX_FLAGS}")

## Compile multi-core  
option(MCRL2_ENABLE_MULTICORE_COMPILATION "Enable/disable multi-core compilation" ON)
if( MCRL2_ENABLE_MULTICORE_COMPILATION )
  set(CMAKE_CXX_FLAGS "/MP ${CMAKE_CXX_FLAGS}")
  set(CMAKE_C_FLAGS "/MP ${CMAKE_C_FLAGS}")
endif( MCRL2_ENABLE_MULTICORE_COMPILATION )
