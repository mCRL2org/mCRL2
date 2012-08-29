# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# This script relocates the prerequisited shared libraries for a
# tool/project.

# CMake 2.6 only supports relocating libraries for the Mac OSX.
# CMake 2.8 support also relocation libraries for windows.
# Since we support version 2.6 and up, we can only facilitate
# relocation of shared libraries only for  Mac OSX.

SET(plugin_dest_dir bin)
SET(qtconf_dest_dir bin)
set (APPS "\${CMAKE_INSTALL_PREFIX}/bin/\${PROJECT_NAME}")
if(APPLE)
  set(plugin_dest_dir bin/${PROJECT_NAME}.app/Contents/MacOS)
  set(qtconf_dest_dir bin/${PROJECT_NAME}.app/Contents/Resources)
  set (APPS "\${CMAKE_INSTALL_PREFIX}/bin/\${PROJECT_NAME}.app")
endif()
if(WIN32)
  set (APPS "\${CMAKE_INSTALL_PREFIX}/bin/\${PROJECT_NAME}.exe")
endif()

# install a qt.conf file
# this inserts some cmake code into the install script to write the file
INSTALL(CODE "
    file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${qtconf_dest_dir}/qt.conf\" \"\")
    " COMPONENT Runtime)

# Set libraries that need to be imported into bundle
set(DIRS
    ${Boost_LIBRARY_DIRS}
    ${QT_LIBRARY_DIRS} )

# On Apple, the MacPorts build of Qt uses a slightly different structure
# Therefore, we need to copy the qt_menu.nib file over to a decent location.
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

INSTALL(CODE "
    file(GLOB_RECURSE QTPLUGINS
      \"\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
    include(BundleUtilities)
    fixup_bundle(\"${APPS}\" \"\${QTPLUGINS}\" \"${DIRS}\")
    " COMPONENT Runtime)
