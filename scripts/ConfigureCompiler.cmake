# Compiler configuration shared on all platforms, on all compilers
set(MCRL2_LIB_DIR "lib/mcrl2")

if(CXX_COMPILER_ARCHITECTURE STREQUAL "x86")
  add_definitions("-DAT_32BIT")
endif()
if(CXX_COMPILER_ARCHITECTURE STREQUAL "x86_64")
  add_definitions("-DAT_64BIT")
endif()

set(CMAKE_EXE_LINKER_FLAGS_MAINTAINER "${CMAKE_EXE_LINKER_FLAGS_MAINTAINER}"
    CACHE STRING "Flags used for linking binaries during maintainer builds.")
set(CMAKE_SHARED_LINKER_FLAGS_MAINTAINER "${CMAKE_SHARED_LINKER_FLAGS_MAINTAINER}"
    CACHE STRING "Flags used by the shared libraries linker during maintainer builds.")
set(CMAKE_C_FLAGS_MAINTAINER "${CMAKE_C_FLAGS_MAINTAINER}" 
    CACHE STRING "Flags used by the C compiler during maintainer builds.")
set(CMAKE_CXX_FLAGS_MAINTAINER "${CMAKE_CXX_FLAGS_MAINTAINER}" 
    CACHE STRING "Flags used by the C++ compiler during maintainer builds.")

mark_as_advanced(
  CMAKE_CXX_FLAGS_MAINTAINER
  CMAKE_C_FLAGS_MAINTAINER
  CMAKE_EXE_LINKER_FLAGS_MAINTAINER
  CMAKE_SHARED_LINKER_FLAGS_MAINTAINER
)

# Perform platform-specific compiler configuration
include(ConfigureWindows)
include(ConfigureApple)
include(ConfigureUnix)

# Perform compiler-specific compiler configuration
include(ConfigureGNU)
include(ConfigureMSVC)
