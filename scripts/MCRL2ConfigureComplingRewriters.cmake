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
set(R_CXXFLAGS ${CMAKE_CXX_FLAGS})
if(CMAKE_BUILD_TYPE)
  string(TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" R_CXXFLAGS_NAME)
  set(R_CXXFLAGS "${R_CXXFLAGS} ${${R_CXXFLAGS_NAME}}")
endif()

# Add build type define for version information (needed by toolset_version_const.h)
if(CMAKE_CFG_INTDIR STREQUAL ".")
  set(R_CXXFLAGS "${R_CXXFLAGS} -DMCRL2_BUILD_TYPE=\"\\\"${CMAKE_BUILD_TYPE}\\\"\"")
else()
  set(R_CXXFLAGS "${R_CXXFLAGS} -DMCRL2_BUILD_TYPE=\"\\\"${CMAKE_CFG_INTDIR}\\\"\"")
endif()

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
  if( CMAKE_OSX_SYSROOT )
    set(R_MCRL2_OSX_ARCH  "${R_MCRL2_OSX_ARCH} -isysroot ${CMAKE_OSX_SYSROOT}" )
  endif( CMAKE_OSX_SYSROOT )
endif( CMAKE_OSX_ARCHITECTURES )
if( CMAKE_OSX_DEPLOYMENT_TARGET )
  set(R_MCRL2_OSX_ARCH "${R_MCRL2_OSX_ARCH} -mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET}" )
endif( CMAKE_OSX_DEPLOYMENT_TARGET )
set(R_CXXFLAGS "${R_CXXFLAGS} ${R_MCRL2_OSX_ARCH}")
set(R_LDFLAGS "${R_LDFLAGS} ${R_MCRL2_OSX_ARCH}")

# Include configuration of Apple specific hacks for boost libraries.
include(MCRL2MacOSXCopyBoostHeaders)

# Configure including/linking the mcrl2 libraries and Boost.
# On Apple, if we build a single bundle application, we know that header files
# are installed relative to the mcrl2compilerewriter script, and the
# mcrl2compilerewriter script is always called with an absolute path,
# see jittyc.cpp. The dirname statement takes the directory of the mcrl2compilerewriter
# script, and uses that path to search for the header files.
if( APPLE AND MCRL2_OSX_PACKAGE )
  set(R_CXXFLAGS "${R_CXXFLAGS} -I\"`dirname $0`/../${MCRL2_INCLUDE_DIR}\"")
else()
  set(R_CXXFLAGS "${R_CXXFLAGS} -I\"${CMAKE_INSTALL_PREFIX}/${MCRL2_INCLUDE_DIR}\"")
  set(R_CXXFLAGS "${R_CXXFLAGS} -I\"${Boost_INCLUDE_DIRS}\"")
endif()

# Configure including and linking for the stage build.
if(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set( MCRL2_COMPILE_REWRITER_SCRIPT_LOCATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mcrl2compilerewriter" )
  set(R_CXXFLAGS "${R_CXXFLAGS} -I\"${CMAKE_CURRENT_SOURCE_DIR}/libraries/aterm/include\"" )
  set(R_CXXFLAGS "${R_CXXFLAGS} -I\"${CMAKE_CURRENT_SOURCE_DIR}/libraries/atermpp/include\"" )
  set(R_CXXFLAGS "${R_CXXFLAGS} -I\"${CMAKE_CURRENT_SOURCE_DIR}/libraries/utilities/include\"" )
  set(R_CXXFLAGS "${R_CXXFLAGS} -I\"${CMAKE_CURRENT_BINARY_DIR}/libraries/utilities/include\"" )
  set(R_CXXFLAGS "${R_CXXFLAGS} -I\"${CMAKE_CURRENT_SOURCE_DIR}/libraries/core/include\"" )
  set(R_CXXFLAGS "${R_CXXFLAGS} -I\"${CMAKE_CURRENT_SOURCE_DIR}/libraries/data/include\"" )
else()
  set( MCRL2_COMPILE_REWRITER_SCRIPT_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/mcrl2compilerewriter" )
endif()

# Finally, include all the information in the mcrl2compilerewriter script
configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/build/autoconf/mcrl2compilerewriter.in" "${MCRL2_COMPILE_REWRITER_SCRIPT_LOCATION}" @ONLY )

# Mark the script as executable, and install it to the bianry directory.
add_executable( mcrl2compilerewriter IMPORTED IMPORTED_LOCATION "${MCRL2_COMPILE_REWRITER_SCRIPT_LOCATION}" )
install(
  PROGRAMS "${MCRL2_COMPILE_REWRITER_SCRIPT_LOCATION}"
  DESTINATION ${MCRL2_BIN_DIR}/
  COMPONENT Applications
)

# Last but not least, generate the man pages.
if (MCRL2_MAN_PAGES)
  add_custom_target(mcrl2compilerewriter_man_page ALL)
  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/doc/sphinx/source/user_manual/tools/man/mcrl2compilerewriter.1 DESTINATION ${MCRL2_SHARE_DIR}/${MCRL2_MAN_DIR} COMPONENT Documentation)
endif (MCRL2_MAN_PAGES)

