# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

##---------------------------------------------------
## Set GCC compiler flags 
##---------------------------------------------------

message( STATUS "Loading GCC specific configuration" )

##---------------------------------------------------
## Set C compile flags 
##---------------------------------------------------
include(AddFlag)

try_add_c_flag(-Wall)
try_add_c_flag(-Wno-inline)
try_add_c_flag(-fno-strict-overflow)
try_add_c_flag(-pipe)

if( NOT MINGW )
  try_add_c_flag(-pthread)
endif( NOT MINGW )

# Following flag will cause warning on MacOSX, if enabled:
#686-apple-darwin9-g++-4.0.1: -as-needed: linker input file unused because linking not done
if( NOT APPLE)
  try_add_c_flag(-Wl,-as-needed)
endif( NOT APPLE )

##---------------------------------------------------
## Set C compile flags 
##---------------------------------------------------

try_add_cxx_flag(-Wall)
try_add_cxx_flag(-Wno-inline)
try_add_cxx_flag(-fno-strict-overflow)
try_add_cxx_flag(-pipe)

if( NOT MINGW )
  try_add_cxx_flag(-pthread)
endif( NOT MINGW )

# Following flag will cause warning on MacOSX, if enabled:
#686-apple-darwin9-g++-4.0.1: -as-needed: linker input file unused because linking not done
if( NOT APPLE)
  try_add_cxx_flag(-Wl,-as-needed)
endif( NOT APPLE )

if(BUILD_SHARED_LIBS)
    check_cxx_compiler_flag(-fPIC CXX_ACCEPTS_FPIC )
    check_c_compiler_flag(-fPIC C_ACCEPTS_FPIC )
    if( CXX_ACCEPTS_FPIC AND C_ACCEPTS_FPIC)
      set(CMAKE_CXX_FLAGS "-fPIC ${CMAKE_CXX_FLAGS}")
      set(CMAKE_C_FLAGS "-fPIC ${CMAKE_C_FLAGS}")
      set(BUILD_SHARED_LIBS_OPTION "-fPIC")
    endif( CXX_ACCEPTS_FPIC AND C_ACCEPTS_FPIC )
endif(BUILD_SHARED_LIBS)
