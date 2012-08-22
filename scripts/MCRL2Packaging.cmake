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

set(CPACK_PACKAGE_NAME "mcrl2")
set(CPACK_PACKAGE_VERSION "${MCRL2_VERSION}")

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Tools for modelling, validation and verification of concurrent systems")
set(CPACK_PACKAGE_CONTACT "mcrl2-users@listserver.tue.nl")
set(CPACK_PACKAGE_INSTALL_DIRECTORY mCRL2) # Used for NSIS

SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "mCRL2")
SET(CPACK_PACKAGE_VENDOR "TUe")

# Stuff for source packages
if(WIN32)
  set(CPACK_SOURCE_GENERATOR "ZIP")
elseif(APPLE OR UNIX)
  set(CPACK_SOURCE_GENERATOR "TGZ")
endif()
set(CPACK_TOPLEVEL_TAG "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME ${CPACK_TOPLEVEL_TAG})
set(CPACK_PACKAGE_FILE_NAME ${CPACK_TOPLEVEL_TAG}_${MCRL2_ARCHITECTURE})

configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/build/SourceVersion.in" "${CMAKE_CURRENT_SOURCE_DIR}/build/SourceVersion" @ONLY )

if( NOT APPLE )
  if( NOT MSVC )
    string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE)
    if(CMAKE_BUILD_TYPE MATCHES "release")
      #Strip files for Release packages
      SET( CPACK_STRIP_FILES true )
    endif(CMAKE_BUILD_TYPE MATCHES "release")
  else( NOT MSVC )
    SET( CPACK_STRIP_FILES true )
  endif( NOT MSVC )
endif( NOT APPLE )

# Create Desktop link to mcrl2-gui
set(CPACK_CREATE_DESKTOP_LINKS mcrl2-gui)
set(CPACK_PACKAGE_EXECUTABLES "grapemcrl2;grapemcrl2" "ltsgraph;ltsgraph" "ltsview;ltsview" "diagraphica;diagraphica" "lpsxsim;lpsxsim" "mcrl2-gui;mcrl2-gui" "mcrl2xi;mcrl2xi")

IF(APPLE AND MCRL2_SINGLE_BUNDLE)
  SET(CPACK_SET_DESTDIR TRUE)

  configure_file(${CMAKE_SOURCE_DIR}/postflight.sh.in ${CMAKE_CURRENT_BINARY_DIR}/postflight.sh)

  SET(CMAKE_POSTFLIGHT_SCRIPT
    ${CMAKE_CURRENT_BINARY_DIR}/postflight.sh)
  SET(CPACK_POSTUPGRADE_SCRIPT
    ${CMAKE_CURRENT_BINARY_DIR}/postflight.sh)

  set(CPACK_PACKAGE_DEFAULT_LOCATION "/Applications")

ENDIF(APPLE AND MCRL2_SINGLE_BUNDLE)

# NSIS VARIABLES
SET(CPACK_NSIS_DISPLAY_NAME "mCRL2")
SET(CPACK_NSIS_PACKAGE_NAME "mCRL2")
SET(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}\\\\build\\\\installer\\\\mcrl2-install-logo.bmp")

# Fix issue where mCRL2 gets installed into "Program Files (x86)" in Win64. 
if(CMAKE_CL_64)
  set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
endif(CMAKE_CL_64)

#Extend PATH variable with mCRL2_INSTALL_DIR/bin if CPACK_NSIS_MODIFY_PATH = ON
SET(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}/${MCRL2_BIN_DIR}")
SET(CPACK_NSIS_MODIFY_PATH ON)

# README and COPYING require a .txt extention when be used with MacOSX's PackageMaker
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/COPYING" "${CMAKE_CURRENT_BINARY_DIR}/COPYING.txt" COPYONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/README"  "${CMAKE_CURRENT_BINARY_DIR}/README.txt" COPYONLY)

set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_BINARY_DIR}/COPYING.txt )
set(CPACK_RESOURCE_FILE_README  ${CMAKE_CURRENT_BINARY_DIR}/README.txt )

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

# Distro versions
# /etc/debian_version
# /etc/gentoo-release
# /etc/knoppix-version
# /etc/mandrake-release
# /etc/pardus-release
# /etc/puppyversion
# /etc/redhat-release
# /etc/sabayon-release
# /etc/slackware-version
# /etc/SuSE-release
# /etc/xandros-desktop-version
# /etc/zenwalk-version

set(MCRL2_BOOST_VER "1.35" ) 
set(MCRL2_WX_VER    "2.8" ) 

# OpenSuSE RPM dependencies
if(EXISTS /etc/SuSE-release )
	message(STATUS "Distribution: OpenSuSE" )
  set(CPACK_RPM_PACKAGE_REQUIRES "gcc, Mesa, wxGTK >= ${MCRL2_WX_VER}, wxGTK-gl >= ${MCRL2_WX_VER}, boost-devel >= ${MCRL2_BOOST_VER}")
endif(EXISTS /etc/SuSE-release )

# Fedora/RedHat RPM dependencies
if(EXISTS /etc/redhat-release )
	message(STATUS "Distribution: RedHat/Fedora" )
  set(CPACK_RPM_PACKAGE_REQUIRES "gcc, Mesa, wxGTK >= ${MCRL2_WX_VER}, wxGTK-gl >= ${MCRL2_WX_VER}, wxBase >= ${MCRL2_WX_VER}, boost-system >= ${MCRL2_BOOST_VER}, boost-serialization >= ${MCRL2_BOOST_VER}, boost-signals >= ${MCRL2_BOOST_VER}, boost-filesystem >= ${MCRL2_BOOST_VER}, boost-regex >= ${MCRL2_BOOST_VER}")
endif(EXISTS /etc/redhat-release )

# Debian/Ubuntu dependencies
if(EXISTS /etc/debian_version )
	message(STATUS "Distribution: Debian/Ubuntu" )
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "gcc, debhelper (>= 5), libboost-dev (>=${MCRL2_BOOST_VER}), libwxgtk2.8-dev, libwxgtk2.8-0, libglu1-mesa-dev (>= 7.0.1)")
endif(EXISTS /etc/debian_version )

include(CPack)
