# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://github.com/mCRL2org/mCRL2/blob/master/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
include(AddFlag)

##---------------------------------------------------
## Add option for profiling support
##---------------------------------------------------

option(MCRL2_ENABLE_PROFILING "Enable/disable profiling support" OFF)
if( MCRL2_ENABLE_PROFILING )
  add_cxx_flag(-pg)
  add_c_flag(-pg)

  add_link_options(-pg)
endif()

##---------------------------------------------------
## Set C compile flags
##---------------------------------------------------

#try_add_c_flag(-std=c11)
add_c_flag(-Wall)
add_c_flag(-Wno-inline)
add_c_flag(-fno-strict-overflow)
add_c_flag(-pipe)
# The amount of warnings produced in third party library makes this useless.
# try_add_c_flag(-pedantic)
add_c_debug_flag(-W)

# Ignore specific warnings produced in Sylvan.
if(MCRL2_IS_CLANG)
  add_c_flag(-Wno-c99-extensions)
  add_c_flag(-Wno-gnu-zero-variadic-macro-arguments)
  add_c_flag(-Wno-zero-length-array)
endif()

if(MCRL2_ENABLE_ADDRESSSANITIZER)
  add_c_flag(-fsanitize=address)
  add_c_flag(-fno-omit-frame-pointer)
endif()

if(MCRL2_ENABLE_THREADSANITIZER)
  add_c_flag(-fsanitize=thread)
  add_c_flag(-fno-omit-frame-pointer)
endif()

##---------------------------------------------------
## Set C++ compile flags
##---------------------------------------------------

# This first flag is only necessary for our compiling rewriter script (that ignores 'other' build flags).
add_cxx_flag(-std=c++17)

add_cxx_flag(-Wall)
add_cxx_flag(-Wno-inline)
add_cxx_flag(-fno-strict-overflow)
add_cxx_flag(-pipe)
add_cxx_flag(-pedantic)

add_cxx_debug_flag(-W)
add_cxx_debug_flag(-Wextra)
add_cxx_debug_flag(-Wunused-variable)
add_cxx_debug_flag(-Wunused-parameter)
add_cxx_debug_flag(-Wunused-function)
add_cxx_debug_flag(-Wunused)
add_cxx_debug_flag(-Wno-system-headers)
add_cxx_debug_flag(-Woverloaded-virtual)
add_cxx_debug_flag(-Wwrite-strings)
add_cxx_debug_flag(-Wmissing-declarations)

# Ignore specific warnings produced in Sylvan.
if(MCRL2_IS_CLANG)
  add_cxx_flag(-Wno-c99-extensions)
  add_cxx_flag(-Wno-gnu-zero-variadic-macro-arguments)
  add_cxx_flag(-Wno-zero-length-array)
endif()

if(MCRL2_ENABLE_ADDRESSSANITIZER)
  add_cxx_flag(-fsanitize=address)
  add_cxx_flag(-fno-omit-frame-pointer)
endif()

if(MCRL2_ENABLE_THREADSANITIZER)
  try_add_cxx_flag(-fsanitize=thread)
  try_add_cxx_flag(-fno-omit-frame-pointer)
endif()

##---------------------------------------------------
## Set linker flags
##---------------------------------------------------

if(APPLE)
  if (NOT DEFINED MCRL2_OSX_STACK_SIZE)
    # Stack size set to 512MB for Mac, which is the maximal size for an arm64 mac. The Maximal size for an x86_64 is 8GB.
    # This number is the number of bytes in Hex
    set (MCRL2_OSX_STACK_SIZE "20000000" CACHE STRING "Maximum stack size allowed on an arm64 Mac in number of bytes in hex (20000000 equals 512MB, for an x86_64 mac this can be 8GB).")
  endif()

  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-stack_size,${MCRL2_OSX_STACK_SIZE}")
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "")

  if (NOT DEFINED CMAKE_APPLE_SILICON_PROCESSOR) 
    set(CMAKE_APPLE_SILICON_PROCESSOR "x86_64" CACHE STRING "Select whether the generated code is x86_64 or arm64" FORCE) 
  endif()
  set_property(CACHE CMAKE_APPLE_SILICON_PROCESSOR PROPERTY STRINGS "x86_64" "arm64")
  set(CMAKE_OSX_ARCHITECTURES ${CMAKE_APPLE_SILICON_PROCESSOR} CACHE INTERNAL "" FORCE)
else()
  add_link_options(-Wl,--as-needed)
  add_debug_link_options(-Wl,--warn-unresolved-symbols,--warn-once)
endif()

if(MCRL2_ENABLE_ADDRESSSANITIZER)
  add_link_options(-fsanitize=address)
endif()


if(MCRL2_ENABLE_THREADSANITIZER)
  add_link_options(-fsanitize=thread)
endif()
