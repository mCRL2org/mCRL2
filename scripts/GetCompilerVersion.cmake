# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

##---------------------------------------------------
## Print compiler information
##---------------------------------------------------

if(NOT MSVC)
execute_process(COMMAND ${CMAKE_CXX_COMPILER} --version
    OUTPUT_VARIABLE CXX_COMPILER_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

STRING(REGEX REPLACE "^([^\n]+).*" "\\1" CXX_COMPILER_VERSION "${CXX_COMPILER_VERSION}" )
else(NOT MSVC)
   set(CXX_COMPILER_VERSION "MSVC-${MSVC_VERSION}")
endif(NOT MSVC)

message(STATUS "CXX COMPILER VERSION: ${CXX_COMPILER_VERSION}")
