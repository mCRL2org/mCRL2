# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

##---------------------------------------------------
## CPack configuration
##---------------------------------------------------

INCLUDE(InstallRequiredSystemLibraries)

# TODO: Remove this variable
set(MCRL2_BOOST_VER "1.35" )

# Configure some files
# --------------------

# Record the version in the package, for proper version reporting of the tools
# and documentation
configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/build/SourceVersion.in" "${CMAKE_CURRENT_SOURCE_DIR}/build/SourceVersion" @ONLY )

# README and COPYING require a .txt extention when be used with MacOSX's PackageMaker
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/COPYING" "${CMAKE_CURRENT_BINARY_DIR}/COPYING.txt" COPYONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/README"  "${CMAKE_CURRENT_BINARY_DIR}/README.txt" COPYONLY)

# ----------------------------------------
# Variables common to all CPack generators
# ----------------------------------------

# The name of the package
set(CPACK_PACKAGE_NAME "mcrl2")

# The name of the package vendor
set(CPACK_PACKAGE_VENDOR "TUe")

# Package full version
set(CPACK_PACKAGE_VERSION "${MCRL2_VERSION}")

# Directory for the installed files
set(CPACK_TOPLEVEL_TAG "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")

# Name of the package file to generate, excluding extension.
set(CPACK_PACKAGE_FILE_NAME ${CPACK_TOPLEVEL_TAG}_${CXX_COMPILER_ARCHITECTURE})

# Create Desktop link to mcrl2-gui
set(CPACK_CREATE_DESKTOP_LINKS mcrl2-gui)

# Text file used to describe project
#set(CPACK_PACKAGE_DESCRIPTION_FILE XXX)

# Short description of the project.
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Tools for modelling, validation and verification of concurrent systems")

# List of the executables and associated text label to be used to create Start Menu shortcuts.
set(CPACK_PACKAGE_EXECUTABLES "grapemcrl2;grapemcrl2" "ltsgraph;ltsgraph" "ltsview;ltsview" "diagraphica;diagraphica" "lpsxsim;lpsxsim" "mcrl2-gui;mcrl2-gui" "mcrl2xi;mcrl2xi")

# Branding image displayed inside the installer
# Install icon for NSIS
# Must be a .bmp file
if( WIN32 )
  set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}\\\\build\\\\packaging\\\\mcrl2-install-logo.bmp")
endif( WIN32 )
# Install icon for DragNDrop
# Must be a .icns file 
if( APPLE ) 
  set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/tools/mcrl2-gui/mcrl2-gui.icns")
endif( APPLE )

# Registry key used when installing; windows only
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "mCRL2")

# E-mail address for contacting
set(CPACK_PACKAGE_CONTACT "mcrl2-users@listserver.tue.nl")

# The directory to which mCRL2 needs to be installed (NSIS only?)
set(CPACK_PACKAGE_INSTALL_DIRECTORY mCRL2)

# License to be embedded in the installer
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_BINARY_DIR}/COPYING.txt )

# Readme file to be embedded in the installer
set(CPACK_RESOURCE_FILE_README  ${CMAKE_CURRENT_BINARY_DIR}/README.txt )

# Warn when a file with absolute installation destination is encountered
set(CPACK_WARN_ON_ABSOLUTE_INSTALL_DESTINATION True)

# Source packages
# ---------------

# Stuff for source packages
if(WIN32)
  set(CPACK_SOURCE_GENERATOR "ZIP")
else()
  set(CPACK_SOURCE_GENERATOR "TGZ")
endif()

# Name of the source package to generate, excluding extension.
set(CPACK_SOURCE_PACKAGE_FILE_NAME ${CPACK_TOPLEVEL_TAG})

# Do not strip files for the source packages
set(CPACK_SOURCE_STRIP_FILES False)

# Binary installers
# -----------------

set(CPACK_STRIP_FILES True)

# -------------------------------------
# Variables concerning CPack Components
# -------------------------------------

# Specify how components are grouped for mult-package component-aware CPack generators
# We pack everything in one component.
set(CPACK_COMPONENTS_CROUPING ALL_COMPONENTS_IN_ONE)

# Group the COMPONENTS such that we get a decent installer
set(CPACK_COMPONENT_APPLICATIONS_GROUP "Runtime")
set(CPACK_COMPONENT_EXAMPLE_GROUP "Documentation")
set(CPACK_COMPONENT_LIBRARIES_GROUP "Development")
set(CPACK_COMPONENT_HEADERS_GROUP "Development")

# Always install the tools
set(CPACK_COMPONENT_APPLICATIONS_REQUIRED TRUE)

# If we build with shared libraries, make sure they are included in the package
if(BUILD_SHARED_LIBS)
  set(CPACK_COMPONENT_APPLICATIONS_DEPENDS Libraries)
endif()

# For non-windows platforms we need to include libraries and headers, in order
# to build the compiling rewriters
if(NOT WIN32)
  set(CPACK_COMPONENT_APPLICATIONS_DEPENDS Headers)
endif()

# For a windows installer create two default configurations,
# Default only installs tools and examples
# Full in addition installs the libraries and headers.
set(CPACK_ALL_INSTALL_TYPES Default Full)
set(CPACK_COMPONENT_APPLICATIONS_INSTALL_TYPES Full Default)
set(CPACK_COMPONENT_LIBRARIES_INSTALL_TYPES Full)
set(CPACK_COMPONENT_HEADERS_INSTALL_TYPES Full)
set(CPACK_COMPONENT_EXAMPLES_INSTALL_TYPES Full Default)

# --------------------------------
# Platform specific configurations
# --------------------------------


# Linux
# -----

set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST
    /usr /usr/bin /usr/lib /usr/share
    /usr/share/applications /usr/share/icons)

#Variables for RPM packaging
set(CPACK_RPM_PACKAGE_LICENSE "Boost Software License, Version 1.0")
set(CPACK_RPM_PACKAGE_GROUP "Productivity/Scientific/Other")
# Following should not exceed 77 chars
set(CPACK_RPM_PACKAGE_DESCRIPTION "the mCRL2 formal specification language toolset
 mCRL2 stands for micro Common Representation Language 2.  It is a
 specification language that can be used to specify and analyse the
 behaviour of distributed systems and protocols and is the successor to
 muCRL.  Using its accompanying toolset, systems can be analysed and
 verified automatically.

 This toolset supports a collection of tools for linearisation, simulation,
 state-space exploration and generation and tools to optimise and analyse
 specifications.  Moreover, state spaces can be manipulated, visualised and
 analysed.")
set(CPACK_RPM_PACKAGE_VENDOR "Technische Universiteit Eindhoven (TU/e)")

# OpenSuSE RPM dependencies
if(EXISTS /etc/SuSE-release )
	message(STATUS "Distribution: OpenSuSE" )
  set(CPACK_RPM_PACKAGE_REQUIRES "gcc-c++, boost-devel >= ${MCRL2_BOOST_VER}")
endif(EXISTS /etc/SuSE-release )

# Fedora/RedHat RPM dependencies
if(EXISTS /etc/redhat-release )
	message(STATUS "Distribution: RedHat/Fedora" )
  set(CPACK_RPM_PACKAGE_REQUIRES "gcc-c++, boost-devel >= ${MCRL2_BOOST_VER}")
endif(EXISTS /etc/redhat-release )

# Debian/Ubuntu dependencies
if(EXISTS /etc/debian_version )
	message(STATUS "Distribution: Debian/Ubuntu" )
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "gcc, debhelper (>= 5), libboost-dev (>=${MCRL2_BOOST_VER}), libglu1-mesa-dev (>= 7.0.1)")
  set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
endif(EXISTS /etc/debian_version )

# Apple
# -----

if(APPLE)
  set(CPACK_STRIP_FILES FALSE)
 
  #set(CPACK_SET_DESTDIR TRUE)
  set(CPACK_PACKAGE_DEFAULT_LOCATION "/Applications")

  configure_file(${CMAKE_SOURCE_DIR}/postflight.sh.in ${CMAKE_CURRENT_BINARY_DIR}/postflight.sh)

  #set(CPACK_POSTFLIGHT_SCRIPT
  #  ${CMAKE_CURRENT_BINARY_DIR}/postflight.sh)
  set(CPACK_POSTUPGRADE_SCRIPT
    ${CMAKE_CURRENT_BINARY_DIR}/postflight.sh)

  #Force installer to behave as a monolithic installer.
  #This workaround enables to install mCRL2 properly. (CMake 2.8.9, PackageMaker 3.0.4)
  set(CPACK_MONOLITHIC_INSTALL 1)

  # Install the tools (when installing the Packagemaker project) in the following directory
  # relative to the directory in which the package is installed.
  # By default ${CPACK_PACKAGE_DEFAULT_LOCATION}
  set(CPACK_PACKAGING_INSTALL_PREFIX "/mCRL2")

endif()

# Windows
# -------

# NSIS VARIABLES
SET(CPACK_NSIS_DISPLAY_NAME "mCRL2")
SET(CPACK_NSIS_PACKAGE_NAME "mCRL2")


# Workaround
# Fix issue where mCRL2 gets installed into "Program Files (x86)" in Win64.
if(CMAKE_CL_64)
  set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
endif(CMAKE_CL_64)

#Extend PATH variable with mCRL2_INSTALL_DIR/bin if CPACK_NSIS_MODIFY_PATH = ON
SET(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}/${MCRL2_BIN_DIR}")
SET(CPACK_NSIS_MODIFY_PATH ON)

# Include CPack specific stuff
# ----------------------------
include(CPack)
