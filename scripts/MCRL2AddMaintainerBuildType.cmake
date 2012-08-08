# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

#
# Macros to easily add flags.
#

include(AddFlag)

#
# Set linker flags
#
if(NOT MSVC)
  # We'll chance it. Originally this was done for any compiler, and we had no
  # complaints...

  check_cxx_compiler_flag(-fprofile-arcs CXX_ACCEPTS_PROFILE_ARCS)
  check_cxx_compiler_flag(-ftest-coverage CXX_ACCEPTS_TEST_COVERAGE )

  if(NOT CMAKE_EXE_LINKER_FLAGS_MAINTAINER)
    set(CMAKE_EXE_LINKER_FLAGS_MAINTAINER "-Wl,--warn-unresolved-symbols,--warn-once"
        CACHE STRING "Flags used for linking binaries during maintainer builds.")
    if(CXX_ACCEPTS_PROFILE_ARCS AND NOT (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_C_COMPILER_ID STREQUAL "Clang"))
      set(CMAKE_EXE_LINKER_FLAGS_MAINTAINER "${CMAKE_EXE_LINKER_FLAGS_MAINTAINER} -fprofile-arcs")
    endif( )
  endif(NOT CMAKE_EXE_LINKER_FLAGS_MAINTAINER)

  if(NOT CMAKE_SHARED_LINKER_FLAGS_MAINTAINER)
    set(CMAKE_SHARED_LINKER_FLAGS_MAINTAINER "-Wl,--warn-unresolved-symbols,--warn-once"
        CACHE STRING "Flags used by the shared libraries linker during maintainer builds.")
    if(CXX_ACCEPTS_PROFILE_ARCS AND NOT (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_C_COMPILER_ID STREQUAL "Clang"))
      set(CMAKE_SHARED_LINKER_FLAGS_MAINTAINER "${CMAKE_SHARED_LINKER_FLAGS_MAINTAINER} -fprofile-arcs")
    endif( )
    if(CXX_ACCEPTS_TEST_COVERAGE AND NOT (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_C_COMPILER_ID STREQUAL "Clang"))
      set(CMAKE_SHARED_LINKER_FLAGS_MAINTAINER "${CMAKE_SHARED_LINKER_FLAGS_MAINTAINER} -ftest-coverage")
    endif( )
  endif(NOT CMAKE_SHARED_LINKER_FLAGS_MAINTAINER)
endif(NOT MSVC)

#
# Set up C compiler flags for Maintainer build
#
if(NOT DEFINED CMAKE_C_FLAGS_MAINTAINER)

  # Clang does not support these, but passes the check_*_flag tests.
  if(NOT (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_C_COMPILER_ID STREQUAL "Clang"))
    try_add_c_flag(-fprofile-arcs  MAINTAINER)
    try_add_c_flag(-ftest-coverage MAINTAINER)
  endif( )

  try_add_c_flag(-g         MAINTAINER)
  try_add_c_flag(-O0        MAINTAINER)
  try_add_c_flag(-Wall      MAINTAINER)
  try_add_c_flag(-pedantic  MAINTAINER)
  try_add_c_flag(-W         MAINTAINER)
  set(CMAKE_C_FLAGS_MAINTAINER "${CMAKE_C_FLAGS_MAINTAINER}" CACHE STRING "Flags used by the C compiler during maintainer builds.")
endif(NOT DEFINED CMAKE_C_FLAGS_MAINTAINER)

#
# Set up C++ compiler flags for Maintainer build
#
if(NOT DEFINED CMAKE_CXX_FLAGS_MAINTAINER)

  # Clang does not support these, but passes the check_*_flag tests.
  if(NOT (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_C_COMPILER_ID STREQUAL "Clang"))
    try_add_cxx_flag(-fprofile-arcs        MAINTAINER)
    try_add_cxx_flag(-ftest-coverage       MAINTAINER)
  endif( )

  try_add_cxx_flag(-g                      MAINTAINER)
  try_add_cxx_flag(-O0                     MAINTAINER)
  try_add_cxx_flag(-Wall                   MAINTAINER)
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
  set(CMAKE_CXX_FLAGS_MAINTAINER "${CMAKE_CXX_FLAGS_MAINTAINER}" CACHE STRING "Flags used by the C++ compiler during maintainer builds.")
endif(NOT DEFINED CMAKE_CXX_FLAGS_MAINTAINER)

if(DEFINED CMAKE_CXX_FLAGS_MAINTAINER)
  message(STATUS "CMAKE MAINTAINER FLAGS: ${CMAKE_C_FLAGS_MAINTAINER}")
else()
  message(STATUS "CMAKE MAINTAINER FLAGS NOT DEFINED")
endif()

mark_as_advanced(
  CMAKE_CXX_FLAGS_MAINTAINER
  CMAKE_C_FLAGS_MAINTAINER
  CMAKE_EXE_LINKER_FLAGS_MAINTAINER
  CMAKE_SHARED_LINKER_FLAGS_MAINTAINER
)

