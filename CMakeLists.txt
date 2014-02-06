cmake_minimum_required(VERSION 2.8.11)
project(mCRL2)

# TODO: Clean up utilities/workarounds.h
# TODO: See if -fPIC can be replaced by CMAKE_POSITION_INDEPENDENT_CODE
# TODO: check RPATH settings
# TODO: remove the build/workarounds include (contains only FlexLexer.h) when we finally stop using the old FSM parser
# TODO: Check why ltsgraph/glscene.cpp is including GLUT in MacOS
include_directories(build/workarounds)

set(MCRL2_MIN_QT_VERSION 5.0.2)
set(MCRL2_MIN_BOOST_VERSION 1.44.0)

set(MCRL2_ENABLE_GUI_TOOLS ON
    CACHE BOOL "Enable compilation of tools with a graphical user interface.")
set(MCRL2_ENABLE_EXPERIMENTAL OFF
    CACHE BOOL "Enable compilation of experimental tools.")
set(MCRL2_ENABLE_DEPRECATED OFF
    CACHE BOOL "Enable compilation of deprecated tools.")
set(MCRL2_ENABLE_DEVELOPER OFF
    CACHE BOOL "Enable compilation of developer tools.")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/scripts")
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(GetCompilerVersion)
include(ConfigureCompiler)
include(AddMCRL2Binary)
include(MCRL2Version)

if(MCRL2_ENABLE_GUI_TOOLS)
  find_package(OpenGL REQUIRED)
  find_package(GL2PS  REQUIRED)
  find_package(TR     REQUIRED)
  find_package(Qt5Widgets ${MCRL2_MIN_QT_VERSION} REQUIRED)
  find_package(Qt5Xml     ${MCRL2_MIN_QT_VERSION} REQUIRED)
  find_package(Qt5Core    ${MCRL2_MIN_QT_VERSION} REQUIRED)
  find_package(Qt5OpenGL  ${MCRL2_MIN_QT_VERSION} REQUIRED)
endif()
find_package(Boost ${MCRL2_MIN_BOOST_VERSION} REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
find_package(BCG)

include(PrintBuildInfo)

add_subdirectory(3rd-party/dparser)

add_subdirectory(libraries)
add_subdirectory(tools)

include(MCRL2Packaging)
