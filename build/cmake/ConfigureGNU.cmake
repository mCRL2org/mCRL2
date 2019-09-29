# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://github.com/mCRL2org/mCRL2/blob/master/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
include(AddFlag)

if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  set(MCRL2_CLANGPP ON)
endif()

if(CMAKE_C_COMPILER_ID MATCHES ".*Clang")
  set(MCRL2_CLANG ON)
endif()

##---------------------------------------------------
## Add option for profiling support
##---------------------------------------------------

option(MCRL2_ENABLE_PROFILING "Enable/disable profiling support" OFF)
if( MCRL2_ENABLE_PROFILING )
  add_definitions( -pg )
  set(CMAKE_EXE_LINKER_FLAGS "-pg ${CMAKE_EXE_LINKER_FLAGS}")
  set(CMAKE_SHARED_LINKER_FLAGS "-pg ${CMAKE_SHARED_LINKER_FLAGS}")
  set(CMAKE_MODULE_LINKER_FLAGS "-pg ${CMAKE_SHARED_LINKER_FLAGS}")
endif(MCRL2_ENABLE_PROFILING)

##---------------------------------------------------
## Set C compile flags
##---------------------------------------------------

try_add_c_flag(-std=c99)
try_add_c_flag(-Wall)
try_add_c_flag(-Wno-inline)
try_add_c_flag(-fno-strict-overflow)
try_add_c_flag(-pipe)
try_add_c_flag(-pedantic                 DEBUG)
try_add_c_flag(-W                        DEBUG)

# The following flags are not implemented in clang and therefore cause warnings.
if(NOT MCRL2_CLANG)
  try_add_c_flag(-fprofile-arcs            DEBUG)
  try_add_c_flag(-ftest-coverage           DEBUG)
endif()

# The following is only implemented in clang, but not on Apple.
if(MCRL2_CLANG AND NOT APPLE)
  # We need to add the proper flag to the linker before we try:
  set(CMAKE_REQUIRED_LIBRARIES "-fsanitize=address")
  try_add_c_flag(-fsanitize=address       DEBUG)
  try_add_c_flag(-fno-omit-frame-pointer  DEBUG)
  unset(CMAKE_REQUIRED_LIBRARIES)
endif()

if(BUILD_SHARED_LIBS)
  try_add_c_flag(-fPIC)
endif()

##---------------------------------------------------
## Set C++ compile flags
##---------------------------------------------------

try_add_cxx_flag(-std=c++17)

if(NOT CXX_ACCEPTS_STD_CPP17)
  message(FATAL_ERROR "Your compiler does not support the -std=c++17 flag. You should upgrade to a newer compiler version to build mCRL2")
endif()

if(APPLE)
  try_add_cxx_flag(-stdlib=libc++)
endif()

try_add_cxx_flag(-Wall)
try_add_cxx_flag(-Wno-inline)
try_add_cxx_flag(-fno-strict-overflow)
try_add_cxx_flag(-pipe)
try_add_cxx_flag(-pedantic)

try_add_cxx_flag(-W                      DEBUG)
try_add_cxx_flag(-Wextra                 DEBUG)
try_add_cxx_flag(-Wunused-variable       DEBUG)
try_add_cxx_flag(-Wunused-parameter      DEBUG)
try_add_cxx_flag(-Wunused-function       DEBUG)
try_add_cxx_flag(-Wunused                DEBUG)
try_add_cxx_flag(-Wno-system-headers     DEBUG)
try_add_cxx_flag(-Woverloaded-virtual    DEBUG)
try_add_cxx_flag(-Wwrite-strings         DEBUG)
try_add_cxx_flag(-Wmissing-declarations  DEBUG)

# The following flags are not implemented in clang and therefore cause warnings.
if(NOT MCRL2_CLANGPP)
  try_add_cxx_flag(-fprofile-arcs            DEBUG)
  try_add_cxx_flag(-ftest-coverage           DEBUG)
endif()

# The following is only implemented in clang
if(MCRL2_CLANGPP AND NOT APPLE)
  # We need to add the proper flag to the linker before we try:
  set(CMAKE_REQUIRED_LIBRARIES "-fsanitize=address")
  try_add_cxx_flag(-fsanitize=address       DEBUG)
  try_add_cxx_flag(-fno-omit-frame-pointer  DEBUG)
  unset(CMAKE_REQUIRED_LIBRARIES)
endif()

if(BUILD_SHARED_LIBS)
  try_add_cxx_flag(-fPIC)
endif()

if(NOT WIN32)
  # Indicates that systems having a GNU compiler the rewriter can be compiled.
  # This flag is not available on MSVC because dynload is not available.
  # It's also not available on mingw for now.
  add_definitions(-DMCRL2_JITTYC_AVAILABLE)   
endif()

##---------------------------------------------------
## Set linker flags
##---------------------------------------------------

if(APPLE)
  # Set the size of the stack on apple to 4Gig, which is much larger than the default size. 
  if (NOT DEFINED MCRL2_OSX_STACK_SIZE)
    # Stack size set to 4GB for Mac, since terminal hard limit is 64MB
    # This number is the number of bytes in Hex
    set (MCRL2_OSX_STACK_SIZE "200000000" CACHE STRING "Maximum stack size to allow on MacOS, in number of bytes in hexadecimal (200000000 equals 4GB).")
  endif()
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-stack_size,${MCRL2_OSX_STACK_SIZE}")
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "")
else()
  set(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed")
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-Wl,--warn-unresolved-symbols,--warn-once")
endif()

if(CXX_ACCEPTS_FPROFILE_ARCS)
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fprofile-arcs")
  set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -fprofile-arcs")
endif()

if(CXX_ACCEPTS_FTEST_COVERAGE)
  set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -ftest-coverage")
endif()

if(CXX_ACCEPTS_FSANITIZE_ADDRESS)
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=address")
  set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -fsanitize=address")
endif()
