cmake_minimum_required(VERSION 2.8.11)
project(mCRL2)

if(NOT ${CMAKE_VERSION} VERSION_LESS 3.0)
  # Ignore variable COMPILE_DEFINITIONS_<CONFIG>
  cmake_policy(SET CMP0043 NEW)
endif()

if (NOT ${CMAKE_VERSION} VERSION_LESS 3.1)
  # Do not implicitly dereference variables or keywords when quoted in an if() command.
  cmake_policy(SET CMP0054 NEW)
endif()

if(NOT ${CMAKE_VERSION} VERSION_LESS 3.10)
  # From CMake 3.10 onwards the AUTOMOC and AUTOUIC properties apply to GENERATED files.
  cmake_policy(SET CMP0071 OLD)
endif()

if(NOT ${CMAKE_VERSION} VERSION_LESS 3.11)
  # Choose the GLVND (GL Vendor Neutral Dispatch library) over OpenGL legacy if both are available.
  cmake_policy(SET CMP0072 NEW)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/build/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
enable_testing()

# TODO: Clean up utilities/workarounds.h
# TODO: See if -fPIC can be replaced by CMAKE_POSITION_INDEPENDENT_CODE
# TODO: See if we can get rid of the __STDC_VERSION__ define in ltsgraph/ltsview/diagraphica (currently included to prevent qopenglext.h from redefining int32_t etc.)
# TODO: Remove DEB-dependency on compiler (g++/clang). Instead let mcrl2compilerewriter report a user-friendly error message if no compiler can be found.
# TODO: Remove trace library (to LPS? or LTS?)
include_directories(build/workarounds)

if(APPLE)
# Lower versions of Qt do not support native menu's correctly on Apple.
# To use a lower version replace set window->menuBar()->setNativeMenuBar(true)
# by window->menuBar()->setNativeMenuBar(false) in line 138 of libraries/gui/include/mcrl2/gui/qt_tool.h.
   set(MCRL2_MIN_QT_VERSION 5.10.0)
else()
   set(MCRL2_MIN_QT_VERSION 5.5.0)
endif()
set(MCRL2_MIN_BOOST_VERSION 1.48.0)

option(MCRL2_ENABLE_STABLE          "Enable compilation of stable tools." ON)
option(MCRL2_ENABLE_GUI_TOOLS       "Enable compilation of tools with a graphical user interface." ON)
option(MCRL2_ENABLE_EXPERIMENTAL    "Enable compilation of experimental tools." OFF)
option(MCRL2_ENABLE_DEPRECATED      "Enable compilation of deprecated tools." OFF)
option(MCRL2_ENABLE_DEVELOPER       "Enable compilation of developer tools." OFF)
option(MCRL2_ENABLE_HEADER_TESTS    "Enable generation of headertest targets." OFF)
option(MCRL2_ENABLE_TESTS           "Enable generation of library, tool and random test targets." OFF)
option(MCRL2_ENABLE_BENCHMARKS      "Enable generation of benchmark targets." OFF)
option(MCRL2_ENABLE_CODE_COVERAGE   "Enable generation of code coverage statistics." OFF)
option(MCRL2_EXTRA_TOOL_TESTS       "Enable testing of tools on more mCRL2 specifications." OFF)
option(MCRL2_SKIP_LONG_TESTS        "Do not compile test code that takes too long when profiling is on." OFF)
option(MCRL2_TEST_JITTYC            "Also test the compiling rewriters in the library tests. This can be time consuming." OFF)
set(MCRL2_QT_APPS "" CACHE INTERNAL "Internally keep track of Qt apps for the packaging procedure")

mark_as_advanced(
  MCRL2_ENABLE_CODE_COVERAGE
  MCRL2_ENABLE_STABLE
  MCRL2_SKIP_LONG_TESTS
)

if(MCRL2_ENABLE_GUI_TOOLS)
  find_package(OpenGL     QUIET REQUIRED)
  find_package(TR         QUIET REQUIRED)
  find_package(CVC3       QUIET)
  find_package(Qt5 ${MCRL2_MIN_QT_VERSION} COMPONENTS Core OpenGL Widgets Xml Gui QUIET REQUIRED)

  set_target_properties(Qt5::Core Qt5::OpenGL Qt5::Widgets Qt5::Xml Qt5::Gui
                        PROPERTIES MAP_IMPORTED_CONFIG_MAINTAINER "RELEASE")

  # The directories for components are derived from Qt5_DIR, as such they can be marked as advanced.
  mark_as_advanced(Qt5_DIR Qt5Core_DIR Qt5OpenGL_DIR Qt5Widgets_DIR Qt5Xml_DIR Qt5Gui_DIR)
endif()

find_package(Boost ${MCRL2_MIN_BOOST_VERSION} QUIET REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

include(ConfigurePlatform)
include(ConfigureCompiler)
include(MCRL2Version)
include(AddMCRL2Binary)

if(MCRL2_ENABLE_CODE_COVERAGE)
  include(CodeCoverage)
endif()

if (${MCRL2_ENABLE_BENCHMARKS})
  add_subdirectory(benchmarks)
endif()

add_subdirectory(3rd-party/dparser)

add_subdirectory(libraries)
add_subdirectory(tools)
add_subdirectory(doc)
add_subdirectory(tests)

install(DIRECTORY examples DESTINATION ${MCRL2_RESOURCE_PATH} COMPONENT Examples)

# Add the packaging as a subdirectory to assure that CMake only performs install
# commands from this script _after_ all install commands from other subdirectories
# are completed.
add_subdirectory(build/packaging)

include(PrintBuildInfo)
