cmake_minimum_required(VERSION 2.8.11)
project(mCRL2)

cmake_policy(SET CMP0020 NEW)
# cmake_policy(SET CMP0043 NEW)  TODO: set policies according to CMake version (this one is defined from 3.0 onwards)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/build/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
enable_testing()

# TODO: Clean up utilities/workarounds.h
# TODO: See if -fPIC can be replaced by CMAKE_POSITION_INDEPENDENT_CODE
# TODO: remove the build/workarounds include (contains only FlexLexer.h) when we finally stop using the old FSM parser
# TODO: See if we can get rid of the __STDC_VERSION__ define in ltsgraph/ltsview/diagraphica (currently included to prevent qopenglext.h from redefining int32_t etc.)
# TODO: Remove DEB-dependency on compiler (g++/clang). Instead let mcrl2compilerewriter report a user-friendly error message if no compiler can be found.
include_directories(build/workarounds)

set(MCRL2_MIN_QT_VERSION 5.2.0)
set(MCRL2_MIN_BOOST_VERSION 1.44.0)

option(MCRL2_ENABLE_STABLE          "Enable compilation of stable tools." ON)
option(MCRL2_ENABLE_GUI_TOOLS       "Enable compilation of tools with a graphical user interface." ON)
option(MCRL2_ENABLE_EXPERIMENTAL    "Enable compilation of experimental tools." OFF)
option(MCRL2_ENABLE_DEPRECATED      "Enable compilation of deprecated tools." OFF)
option(MCRL2_ENABLE_DEVELOPER       "Enable compilation of developer tools." OFF)
option(MCRL2_ENABLE_TEST_EXAMPLES   "Enable the tests in the example directory of each library." ON)
option(MCRL2_ENABLE_TEST_LIBRARIES  "Enable the tests in the test directory of each library." ON)
option(MCRL2_ENABLE_TEST_HEADERS    "Enable testing of self-containment of headers." OFF)
option(MCRL2_EXTRA_TOOL_TESTS       "Enable testing of tools on more mCRL2 specifications." OFF)
set(MCRL2_QT_APPS "" CACHE INTERNAL "Internally keep track of Qt apps for the packaging procedure")

mark_as_advanced(MCRL2_ENABLE_STABLE)

include(ConfigurePlatform)
include(ConfigureCompiler)
include(MCRL2Version)
include(AddMCRL2Binary)

if(MCRL2_ENABLE_GUI_TOOLS)
  find_package(OpenGL     QUIET REQUIRED)
  find_package(GL2PS      QUIET REQUIRED)
  find_package(TR         QUIET REQUIRED)
  find_package(CVC3       QUIET)
  find_package(Qt5Core    QUIET REQUIRED)
  find_package(Qt5OpenGL  QUIET REQUIRED)
  find_package(Qt5Widgets QUIET REQUIRED)
  find_package(Qt5Xml     QUIET REQUIRED)
  find_package(Qt5Gui     QUIET REQUIRED)
  set_target_properties(Qt5::Core Qt5::OpenGL Qt5::Widgets Qt5::Xml Qt5::Gui
                        PROPERTIES MAP_IMPORTED_CONFIG_MAINTAINER "DEBUG")
endif()
find_package(Boost ${MCRL2_MIN_BOOST_VERSION} QUIET REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

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
