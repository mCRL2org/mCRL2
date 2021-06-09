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
#     is equal to the current Git commit short-hash. A locally modified Git
#     commit will result in a MCRL2_MINOR_VERSION that ends in "M".
#   MCRL2_VERSION: The version string that should be displayed in tools. It
#     consists of MCRL2_MAJOR_VERSION and MCRL2_MINOR_VERSION for development
#     builds, and only MCRL2_MAJOR_VERSION for release builds.
#
# The script also defines one advanced option:
#
#   MCRL2_PACKAGE_RELEASE: This boolean allows you to create a release package.
#     When set, MCRL2_VERSION is changed accordingly, and extra checks are put
#     in place to prevent users from unknowingly publishing a release that can
#     not be generated from a clean Git clone.
#
# Package maintainers may set the variable below to issue a new release.
set(MCRL2_MAJOR_VERSION "202106.0")
string(SUBSTRING ${MCRL2_MAJOR_VERSION} 0 4 MCRL2_COPYRIGHT_YEAR)

option(MCRL2_PACKAGE_RELEASE "Include release version information. This discards Git commit information and only uses the MCRL2_MAJOR_VERSION CMake variable." FALSE)
mark_as_advanced(MCRL2_PACKAGE_RELEASE)

# Find Git information: git short-hash and modified/not modified
set(MCRL2_MINOR_VERSION "Unknown")

if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  find_package(Git REQUIRED)
  # Prints the commit hash of the last commit and its short version.
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-list --max-count=1 --format=%h --abbrev=10 HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_REV_LIST
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  # Convert multiple lines into a list
  STRING(REPLACE "\n" ";" GIT_REV_LIST ${GIT_REV_LIST})
  # The second line is the short-hash, specified by %h
  LIST(GET GIT_REV_LIST 1 GIT_COMMIT_HASH)

  # List the changed files compared to the last pushed commit.
  execute_process(
    COMMAND ${GIT_EXECUTABLE} status --porcelain
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_FILES_CHANGED
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  # When the files changed are empty it was not modified.
  if("${GIT_FILES_CHANGED}" STREQUAL "")
    set(MCRL2_MINOR_VERSION "${GIT_COMMIT_HASH}")
  else()
    set(MCRL2_MINOR_VERSION "${GIT_COMMIT_HASH}M")
    if (${MCRL2_PACKAGE_RELEASE})
      message(FATAL_ERROR "You are trying to package a release from a Git repository that has local modifications.")
    endif()
  endif()

  # Try to read build/SourceVersion, and set that version
  if("${MCRL2_MINOR_VERSION}" STREQUAL "Unknown" AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/build/SourceVersion")
    include(${CMAKE_CURRENT_SOURCE_DIR}/build/SourceVersion)
    if(MCRL2_SOURCE_PACKAGE_REVISION)
      message(WARNING "You are building from a source package; assuming no local modifications.")
      set(MCRL2_MINOR_VERSION ${MCRL2_SOURCE_PACKAGE_REVISION})
    endif()
  else()
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/build/SourceVersion.in"
                   "${CMAKE_CURRENT_SOURCE_DIR}/build/SourceVersion" @ONLY)
  endif()
else()
  message(WARNING "No version information could be included because ${CMAKE_SOURCE_DIR} is unversioned.")
endif()

if(MCRL2_PACKAGE_RELEASE)
  if("${MCRL2_MINOR_VERSION}" STREQUAL "Unknown")
    # Allow packaging release from an exported version; this is apparently needed
    # for some linux distros.
    message(STATUS "WARNING! You are packaging a release without any version information being available!")
  endif()
  set(MCRL2_VERSION "${MCRL2_MAJOR_VERSION}")
else()
  set(MCRL2_VERSION "${MCRL2_MAJOR_VERSION}.${MCRL2_MINOR_VERSION}")
endif()

# This script can be included from other CMake files to set variables and add definitions,
# but it can also be run standalone in script mode. In that case, it will output the mCRL2
# version. This is used by the documentation and ensures that the version number of mCRL2
# needs to be changed in only one place: this file.
if(CMAKE_SCRIPT_MODE_FILE AND CMAKE_SCRIPT_MODE_FILE STREQUAL CMAKE_PARENT_LIST_FILE)
  message(STATUS "MCRL2_MAJOR_VERSION ${MCRL2_MAJOR_VERSION}")
  message(STATUS "MCRL2_MINOR_VERSION ${MCRL2_MINOR_VERSION}")
else()
  if("${CMAKE_CFG_INTDIR}" STREQUAL ".")
    add_definitions(-DMCRL2_BUILD_TYPE=${CMAKE_BUILD_TYPE})
  else()
    add_definitions(-DMCRL2_BUILD_TYPE=${CMAKE_CFG_INTDIR})
  endif()
endif()
