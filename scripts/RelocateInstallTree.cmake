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

if(APPLE AND NOT(MACOSX_BUNDLE_NAME))
  return()
endif()

# Warning: this seems to only make sense on Windows and MacOSX!
if(APPLE OR WIN32)
    include(DeployQt5)
    if(APPLE)
        set(EXECUTABLE "${MCRL2_BIN_DIR}/${PROJECT_NAME}.app")
    elseif(WIN32)
        set(EXECUTABLE "${MCRL2_BIN_DIR}/${PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX}")
    endif()

    # Synopsis:
    # INSTALL_QT4_EXECUTABLE(<executable> [<qtplugins> <libs> <dirs> <plugins_dir> <request_qt_conf> <component>])
    install_qt5_executable(${EXECUTABLE} "" "" ${CMAKE_PREFIX_PATH} "" "" Applications)
endif()
