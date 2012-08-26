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

if( APPLE AND BUILD_SHARED_LIBS AND NOT MCRL2_SINGLE_BUNDLE )
if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.app" )

# install a qt.conf file
# this inserts some cmake code into the install script to write the file
INSTALL(CODE "
    file(WRITE \"\${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}.app/Contents/Resources/qt.conf\" \"\")
    " COMPONENT Runtime)

# When a QtGui Library is a shared library, copy the required files manually. 
if( ${QT_QTGUI_LIBRARY} MATCHES "^.*${CMAKE_SHARED_LIBRARY_SUFFIX}" )

  if(EXISTS "${QT_LIBRARY_DIR}/Resources/qt_menu.nib")
  # Install the required qt_menu into the Contents/Resources
  install(DIRECTORY 
      "${QT_LIBRARY_DIR}/Resources/qt_menu.nib" DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}.app/Contents/Resources/")
  else()
      message(WARNING "${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}.app is probably corrupt.")
  endif()
endif()

# Set libraries that need to be imported into bundle
set(DIRS 
    ${Boost_LIBRARY_DIRS}
    ${QT_LIBRARY_DIRS} )

install(CODE "
    include(BundleUtilities) 
    fixup_bundle(\"${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}.app\" \"\" \"${DIRS}\")
    " COMPONENT Runtime)
endif(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.app" )

endif( APPLE AND BUILD_SHARED_LIBS AND NOT MCRL2_SINGLE_BUNDLE )
