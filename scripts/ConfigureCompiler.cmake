# Compiler configuration shared on all platforms, on all compilers
if(CXX_COMPILER_ARCHITECTURE STREQUAL "x86")
  add_definitions("-DAT_32BIT")
endif()
if(CXX_COMPILER_ARCHITECTURE STREQUAL "x86_64")
  add_definitions("-DAT_64BIT")
endif()

set(CMAKE_EXE_LINKER_FLAGS_MAINTAINER "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${CMAKE_EXE_LINKER_FLAGS_MAINTAINER}"
    CACHE STRING "Flags used for linking binaries during maintainer builds.")
set(CMAKE_SHARED_LINKER_FLAGS_MAINTAINER "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} ${CMAKE_SHARED_LINKER_FLAGS_MAINTAINER}"
    CACHE STRING "Flags used by the shared libraries linker during maintainer builds.")
set(CMAKE_C_FLAGS_MAINTAINER "${CMAKE_C_FLAGS_DEBUG} ${CMAKE_C_FLAGS_MAINTAINER}" 
    CACHE STRING "Flags used by the C compiler during maintainer builds.")
set(CMAKE_CXX_FLAGS_MAINTAINER "${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_CXX_FLAGS_MAINTAINER}" 
    CACHE STRING "Flags used by the C++ compiler during maintainer builds.")

mark_as_advanced(
  CMAKE_CXX_FLAGS_MAINTAINER
  CMAKE_C_FLAGS_MAINTAINER
  CMAKE_EXE_LINKER_FLAGS_MAINTAINER
  CMAKE_SHARED_LINKER_FLAGS_MAINTAINER
)

if (CMAKE_CONFIGURATION_TYPES)
  list(APPEND CMAKE_CONFIGURATION_TYPES Maintainer)
  list(REMOVE_DUPLICATES CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
  "Semicolon separated list of configuration types, only supports Debug, Release, RelSizeMin, RelWithDebInfo, and Maintainer, anything else will be ignored"
  FORCE)
endif()

# Mark as debug property to ensure that MSVC links against debug QT libraries
set_property(GLOBAL PROPERTY DEBUG_CONFIGURATIONS "Debug;Maintainer;RelWithDebInfo")

# Perform platform-specific compiler configuration
include(ConfigureWindows)
include(ConfigureApple)
include(ConfigureUnix)

# Perform compiler-specific compiler configuration
include(ConfigureGNU)
include(ConfigureMSVC)

# Check supported C++11 features
include(CheckCXX11Features)
