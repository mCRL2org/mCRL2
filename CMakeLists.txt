cmake_minimum_required(VERSION 2.8.11)
project(mCRL2)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/build/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# TODO: Clean up utilities/workarounds.h
# TODO: See if -fPIC can be replaced by CMAKE_POSITION_INDEPENDENT_CODE
# TODO: check RPATH settings
# TODO: remove the build/workarounds include (contains only FlexLexer.h) when we finally stop using the old FSM parser
# TODO: See if we can get rid of the __STDC_VERSION__ define in ltsgraph/ltsview/diagraphica (currently included to prevent qopenglext.h from redefining int32_t etc.)
include_directories(build/workarounds)

set(MCRL2_MIN_QT_VERSION 5.2.0)
set(MCRL2_MIN_BOOST_VERSION 1.44.0)

option(MCRL2_ENABLE_STABLE       "Enable compilation of stable tools." ON)
option(MCRL2_ENABLE_GUI_TOOLS    "Enable compilation of tools with a graphical user interface." ON)
option(MCRL2_ENABLE_EXPERIMENTAL "Enable compilation of experimental tools." OFF)
option(MCRL2_ENABLE_DEPRECATED   "Enable compilation of deprecated tools." OFF)
option(MCRL2_ENABLE_DEVELOPER    "Enable compilation of developer tools." OFF)

include(ConfigurePlatform)
include(ConfigureCompiler)
include(MCRL2Version)
include(AddMCRL2Binary)

if(MCRL2_ENABLE_GUI_TOOLS)
  find_package(OpenGL QUIET REQUIRED)
  find_package(GL2PS  QUIET REQUIRED)
  find_package(TR     QUIET REQUIRED)
  find_package(Qt5Widgets ${MCRL2_MIN_QT_VERSION} QUIET REQUIRED)
  find_package(Qt5Xml     ${MCRL2_MIN_QT_VERSION} QUIET REQUIRED)
  find_package(Qt5Core    ${MCRL2_MIN_QT_VERSION} QUIET REQUIRED)
  find_package(Qt5OpenGL  ${MCRL2_MIN_QT_VERSION} QUIET REQUIRED)
endif()
find_package(Boost ${MCRL2_MIN_BOOST_VERSION} QUIET REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
find_package(BCG QUIET)

add_subdirectory(3rd-party/dparser)

add_subdirectory(libraries)
add_subdirectory(tools)
add_subdirectory(doc)

include(MCRL2CompilingRewriter)
include(MCRL2Packaging)
include(PrintBuildInfo)
