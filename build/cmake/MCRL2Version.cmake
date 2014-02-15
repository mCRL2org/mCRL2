# MCRL2Version.cmake
# 
# This script defines the version information that is displayed in the tools and
# the documentation. It defines the following variables:
#
#   MCRL2_MAJOR_VERSION: The major version number. This number should be set
#     by the maintainer who releases a new version of the toolset. This version
#     number should be of the form YYYYMM.B, where YYYY is the year of the 
#     release, MM the month of the release, and B the bugfix release number. For
#     a regular (scheduled) release, B should be set to 0. Example: the first
#     bugfix of the July 2011 release should be numbered 201107.1
#   MCRL2_MINOR_VERSION: This is the minor version number of the toolset, which
#     is equal to the current SVN revision number. A locally modified SVN 
#     revision will result in a MCRL2_MINOR_VERSION that ends in "M".
#   MCRL2_VERSION: The version string that should be displayed in tools. It 
#     consists of MCRL2_MAJOR_VERSION and MCRL2_MINOR_VERSION for development
#     builds, and only MCRL2_MAJOR_VERSION for release builds.
#
# The script also defines one advanced option:
#
#   MCRL2_PACKAGE_RELEASE: This boolean allows you to create a release package.
#     When set, MCRL2_VERSION is changed accordingly, and extra checks are put
#     in place to prevent users from unknowingly publishing a release that can
#     not be generated from a clean SVN checkout.
#
# Package maintainers may set the variable below to issue a new release.
set(MCRL2_MAJOR_VERSION "201310.0")
string(SUBSTRING ${MCRL2_MAJOR_VERSION} 0 4 MCRL2_COPYRIGHT_YEAR)

option(MCRL2_PACKAGE_RELEASE "Include release version information. This discards SVN revision information and uses only the MCRL2_MAJOR_VERSION CMake variable." FALSE)
mark_as_advanced(MCRL2_PACKAGE_RELEASE)

#
# Find Subversion information: revision number and a modified/not modified
# status.
#
set(MCRL2_MINOR_VERSION "Unknown")
find_package(Subversion)
if(SUBVERSION_FOUND)
  if(EXISTS "${CMAKE_SOURCE_DIR}/.svn")
    # Subversion available: minor revision becomes SVN revision
    Subversion_WC_INFO(${PROJECT_SOURCE_DIR} Project)
    set(MCRL2_MINOR_VERSION ${Project_WC_REVISION})
    # Check for local changes; if there are any, then minor revision gets a 
    # postfix "M"
    execute_process(
      COMMAND ${Subversion_SVN_EXECUTABLE} st -q ${PROJECT_SOURCE_DIR}
      OUTPUT_VARIABLE Project_WC_REVISION_M
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(NOT ${Project_WC_REVISION_M} STREQUAL "")
      set(MCRL2_MINOR_VERSION "${MCRL2_MINOR_VERSION}M")
      if(MCRL2_PACKAGE_RELEASE)
        # Do not allow maintainers to package a release from a modified build
        message(FATAL_ERROR "You are trying to package a release from an SVN repository that has local modifications.")
      endif()
    endif()
  else()
    message(WARNING "No version information could be included because ${CMAKE_SOURCE_DIR} is unversioned.")
  endif()
else()
  message(WARNING "No version information could be included because the Subversion package could not be found (are you using CMake <2.8?)")
endif()

# Try to read build/SourceVersion, and set that version
if(MCRL2_MINOR_VERSION STREQUAL "Unknown" AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/build/SourceVersion")
  include(${CMAKE_CURRENT_SOURCE_DIR}/build/SourceVersion)
  if(MCRL2_SOURCE_PACKAGE_REVISION)
    message(WARNING "You are building from a source package; assuming no local modifications.")
    set(MCRL2_MINOR_VERSION ${MCRL2_SOURCE_PACKAGE_REVISION})
  endif()
else()
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/build/SourceVersion.in" "${CMAKE_CURRENT_SOURCE_DIR}/build/SourceVersion" @ONLY)
endif()

if(MCRL2_PACKAGE_RELEASE)
  if(MCRL2_MINOR_VERSION STREQUAL "Unknown")
    # Allow packaging release from an exported version; this is apparently needed
    # for some linux distros.
    message(STATUS "WARNING! You are packaging a release without any version information being available!")
  endif(MCRL2_MINOR_VERSION STREQUAL "Unknown")
  set(MCRL2_VERSION "${MCRL2_MAJOR_VERSION}")
else()
  set(MCRL2_VERSION "${MCRL2_MAJOR_VERSION}.${MCRL2_MINOR_VERSION}")
endif()

if(CMAKE_CFG_INTDIR STREQUAL ".")
  add_definitions(-DMCRL2_BUILD_TYPE=${CMAKE_BUILD_TYPE})
else()
  add_definitions(-DMCRL2_BUILD_TYPE=${CMAKE_CFG_INTDIR})
endif()
