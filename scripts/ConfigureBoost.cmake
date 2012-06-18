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
endif( NOT BUILD_SHARED_LIBS)

if(MSVC)
  SET (Boost_USE_STATIC_LIBS ON)

  find_package(Boost ${MCRL2_BOOST_MINIMUM_VERSION} COMPONENTS filesystem signals system serialization REQUIRED)
     # Disable boost::regex and use sregex instead.
     # using MSVC 2010, BoostPro 1.44, results in following link error:
     # mcrl2_utilities.lib(command_line_interface.obj) :
     # error LNK2019: unresolved external symbol "char __fastcall boost::re_detail::w32_toupper(char,unsigned long)" (?w32_toupper@re_detail@boost@@YIDDK@Z) referenced in function "public: char __thiscall boost::w32_regex_traits<char>::toupper(char)const " (?toupper@?$w32_regex_traits@D@boost@@QBEDD@Z)
     add_definitions( -DMCRL2_DISABLE_BOOST_REGEX )
else(MSVC)
  find_package(Boost ${MCRL2_BOOST_MINIMUM_VERSION} COMPONENTS system serialization signals filesystem regex REQUIRED)
endif(MSVC)

if(NOT Boost_FOUND)
  message( STATUS "The mCRL2 toolset requires the installation of Boost version v1.35 or higher." )
  message( STATUS "See http://www.mcrl2.org/mcrl2/wiki/index.php/CMake_build_instructions for" )
  message( FATAL_ERROR "instructions on building mCRL2 with an external version of boost.")
endif(NOT Boost_FOUND)

if (Boost_FOUND)
   include_directories(${Boost_INCLUDE_DIRS})
   link_directories(${Boost_LIBRARY_DIRS})
endif (Boost_FOUND)


## Print additional warnings
if( Boost_FOUND AND APPLE AND BUILD_SHARED_LIBS )
  message( STATUS "")
  message( STATUS "Warning: Ensure that \"${Boost_LIBRARY_DIRS}\" is included in DYLD_LIBRARY_PATH.")
  message( STATUS "")
endif( Boost_FOUND AND APPLE AND BUILD_SHARED_LIBS )

