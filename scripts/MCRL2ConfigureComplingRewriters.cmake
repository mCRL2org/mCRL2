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

# Configure compiler
set(CC  ${CMAKE_C_COMPILER})
set(CXX ${CMAKE_CXX_COMPILER})

# Configure the default build options
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

# Do not use debug symbols in the rewriter, and build using the appropriate
# flags for the aterm library
set(R_CXXFLAGS "${R_CXXFLAGS} -std=c++98 -DNDEBUG ${ATERM_FLAGS}")

# Make sure the shared library for the rewriter is build using position
# independent code, if we were configured as static build ourselves, otherwise
# use the defaults.
if(NOT BUILD_SHARED_LIBS)
  set(R_CXXFLAGS "${R_CXXFLAGS} -fPIC")
else()
  set(R_CXXFLAGS "${R_CXXFLAGS} ${BUILD_SHARED_LIBS_OPTION}")
endif()

# Make sure we use shared linking.
if(NOT APPLE)
  set(R_LDFLAGS "-shared")
else ()
  set(R_LDFLAGS "-bundle -undefined dynamic_lookup")
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

set(R_CXXFLAGS "${R_CXXFLAGS} ${R_MCRL2_OSX_ARCH}")
set(R_LDFLAGS "${R_LDFLAGS} ${R_MCRL2_OSX_ARCH}")

# Configure including/linking the mcrl2 libraries and Boost.
if( APPLE AND MCRL2_SINGLE_BUNDLE AND CMAKE_INSTALL_PREFIX STREQUAL "/" )
  set(R_LDFLAGS "${R_LDFLAGS} -L\"../lib/mcrl2\"")
  set(R_LDFLAGS "${R_LDFLAGS} -L\"${BOOST_LIB}\"")
  set(R_CXXFLAGS "${R_CXXFLAGS} -I\"../include\"")
  set(R_CXXFLAGS "${R_CXXFLAGS} -I\"${BOOST_INCLUDE}\"")
else()
  set(R_LDFLAGS "${R_LDFLAGS} -L\"${CMAKE_INSTALL_PREFIX}/lib/mcrl2\"")
  set(R_LDFLAGS "${R_LDFLAGS} -L\"${Boost_LIBRARY_DIRS}\"")
  set(R_CXXFLAGS "${R_CXXFLAGS} -I\"${CMAKE_INSTALL_PREFIX}/include\"")
  set(R_CXXFLAGS "${R_CXXFLAGS} -I\"${Boost_INCLUDE_DIRS}\"")
endif()

# Configure including and linking for the stage build.
if(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set( MCRL2_COMPILE_REWRITER_SCRIPT_LOCATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mcrl2compilerewriter" )
  set(R_CXXFLAGS "${R_CXXFLAGS} -I\"${CMAKE_CURRENT_SOURCE_DIR}/libraries/aterm/include\"" )
  set(R_CXXFLAGS "${R_CXXFLAGS} -I\"${CMAKE_CURRENT_SOURCE_DIR}/libraries/atermpp/include\"" )
  set(R_CXXFLAGS "${R_CXXFLAGS} -I\"${CMAKE_CURRENT_SOURCE_DIR}/libraries/utilities/include\"" )
  set(R_CXXFLAGS "${R_CXXFLAGS} -I\"${CMAKE_CURRENT_SOURCE_DIR}/libraries/core/include\"" )
  set(R_CXXFLAGS "${R_CXXFLAGS} -I\"${CMAKE_CURRENT_SOURCE_DIR}/libraries/data/include\"" )
  set(R_LDFLAGS "${R_LDFLAGS} -L\"${CMAKE_LIBRARY_OUTPUT_DIRECTORY}\"")
else()
  set( MCRL2_COMPILE_REWRITER_SCRIPT_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/mcrl2compilerewriter" )
endif()

# Link against the aterm library; these symbols are not always exported by the
# calling executable
set(R_LDFLAGS "${R_LDFLAGS} -lmcrl2_aterm")

# Finally, include all the information in the mcrl2compilerewriter script
configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/build/autoconf/mcrl2compilerewriter.in" "${MCRL2_COMPILE_REWRITER_SCRIPT_LOCATION}" @ONLY )

# Mark the script as executable, and install it to the bianry directory.
add_executable( mcrl2compilerewriter IMPORTED IMPORTED_LOCATION "${MCRL2_COMPILE_REWRITER_SCRIPT_LOCATION}" )
install(
  PROGRAMS "${MCRL2_COMPILE_REWRITER_SCRIPT_LOCATION}"
  DESTINATION ${MCRL2_BIN_DIR}/
)

# Last but not least, generate the man pages.
if (MCRL2_MAN_PAGES)
  add_custom_target(mcrl2compilerewriter_man_page ALL)
  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/doc/sphinx/source/user_manual/tools/man/mcrl2compilerewriter.1 DESTINATION share/man/man1)
endif (MCRL2_MAN_PAGES)

