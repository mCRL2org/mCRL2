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

set ( R_STAGE_INCLUDE_DIRS "" )
if( CMAKE_RUNTIME_OUTPUT_DIRECTORY )
  set( MCRL2_COMPILE_REWRITER_SCRIPT_LOCATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mcrl2compilerewriter" )
  set( R_STAGE_INCLUDE_DIRS "${R_STAGE_INCLUDE_DIRS} -I\"${CMAKE_CURRENT_SOURCE_DIR}/libraries/aterm/include\"" )
  set( R_STAGE_INCLUDE_DIRS "${R_STAGE_INCLUDE_DIRS} -I\"${CMAKE_CURRENT_SOURCE_DIR}/libraries/atermpp/include\"" )
  set( R_STAGE_INCLUDE_DIRS "${R_STAGE_INCLUDE_DIRS} -I\"${CMAKE_CURRENT_SOURCE_DIR}/libraries/utilities/include\"" )
  set( R_STAGE_INCLUDE_DIRS "${R_STAGE_INCLUDE_DIRS} -I\"${CMAKE_CURRENT_SOURCE_DIR}/libraries/core/include\"" )
  set( R_STAGE_INCLUDE_DIRS "${R_STAGE_INCLUDE_DIRS} -I\"${CMAKE_CURRENT_SOURCE_DIR}/libraries/data/include\"" )
else( CMAKE_RUNTIME_OUTPUT_DIRECTORY )
  set( MCRL2_COMPILE_REWRITER_SCRIPT_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/mcrl2compilerewriter" )
endif( CMAKE_RUNTIME_OUTPUT_DIRECTORY )

if( APPLE AND MCRL2_SINGLE_BUNDLE AND CMAKE_INSTALL_PREFIX STREQUAL "/" )
  set( R_CMAKE_INSTALL_PREFIX "..")
  set( R_Boost_INCLUDE_DIRS "../Boost" )
  set( R_Boost_LIBRARY_DIRS "../Boost/lib" )

  set( R_MCRL2_BOOST_HEADER_FILES
    boost/assert.hpp
    boost/bind.hpp
    boost/call_traits.hpp
    boost/checked_delete.hpp
    boost/concept_check.hpp
    boost/config.hpp
    boost/current_function.hpp
    boost/get_pointer.hpp
    boost/implicit_cast.hpp
    boost/integer_traits.hpp
    boost/is_placeholder.hpp
    boost/iterator.hpp
    boost/lexical_cast.hpp
    boost/limits.hpp
    boost/mem_fn.hpp
    boost/memory_order.hpp
    boost/next_prior.hpp
    boost/noncopyable.hpp
    boost/ref.hpp
    boost/shared_ptr.hpp
    boost/smart_ptr.hpp
    boost/static_assert.hpp
    boost/throw_exception.hpp
    boost/type.hpp
    boost/utility.hpp
    boost/visit_each.hpp
  )

  set ( R_MCRL2_BOOST_HEADER_DIRS
    boost/bind
    boost/concept
    boost/config
    boost/detail
    boost/exception
    boost/iterator
    boost/mpl
    boost/numeric
    boost/preprocessor
    boost/range
    boost/smart_ptr
    boost/type_traits
    boost/utility
  )

  foreach( hfile ${R_MCRL2_BOOST_HEADER_FILES})
    string(FIND ${hfile} "/" IDX REVERSE)
    string(SUBSTRING ${hfile} 0 ${IDX} DEST)
    install(FILES "${Boost_INCLUDE_DIRS}/${hfile}" DESTINATION "Boost/${DEST}/")
  endforeach( hfile ${R_MCRL2_BOOST_HEADER_FILES})

  foreach( hdir ${R_MCRL2_BOOST_HEADER_DIRS})
    string(FIND ${hdir} "/" IDX REVERSE)
    string(SUBSTRING ${hdir} 0 ${IDX} DEST)
    install(DIRECTORY "${Boost_INCLUDE_DIRS}/${hdir}" DESTINATION "Boost/${DEST}/")
  endforeach( hdir ${R_MCRL2_BOOST_HEADER_DIRS})


else( APPLE AND MCRL2_SINGLE_BUNDLE AND CMAKE_INSTALL_PREFIX STREQUAL "/" )
  set( R_CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
  set( R_Boost_INCLUDE_DIRS "${Boost_INCLUDE_DIRS}" )
  set( R_Boost_LIBRARY_DIRS "${Boost_LIBRARY_DIRS}" )
endif( APPLE AND MCRL2_SINGLE_BUNDLE AND CMAKE_INSTALL_PREFIX STREQUAL "/" )

# Set MacOS-X specific compile flags
if( CMAKE_OSX_ARCHITECTURES )
  set(R_MCRL2_OSX_ARCH  "-arch ${CMAKE_OSX_ARCHITECTURES}" )
endif( CMAKE_OSX_ARCHITECTURES )
if( CMAKE_OSX_SYSROOT )
  set(R_MCRL2_OSX_ARCH  "${R_MCRL2_OSX_ARCH} -isysroot ${CMAKE_OSX_SYSROOT}" )
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

ENDIF( UNIX )
