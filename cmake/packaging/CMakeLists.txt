# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://github.com/mCRL2org/mCRL2/blob/master/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

include(InstallRequiredSystemLibraries)

# ----------------------------------------
# Variables common to all CPack generators
# ----------------------------------------

configure_file(${CMAKE_SOURCE_DIR}/COPYING ${CMAKE_BINARY_DIR}/COPYING.txt COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/README.md ${CMAKE_BINARY_DIR}/README.txt COPYONLY)

set(CPACK_PACKAGE_NAME "mcrl2")
set(CPACK_PACKAGE_VENDOR "TUe")
set(CPACK_PACKAGE_VERSION "${MCRL2_VERSION}")
set(CPACK_TOPLEVEL_TAG "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}") # Directory for the installed files
set(CPACK_PACKAGE_FILE_NAME ${CPACK_TOPLEVEL_TAG}_${CMAKE_SYSTEM_PROCESSOR})
set(CPACK_CREATE_DESKTOP_LINKS mcrl2-gui)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "Tools for modelling, validation and verification of concurrent systems")
set(CPACK_PACKAGE_EXECUTABLES # exename/displayname to create start menu shortcuts
    "ltsgraph;LTSGraph" "ltsview;LTSView" "diagraphica;DiaGraphica" "lpsxsim;LPS XSim"
    "mcrl2-gui;mCRL2 GUI" "mcrl2xi;mCRL2 XI" "mcrl2ide;mCRL2 IDE")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "mCRL2")
set(CPACK_PACKAGE_CONTACT "mCRL2 Development team <mcrl2-users@listserver.tue.nl>")
set(CPACK_PACKAGE_INSTALL_DIRECTORY mCRL2)
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_BINARY_DIR}/COPYING.txt )
set(CPACK_RESOURCE_FILE_README  ${CMAKE_BINARY_DIR}/README.txt )

if(APPLE)
  configure_file(${CMAKE_SOURCE_DIR}/cmake/packaging/osx/Contents/Info.plist
            ${MCRL2_STAGE_PATH}/${MCRL2_BUNDLE_PATH}/Contents/Info.plist @ONLY)
  configure_file(${CMAKE_SOURCE_DIR}/cmake/packaging/osx/Contents/MacOS/mCRL2
            ${MCRL2_STAGE_PATH}/${MCRL2_BUNDLE_PATH}/Contents/MacOS/mCRL2 @ONLY)

  add_executable(mCRL2 IMPORTED
    IMPORTED_LOCATION ${MCRL2_STAGE_PATH}/${MCRL2_BUNDLE_PATH}/Contents/MacOS/mCRL2)

  install(FILES ${MCRL2_STAGE_PATH}/${MCRL2_BUNDLE_PATH}/Contents/Info.plist
          DESTINATION ${MCRL2_BUNDLE_PATH}/Contents)
  install(PROGRAMS ${MCRL2_STAGE_PATH}/${MCRL2_BUNDLE_PATH}/Contents/MacOS/mCRL2
          DESTINATION ${MCRL2_BUNDLE_PATH}/Contents/MacOS)
  install(FILES ${CMAKE_SOURCE_DIR}/cmake/packaging/icons/mcrl2-orange.icns
          DESTINATION ${MCRL2_BUNDLE_PATH}/Contents/Resources)
  install(FILES ${CMAKE_SOURCE_DIR}/cmake/packaging/osx/Contents/bin/qt.conf
          DESTINATION ${MCRL2_BUNDLE_PATH}/Contents/bin)

  if(MCRL2_ENABLE_GUI_TOOLS)
    find_program(MACDEPLOYQT_EXECUTABLE NAMES macdeployqt PATHS ${Qt6_DIR}/../../../bin DOC "Path to macdeployqt executable (in the Qt binary directory)" REQUIRED)
    set(EXECUTABLES "")
    foreach(EXECUTABLE ${MCRL2_QT_APPS})
      set(EXECUTABLES "${EXECUTABLES} -executable=${MCRL2_RUNTIME_PATH}/${EXECUTABLE}")
    endforeach()
    # We are running macdeployqt twice since somehow it does not take into account transitive dependencies anymore. (2023/11/08)
    install(CODE "
      message(STATUS \"Executing ${MACDEPLOYQT_EXECUTABLE} mCRL2.app ${EXECUTABLES} in directory \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}\")
      execute_process(
        COMMAND ${MACDEPLOYQT_EXECUTABLE} mCRL2.app ${EXECUTABLES}
        WORKING_DIRECTORY \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}
        RESULT_VARIABLE retval
      )
      execute_process(
        COMMAND ${MACDEPLOYQT_EXECUTABLE} mCRL2.app ${EXECUTABLES}
        WORKING_DIRECTORY \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}
        RESULT_VARIABLE retval
      )       
      if(retval AND NOT retval EQUAL 0)
        message(FATAL_ERROR \"Failed: ${retval}\")
      endif()

    "
    COMPONENT Stable)
  endif()
endif()

# Branding image displayed inside the installer
if(WIN32)
  # Install icon for NSIS (must be a .bmp file)
  set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}\\\\cmake\\\\packaging\\\\mcrl2-install-logo.bmp")
elseif(APPLE)
  set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/cmake/packaging/icons/mcrl2-green.icns")
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
set(CPACK_COMPONENT_STABLE_GROUP "Runtime")
set(CPACK_COMPONENT_EXPERIMENTAL_GROUP "Runtime")
set(CPACK_COMPONENT_DEPRECATED_GROUP "Runtime")
set(CPACK_COMPONENT_DEVELOPER_GROUP "Runtime")
set(CPACK_COMPONENT_SHARED_GROUP "Runtime")
set(CPACK_COMPONENT_SHARED_REQUIRED TRUE)
if(BUILD_SHARED_LIBS)
  set(CPACK_COMPONENT_LIBRARIES_GROUP "Runtime")
else()
  set(CPACK_COMPONENT_LIBRARIES_GROUP "Development")
endif()
set(CPACK_COMPONENT_EXAMPLES_GROUP "Documentation")
set(CPACK_COMPONENT_HEADERS_GROUP "Development")
if(BUILD_SHARED_LIBS)
  list(APPEND CPACK_COMPONENT_STABLE_DEPENDS Libraries)
  list(APPEND CPACK_COMPONENT_EXPERIMENTAL_DEPENDS Libraries)
  list(APPEND CPACK_COMPONENT_DEPRECATED_DEPENDS Libraries)
  list(APPEND CPACK_COMPONENT_DEVELOPER_DEPENDS Libraries)
endif()
if(NOT WIN32)
  list(APPEND CPACK_COMPONENT_STABLE_DEPENDS Headers)
  list(APPEND CPACK_COMPONENT_EXPERIMENTAL_DEPENDS Headers)
  list(APPEND CPACK_COMPONENT_DEPRECATED_DEPENDS Headers)
  list(APPEND CPACK_COMPONENT_DEVELOPER_DEPENDS Headers)
endif()

set(CPACK_ALL_INSTALL_TYPES Default Full)
set(CPACK_COMPONENT_STABLE_INSTALL_TYPES Full Default)
set(CPACK_COMPONENT_EXPERIMENTAL_INSTALL_TYPES Full)
set(CPACK_COMPONENT_DEPRECATED_INSTALL_TYPES Full)
set(CPACK_COMPONENT_DEVELOPER_INSTALL_TYPES Full)
set(CPACK_COMPONENT_LIBRARIES_INSTALL_TYPES Full)
set(CPACK_COMPONENT_HEADERS_INSTALL_TYPES Full)
set(CPACK_COMPONENT_EXAMPLES_INSTALL_TYPES Full Default)

# --------------------------------
# Platform specific configurations
# --------------------------------


# Linux
# -----

#Variables for RPM packaging
set(CPACK_RPM_PACKAGE_LICENSE "Boost")
set(CPACK_RPM_PACKAGE_GROUP "Productivity/Scientific/Other")
set(CPACK_RPM_PACKAGE_VENDOR "Technische Universiteit Eindhoven (TU/e)")
set(CPACK_RPM_PACKAGE_URL "https://mcrl2.org")
set(CPACK_RPM_PACKAGE_REQUIRES "gcc")
set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION
    "/usr/share/applications"
    "/usr/share/icons"
    "/usr/share/man"
    "/usr/share/man/man1"
)
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

set(CPACK_DEBIAN_PACKAGE_DEPENDS "g++ | clang")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "http://www.mcrl2.org")
set(CPACK_DEBIAN_PACKAGE_SECTION "science")
set(CPACK_DEBIAN_PACKAGE_PRIORITY ${CPACK_DEBIAN_PACKAGE_PRIORITY})
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_SOURCE_DIR}/cmake/packaging/debian/postinst")
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
  # Debian requires applications to install their copyright information in a specific location:
  install(FILES ${CMAKE_SOURCE_DIR}/COPYING DESTINATION share/doc/mcrl2 RENAME copyright)
endif()

# Windows
# -------

if (WIN32 AND MCRL2_ENABLE_GUI_TOOLS)
  find_program(WINDEPLOYQT_EXECUTABLE NAMES "windeployqt.exe" PATHS "${Qt6_DIR}/../../../bin/" DOC "Path to windeployqt executable (in the Qt binary directory)" REQUIRED)
  set(EXECUTABLES "")
  foreach(EXECUTABLE ${MCRL2_QT_APPS})
    set(EXECUTABLES "${EXECUTABLES} ${EXECUTABLE}.exe")
  endforeach()
  
  install(CODE "
    message(STATUS \"Executing ${WINDEPLOYQT_EXECUTABLE} for ${EXECUTABLES} in directory \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${MCRL2_RUNTIME_PATH}\")
    execute_process(
      COMMAND ${WINDEPLOYQT_EXECUTABLE} --no-translations --no-system-d3d-compiler --no-quick-import ${EXECUTABLES}
      WORKING_DIRECTORY \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${MCRL2_RUNTIME_PATH}
      RESULT_VARIABLE retval
    )
    if(retval AND NOT retval EQUAL 0)
      message(FATAL_ERROR \"Failed: ${retval}\")
    endif()
  "
  COMPONENT Stable)
endif()

# NSIS VARIABLES
SET(CPACK_NSIS_DISPLAY_NAME "mCRL2")
SET(CPACK_NSIS_PACKAGE_NAME "mCRL2")
SET(CPACK_NSIS_MODIFY_PATH ON)

# Include CPack specific stuff
# ----------------------------
include(CPack)
