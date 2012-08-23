# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

#
# This script sets the following variables:
#   CXX_COMPILER_VERSION: The version of the compiler (includes compiler name)
#   CXX_COMPILER_ARCHITECTURE: Either x86 or x86_64, based on sizeof(void*)
#   MCRL2_MSVC: True iff compiled using the MSVC C/C++ compiler
#   MCRL2_GNU: True iff compiled using a GCC-compatible C-compiler AND a GCC-compatible C++-compiler
#   MCRL2_CLANG: True iff either the C or C++-compiler is Clang
#
# The script aborts configuration if it cannot characterize the compiler using
# the MCRL2_{MSVC,GNU,CLANG} variables.
#
# The script aborts configuration if for some reason sizeof(void*) is not equal
# to 4 or 8.
#

if(MSVC)
  set(CXX_COMPILER_VERSION "MSVC-${MSVC_VERSION}")
  set(MCRL2_MSVC "1")
else()
  execute_process(COMMAND ${CMAKE_CXX_COMPILER} --version
    OUTPUT_VARIABLE CXX_COMPILER_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND ${CMAKE_C_COMPILER} --version
    OUTPUT_VARIABLE C_COMPILER_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(MCRL2_CLANGC CMAKE_C_COMPILER_ID STREQUAL "Clang")
  set(MCRL2_CLANGCXX CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  if((CMAKE_COMPILER_IS_GNUCC OR MCRL2_CLANGC) AND 
     (CMAKE_COMPILER_IS_GNUCXX OR MCRL2_CLANGCXX))
    set(MCRL2_GNU "1")
  endif()
  if(MCRL2_CLANGC OR MCRL2_CLANGCXX)
    set(MCRL2_GNU "1")
    set(MCRL2_CLANG "1")
  endif()
endif()

if(NOT (MCRL2_GNU OR MCRL2_MSVC))
  message(FATAL_ERROR "Unsupported compiler setup (C: ${CMAKE_C_COMPILER_ID} / C++: ${CMAKE_CXX_COMPILER_ID}).")
endif()

if (CMAKE_SIZEOF_VOID_P MATCHES "8")
  set(ATERM_FLAGS "-m64 -DAT_64BIT")
  set(CXX_COMPILER_ARCHITECTURE "x86_64")
else ()
  if (CMAKE_SIZEOF_VOID_P MATCHES "4")
    set(ATERM_FLAGS "-m32")
    set(CXX_COMPILER_ARCHITECTURE "x86")
  else ()
    message(FATAL_ERROR "Could not determine architecture.")
  endif()
endif()

