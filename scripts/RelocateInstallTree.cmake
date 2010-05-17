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
install(CODE "
    include(${CMAKE_SOURCE_DIR}/scripts/MCRL2BundleUtilities.cmake) 
    fixup_bundle(\"${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}.app\" \"\" \"${MCRL2_LIB_DIR}\")
    " COMPONENT Runtime)
endif(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.app" )

endif( APPLE AND BUILD_SHARED_LIBS AND NOT MCRL2_SINGLE_BUNDLE )

if( APPLE AND MCRL2_SINGLE_BUNDLE )
  if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.app" )
    FILE(APPEND ${CMAKE_BINARY_DIR}/install_tools "${PROJECT_NAME}.app\n" )
  else(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.app" )
    FILE(APPEND ${CMAKE_BINARY_DIR}/install_tools "${PROJECT_NAME}\n" )
  endif(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.app" )
endif( APPLE AND MCRL2_SINGLE_BUNDLE )
