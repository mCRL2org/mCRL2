##---------------------------------------------------
## Set Boost configuration
##---------------------------------------------------

# Set minimum version of Boost
set(MCRL2_BOOST_MINIMUM_VERSION 1.44.0)

# Use multithreaded
set(Boost_USE_MULTITHREADED ON)

# Use static libraries when compiled static
if( NOT BUILD_SHARED_LIBS )
  set(Boost_USE_STATIC_LIBS ON)
endif()

##----------------------------------------------------------
## Boost header files
##----------------------------------------------------------
find_package(Boost ${MCRL2_BOOST_MINIMUM_VERSION})

if(NOT Boost_FOUND)
  message( STATUS "The mCRL2 toolset requires the installation of Boost version ${MCRL2_BOOST_MINIMUM_VERSION} or higher." )
  message( STATUS "See http://www.mcrl2.org/release/user_manual/build_instructions/index.html for" )
  message( FATAL_ERROR "instructions on building mCRL2 with an external version of boost.")
endif()

# From this point on we assume that Boost_FOUND!
##----------------------------------------------
# Boost libraries are available for inclusion globally
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

# We only use header-only libraries, so no need to add the link directories
#link_directories(${Boost_LIBRARY_DIRS})

## Print additional warnings
#if( APPLE AND BUILD_SHARED_LIBS )
#  message( STATUS "")
#  message( STATUS "Warning: Ensure that \"${Boost_LIBRARY_DIRS}\" is included in DYLD_LIBRARY_PATH.")
#  message( STATUS "")
#endif()

