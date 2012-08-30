# Authors: Frank Stappers, Jeroen Keiren
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# This script relocates the prerequisited shared libraries for a
# tool/project.

# CMake 2.8 supports relocation libraries for Mac OSX and windows.

# This is based on the example on http://www.cmake.org/Wiki/BundleUtilitiesExample
# Since we do not include Qt plugins, these have been left out.

# Source files, Moc headers, ui files and resource files have been added in
# the CMake script that includes this script.
# Here we only need to take care of the installation stuff

# WARNING: Relocation is only enabled for GUI tools
# At the moment we detect GUI tools using an extremely ugly check,
# we rely on MCRL2MacOSXBundleInformation to be included before this file,
# and check whether the MACOSX_BUNDLE_NAME has been set.
#
# TODO: Make this into a proper solution
if(NOT(MACOSX_BUNDLE_NAME) OR MCRL2_SINGLE_BUNDLE)
  return()
endif()

#-------------------------------------------------------------------------------
# Now the installation stuff below
#-------------------------------------------------------------------------------
set(plugin_dest_dir bin)
set(qtconf_dest_dir bin)
set(APPS "\${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}")
if(APPLE)
  set(plugin_dest_dir bin/${PROJECT_NAME}.app/Contents/MacOS)
  set(qtconf_dest_dir bin/${PROJECT_NAME}.app/Contents/Resources)
  set (APPS "\${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}.app")
endif()
if(WIN32)
  set (APPS "\${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}.exe")
endif()

#--------------------------------------------------------------------------------
# install a qt.conf file
# this inserts some cmake code into the install script to write the file
INSTALL(CODE "
    file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${qtconf_dest_dir}/qt.conf\" \"\")
    " COMPONENT Runtime)

#--------------------------------------------------------------------------------
# Use BundleUtilities to get all other dependencies for the application to work.
# It takes a bundle or executable along with possible plugins and inspects it
# for dependencies.  If they are not system dependencies, they are copied.

# directories to look for dependencies
# Note: dlls for QT are installed in QT_BINARY_DIR if it is installed through
# the windows installer.
SET(DIRS ${QT_LIBRARY_DIR} ${QT_BINARY_DIR})

# On Apple, the MacPorts build of Qt uses a slightly different structure
# Therefore, we need to copy the qt_menu.nib file over to a decent location,
# such that it can be found by fixup_bundle.
if(APPLE)
  if(${QT_QTGUI_LIBRARY} MATCHES "^.*${CMAKE_SHARED_LIBRARY_SUFFIX}")
    if(EXISTS "${QT_LIBRARY_DIR}/Resources/qt_menu.nib")
      install(DIRECTORY
        "${QT_LIBRARY_DIR}/Resources/qt_menu.nib" DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}.app/Contents/Resources/")
      else()
        message(WARNING "${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}.app is probably corrupt.")
      endif()
  endif()
endif()

# Now the work of copying dependencies into the bundle/package
# The quotes are escaped and variables to use at install time have their $ escaped
# An alternative is the do a configure_file() on a script and use install(SCRIPT  ...).
# Note that the image plugins depend on QtSvg and QtXml, and it got those copied
# over.
INSTALL(CODE "
    file(GLOB_RECURSE QTPLUGINS
	  \"\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
    include(BundleUtilities)
    fixup_bundle(\"${APPS}\" \"\${QTPLUGINS}\" \"${DIRS}\")
    " COMPONENT Runtime)
