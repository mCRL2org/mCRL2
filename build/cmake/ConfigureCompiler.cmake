# Compiler configuration shared on all platforms, on all compilers
set(CMAKE_EXE_LINKER_FLAGS_MAINTAINER "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${CMAKE_EXE_LINKER_FLAGS_MAINTAINER}")
set(CMAKE_SHARED_LINKER_FLAGS_MAINTAINER "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${CMAKE_EXE_LINKER_FLAGS_MAINTAINER}")
set(CMAKE_C_FLAGS_MAINTAINER "${CMAKE_C_FLAGS_DEBUG} ${CMAKE_C_FLAGS_MAINTAINER}")
set(CMAKE_CXX_FLAGS_MAINTAINER "${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_CXX_FLAGS_MAINTAINER}")

# Perform compiler-specific compiler configuration
if(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
  include(ConfigureMSVC)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
  include(ConfigureGNU)
else()
  message(FATAL_ERROR "Unsupported compiler setup (C: ${CMAKE_C_COMPILER_ID} / C++: ${CMAKE_CXX_COMPILER_ID}).")
endif()

# Check supported C++11 features
include(CheckCXX11Features)

# Add Maintainer mode to multi-configuration builds
if(CMAKE_CONFIGURATION_TYPES)
  list(APPEND CMAKE_CONFIGURATION_TYPES Maintainer)
  list(REMOVE_DUPLICATES CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
  "Semicolon separated list of configuration types, only supports Debug, Release, RelSizeMin, RelWithDebInfo, and Maintainer, anything else will be ignored"
  FORCE)
endif()
# Document maintainer mode for single-configuration builds
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  # Default type is set to Release as the performance of the toolset is bad in Debug or Maintainer mode.
  # The unitialised user of the toolset may not realise that this is the case causing a bad impression.
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "One of Debug, Release, RelWithDebInfo, MinSizeRel, Maintainer. Defaults to Release." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Maintainer" "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Mark as debug property to ensure that MSVC links against debug QT libraries
set_property(GLOBAL PROPERTY DEBUG_CONFIGURATIONS "Debug;Maintainer;RelWithDebInfo")

set(CMAKE_EXE_LINKER_FLAGS_MAINTAINER ${CMAKE_EXE_LINKER_FLAGS_MAINTAINER}
    CACHE STRING "Flags used for linking binaries ${BUILDTYPE}.")
set(CMAKE_SHARED_LINKER_FLAGS_MAINTAINER ${CMAKE_SHARED_LINKER_FLAGS_MAINTAINER}
    CACHE STRING "Flags used for linking shared libraries ${BUILDTYPE}.")
set(CMAKE_C_FLAGS_MAINTAINER ${CMAKE_C_FLAGS_MAINTAINER}
    CACHE STRING "Flags used for compiling C source files ${BUILDTYPE}.")
set(CMAKE_CXX_FLAGS_MAINTAINER ${CMAKE_CXX_FLAGS_MAINTAINER}
    CACHE STRING "Flags used for compiling C++ source files ${BUILDTYPE}.")
mark_as_advanced(
  CMAKE_CXX_FLAGS_MAINTAINER
  CMAKE_C_FLAGS_MAINTAINER
  CMAKE_EXE_LINKER_FLAGS_MAINTAINER
  CMAKE_SHARED_LINKER_FLAGS_MAINTAINER
)
