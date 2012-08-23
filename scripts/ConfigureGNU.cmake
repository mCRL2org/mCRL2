# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

if(NOT MCRL2_GNU)
  return()
endif()

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
try_add_c_flag(-g                        MAINTAINER)
try_add_c_flag(-O0                       MAINTAINER)
try_add_c_flag(-pedantic                 MAINTAINER)
try_add_c_flag(-W                        MAINTAINER)

if(NOT MINGW)
  try_add_c_flag(-pthread)
endif()

# Following flag will cause warning on MacOSX, if enabled:
#686-apple-darwin9-g++-4.0.1: -as-needed: linker input file unused because linking not done
if(NOT APPLE)
  try_add_c_flag(-Wl,-as-needed)
endif()

# The following flags are not implemented in clang and therefore cause warnings.
if(NOT MCRL2_CLANG)
  try_add_c_flag(-fprofile-arcs            MAINTAINER)
  try_add_c_flag(-ftest-coverage           MAINTAINER)
endif()

##---------------------------------------------------
## Set C++ compile flags 
##---------------------------------------------------

try_add_cxx_flag(-Wall)
try_add_cxx_flag(-Wno-inline)
try_add_cxx_flag(-fno-strict-overflow)
try_add_cxx_flag(-pipe)

try_add_cxx_flag(-g                      MAINTAINER)
try_add_cxx_flag(-O0                     MAINTAINER)
try_add_cxx_flag(-W                      MAINTAINER)
try_add_cxx_flag(-Wextra                 MAINTAINER)
try_add_cxx_flag(-Wunused-variable       MAINTAINER)
try_add_cxx_flag(-Wunused-parameter      MAINTAINER)
try_add_cxx_flag(-Wunused-function       MAINTAINER)
try_add_cxx_flag(-Wunused                MAINTAINER)
try_add_cxx_flag(-Wno-system-headers     MAINTAINER)
try_add_cxx_flag(-Woverloaded-virtual    MAINTAINER)
try_add_cxx_flag(-Wwrite-strings         MAINTAINER)
try_add_cxx_flag(-Wmissing-declarations  MAINTAINER)

if(NOT MINGW)
  try_add_cxx_flag(-pthread)
endif()

# The following flags are not implemented in clang and therefore cause warnings.
if(NOT MCRL2_CLANG)
  try_add_cxx_flag(-fprofile-arcs            MAINTAINER)
  try_add_cxx_flag(-ftest-coverage           MAINTAINER)
endif()

if(BUILD_SHARED_LIBS)
  check_cxx_compiler_flag(-fPIC CXX_ACCEPTS_FPIC )
  check_c_compiler_flag(-fPIC C_ACCEPTS_FPIC )
  if(CXX_ACCEPTS_FPIC AND C_ACCEPTS_FPIC)
    set(CMAKE_CXX_FLAGS "-fPIC ${CMAKE_CXX_FLAGS}")
    set(CMAKE_C_FLAGS "-fPIC ${CMAKE_C_FLAGS}")
    set(BUILD_SHARED_LIBS_OPTION "-fPIC")
  endif()
endif()

##---------------------------------------------------
## Set linker flags
##---------------------------------------------------

if(NOT APPLE)
  set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--as-needed")
  set(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed")
else()
  set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-dead_strip")
  set(CMAKE_EXE_LINKER_FLAGS "-Wl,-dead_strip")
endif()

set(CMAKE_EXE_LINKER_FLAGS_MAINTAINER "-Wl,--warn-unresolved-symbols,--warn-once"
    CACHE STRING "Flags used for linking binaries during maintainer builds.")
set(CMAKE_SHARED_LINKER_FLAGS_MAINTAINER "-Wl,--warn-unresolved-symbols,--warn-once"
    CACHE STRING "Flags used by the shared libraries linker during maintainer builds.")

# The following flags are not implemented in clang and therefore cause warnings.
if(NOT MCRL2_CLANG)
  if(CXX_ACCEPTS_FPROFILE_ARCS)
    set(CMAKE_EXE_LINKER_FLAGS_MAINTAINER "${CMAKE_EXE_LINKER_FLAGS_MAINTAINER} -fprofile-arcs")
    set(CMAKE_SHARED_LINKER_FLAGS_MAINTAINER "${CMAKE_SHARED_LINKER_FLAGS_MAINTAINER} -fprofile-arcs")
  endif()
  if(CXX_ACCEPTS_FTEST_COVERAGE)
    set(CMAKE_SHARED_LINKER_FLAGS_MAINTAINER "${CMAKE_SHARED_LINKER_FLAGS_MAINTAINER} -ftest-coverage")
  endif()
endif()
