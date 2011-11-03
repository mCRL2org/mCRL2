# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

if( CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX )
  set( CXX_ACCEPTS_PROFILE_ARCS true)
  set( CXX_ACCEPTS_TEST_COVERAGE true)
elseif( CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_C_COMPILER_ID STREQUAL "Clang" )
  # Workaround because clang does accept the flags, but then raises warnings
  # during the build.
  set( CXX_ACCEPTS_PROFILE_ARCS false)
  set( CXX_ACCEPTS_TEST_COVERAGE false)
else( )
    check_cxx_compiler_flag(-fprofile-arcs CXX_ACCEPTS_PROFILE_ARCS )
    check_cxx_compiler_flag(-ftest-coverage CXX_ACCEPTS_TEST_COVERAGE )
endif( )

set(CMAKE_CXX_FLAGS_MAINTAINER "-g -O0 -Wall -W -Wextra -Wunused-variable -Wunused-parameter -Wunused-function -Wunused -Wno-system-headers -Woverloaded-virtual -Wwrite-strings -Wmissing-declarations" CACHE STRING "Flags used by the C++ compiler during maintainer builds.")
set(CMAKE_C_FLAGS_MAINTAINER "-g -O0 -Wall -pedantic -W" CACHE STRING "Flags used by the C compiler during maintainer builds.")
if(CXX_ACCEPTS_PROFILE_ARCS)
  set(CMAKE_CXX_FLAGS_MAINTAINER "${CMAKE_CXX_FLAGS_MAINTAINER} -fprofile-arcs")
  set(CMAKE_C_FLAGS_MAINTAINER "${CMAKE_C_FLAGS_MAINTAINER} -fprofile-arcs")
endif(CXX_ACCEPTS_PROFILE_ARCS)
if(CXX_ACCEPTS_TEST_COVERAGE)
  set(CMAKE_CXX_FLAGS_MAINTAINER "${CMAKE_CXX_FLAGS_MAINTAINER} -ftest-coverage")
  set(CMAKE_C_FLAGS_MAINTAINER "${CMAKE_C_FLAGS_MAINTAINER} -ftest-coverage")
endif(CXX_ACCEPTS_TEST_COVERAGE)

set(CMAKE_EXE_LINKER_FLAGS_MAINTAINER "-Wl,--warn-unresolved-symbols,--warn-once" CACHE STRING "Flags used for linking binaries during maintainer builds.")
set(CMAKE_SHARED_LINKER_FLAGS_MAINTAINER "-Wl,--warn-unresolved-symbols,--warn-once" CACHE STRING "Flags used by the shared libraries linker during maintainer builds.")

if(CXX_ACCEPTS_PROFILE_ARCS)
  set(CMAKE_EXE_LINKER_FLAGS_MAINTAINER "${CMAKE_EXE_LINKER_FLAGS_MAINTAINER} -fprofile-arcs")
  set(CMAKE_SHARED_LINKER_FLAGS_MAINTAINER "${CMAKE_SHARED_LINKER_FLAGS_MAINTAINER} -fprofile-arcs")
endif(CXX_ACCEPTS_PROFILE_ARCS)
if(CXX_ACCEPTS_TEST_COVERAGE)
  set(CMAKE_SHARED_LINKER_FLAGS_MAINTAINER "${CMAKE_SHARED_LINKER_FLAGS_MAINTAINER} -ftest-coverage")
endif(CXX_ACCEPTS_TEST_COVERAGE)

mark_as_advanced(
  CMAKE_CXX_FLAGS_MAINTAINER
  CMAKE_C_FLAGS_MAINTAINER
  CMAKE_EXE_LINKER_FLAGS_MAINTAINER
  CMAKE_SHARED_LINKER_FLAGS_MAINTAINER
)
