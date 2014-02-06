# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

if(NOT MSVC)
  return()
endif()

include_directories( build/workarounds/msvc )

##---------------------------------------------------
## Set MSVC specific compiler flags
##---------------------------------------------------

include(AddFlag)

try_add_cxx_flag(/MD)                   # Creates multithreaded DLLs using MSVCRT.lib
try_add_cxx_flag(/EHs)                  # Synchronous exception handling (TODO: check why)
try_add_cxx_flag(/bigobj)               # Allow big object files
try_add_cxx_flag(/MP)                   # Use multicore compilation where possible
try_add_cxx_flag(/W3        MAINTAINER) # More warnings in Maintainer builds

try_add_c_flag(/MD)
try_add_c_flag(/EHs)
try_add_c_flag(/bigobj)
try_add_c_flag(/MP)
try_add_c_flag(/W3          MAINTAINER)

add_definitions(-D_USE_MATH_DEFINES)