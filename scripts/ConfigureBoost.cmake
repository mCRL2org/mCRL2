##---------------------------------------------------
## Set Boost configuration
##---------------------------------------------------

# Set minimum version of Boost
set(MCRL2_BOOST_MINIMUM_VERSION 1.35.0)

# Use multithreaded
set(Boost_USE_MULTITHREADED ON)

# Use static libraries when compiled static
if( NOT BUILD_SHARED_LIBS )
  set(Boost_USE_STATIC_LIBS ON)
endif()

##----------------------------------------------------------
## Boost System libraries are only required for test targets
##----------------------------------------------------------
find_package(Boost ${MCRL2_BOOST_MINIMUM_VERSION} COMPONENTS system REQUIRED)


if(NOT Boost_FOUND)
  message( STATUS "The mCRL2 toolset requires the installation of Boost version ${MCRL2_BOOST_MINIMUM_VERSION} or higher." )
  message( STATUS "See http://www.mcrl2.org/mcrl2/wiki/index.php/CMake_build_instructions for" )
  message( FATAL_ERROR "instructions on building mCRL2 with an external version of boost.")
endif()

# From this point on we assume that Boost_FOUND!
##----------------------------------------------

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

## Print additional warnings
if( APPLE AND BUILD_SHARED_LIBS )
  message( STATUS "")
  message( STATUS "Warning: Ensure that \"${Boost_LIBRARY_DIRS}\" is included in DYLD_LIBRARY_PATH.")
  message( STATUS "")
endif()

