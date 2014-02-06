# Compiler configuration shared on all platforms, on all compilers
set(CMAKE_EXE_LINKER_FLAGS_MAINTAINER ${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${CMAKE_EXE_LINKER_FLAGS_MAINTAINER})
set(CMAKE_SHARED_LINKER_FLAGS_MAINTAINER ${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${CMAKE_EXE_LINKER_FLAGS_MAINTAINER})
set(CMAKE_C_FLAGS_MAINTAINER ${CMAKE_C_FLAGS_DEBUG} ${CMAKE_C_FLAGS_MAINTAINER})
set(CMAKE_CXX_FLAGS_MAINTAINER ${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_CXX_FLAGS_MAINTAINER})

# Perform platform-specific compiler configuration
include(ConfigureWindows)
include(ConfigureApple)
include(ConfigureUnix)

# Perform compiler-specific compiler configuration
include(ConfigureGNU)
include(ConfigureMSVC)

# Check supported C++11 features
include(CheckCXX11Features)

# Add Maintainer mode to multi-configuration builds
if (CMAKE_CONFIGURATION_TYPES)
  list(APPEND CMAKE_CONFIGURATION_TYPES Maintainer)
  list(REMOVE_DUPLICATES CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
  "Semicolon separated list of configuration types, only supports Debug, Release, RelSizeMin, RelWithDebInfo, and Maintainer, anything else will be ignored"
  FORCE)
endif()
# Document maintainer mode for single-configuration builds
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE}
    CACHE STRING "One of Debug, Release, RelWithDebInfo, MinSizeRel, Maintainer. Defaults to Debug." FORCE)

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
