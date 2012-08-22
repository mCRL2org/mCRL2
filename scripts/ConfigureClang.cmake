# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

##---------------------------------------------------
## Set Clang compiler flags 
##---------------------------------------------------

message( STATUS "Loading Clang specific configuration" )
##---------------------------------------------------
## Set C compile flags 
##---------------------------------------------------
include(AddFlag)

try_add_c_flag(-Wall)
try_add_c_flag(-Wno-inline)
try_add_c_flag(-pipe)

try_add_c_flag(-g                        MAINTAINER)
try_add_c_flag(-O0                       MAINTAINER)
try_add_c_flag(-pedantic                 MAINTAINER)
try_add_c_flag(-W                        MAINTAINER)

##---------------------------------------------------
## Set CXX compile flags 
##---------------------------------------------------

try_add_cxx_flag(-Wall)
try_add_cxx_flag(-Wno-inline)
try_add_cxx_flag(-pipe)

try_add_cxx_flag(-g                      MAINTAINER)
try_add_cxx_flag(-O0                     MAINTAINER)
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

if(BUILD_SHARED_LIBS)
    check_cxx_compiler_flag(-fPIC CXX_ACCEPTS_FPIC )
    check_c_compiler_flag(-fPIC C_ACCEPTS_FPIC )
    if( CXX_ACCEPTS_FPIC AND C_ACCEPTS_FPIC )
      set(CMAKE_CXX_FLAGS "-fPIC ${CMAKE_CXX_FLAGS}")
      set(CMAKE_C_FLAGS "-fPIC ${CMAKE_C_FLAGS}")
      set(BUILD_SHARED_LIBS_OPTION "-fPIC")
    endif( CXX_ACCEPTS_FPIC AND C_ACCEPTS_FPIC)
endif(BUILD_SHARED_LIBS)
