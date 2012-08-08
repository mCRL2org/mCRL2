# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

include_directories( build/workarounds/msvc )
set(BUILD_SHARED_LIBS OFF)

##---------------------------------------------------
## Set MSVC specific compiler flags
##---------------------------------------------------

include(AddFlag)

try_add_cxx_flag(/MD)
try_add_cxx_flag(/EHs)
try_add_cxx_flag(/bigobj)
try_add_c_flag(/MD)
try_add_c_flag(/EHs)
try_add_c_flag(/bigobj)

## Compile multi-core  
option(MCRL2_ENABLE_MULTICORE_COMPILATION "Enable/disable multi-core compilation" ON)
if( MCRL2_ENABLE_MULTICORE_COMPILATION )
  try_add_cxx_flag(/MP)
  try_add_c_flag(/MP)
endif( MCRL2_ENABLE_MULTICORE_COMPILATION )
