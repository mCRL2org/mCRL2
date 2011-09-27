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

check_cxx_compiler_flag(-Wall CXX_ACCEPTS_WALL )
if( CXX_ACCEPTS_WALL )
  set(COMPILE_FLAGS "${COMPILE_FLAGS} -Wall" )
endif( CXX_ACCEPTS_WALL )

check_cxx_compiler_flag(-Wno-inline CXX_ACCEPTS_WNO-INLINE )
if( CXX_ACCEPTS_WNO-INLINE )
  set(COMPILE_FLAGS "${COMPILE_FLAGS} -Wno-inline" )
endif( CXX_ACCEPTS_WNO-INLINE )

 check_cxx_compiler_flag(-fno-strict-overflow CXX_ACCEPTS_FNO-STRICT-OVERFLOW )
 if( CXX_ACCEPTS_FNO-STRICT-OVERFLOW )
   set(COMPILE_FLAGS "${COMPILE_FLAGS} -fno-strict-overflow" )
 endif( CXX_ACCEPTS_FNO-STRICT-OVERFLOW )

check_cxx_compiler_flag(-pipe CXX_ACCEPTS_PIPE )
if( CXX_ACCEPTS_PIPE )
  set(COMPILE_FLAGS "${COMPILE_FLAGS} -pipe" )
endif( CXX_ACCEPTS_PIPE )

# Following flag will cause warning on MacOSX, if enabled:
#686-apple-darwin9-g++-4.0.1: -as-needed: linker input file unused because linking not done
if( NOT APPLE)
  check_cxx_compiler_flag(-Wl,-as-needed CXX_ACCEPTS_AS_NEEDED )
  if( CXX_ACCEPTS_AS_NEEDED )
    set(COMPILE_FLAGS "${COMPILE_FLAGS} -Wl,-as-needed" )
  endif( CXX_ACCEPTS_AS_NEEDED )
endif( NOT APPLE )

if (CMAKE_SIZEOF_VOID_P MATCHES "8")
  add_definitions("-DAT_64BIT")
  # set(ARCHITECTURE "x86_64")
endif (CMAKE_SIZEOF_VOID_P MATCHES "8")

if(BUILD_SHARED_LIBS)
    check_cxx_compiler_flag(-fPIC CXX_ACCEPTS_FPIC )
    if( CXX_ACCEPTS_FPIC )
      add_definitions( -fPIC )
      set(BUILD_SHARED_LIBS_OPTION "-fPIC")
    endif( CXX_ACCEPTS_FPIC )
endif(BUILD_SHARED_LIBS)

## --------------------------------------------------
## Set Aterm Flags for compiling rewriters
## --------------------------------------------------

if (CMAKE_SIZEOF_VOID_P MATCHES "8")
  set(ATERM_FLAGS "-m64 -DAT_64BIT")
else(CMAKE_SIZEOF_VOID_P MATCHES "8")
  set(ATERM_FLAGS "-m32")
endif (CMAKE_SIZEOF_VOID_P MATCHES "8")
