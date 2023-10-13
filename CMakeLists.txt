cmake_minimum_required(VERSION 3.14)
project(mCRL2)

if(POLICY CMP0054)
  cmake_policy(SET CMP0054 NEW)
endif()

if(POLICY CMP0065)
  # Do not export symbols from executables unless ENABLE_EXPORTS is explicitly set.
  cmake_policy(SET CMP0065 NEW)
endif()

if(POLICY CMP0071)
  # The AUTOMOC and AUTOUIC properties should not apply to GENERATED files.
  cmake_policy(SET CMP0071 NEW)
endif()

if(POLICY CMP0072)
  # Choose the GLVND (GL Vendor Neutral Dispatch library) over OpenGL legacy if both are available.
  cmake_policy(SET CMP0072 NEW)
endif()

if(POLICY CMP0110)
  # add_test() supports arbitrary characters in test names.
  cmake_policy(SET CMP0110 NEW)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/build/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
enable_testing()

option(MCRL2_ENABLE_GUI_TOOLS       "Enable compilation of tools with a graphical user interface." ON)
option(MCRL2_ENABLE_BOOST_JSON_SUPPORT "Enable compilation of tools and libraries that require boost::json. This raises the minimal boost version requirement to 1.71.0." OFF)
option(MCRL2_ENABLE_EXPERIMENTAL    "Enable compilation of experimental tools." OFF)
option(MCRL2_ENABLE_DEPRECATED      "Enable compilation of deprecated tools." OFF)
option(MCRL2_ENABLE_DEVELOPER       "Enable compilation of developer tools." OFF)
option(MCRL2_ENABLE_HEADER_TESTS    "Enable generation of headertest targets." OFF)
option(MCRL2_ENABLE_TESTS           "Enable generation of library, tool and random test targets." OFF)
option(MCRL2_ENABLE_DOCUMENTATION   "Enable generation of documentation." OFF)
option(MCRL2_ENABLE_DOC_MANUAL      "Enable generation of tool manual pages, which requires built tools." ON)
option(MCRL2_ENABLE_DOC_DOXYGEN     "Enable generation of code reference using Doxygen." ON)
option(MCRL2_ENABLE_DOC_PDFLATEX    "Enable generation of reference pdfs using pdflatex, which requires tikz and amsmath packages." ON)
option(MCRL2_ENABLE_BENCHMARKS      "Enable benchmarks. Build the 'benchmarks' target to generate the necessary files and tools. Run the benchmarks using ctest." OFF)
option(MCRL2_ENABLE_SYLVAN          "Enable the Sylvan library required by the following symbolic tools: lpsreach, pbessolvesymbolic and ltsconvertsymbolic" ${UNIX})
option(MCRL2_ENABLE_MULTITHREADING  "Enable the usage of multiple threads. Disabling removes usage of synchronisation primitives" ON)
option(MCRL2_EXTRA_TOOL_TESTS       "Enable testing of tools on more mCRL2 specifications." OFF)
option(MCRL2_TEST_JITTYC            "Also test the compiling rewriters in the library tests. This can be time consuming." OFF)
set(MCRL2_QT_APPS "" CACHE INTERNAL "Internally keep track of Qt apps for the packaging procedure")

option(MCRL2_ENABLE_ADDRESSSANITIZER "Enable additional memory run-time checks implemented by the AddressSanitizer." OFF)
option(MCRL2_ENABLE_THREADSANITIZER  "Enable additional data race run-time checks implemented by the ThreadSanitizer." OFF)
option(MCRL2_ENABLE_CODE_COVERAGE    "Enable generation of code coverage statistics." OFF)
option(MCRL2_ENABLE_DEBUG_SOUNDNESS_CHECKS "Enable extensive soundness check in the Debug build type." ON)
option(MCRL2_ENABLE_MSVC_CCACHE     "Enable MSVC specific flags to enable building with ccache" OFF)
option(MCRL2_ENABLE_STABLE          "Enable compilation of stable tools." ON)
option(MCRL2_SKIP_LONG_TESTS        "Do not execute tests that take a long time to run." OFF)

mark_as_advanced(
  MCRL2_ENABLE_ADDRESSSANITIZER
  MCRL2_ENABLE_CODE_COVERAGE
  MCRL2_ENABLE_DEBUG_SOUNDNESS_CHECKS 
  MCRL2_ENABLE_MSVC_CCACHE
  MCRL2_ENABLE_STABLE
)

if(MCRL2_ENABLE_BOOST_JSON_SUPPORT)
  set(MCRL2_MIN_BOOST_VERSION 1.75.0)
else()
  set(MCRL2_MIN_BOOST_VERSION 1.71.0)
endif()


if(MCRL2_ENABLE_GUI_TOOLS)
  find_package(OpenGL     QUIET REQUIRED)
  find_package(TR         QUIET REQUIRED)
  find_package(CVC3       QUIET)
  find_package(Qt5 5.12.0 COMPONENTS Core OpenGL Widgets Xml Gui QUIET REQUIRED)
	
  # The directories for components are derived from Qt5_DIR, as such they can be marked as advanced.
  mark_as_advanced(Qt5_DIR Qt5Core_DIR Qt5OpenGL_DIR Qt5Widgets_DIR Qt5Xml_DIR Qt5Gui_DIR)
endif()

find_package(Boost ${MCRL2_MIN_BOOST_VERSION} QUIET REQUIRED)

include(ConfigurePlatform)
include(ConfigureCompiler)
include(MCRL2Version)
include(AddMCRL2Binary)

if(MCRL2_ENABLE_CODE_COVERAGE)
  include(CodeCoverage)
endif()

add_subdirectory(3rd-party/dparser)
if (MCRL2_ENABLE_SYLVAN)
  add_subdirectory(3rd-party/lace)
  add_subdirectory(3rd-party/sylvan)
endif()

if (MCRL2_ENABLE_BENCHMARKS)
  add_subdirectory(benchmarks)
endif()

# A list of tools that is being built by the current configuration (populated by build/cmake/AddMCRL2Binary.cmake)
set_property(GLOBAL PROPERTY MCRL2_TOOLS)

add_subdirectory(libraries)
add_subdirectory(tools)

if (MCRL2_ENABLE_DOCUMENTATION)
  add_subdirectory(doc)
endif()

if (MCRL2_ENABLE_TESTS)
  add_subdirectory(tests)
endif()

install(DIRECTORY examples DESTINATION ${MCRL2_RESOURCE_PATH} COMPONENT Examples)

# Add the packaging as a subdirectory to assure that CMake only performs install
# commands from this script _after_ all install commands from other subdirectories
# are completed.
add_subdirectory(build/packaging)

include(PrintBuildInfo)
