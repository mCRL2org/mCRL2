# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

set(CMAKE_EXE_LINKER_FLAGS_MAINTAINER "${CMAKE_EXE_LINKER_FLAGS_MAINTAINER}"
    CACHE STRING "Flags used for linking binaries during maintainer builds.")
set(CMAKE_SHARED_LINKER_FLAGS_MAINTAINER "${CMAKE_SHARED_LINKER_FLAGS_MAINTAINER}"
    CACHE STRING "Flags used by the shared libraries linker during maintainer builds.")
set(CMAKE_C_FLAGS_MAINTAINER "${CMAKE_C_FLAGS_MAINTAINER}" 
    CACHE STRING "Flags used by the C compiler during maintainer builds.")
set(CMAKE_CXX_FLAGS_MAINTAINER "${CMAKE_CXX_FLAGS_MAINTAINER}" 
    CACHE STRING "Flags used by the C++ compiler during maintainer builds.")

mark_as_advanced(
  CMAKE_CXX_FLAGS_MAINTAINER
  CMAKE_C_FLAGS_MAINTAINER
  CMAKE_EXE_LINKER_FLAGS_MAINTAINER
  CMAKE_SHARED_LINKER_FLAGS_MAINTAINER
)

