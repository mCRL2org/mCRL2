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

#try_add_c_flag(-std=c11)
try_add_c_flag(-Wall)
try_add_c_flag(-Wno-inline)
try_add_c_flag(-fno-strict-overflow)
try_add_c_flag(-pipe)
# The amount of warnings produced in third party library makes this useless.
# try_add_c_flag(-pedantic)
try_add_c_flag(-W                      DEBUG)

# Ignore specific warnings produced in Sylvan.
if(MCRL2_CLANG)
  try_add_c_flag(-Wno-c99-extensions)
  try_add_c_flag(-Wno-gnu-zero-variadic-macro-arguments)
  try_add_c_flag(-Wno-zero-length-array)
endif()

if(MCRL2_ENABLE_ADDRESSSANITIZER)
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

# This first flag is only necessary for our compiling rewriter script (that ignores 'other' build flags).
try_add_cxx_flag(-std=c++17)

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

# Ignore specific warnings produced in Sylvan.
if(MCRL2_CLANG)
  try_add_cxx_flag(-Wno-c99-extensions)
  try_add_cxx_flag(-Wno-gnu-zero-variadic-macro-arguments)
  try_add_cxx_flag(-Wno-zero-length-array)
endif()

if(MCRL2_ENABLE_ADDRESSSANITIZER)
  # We need to add the proper flag to the linker before we try:
  set(CMAKE_REQUIRED_LIBRARIES "-fsanitize=address")
  try_add_cxx_flag(-fsanitize=address)
  try_add_cxx_flag(-fno-omit-frame-pointer)
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
  if (NOT DEFINED CMAKE_APPLE_SILICON_PROCESSOR) 
    set(CMAKE_APPLE_SILICON_PROCESSOR "x86_64" CACHE STRING "Select whether the generated code is x86_64 or arm64" FORCE) 
  endif()
  set_property(CACHE CMAKE_APPLE_SILICON_PROCESSOR PROPERTY STRINGS "x86_64" "arm64")
  set(CMAKE_OSX_ARCHITECTURES ${CMAKE_APPLE_SILICON_PROCESSOR} CACHE INTERNAL "" FORCE)
else()
  set(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed")
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-Wl,--warn-unresolved-symbols,--warn-once")
endif()

if(MCRL2_ENABLE_ADDRESSSANITIZER)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=address")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -fsanitize=address")
endif()
