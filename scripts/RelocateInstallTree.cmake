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
# relocation of shared libraries only for the Mac OSX. 

if( APPLE )
install(CODE "
    include(BundleUtilities) 
    fixup_bundle(\"${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX}\" \"\" \"${CMAKE_INSTALL_PREFIX}/lib\")
    " COMPONENT Runtime)
endif( APPLE )
