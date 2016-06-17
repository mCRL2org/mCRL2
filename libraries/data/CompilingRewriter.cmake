# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

set(R_NAME "mcrl2compilerewriter")
set(R_IN_PATH "source/${R_NAME}.in")
set(R_PATH "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${R_NAME}" )

# Configure compiler
get_filename_component(CC ${CMAKE_C_COMPILER} NAME)
get_filename_component(CXX ${CMAKE_CXX_COMPILER} NAME)

# Configure the default build options
set(R_CXXFLAGS ${CMAKE_CXX_FLAGS})
if(CMAKE_BUILD_TYPE)
  string(TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" R_CXXFLAGS_NAME)
  set(R_CXXFLAGS "${R_CXXFLAGS} ${${R_CXXFLAGS_NAME}}")
endif()

# Add compiler flags to allow to compile rewritercode with a large number
# of recursively used templates. 
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(R_CXXFLAGS "${R_CXXFLAGS} -ftemplate-depth=2000 -fbracket-depth=1024")
else()
  set(R_CXXFLAGS "${R_CXXFLAGS} -ftemplate-depth=2000")
endif()

# Add the other definitions that were added using add_definitions to build flags
get_directory_property(R_COMPILER_DEFINITIONS COMPILE_DEFINITIONS)
foreach(d ${R_COMPILER_DEFINITIONS})
  set(R_CXXFLAGS "${R_CXXFLAGS} -D${d}")
endforeach()

# Make sure we use shared linking.
if(NOT APPLE)
  set(R_LDFLAGS "-shared")
else()
  set(R_LDFLAGS "-bundle -undefined dynamic_lookup")
endif()

# Set MacOS-X specific compile flags
if(CMAKE_OSX_ARCHITECTURES)
  set(R_OSX_FLAGS  "-arch ${CMAKE_OSX_ARCHITECTURES}" )
  if(CMAKE_OSX_SYSROOT)
    set(R_OSX_FLAGS  "${R_OSX_FLAGS} -isysroot ${CMAKE_OSX_SYSROOT}" )
  endif()
endif()
if(CMAKE_OSX_DEPLOYMENT_TARGET)
  set(R_OSX_FLAGS "${R_OSX_FLAGS} -mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET}" )
endif()
set(R_CXXFLAGS "${R_CXXFLAGS} ${R_OSX_FLAGS}")
set(R_LDFLAGS "${R_LDFLAGS} ${R_OSX_FLAGS}")

# Find mCRL2 include directory
file(RELATIVE_PATH REL_INCLUDE_PATH ${CMAKE_INSTALL_PREFIX}/${MCRL2_RUNTIME_PATH} ${CMAKE_INSTALL_PREFIX}/${MCRL2_INCLUDE_PATH})

# Configure one version for deployment
set(R_INCLUDE_DIRS "-I\"`dirname $0`/${REL_INCLUDE_PATH}\"")
configure_file(${R_IN_PATH} "${R_PATH}.install" @ONLY)

# Configure one version for use in the build tree
set(R_INCLUDE_DIRS "-I\"${CMAKE_BINARY_DIR}/libraries/utilities\" ")
foreach(LIB "atermpp" "utilities" "core" "data")
  set(R_INCLUDE_DIRS "${R_INCLUDE_DIRS}-I\"${CMAKE_SOURCE_DIR}/libraries/${LIB}/include\" " )
endforeach()
configure_file(${R_IN_PATH} "${R_PATH}" @ONLY)

# Make the build tree version available in the rest of the build as a target
add_executable(${R_NAME} IMPORTED IMPORTED_LOCATION ${R_PATH})

# Make the deployment version available at install time
install(PROGRAMS ${R_PATH}.install RENAME ${R_NAME} DESTINATION ${MCRL2_RUNTIME_PATH} COMPONENT Runtime)
