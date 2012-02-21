# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required (VERSION 2.6)
message(STATUS "CMAKE VERSION: ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}")
project (MCRL2)
# Commonly used options
# ---------------------
#
# The following lists some commonly used options to configure the build system
# (an option name following by (*) indicates the default option):
#
#   CMAKE_INSTALL_PREFIX = /usr/local(*) (any path can be used)
#   CMAKE_BUILD_TYPE     = None | Debug | Release(*) | RelwithDebInfo | MinSizeRel
#   BUILD_SHARED_LIBS    = ON    | OFF(*)
#   MCRL2_ENABLE_EXPERIMENTAL  = ON    | OFF(*)
#   MCRL2_ENABLE_DEPRECATED    = ON    | OFF(*)


# Note on adding definitions containing quotes (")
# ------------------------------------------------
#
# Compiler definitions can be added to CMake using the ADD_DEFINITIONS command
# or COMPILE_DEFINITIONS property. Be careful when adding a definition that
# contains quotes ("), as CMake cannot guarantee the generation of platform
# independent scripts. Typically these definitions are of the form FOO="bar",
# which can be added as follows:
#
#   add_definitions(-DFOO="bar")
#
# From this, CMake is able to generate correct Makefiles. However, CMake is
# not able to generate a correct Eclipse CDT4 project.
#
# The solution that is currently employed is by adding compiler definitions
# that contain quotes using the set_source_files_properties command with the
# the COMPILE_DEFINITIONS property:
#
#   set_source_files_properties(baz.cpp
#     PROPERTIES COMPILE_DEFINITIONS FOO="bar"
#   )
#
# This way, these compiler definitions are not included in the Eclipse CDT4
# project file, circumventing the problem.
#
# When this solution cannot be applied, there is another workaround: by quoting
# the entire compiling definition it is prevented from being incorporated in
# the Eclipse CDT4 project:
#
#   add_definitions(-D"FOO=\\"bar\\"")
#
# However, the best solution is to avoid the need for these quotes in the
# first place. This is mentioned as a disclaimer in the CMake 2.6 documentation
# for the COMPILER_DEFINITIONS property:
#
#   Dislaimer: Most native build tools have poor support for escaping
#   certain values. CMake has work-arounds for many cases but some values
#   may just not be possible to pass correctly. If a value does not seem to
#   be escaped correctly, do not attempt to work-around the problem by
#   adding escape sequences to the value. Your work-around may break in a
#   future version of CMake that has improved escape support. Instead
#   consider defining the macro in a (configured) header file. Then report
#   the limitation.
#
# The disclaimer with some additional information can be found here:
#
#   http://www.cmake.org/cmake/help/cmake2.6docs.html#prop_dir:COMPILE_DEFINITIONS
#

set( MCRL2_ENABLE_SVN true )
set( MCRL2_ENABLE_PARSERS true )

IF( "${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" STRLESS "2.8" )
  message(STATUS "=========================================" )
  message(STATUS "CMake version 2.8+ is required for:" )
  message(STATUS "  svn revision in tools" )
  message(STATUS "  generating parsers" )
  message(STATUS "Current version of CMake is NOT suitable" )
  message(STATUS "for development purposes." )
  message(STATUS "=========================================" )

  set( MCRL2_ENABLE_SVN false )
  set( MCRL2_ENABLE_PARSERS false )
ENDIF( "${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" STRLESS "2.8" )

# Set path where additional modules can be found
set( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/scripts" )

# Variables required for an additional build type "Maintainer"

include(CheckCXXCompilerFlag)

##---------------------------------------------------
## Add "Maintainer" Build type
##---------------------------------------------------
include(MCRL2AddMaintainerBuildType)

##---------------------------------------------------
## Determine build type
##---------------------------------------------------

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE} " )

##---------------------------------------------------
## Print CMake install prefix (if any)
##---------------------------------------------------

message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

##-----------------------------------------------------
## Set platform specific options
##-----------------------------------------------------
if(WIN32)
  include(ConfigureWindows)
endif(WIN32)
if(UNIX AND NOT APPLE)
  include(ConfigureUnix)
endif(UNIX AND NOT APPLE)
if(APPLE)
  include(ConfigureApple)
endif(APPLE)
set(MCRL2_LIB_DIR "lib/mcrl2")

message(STATUS "BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}")

##-----------------------------------------------------
## Set runtime path variable RPATH for install targets
##-----------------------------------------------------

# use, i.e. don't skip the full RPATH for the build tree
set(CMAKE_SKIP_BUILD_RPATH false)

# when building, don't use the install RPATH already
# (but later on when installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH false)

# the RPATH to be used when installing (non-OSX, use CMAKE_INSTALL_NAME_DIR instead)
if(NOT CMAKE_INSTALL_RPATH)
  set(CMAKE_INSTALL_RPATH "$ORIGIN/../${MCRL2_LIB_DIR}")
endif(NOT CMAKE_INSTALL_RPATH)

# Mac OSX directory name for installed targets
# Installing prerequisite shared libraries is dealt with in "RelocateInstallTree.cmake"
set(CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/${MCRL2_LIB_DIR}")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH true)


##---------------------------------------------------
## Print compiler information
##---------------------------------------------------
include(GetCompilerVersion)

##---------------------------------------------------
## Print architecture information
##---------------------------------------------------
include(GetArchitectureInformation)

##-----------------------------------------------------
## Set Compiler specific options
##-----------------------------------------------------
if( CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX )
  include(ConfigureGCC)
endif( CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX )
if( CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_C_COMPILER_ID STREQUAL "Clang" )
  include(ConfigureClang)
endif( CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_C_COMPILER_ID STREQUAL "Clang" )
if(MSVC)
  include(ConfigureMSVC)
endif(MSVC)

##---------------------------------------------------
## Configure Boost
##---------------------------------------------------
include(ConfigureBoost)

##---------------------------------------------------
## Option to enable/disable mcrl2-gui tools
##---------------------------------------------------
option(MCRL2_ENABLE_GUI_TOOLS "Enable/disable creation of GUI tools" ON)

##---------------------------------------------------
## Detect OpenGL
##---------------------------------------------------

if( MCRL2_ENABLE_GUI_TOOLS )
  include(FindOpenGL)
  message(STATUS "OpenGL: ${OPENGL_FOUND}")

  if(NOT OPENGL_FOUND)
    message( FATAL_ERROR "OpenGL not found. Set MCRL2_ENABLE_GUI_TOOLS to FALSE to build without GUI-tools")
  else( OPENGL_FOUND )
    if ( APPLE )
      include_directories( ${OPENGL_INCLUDE_DIR} )
      link_directories(${OPENGL_LIBRARIES})
      set(CMAKE_EXE_LINKER_FLAGS "-framework OpenGL ${CMAKE_EXE_LINKER_FLAGS}")
      set(CMAKE_SHARED_LINKER_FLAGS "-framework OpenGL ${CMAKE_SHARED_LINKER_FLAGS}")
      set(CMAKE_MODULE_LINKER_FLAGS "-framework OpenGL ${CMAKE_SHARED_LINKER_FLAGS}")
    endif (APPLE)
  endif(NOT OPENGL_FOUND)
endif( MCRL2_ENABLE_GUI_TOOLS )

##---------------------------------------------------
## Configure wxWidgets
##---------------------------------------------------
include(ConfigureWxWidgets)

##---------------------------------------------------
## Determine latest SVN revision
##---------------------------------------------------

include(MCRL2Version)
message(STATUS "mCRL2 version: ${MCRL2_VERSION}")
message(STATUS "SVN data: ${Project_WC_LAST_CHANGED_DATE}")

##---------------------------------------------------
## Store copyright information in variable
##---------------------------------------------------
file(READ COPYING MCRL2_COPYRIGHT_TEXT)

##---------------------------------------------------
## Find cvc3
##---------------------------------------------------

find_package( cvc3 )
if( cvc3_FOUND )
  message(STATUS "CVC3 version: ${cvc3_VERSION}")
endif( cvc3_FOUND )

##---------------------------------------------------
## Find gl2ps library, and use if available
##---------------------------------------------------
if(MCRL2_ENABLE_GUI_TOOLS)
  find_package(gl2ps)
  if( GL2PS_FOUND )
    message(STATUS "using system gl2ps library")
  else( GL2PS_FOUND )
    message(STATUS "using mcrl2-provided gl2ps library")

    ##---------------------------------------------------
    ## Find mlib / required by gl2ps to resolve undefined symbols
    ##---------------------------------------------------
    if(UNIX)
      find_package( mlib )
      if( mlib_FOUND )
        message(STATUS "mlib found")
      else( mlib_FOUND )
        message( FATAL_ERROR "mlib is required")
      endif( mlib_FOUND )
    endif(UNIX)
  endif( GL2PS_FOUND)
endif(MCRL2_ENABLE_GUI_TOOLS)

##---------------------------------------------------
## TODO: Find dparser
##---------------------------------------------------

##---------------------------------------------------
## Option to enable/disable CADP support
##---------------------------------------------------
option(MCRL2_ENABLE_CADP_SUPPORT "Enable/disable support for CADP" OFF)

##---------------------------------------------------
## Setup dependency to CADP
##---------------------------------------------------

include(MCRL2CADPSupport)

##---------------------------------------------------
## Library and executable staging
##---------------------------------------------------

set(MCRL2_STAGE_ROOTDIR ""
  CACHE PATH "Path to stage executables and libraries." )
  message( STATUS "MCRL2_STAGE_ROOTDIR: ${MCRL2_STAGE_ROOTDIR}" )
if( MCRL2_STAGE_ROOTDIR )
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${MCRL2_STAGE_ROOTDIR}/bin")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${MCRL2_STAGE_ROOTDIR}/lib")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${MCRL2_STAGE_ROOTDIR}/share/mcrl2")
endif( MCRL2_STAGE_ROOTDIR )

##---------------------------------------------------
## 3rd party dependencies
##---------------------------------------------------

add_subdirectory ( 3rd-party/svc )
if(MCRL2_ENABLE_GUI_TOOLS)
  add_subdirectory ( 3rd-party/tr)
  if(NOT GL2PS_FOUND)
    add_subdirectory ( 3rd-party/gl2ps )
  endif(NOT GL2PS_FOUND)
endif(MCRL2_ENABLE_GUI_TOOLS)

##---------------------------------------------------
## DParser
##---------------------------------------------------

add_subdirectory ( 3rd-party/dparser )
add_subdirectory ( tools/mcrl2parse )

##---------------------------------------------------
## Library dependencies
##---------------------------------------------------

add_subdirectory ( libraries/aterm )
add_subdirectory ( libraries/atermpp )
add_subdirectory ( libraries/bes )
add_subdirectory ( libraries/core )
add_subdirectory ( libraries/data )
add_subdirectory ( libraries/lps )
add_subdirectory ( libraries/lts )
add_subdirectory ( libraries/pbes )
add_subdirectory ( libraries/process )
add_subdirectory ( libraries/trace )
add_subdirectory ( libraries/utilities )

##---------------------------------------------------
## Tool dependencies
##---------------------------------------------------

add_subdirectory ( tools/besinfo )
add_subdirectory ( tools/bespp )
add_subdirectory ( tools/diagraphica )
add_subdirectory ( tools/formulacheck )
add_subdirectory ( tools/grapemcrl2 )
add_subdirectory ( tools/lps2lts )
add_subdirectory ( tools/lps2pbes )
add_subdirectory ( tools/lps2torx )
add_subdirectory ( tools/lpsactionrename )
add_subdirectory ( tools/lpsbinary )
add_subdirectory ( tools/lpsconfcheck )
add_subdirectory ( tools/lpsconstelm )
add_subdirectory ( tools/lpsinfo )
add_subdirectory ( tools/lpsinvelm )
add_subdirectory ( tools/lpsparelm )
add_subdirectory ( tools/lpsparunfold )
add_subdirectory ( tools/lpspp )
add_subdirectory ( tools/lpsrewr )
add_subdirectory ( tools/lpssumelm )
add_subdirectory ( tools/lpssuminst )
add_subdirectory ( tools/lpsuntime )
add_subdirectory ( tools/lpssim )
add_subdirectory ( tools/lpsxsim )
add_subdirectory ( tools/lts2lps )
add_subdirectory ( tools/lts2pbes )
add_subdirectory ( tools/ltsconvert )
add_subdirectory ( tools/ltscompare )
add_subdirectory ( tools/ltsinfo )
add_subdirectory ( tools/ltsgraph )
add_subdirectory ( tools/ltsview )
add_subdirectory ( tools/mcrl22lps )
add_subdirectory ( tools/mcrl2i )
add_subdirectory ( tools/mcrl2xi )
add_subdirectory ( tools/mcrl2-gui )
add_subdirectory ( tools/pbes2bool )
add_subdirectory ( tools/pbes2bes )
add_subdirectory ( tools/pbesconstelm )
add_subdirectory ( tools/pbesinfo )
add_subdirectory ( tools/pbesparelm )
add_subdirectory ( tools/pbespgsolve )
add_subdirectory ( tools/pbespp )
add_subdirectory ( tools/pbesrewr )
add_subdirectory ( tools/tbf2lps )
add_subdirectory ( tools/tracepp )
add_subdirectory ( tools/txt2pbes )
add_subdirectory ( tools/txt2lps )

## Compile installer tools for mac
if(APPLE AND MCRL2_SINGLE_BUNDLE)
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/build/macosx)

  # Install "install_tools" in share/installer
  # install_tools contains the tools marked for symlink export
  install(FILES ${CMAKE_BINARY_DIR}/install_tools DESTINATION share/installer)
  #Process Info.plist with Copyright Information
  configure_file( ${CMAKE_SOURCE_DIR}/build/macosx/Info.plist.in ${CMAKE_BINARY_DIR}/build/macosx/Info.plist @ONLY)
  #Install Info.plist for single application bundle
  install(FILES ${CMAKE_BINARY_DIR}/build/macosx/Info.plist DESTINATION . )
  #Install mcrl2-logo for single application bundle
  install(FILES ${CMAKE_SOURCE_DIR}/build/macosx/mcrl2-logo.icns DESTINATION Resources )
endif(APPLE AND MCRL2_SINGLE_BUNDLE)

##---------------------------------------------------
## Experimental tool dependencies
##---------------------------------------------------

option(MCRL2_ENABLE_EXPERIMENTAL "Enable/disable creation of experimental tools" OFF)
message(STATUS "MCRL2_ENABLE_EXPERIMENTAL: ${MCRL2_ENABLE_EXPERIMENTAL}" )

if(MCRL2_ENABLE_EXPERIMENTAL)
  add_subdirectory ( tools/besconvert )
  add_subdirectory ( tools/bessolve )
  add_subdirectory ( tools/lpsbisim2pbes )
  add_subdirectory ( tools/lpsrealelm )
  add_subdirectory ( tools/pbesabstract )
  add_subdirectory ( tools/pbesabsinthe )
  add_subdirectory ( tools/pbesinst )
  add_subdirectory ( tools/pbespareqelm )
  add_subdirectory ( tools/txt2bes )
endif(MCRL2_ENABLE_EXPERIMENTAL)

##---------------------------------------------------
## Deprecated tool dependencies
##---------------------------------------------------

option(MCRL2_ENABLE_DEPRECATED "Enable/disable creation of deprecated tools" OFF)
message(STATUS "MCRL2_ENABLE_DEPRECATED: ${MCRL2_ENABLE_DEPRECATED}" )

if(MCRL2_ENABLE_DEPRECATED)
  add_subdirectory ( tools/ltsmin )
endif(MCRL2_ENABLE_DEPRECATED)

##---------------------------------------------------
## Documentation
##---------------------------------------------------

# Documentation requires python
include(ConfigurePython)
# Documentation requires doxygen
include(ConfigureDoxygen)
# Documentation requires xsltproc
include(FindXsltproc)

if(PYTHONINTERP_FOUND)
  if ( MCRL2_PY_SPHINX )
    if ( MCRL2_PY_ARGPARSE )
      if( DOXYGEN_FOUND )
        if (XSLTPROC )
          add_subdirectory ( doc/sphinx )
        else( XSLTPROC )
          message( STATUS "Documentation cannot be generated: xsltproc is missing." )
        endif( XSLTPROC)
      else( DOXYGEN_FOUND )
        message( STATUS "Documentation cannot be generated: doxygen is missing." )
      endif( DOXYGEN_FOUND )
    else ( MCRL2_PY_ARGPARSE )
      message( STATUS "Documentation cannot be generated: Python argparse-module is missing." )
    endif( MCRL2_PY_ARGPARSE )
  else ( MCRL2_PY_SPHINX )
    message( STATUS "Documentation cannot be generated: Python sphinx-module is missing." )
  endif( MCRL2_PY_SPHINX )
elseif(PYTHONINTERP_FOUND)
  message( STATUS "Documentation cannot be generated: Python is missing." )
endif(PYTHONINTERP_FOUND)

##---------------------------------------------------
## Configure compiling rewriters
##---------------------------------------------------

include(MCRL2ConfigureComplingRewriters)

##---------------------------------------------------
## Install headers
## --------------------------------------------------

option(MCRL2_INSTALL_HEADERS "Enable/disable install headers" ON)
message(STATUS "MCRL2_INSTALL_HEADERS: ${MCRL2_INSTALL_HEADERS}" )

if ( MCRL2_INSTALL_HEADERS )
install(
  DIRECTORY
    libraries/aterm/include/
    libraries/atermpp/include/
    libraries/core/include/
    libraries/data/include/
    libraries/lps/include/
    libraries/lts/include/
    libraries/bes/include/
    libraries/pbes/include/
    libraries/process/include/
    libraries/trace/include/
    libraries/utilities/include/
  DESTINATION include
  PATTERN ".svn" EXCLUDE
)
FILE(GLOB dparser_headers "3rd-party/dparser/*.h")
install(
  FILES
    ${dparser_headers}
  DESTINATION include
)
endif( MCRL2_INSTALL_HEADERS )

##---------------------------------------------------
## Install examples
## --------------------------------------------------

option(MCRL2_INSTALL_EXAMPLES "Enable/disable install examples" ON)
message(STATUS "MCRL2_INSTALL_EXAMPLES: ${MCRL2_INSTALL_EXAMPLES}" )

if ( MCRL2_INSTALL_EXAMPLES )
install(
  DIRECTORY examples/
  DESTINATION share/doc/mcrl2/examples
  PATTERN ".svn" EXCLUDE
)
endif ( MCRL2_INSTALL_EXAMPLES )


##---------------------------------------------------
## Create test targets
##---------------------------------------------------

include(MCRL2TestTargets)

##---------------------------------------------------
## Create make targets
##---------------------------------------------------

# Create make targets requires python, covered by documentation
include(MCRL2MakeTargets)

##---------------------------------------------------
## Packaging
##---------------------------------------------------

## Preprocessing:
## Setup mCRL2 for creating a single re-locatable bundle for MacOSX
if(APPLE)

  if(MCRL2_SINGLE_BUNDLE)
    if(NOT "${CMAKE_INSTALL_PREFIX}" STREQUAL "/")
      message( STATUS "CMAKE_INSTALL_PREFIX is not equal to \"/\". This creates a non-standard installer when using CPack." )
    endif(NOT "${CMAKE_INSTALL_PREFIX}" STREQUAL "/")

    SET(CMAKE_BUNDLE_NAME "mCRL2")

    SET(CMAKE_BUNDLE_LOCATION "${CMAKE_INSTALL_PREFIX}")
    # make sure CMAKE_INSTALL_PREFIX ends in /
    STRING(LENGTH "${CMAKE_INSTALL_PREFIX}" LEN)
    MATH(EXPR LEN "${LEN} -1" )
    STRING(SUBSTRING "${CMAKE_INSTALL_PREFIX}" ${LEN} 1 ENDCH)
    IF(NOT "${ENDCH}" STREQUAL "/")
      SET(CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/")
    ENDIF(NOT "${ENDCH}" STREQUAL "/")
    SET(CMAKE_INSTALL_PREFIX
      "${CMAKE_INSTALL_PREFIX}${CMAKE_BUNDLE_NAME}.app/Contents")


  endif(MCRL2_SINGLE_BUNDLE)

ENDIF(APPLE)

## Preprocessing:
include(MCRL2Packaging)

##---------------------------------------------------
## Add LTSmin as a last target, such that it is
## processed after the installation of the libraries
##---------------------------------------------------

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/scripts/ltsmin)

