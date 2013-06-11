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

# The following flags are not implemented in clang and therefore cause warnings.
if(NOT MCRL2_CLANG)
  try_add_c_flag(-fprofile-arcs            MAINTAINER)
  try_add_c_flag(-ftest-coverage           MAINTAINER)
endif()

# The following is only implemented in clang, but not on Apple.
if (NOT APPLE)    
  if (MCRL2_CLANG)
    # We need to add the proper flag to the linker before we try:
     set(CMAKE_REQUIRED_LIBRARIES "-fsanitize=address")
     try_add_c_flag(-fsanitize=address       MAINTAINER)
  endif()
endif()

##---------------------------------------------------
## Set C++ compile flags
##---------------------------------------------------

try_add_cxx_flag(-std=c++11)
if(NOT CXX_ACCEPTS_STD_CPP11)
  try_add_cxx_flag(-std=c++0x)
endif()

if((NOT CXX_ACCEPTS_STD_CPP11) AND (NOT CXX_ACCEPTS_STD_CPP0X))
  message(FATAL_ERROR "Your compiler does not support -std=c++11 or -std=c++0x. You should upgrade to a newer compiler version to build mCRL2")
endif()

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
# GLIBCXX_DEBUG has a problem with ostringstream on Apple Xcode version 4.2.
if (NOT APPLE)  
   try_add_cxx_flag(-D_GLIBCXX_DEBUG        MAINTAINER)
endif()

# The following flags are not implemented in clang and therefore cause warnings.
if(NOT MCRL2_CLANG)
  try_add_cxx_flag(-fprofile-arcs            MAINTAINER)
  try_add_cxx_flag(-ftest-coverage           MAINTAINER)
endif()

# The following is only implemented in clang
if (NOT APPLE)
  if(MCRL2_CLANG)
    # We need to add the proper flag to the linker before we try:
    set(CMAKE_REQUIRED_LIBRARIES "-fsanitize=address")
    try_add_cxx_flag(-fsanitize=address       MAINTAINER)
  endif()
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

if(APPLE)
  # On the Mac, do never, ever, strip the executables, since this is bound to break the
  # compiling rewriter.
  # The symptoms when stripping, are that the rewriter cannot find some of the symbols
  # that should be included in the executable.
  ## set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-dead_strip")
  ## set(CMAKE_EXE_LINKER_FLAGS "-Wl,-dead_strip")
else()
  set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--as-needed")
  set(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed")
endif()

if(APPLE)
  set(CMAKE_EXE_LINKER_FLAGS_MAINTAINER ""
      CACHE STRING "Flags used for linking binaries during maintainer builds.")
  set(CMAKE_SHARED_LINKER_FLAGS_MAINTAINER ""
      CACHE STRING "Flags used by the shared libraries linker during maintainer builds.")
else()
  set(CMAKE_EXE_LINKER_FLAGS_MAINTAINER "-Wl,--warn-unresolved-symbols,--warn-once"
      CACHE STRING "Flags used for linking binaries during maintainer builds.")
  set(CMAKE_SHARED_LINKER_FLAGS_MAINTAINER "-Wl,--warn-unresolved-symbols,--warn-once"
      CACHE STRING "Flags used by the shared libraries linker during maintainer builds.")
endif()

if(CXX_ACCEPTS_FPROFILE_ARCS)
  set(CMAKE_EXE_LINKER_FLAGS_MAINTAINER "${CMAKE_EXE_LINKER_FLAGS_MAINTAINER} -fprofile-arcs")
  set(CMAKE_SHARED_LINKER_FLAGS_MAINTAINER "${CMAKE_SHARED_LINKER_FLAGS_MAINTAINER} -fprofile-arcs")
endif()
if(CXX_ACCEPTS_FTEST_COVERAGE)
  set(CMAKE_SHARED_LINKER_FLAGS_MAINTAINER "${CMAKE_SHARED_LINKER_FLAGS_MAINTAINER} -ftest-coverage")
endif()
if(CXX_ACCEPTS_FSANITIZE_ADDRESS)
  set(CMAKE_EXE_LINKER_FLAGS_MAINTAINER "${CMAKE_EXE_LINKER_FLAGS_MAINTAINER} -fsanitize=address")
  set(CMAKE_SHARED_LINKER_FLAGS_MAINTAINER "${CMAKE_SHARED_LINKER_FLAGS_MAINTAINER} -fsanitize=address")
endif()
