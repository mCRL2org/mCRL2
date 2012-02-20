# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

##---------------------------------------------------
## Set Clang compiler flags 
##---------------------------------------------------

message( STATUS "Loading Clang specific configuration" )
##---------------------------------------------------
## Set C compile flags 
##---------------------------------------------------
include(CheckCCompilerFlag)

check_c_compiler_flag(-Wall C_ACCEPTS_WALL )
if( C_ACCEPTS_WALL )
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall" )
endif( C_ACCEPTS_WALL )

check_c_compiler_flag(-Wno-inline C_ACCEPTS_WNO-INLINE )
if( C_ACCEPTS_WNO-INLINE )
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-inline" )
endif( C_ACCEPTS_WNO-INLINE )

check_c_compiler_flag(-pipe C_ACCEPTS_PIPE )
if( C_ACCEPTS_PIPE )
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pipe" )
endif( C_ACCEPTS_PIPE )

##---------------------------------------------------
## Set CXX compile flags 
##---------------------------------------------------

check_cxx_compiler_flag(-std=c++0x CXX_ACCEPTS_CPP0X)
if (CXX_ACCEPTS_CPP0X)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif (CXX_ACCEPTS_CPP0X)

check_cxx_compiler_flag(-Wall CXX_ACCEPTS_WALL )
if( CXX_ACCEPTS_WALL )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall" )
endif( CXX_ACCEPTS_WALL )

check_cxx_compiler_flag(-Wno-inline CXX_ACCEPTS_WNO-INLINE )
if( CXX_ACCEPTS_WNO-INLINE )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-inline" )
endif( CXX_ACCEPTS_WNO-INLINE )

# check_cxx_compiler_flag(-fno-strict-overflow CXX_ACCEPTS_FNO-STRICT-OVERFLOW )
# if( CXX_ACCEPTS_FNO-STRICT-OVERFLOW )
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-overflow" )
# endif( CXX_ACCEPTS_FNO-STRICT-OVERFLOW )

check_cxx_compiler_flag(-pipe CXX_ACCEPTS_PIPE )
if( CXX_ACCEPTS_PIPE )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe" )
endif( CXX_ACCEPTS_PIPE )

if(BUILD_SHARED_LIBS)
    check_cxx_compiler_flag(-fPIC CXX_ACCEPTS_FPIC )
    check_c_compiler_flag(-fPIC C_ACCEPTS_FPIC )
    if( CXX_ACCEPTS_FPIC AND C_ACCEPTS_FPIC )

      set(CMAKE_CXX_FLAGS "-fPIC ${CMAKE_CXX_FLAGS}")
      set(CMAKE_C_FLAGS "-fPIC ${CMAKE_C_FLAGS}")

      # Setting -fPIC option for compiling rewriters
      set(BUILD_SHARED_LIBS_OPTION "-fPIC")
    endif( CXX_ACCEPTS_FPIC AND C_ACCEPTS_FPIC)
endif(BUILD_SHARED_LIBS)
