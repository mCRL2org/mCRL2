# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

if( APPLE )
  # Graphical tools depend on the CMake Module DeployQt4. This module is used 
  # for the deployment (installation and packaging) of Qt. This module is 
  # available as of version 2.8.7. To include Qt libraries in a package we require
  # version 2.8.10.  
  cmake_minimum_required (VERSION 2.8.7)
else()
  cmake_minimum_required (VERSION 2.8)
endif()

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

option(MCRL2_ENABLE_GUI_TOOLS
       "Enable/disable creation of GUI tools"          ON)
option(MCRL2_ENABLE_CADP_SUPPORT
       "Enable/disable support for CADP"               OFF)
option(MCRL2_ENABLE_EXPERIMENTAL
       "Enable/disable creation of experimental tools" OFF)
option(MCRL2_ENABLE_DEPRECATED
       "Enable/disable creation of deprecated tools"   OFF)
option(MCRL2_INSTALL_HEADERS
       "Enable/disable install headers"                ON)

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

##---------------------------------------------------
## Configure CMake
##---------------------------------------------------

## Set path where additional modules can be found
set( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/scripts" )

## Determine build type
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

#Paths for the installation
set(MCRL2_BIN_DIR "bin")
set(MCRL2_LIB_DIR "lib/mcrl2")
set(MCRL2_SHARE_DIR "share/mcrl2")
  #Subdirectories of MCRL2_SHARE_DIR
  set(MCRL2_MAN_DIR "man/man1")
  set(MCRL2_EXAMPLES_DIR "doc/examples")
set(MCRL2_INCLUDE_DIR "include")


## Configure the compiler
include(GetCompilerVersion)
include(ConfigureCompiler)

## Library and executable staging
set(MCRL2_STAGE_ROOTDIR ""
  CACHE PATH "Path to stage executables and libraries." )
  message( STATUS "MCRL2_STAGE_ROOTDIR: ${MCRL2_STAGE_ROOTDIR}" )
if( MCRL2_STAGE_ROOTDIR )
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${MCRL2_STAGE_ROOTDIR}/${MCRL2_BIN_DIR}")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${MCRL2_STAGE_ROOTDIR}/${MCRL2_LIB_DIR}")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${MCRL2_STAGE_ROOTDIR}/${MCRL2_SHARE_DIR}")
endif( MCRL2_STAGE_ROOTDIR )

## Set runtime path variable RPATH for install targets

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

## Setup dependency to CADP
include(MCRL2CADPSupport)

## Determine latest SVN revision
include(MCRL2Version)

##---------------------------------------------------
## Print additional configuration information
##---------------------------------------------------
include(PrintBuildInfo)

##---------------------------------------------------
## Find dependencies (Boost, OpenGL, [CVC3, gl2ps])
##---------------------------------------------------
include(ConfigureBoost)
find_package(cvc3)

if( MCRL2_ENABLE_GUI_TOOLS )
  find_package(OpenGL)
  find_package(gl2ps)

  if(NOT OPENGL_FOUND)
    message( FATAL_ERROR "OpenGL not found. Set MCRL2_ENABLE_GUI_TOOLS to FALSE to build without GUI-tools.")
  else( OPENGL_FOUND )
    if ( APPLE )
      include_directories( ${OPENGL_INCLUDE_DIR} )
      link_directories(${OPENGL_LIBRARIES})
      set(CMAKE_EXE_LINKER_FLAGS "-framework OpenGL ${CMAKE_EXE_LINKER_FLAGS}")
      set(CMAKE_SHARED_LINKER_FLAGS "-framework OpenGL ${CMAKE_SHARED_LINKER_FLAGS}")
      set(CMAKE_MODULE_LINKER_FLAGS "-framework OpenGL ${CMAKE_SHARED_LINKER_FLAGS}")
    endif (APPLE)
  endif(NOT OPENGL_FOUND)

  find_package(Qt4 COMPONENTS QtCore QtGui QtXml QtOpenGL)
  if(QT4_FOUND)
    include(${QT_USE_FILE})
  else(QT4_FOUND)
    message(FATAL_ERROR "Qt4 not found. Set MCRL2_ENABLE_GUI_TOOLS to FALSE to build without GUI-tools.")
  endif(QT4_FOUND)

  message(STATUS "QT Version: ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}")

endif( MCRL2_ENABLE_GUI_TOOLS )

##---------------------------------------------------
## 3rd party libraries
##---------------------------------------------------

if(MCRL2_ENABLE_GUI_TOOLS)
  add_subdirectory ( 3rd-party/tr)
  if( GL2PS_FOUND )
    message(STATUS "Using system gl2ps library.")
  else( GL2PS_FOUND )
    message(STATUS "Using gl2ps library provided by mCRL2.")
    add_subdirectory ( 3rd-party/gl2ps )
  endif( GL2PS_FOUND)
endif(MCRL2_ENABLE_GUI_TOOLS)

add_subdirectory ( 3rd-party/svc )
add_subdirectory ( 3rd-party/dparser )

##---------------------------------------------------
## mCRL2 libraries
##---------------------------------------------------

# add_subdirectory ( libraries/aterm )
add_subdirectory ( libraries/atermpp )
add_subdirectory ( libraries/bes )
add_subdirectory ( libraries/core )
add_subdirectory ( libraries/data )
add_subdirectory ( libraries/lps )
add_subdirectory ( libraries/lts )
add_subdirectory ( libraries/modal_formula )
add_subdirectory ( libraries/pbes )
add_subdirectory ( libraries/process )
add_subdirectory ( libraries/trace )
add_subdirectory ( libraries/utilities )

##---------------------------------------------------
## Command-line tools
##---------------------------------------------------

add_subdirectory ( tools/alphabet )
add_subdirectory ( tools/besinfo )
add_subdirectory ( tools/bespp )
add_subdirectory ( tools/formulacheck )
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
add_subdirectory ( tools/lts2lps )
# add_subdirectory ( tools/lts2mathematica )
add_subdirectory ( tools/lts2pbes )
add_subdirectory ( tools/ltsconvert )
add_subdirectory ( tools/ltscompare )
add_subdirectory ( tools/ltsinfo )
add_subdirectory ( tools/mcrl22lps )
add_subdirectory ( tools/mcrl2i )
add_subdirectory ( tools/mcrl2parse )
add_subdirectory ( tools/pbes2bool )
add_subdirectory ( tools/pbes2bes )
add_subdirectory ( tools/pbesconstelm )
add_subdirectory ( tools/pbesinfo )
add_subdirectory ( tools/pbesparelm )
add_subdirectory ( tools/pbespgsolve )
add_subdirectory ( tools/pbespp )
add_subdirectory ( tools/pbesrewr )
add_subdirectory ( tools/symbolic_exploration )
add_subdirectory ( tools/tracepp )
add_subdirectory ( tools/txt2pbes )
add_subdirectory ( tools/txt2lps )

##---------------------------------------------------
## Graphical tools
##---------------------------------------------------
if(MCRL2_ENABLE_GUI_TOOLS)
  add_subdirectory ( tools/diagraphica )
  add_subdirectory ( tools/lpsxsim )
  add_subdirectory ( tools/ltsgraph )
  add_subdirectory ( tools/ltsview )
  add_subdirectory ( tools/mcrl2-gui )
  add_subdirectory ( tools/mcrl2xi )
endif(MCRL2_ENABLE_GUI_TOOLS)

##---------------------------------------------------
## Experimental tools
##---------------------------------------------------

if(MCRL2_ENABLE_EXPERIMENTAL)
  add_subdirectory ( tools/besconvert )
  add_subdirectory ( tools/bessolve )
  add_subdirectory ( tools/complps2pbes )
  add_subdirectory ( tools/lpsbisim2pbes )
  add_subdirectory ( tools/lpsrealelm )
  add_subdirectory ( tools/pbesabstract )
  add_subdirectory ( tools/pbesabsinthe )
  add_subdirectory ( tools/pbesinst )
  add_subdirectory ( tools/pbespareqelm )
  add_subdirectory ( tools/pbesstategraph )
  add_subdirectory ( tools/txt2bes )
endif(MCRL2_ENABLE_EXPERIMENTAL)

##---------------------------------------------------
## Deprecated tools
##---------------------------------------------------

if(MCRL2_ENABLE_DEPRECATED)
  add_subdirectory ( tools/tbf2lps )
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

if ( MCRL2_INSTALL_HEADERS )
install(
  DIRECTORY
    libraries/atermpp/include/
    libraries/bes/include/
    libraries/core/include/
    libraries/data/include/
    libraries/lps/include/
    libraries/lts/include/
    libraries/modal_formula/include/
    libraries/pbes/include/
    libraries/process/include/
    libraries/trace/include/
    libraries/utilities/include/
  DESTINATION ${MCRL2_INCLUDE_DIR}
  COMPONENT Headers
  PATTERN ".svn" EXCLUDE
)
FILE(GLOB dparser_headers "3rd-party/dparser/*.h")
install(
  FILES
    ${dparser_headers}
  DESTINATION ${MCRL2_INCLUDE_DIR}/dparser
  COMPONENT Headers
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
  DESTINATION ${MCRL2_SHARE_DIR}/${MCRL2_EXAMPLES_DIR}
  COMPONENT Examples
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

include(MCRL2Packaging)
