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

if(NOT UNIX)
  return()
endif()

set(CC  ${CMAKE_C_COMPILER})
set(CXX ${CMAKE_CXX_COMPILER})

if(CMAKE_BUILD_TYPE)
  string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE)
  if(CMAKE_BUILD_TYPE MATCHES "release")
    set(R_CXXFLAGS ${CMAKE_CXX_FLAGS_RELEASE})
  endif()
  if(CMAKE_BUILD_TYPE MATCHES "debug")
    set(R_CXXFLAGS ${CMAKE_CXX_FLAGS_DEBUG})
  endif()
  if(CMAKE_BUILD_TYPE MATCHES "relwithdebinfo")
    set(R_CXXFLAGS ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
  endif()
endif()

set(R_CXXFLAGS "${R_CXXFLAGS} -std=c++98")
set(R_CPPFLAGS "-DNDEBUG")

if(NOT BUILD_SHARED_LIBS)
  set(R_SCXXFLAGS "-fPIC")
else()
  set(R_SCXXFLAGS ${BUILD_SHARED_LIBS_OPTION})
endif()

if(NOT APPLE)
  set(R_SLDFLAGS "-shared")
else ()
  set(R_SLDFLAGS "-bundle -undefined dynamic_lookup")
endif()

set(R_STAGE_INCLUDE_DIRS "")
if(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set( MCRL2_COMPILE_REWRITER_SCRIPT_LOCATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mcrl2compilerewriter" )
  set( R_STAGE_INCLUDE_DIRS "${R_STAGE_INCLUDE_DIRS} -I\"${CMAKE_CURRENT_SOURCE_DIR}/libraries/aterm/include\"" )
  set( R_STAGE_INCLUDE_DIRS "${R_STAGE_INCLUDE_DIRS} -I\"${CMAKE_CURRENT_SOURCE_DIR}/libraries/atermpp/include\"" )
  set( R_STAGE_INCLUDE_DIRS "${R_STAGE_INCLUDE_DIRS} -I\"${CMAKE_CURRENT_SOURCE_DIR}/libraries/utilities/include\"" )
  set( R_STAGE_INCLUDE_DIRS "${R_STAGE_INCLUDE_DIRS} -I\"${CMAKE_CURRENT_SOURCE_DIR}/libraries/core/include\"" )
  set( R_STAGE_INCLUDE_DIRS "${R_STAGE_INCLUDE_DIRS} -I\"${CMAKE_CURRENT_SOURCE_DIR}/libraries/data/include\"" )
else()
  set( MCRL2_COMPILE_REWRITER_SCRIPT_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/mcrl2compilerewriter" )
endif()

if( APPLE AND MCRL2_SINGLE_BUNDLE AND CMAKE_INSTALL_PREFIX STREQUAL "/" )
  set( R_CMAKE_INSTALL_PREFIX "..")
  set( R_Boost_INCLUDE_DIRS "$${BOOST_INCLUDE}" )
  set( R_Boost_LIBRARY_DIRS "$${BOOST_LIB}" )
else()
  set( R_CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
  set( R_Boost_INCLUDE_DIRS "${Boost_INCLUDE_DIRS}" )
  set( R_Boost_LIBRARY_DIRS "${Boost_LIBRARY_DIRS}" )
endif()

# Set MacOS-X specific compile flags
if( CMAKE_OSX_ARCHITECTURES )
  set(R_MCRL2_OSX_ARCH  "-arch ${CMAKE_OSX_ARCHITECTURES}" )
endif( CMAKE_OSX_ARCHITECTURES )
if( CMAKE_OSX_SYSROOT )
  if (EXISTS ${R_MCRL2_OSX_ARCH})
    set(R_MCRL2_OSX_ARCH  "${R_MCRL2_OSX_ARCH} -isysroot ${CMAKE_OSX_SYSROOT}" )
  endif (EXISTS ${R_MCRL2_OSX_ARCH})
endif( CMAKE_OSX_SYSROOT )
if( CMAKE_OSX_DEPLOYMENT_TARGET )
  set(R_MCRL2_OSX_ARCH "${R_MCRL2_OSX_ARCH} -mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET}" )
endif( CMAKE_OSX_DEPLOYMENT_TARGET )

configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/build/autoconf/mcrl2compilerewriter.in" "${MCRL2_COMPILE_REWRITER_SCRIPT_LOCATION}" @ONLY )

add_executable( mcrl2compilerewriter IMPORTED IMPORTED_LOCATION "${MCRL2_COMPILE_REWRITER_SCRIPT_LOCATION}" )
install(
  PROGRAMS "${MCRL2_COMPILE_REWRITER_SCRIPT_LOCATION}"
  DESTINATION ${MCRL2_BIN_DIR}/
)


if (MCRL2_MAN_PAGES)
  add_custom_target(mcrl2compilerewriter_man_page ALL)
  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/doc/sphinx/source/user_manual/tools/man/mcrl2compilerewriter.1 DESTINATION share/man/man1)
endif (MCRL2_MAN_PAGES)

