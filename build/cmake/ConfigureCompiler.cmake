# Perform compiler-specific compiler configuration
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  if(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 19.20)
    message(FATAL_ERROR "MSVC version 19.20 (aka MSVC 2019 v16.0) is required.")
  endif()
  include(ConfigureMSVC)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  if(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 7.0)
    message(FATAL_ERROR "GCC version must be at least 7.0.")
  endif()
  include(ConfigureGNU)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  if(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 5.0)
    message(FATAL_ERROR "Clang version must be at least 5.0.")
  endif()
  include(ConfigureGNU)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
  if(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 10.0.1)
    message(FATAL_ERROR "AppleClang version must be at least 10.0.1")
  endif()
  include(ConfigureGNU)
else()
  message(FATAL_ERROR "Unsupported compiler setup (C: ${CMAKE_C_COMPILER_ID} / C++: ${CMAKE_CXX_COMPILER_ID}).")
endif()

# Sets the default build type if none was provided.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Sets the build type, defaults to Release." FORCE)
endif()

if(CMAKE_BUILD_TYPE)
  # Enables a selection of build types instead of typing it.
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo") 
endif()

# Add the definition to disable soundness checks when the configuration is set to OFF.
if(NOT ${MCRL2_ENABLE_DEBUG_SOUNDNESS_CHECKS})
  add_definitions(-DMCRL2_NO_SOUNDNESS_CHECKS)
endif()

# Enable C++17 for all targets.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
