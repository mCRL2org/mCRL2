# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://github.com/mCRL2org/mCRL2/blob/master/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

set(R_NAME "mcrl2compilerewriter")
set(R_IN_PATH "source/${R_NAME}.in")
set(R_PATH "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${R_NAME}" )

# Find the compiler path
get_filename_component(CC_PATH ${CMAKE_C_COMPILER} NAME)
get_filename_component(CXX_PATH ${CMAKE_CXX_COMPILER} NAME)

# Add the standard flags
set(R_CXXFLAGS ${CMAKE_CXX_FLAGS})
if(CMAKE_BUILD_TYPE)
  # Add build specific configuration flags, does not work for multi-config generators.
  string(TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" R_CXXFLAGS_NAME)
  set(R_CXXFLAGS "${R_CXXFLAGS} ${${R_CXXFLAGS_NAME}}")
endif()

# Add the MCRL2_JITTYC_ARGUMENTS, and c++17 since it is not included by default.
set(R_CXXFLAGS "${R_CXXFLAGS} -std=c++17 ${MCRL2_JITTYC_ARGUMENTS}")

# Add compiler flags to allow to compile rewritercode with a large number
# of recursively used templates. The value 2000 is taken
# rather arbitrarily, and may need to be increased for more complex
# rewriters. The default on some compilers appears to be 900.
check_cxx_compiler_flag(-ftemplate-depth=2000 CXX_ACCEPTS_TEMPLATE_DEPTH )
if (CXX_ACCEPTS_TEMPLATE_DEPTH)
  set(R_CXXFLAGS "${R_CXXFLAGS} -ftemplate-depth=2000")
endif()

# The resulting shared object always required position independent code.
check_cxx_compiler_flag(-fPIC CXX_ACCEPTS_PIC)
if (CXX_ACCEPTS_PIC)
  set(R_CXXFLAGS "${R_CXXFLAGS} -fPIC")
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

if(MCRL2_ENABLE_CODE_COVERAGE)
  set(R_LDFLAGS "${R_LDFLAGS} -fprofile-arcs")
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
configure_file(${R_IN_PATH} ${R_PATH}.install @ONLY)

# Configure one version for use in the build tree
set(R_INCLUDE_DIRS "-I\"${CMAKE_BINARY_DIR}/libraries/utilities\" ")
foreach(LIB "atermpp" "utilities" "core" "data")
  set(R_INCLUDE_DIRS "${R_INCLUDE_DIRS}-I\"${CMAKE_SOURCE_DIR}/libraries/${LIB}/include\" " )
endforeach()
configure_file(${R_IN_PATH} ${R_PATH} @ONLY)

# Make the deployment version available at install time
install(PROGRAMS ${R_PATH}.install RENAME ${R_NAME} DESTINATION ${MCRL2_RUNTIME_PATH} COMPONENT Runtime)
