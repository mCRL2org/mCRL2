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


if (CMAKE_SIZEOF_VOID_P MATCHES "8")
  set(CXX_COMPILER_ARCHITECTURE "x86_64")
else ()
  if (CMAKE_SIZEOF_VOID_P MATCHES "4")
    set(CXX_COMPILER_ARCHITECTURE "x86")
  else ()
    message(FATAL_ERROR "Could not determine architecture.")
  endif()
endif()

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

set(CPACK_PACKAGE_NAME "mcrl2")
set(CPACK_PACKAGE_VENDOR "TUe")
set(CPACK_PACKAGE_VERSION "${MCRL2_VERSION}")
set(CPACK_TOPLEVEL_TAG "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}") # Directory for the installed files
set(CPACK_PACKAGE_FILE_NAME ${CPACK_TOPLEVEL_TAG}_${CXX_COMPILER_ARCHITECTURE})
set(CPACK_CREATE_DESKTOP_LINKS mcrl2-gui)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "Tools for modelling, validation and verification of concurrent systems")
set(CPACK_PACKAGE_EXECUTABLES # exename/displayname to create start menu shortcuts
    "ltsgraph;LTSGraph" "ltsview;LTSView" "diagraphica;DiaGraphica" "lpsxsim;LPS XSim"
    "mcrl2-gui;mCRL2 GUI" "mcrl2xi;mCRL2 XI")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "mCRL2")
set(CPACK_PACKAGE_CONTACT "mCRL2 Development team <mcrl2-users@listserver.tue.nl>")
set(CPACK_PACKAGE_INSTALL_DIRECTORY mCRL2)
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_BINARY_DIR}/COPYING.txt )
set(CPACK_RESOURCE_FILE_README  ${CMAKE_CURRENT_BINARY_DIR}/README.txt )
set(CPACK_WARN_ON_ABSOLUTE_INSTALL_DESTINATION True)

# Text file used to describe project
#set(CPACK_PACKAGE_DESCRIPTION_FILE XXX)

# Branding image displayed inside the installer
if(WIN32)
  # Install icon for NSIS (must be a .bmp file)
  set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}\\\\build\\\\packaging\\\\mcrl2-install-logo.bmp")
elseif(APPLE)
  # TODO: Check if this is actually used
  set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/tools/mcrl2-gui/mcrl2-gui.icns")
endif()

# Source packages
# ---------------

# Stuff for source packages
if(WIN32)
  set(CPACK_SOURCE_GENERATOR "ZIP")
else()
  set(CPACK_SOURCE_GENERATOR "TGZ")
endif()

set(CPACK_SOURCE_PACKAGE_FILE_NAME ${CPACK_TOPLEVEL_TAG})
set(CPACK_SOURCE_STRIP_FILES False)

# Binary installers
# -----------------

set(CPACK_STRIP_FILES True)

# -------------------------------------
# Variables concerning CPack Components
# -------------------------------------

set(CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)
set(CPACK_COMPONENT_APPLICATIONS_GROUP "Runtime")
set(CPACK_COMPONENT_EXAMPLE_GROUP "Documentation")
set(CPACK_COMPONENT_LIBRARIES_GROUP "Development")
set(CPACK_COMPONENT_HEADERS_GROUP "Development")
set(CPACK_COMPONENT_APPLICATIONS_REQUIRED TRUE)
if(BUILD_SHARED_LIBS)
  list(APPEND CPACK_COMPONENT_APPLICATIONS_DEPENDS Libraries)
endif()
if(NOT WIN32)
  list(APPEND CPACK_COMPONENT_APPLICATIONS_DEPENDS Headers)
endif()

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

#Variables for RPM packaging
set(CPACK_RPM_PACKAGE_LICENSE "Boost Software License, Version 1.0")
set(CPACK_RPM_PACKAGE_GROUP "Productivity/Scientific/Other")
set(CPACK_RPM_PACKAGE_VENDOR "Technische Universiteit Eindhoven (TU/e)")
set(CPACK_RPM_PACKAGE_REQUIRES "gcc, Mesa, boost-devel >= ${MCRL2_MIN_BOOST_VERSION}")
set(CPACK_RPM_PACKAGE_DESCRIPTION
# -----------------------------------------------------------------------------
"toolset for the mCRL2 formal specification language
mCRL2 stands for micro Common Representation Language 2.  It is a specification
language that can be used to specify and analyse the behaviour of distributed
systems and protocols and is the successor to muCRL.  Using its accompanying
toolset, systems can be analysed and verified automatically.

This toolset supports a collection of tools for linearisation, simulation,
state-space exploration and generation and tools to optimise and analyse
specifications.  Moreover, state spaces can be manipulated, visualised and
analysed.")

set(CPACK_DEBIAN_PACKAGE_DEPENDS "g++, libboost-dev (>=${MCRL2_MIN_BOOST_VERSION})")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "http://www.mcrl2.org")
set(CPACK_DEBIAN_PACKAGE_SECTION "science")
set(CPACK_DEBIAN_PACKAGE_PRIORITY ${CPACK_DEBIAN_PACKAGE_PRIORITY})
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_SOURCE_DIR}/build/packaging/debian/postinst")
message(STATUS ${CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA})
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION
# -----------------------------------------------------------------------------
"toolset for the mCRL2 formal specification language
 mCRL2 stands for micro Common Representation Language 2.  It is a
 specification language that can be used to specify and analyse the behaviour
 of distributed systems and protocols and is the successor to muCRL.  Using its
 accompanying toolset, systems can be analysed and verified automatically.
 .
 This toolset supports a collection of tools for linearisation, simulation,
 state-space exploration and generation and tools to optimise and analyse
 specifications.  Moreover, state spaces can be manipulated, visualised and
 analysed.")

if(EXISTS /etc/debian_version)
  install(FILES ${CMAKE_SOURCE_DIR}/COPYING DESTINATION share/doc/mcrl2 RENAME copyright)
endif()

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

endif()

# Windows
# -------

# NSIS VARIABLES
SET(CPACK_NSIS_DISPLAY_NAME "mCRL2")
SET(CPACK_NSIS_PACKAGE_NAME "mCRL2")
SET(CPACK_NSIS_MODIFY_PATH ON)

# Include CPack specific stuff
# ----------------------------
set(CPACK_BINARY_DRAGNDROP ON)
include(CPack)
