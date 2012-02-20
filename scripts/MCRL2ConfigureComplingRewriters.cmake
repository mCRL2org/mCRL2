# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

##---------------------------------------------------
## Configure compiling rewriters
##---------------------------------------------------

IF( UNIX )

set(CC  ${CMAKE_C_COMPILER})
set(CXX ${CMAKE_CXX_COMPILER})

if( CMAKE_BUILD_TYPE )
  string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE)
  if(CMAKE_BUILD_TYPE MATCHES "release")
    set(R_CXXFLAGS ${CMAKE_CXX_FLAGS_RELEASE})
  endif(CMAKE_BUILD_TYPE MATCHES "release")
  if(CMAKE_BUILD_TYPE MATCHES "debug")
    set(R_CXXFLAGS ${CMAKE_CXX_FLAGS_DEBUG})
  endif(CMAKE_BUILD_TYPE MATCHES "debug")
  if(CMAKE_BUILD_TYPE MATCHES "relwithdebinfo")
    set(R_CXXFLAGS ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
  endif(CMAKE_BUILD_TYPE MATCHES "relwithdebinfo")
endif( CMAKE_BUILD_TYPE )

if(CMAKE_C_COMPILER MATCHES "icc")
    set(R_CXXFLAGS "${R_CXXFLAGS} -Wno-unused -rdynamic")
else(CMAKE_C_COMPILER MATCHES "icc")
  check_cxx_compiler_flag(-Wno-unused C_ACCEPTS_WNO-UNUSED )
  if( C_ACCEPTS_WNO-UNUSED )
    set(R_CXXFLAGS "${R_CXXFLAGS} -Wno-unused")
  endif( C_ACCEPTS_WNO-UNUSED )

  check_cxx_compiler_flag(-rdynamic C_ACCEPTS_RDYNAMIC )
  if( C_ACCEPTS_RDYNAMIC )
    set(R_CXXFLAGS "${R_CXXFLAGS} -rdynamic")
  endif( C_ACCEPTS_RDYNAMIC )
endif(CMAKE_C_COMPILER MATCHES "icc")

set(R_CXXFLAGS "${R_CXXFLAGS} -std=c++98")
set(R_CPPFLAGS "-DNDEBUG")

if(NOT BUILD_SHARED_LIBS)
  set(R_SCXXFLAGS "-fPIC")
else(NOT BUILD_SHARED_LIBS)
  set(R_SCXXFLAGS ${BUILD_SHARED_LIBS_OPTION})
endif(NOT BUILD_SHARED_LIBS)

if( NOT APPLE )
  set(R_SLDFLAGS "-shared")
else ( NOT APPLE )
  set(R_SLDFLAGS "-bundle -undefined dynamic_lookup")
endif( NOT APPLE )

if( CMAKE_RUNTIME_OUTPUT_DIRECTORY )
  set( MCRL2_COMPILE_REWRITER_SCRIPT_LOCATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mcrl2compilerewriter" )
else( CMAKE_RUNTIME_OUTPUT_DIRECTORY )
  set( MCRL2_COMPILE_REWRITER_SCRIPT_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/mcrl2compilerewriter" )
endif( CMAKE_RUNTIME_OUTPUT_DIRECTORY )
configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/build/autoconf/mcrl2compilerewriter.in" "${MCRL2_COMPILE_REWRITER_SCRIPT_LOCATION}" @ONLY )
add_executable( mcrl2compilerewriter IMPORTED IMPORTED_LOCATION "${MCRL2_COMPILE_REWRITER_SCRIPT_LOCATION}" )
install( 
  PROGRAMS "${MCRL2_COMPILE_REWRITER_SCRIPT_LOCATION}"
  DESTINATION bin/
)

ENDIF( UNIX )
