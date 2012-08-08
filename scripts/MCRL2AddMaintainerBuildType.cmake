# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

set(CMAKE_C_FLAGS_MAINTAINER "" CACHE STRING "Flags used by the C compiler during maintainer builds.")
set(CMAKE_CXX_FLAGS_MAINTAINER "" CACHE STRING "Flags used by the C++ compiler during maintainer builds.")
set(CMAKE_EXE_LINKER_FLAGS_MAINTAINER "" CACHE STRING "Flags used for linking binaries during maintainer builds.")
set(CMAKE_SHARED_LINKER_FLAGS_MAINTAINER "" CACHE STRING "Flags used by the shared libraries linker during maintainer builds.")

mark_as_advanced(
  CMAKE_CXX_FLAGS_MAINTAINER
  CMAKE_C_FLAGS_MAINTAINER
  CMAKE_EXE_LINKER_FLAGS_MAINTAINER
  CMAKE_SHARED_LINKER_FLAGS_MAINTAINER
)

#
# Macros to easily add flags.
#

include(CheckCCompilerFlag)

macro(try_add_c_flag FLAG)
  check_c_compiler_flag("${FLAG}" C_ACCEPTS_REQUESTED_FLAG)
  if (C_ACCEPTS_REQUESTED_FLAG)
    set(CMAKE_C_FLAGS_MAINTAINER "${CMAKE_C_FLAGS_MAINTAINER} ${FLAG}")
  endif (C_ACCEPTS_REQUESTED_FLAG)
endmacro(try_add_c_flag)

macro(try_add_cxx_flag FLAG)
  check_cxx_compiler_flag("${FLAG}" CXX_ACCEPTS_REQUESTED_FLAG)
  if (CXX_ACCEPTS_REQUESTED_FLAG)
    set(CMAKE_CXX_FLAGS_MAINTAINER "${CMAKE_CXX_FLAGS_MAINTAINER} ${FLAG}")
  endif (CXX_ACCEPTS_REQUESTED_FLAG)
endmacro(try_add_cxx_flag)

#
# Special treatment for the -fprofile-arcs and -ftest-coverage flags
#
check_cxx_compiler_flag(-fprofile-arcs CXX_ACCEPTS_PROFILE_ARCS)
check_cxx_compiler_flag(-ftest-coverage CXX_ACCEPTS_TEST_COVERAGE )
# Clang accepts some flags for compatibility, but then raises warnings because 
# it doesn't implement them. We therefore override the detected acceptance 
# status for the clang compiler.
if( CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_C_COMPILER_ID STREQUAL "Clang" )
  set( CXX_ACCEPTS_PROFILE_ARCS false)
  set( CXX_ACCEPTS_TEST_COVERAGE false)
  set( C_ACCEPTS_PROFILE_ARCS false)
  set( C_ACCEPTS_TEST_COVERAGE false)
endif( )
if(CXX_ACCEPTS_PROFILE_ARCS)
  set(CMAKE_CXX_FLAGS_MAINTAINER "${CMAKE_CXX_FLAGS_MAINTAINER} -fprofile-arcs")
  set(CMAKE_C_FLAGS_MAINTAINER "${CMAKE_C_FLAGS_MAINTAINER} -fprofile-arcs")
endif(CXX_ACCEPTS_PROFILE_ARCS)
if(CXX_ACCEPTS_TEST_COVERAGE)
  set(CMAKE_CXX_FLAGS_MAINTAINER "${CMAKE_CXX_FLAGS_MAINTAINER} -ftest-coverage")
  set(CMAKE_C_FLAGS_MAINTAINER "${CMAKE_C_FLAGS_MAINTAINER} -ftest-coverage")
endif(CXX_ACCEPTS_TEST_COVERAGE)
if(CXX_ACCEPTS_PROFILE_ARCS)
  set(CMAKE_EXE_LINKER_FLAGS_MAINTAINER "${CMAKE_EXE_LINKER_FLAGS_MAINTAINER} -fprofile-arcs")
  set(CMAKE_SHARED_LINKER_FLAGS_MAINTAINER "${CMAKE_SHARED_LINKER_FLAGS_MAINTAINER} -fprofile-arcs")
endif(CXX_ACCEPTS_PROFILE_ARCS)
if(CXX_ACCEPTS_TEST_COVERAGE)
  set(CMAKE_SHARED_LINKER_FLAGS_MAINTAINER "${CMAKE_SHARED_LINKER_FLAGS_MAINTAINER} -ftest-coverage")
endif(CXX_ACCEPTS_TEST_COVERAGE)

#
# Set up C compiler flags for Maintainer build
#
try_add_c_flag(-g)
try_add_c_flag(-O0)
try_add_c_flag(-Wall)
try_add_c_flag(-pedantic)
try_add_c_flag(-W)

#
# Set up C++ compiler flags for Maintainer build
#
try_add_cxx_flag(-g)
try_add_cxx_flag(-O0)
try_add_cxx_flag(-Wall)
try_add_cxx_flag(-W)
try_add_cxx_flag(-Wextra)
try_add_cxx_flag(-Wunused-variable)
try_add_cxx_flag(-Wunused-parameter)
try_add_cxx_flag(-Wunused-function)
try_add_cxx_flag(-Wunused)
try_add_cxx_flag(-Wno-system-headers)
try_add_cxx_flag(-Woverloaded-virtual)
try_add_cxx_flag(-Wwrite-strings)
try_add_cxx_flag(-Wmissing-declarations)

if(!MSVC)
  # We'll chance it. Originally this was done for any compiler, and we had no
  # complaints...
  set(CMAKE_EXE_LINKER_FLAGS_MAINTAINER "-Wl,--warn-unresolved-symbols,--warn-once")
  set(CMAKE_SHARED_LINKER_FLAGS_MAINTAINER "-Wl,--warn-unresolved-symbols,--warn-once")
endif(!MSVC)
