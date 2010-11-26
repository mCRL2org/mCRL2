# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# Set Warning

cmake_minimum_required (VERSION 2.6)
message(STATUS "CMAKE VERSION: ${CMAKE_VERSION}")
project (MCRL2)

# Commonly used options
# ---------------------
#
# The following lists some commonly used options to configure the build system
# (an option name following by (*) indicates the default option):
#
#   CMAKE_INSTALL_PREFIX = /usr/local(*) (any path can be used)
#   CMAKE_BUILD_TYPE     = None | Debug | Release(*) | RelwithDebInfo | MinSizeRel
#   BUILD_SHARED_LIBS    = ON    | OFF(*)
#   MCRL2_ENABLE_EXPERIMENTAL  = ON    | OFF(*)
#   MCRL2_ENABLE_DEPRECATED    = ON    | OFF(*)


# Note on adding definitions containing quotes (")
# ------------------------------------------------
#
# Compiler definitions can be added to CMake using the ADD_DEFINITIONS command
# or COMPILE_DEFINITIONS property. Be careful when adding a definition that
# contains quotes ("), as CMake cannot guarantee the generation of platform
# independent scripts. Typically these definitions are of the form FOO="bar",
# which can be added as follows:
#
#   add_definitions(-DFOO="bar")
#
# From this, CMake is able to generate correct Makefiles. However, CMake is
# not able to generate a correct Eclipse CDT4 project.
#
# The solution that is currently employed is by adding compiler definitions
# that contain quotes using the set_source_files_properties command with the
# the COMPILE_DEFINITIONS property:
#
#   set_source_files_properties(baz.cpp
#     PROPERTIES COMPILE_DEFINITIONS FOO="bar"
#   )
#
# This way, these compiler definitions are not included in the Eclipse CDT4
# project file, circumventing the problem.
#
# When this solution cannot be applied, there is another workaround: by quoting
# the entire compiling definition it is prevented from being incorporated in
# the Eclipse CDT4 project:
#
#   add_definitions(-D"FOO=\\"bar\\"")
#
# However, the best solution is to avoid the need for these quotes in the
# first place. This is mentioned as a disclaimer in the CMake 2.6 documentation
# for the COMPILER_DEFINITIONS property:
#
#   Dislaimer: Most native build tools have poor support for escaping
#   certain values. CMake has work-arounds for many cases but some values
#   may just not be possible to pass correctly. If a value does not seem to
#   be escaped correctly, do not attempt to work-around the problem by
#   adding escape sequences to the value. Your work-around may break in a
#   future version of CMake that has improved escape support. Instead
#   consider defining the macro in a (configured) header file. Then report
#   the limitation.
#
# The disclaimer with some additional information can be found here:
#
#   http://www.cmake.org/cmake/help/cmake2.6docs.html#prop_dir:COMPILE_DEFINITIONS
#

# Variables required for an additional build type "Maintainer"

set(CMAKE_CXX_FLAGS_MAINTAINER "-g -O0 -Wall -W -Wunused-variable -Wunused-parameter -Wunused-function -Wunused -Wno-system-headers -Woverloaded-virtual -Wwrite-strings -fprofile-arcs -ftest-coverage" CACHE STRING "Flags used by the C++ compiler during maintainer builds." FORCE)
set(CMAKE_C_FLAGS_MAINTAINER "-g -O0 -Wall -pedantic -W -fprofile-arcs -ftest-coverage" CACHE STRING "Flags used by the C compiler during maintainer builds." FORCE)
set(CMAKE_EXE_LINKER_FLAGS_MAINTAINER "-Wl,--warn-unresolved-symbols,--warn-once -fprofile-arcs -ftest-coverage" CACHE STRING "Flags used for linking binaries during maintainer builds." FORCE)
set(CMAKE_SHARED_LINKER_FLAGS_MAINTAINER "-Wl,--warn-unresolved-symbols,--warn-once -fprofile-arcs -ftest-coverage" CACHE STRING "Flags used by the shared libraries linker during maintainer builds." FORCE)
mark_as_advanced(
  CMAKE_CXX_FLAGS_MAINTAINER
  CMAKE_C_FLAGS_MAINTAINER
  CMAKE_EXE_LINKER_FLAGS_MAINTAINER
  CMAKE_SHARED_LINKER_FLAGS_MAINTAINER
)

##---------------------------------------------------
## Print compiler information
##---------------------------------------------------

if(NOT MSVC)
execute_process(COMMAND ${CMAKE_CXX_COMPILER} --version
    OUTPUT_VARIABLE CXX_COMPILER_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

STRING(REGEX REPLACE "^([^\n]+).*" "\\1" CXX_COMPILER_VERSION "${CXX_COMPILER_VERSION}" )
else(NOT MSVC)
   set(CXX_COMPILER_VERSION "MSVC-${MSVC_VERSION}")
endif(NOT MSVC)

message(STATUS "CXX COMPILER VERSION: ${CXX_COMPILER_VERSION}")

##---------------------------------------------------
## Determine build type
##---------------------------------------------------

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE} " )

if( CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX )

  ##---------------------------------------------------
  ## Set global compiler flags (if available)
  ##---------------------------------------------------

  include(CheckCXXCompilerFlag)
  check_cxx_compiler_flag(-Wall CXX_ACCEPTS_WALL )
  if( CXX_ACCEPTS_WALL )
    add_definitions( -Wall )
  endif( CXX_ACCEPTS_WALL )

  check_cxx_compiler_flag(-Wno-inline CXX_ACCEPTS_WNO-INLINE )
  if( CXX_ACCEPTS_WNO-INLINE )
    add_definitions( -Wno-inline )
  endif( CXX_ACCEPTS_WNO-INLINE )

  if( NOT MINGW )
    check_cxx_compiler_flag(-pthread CXX_ACCEPTS_PTHREAD )
    if( CXX_ACCEPTS_PTHREAD )
      add_definitions( -pthread )
    endif( CXX_ACCEPTS_PTHREAD )
  endif( NOT MINGW )

  check_cxx_compiler_flag(-fno-strict-overflow CXX_ACCEPTS_FNO-STRICT-OVERFLOW )
  if( CXX_ACCEPTS_FNO-STRICT-OVERFLOW )
    add_definitions( -fno-strict-overflow )
  endif( CXX_ACCEPTS_FNO-STRICT-OVERFLOW )

  check_cxx_compiler_flag(-pipe CXX_ACCEPTS_PIPE )
  if( CXX_ACCEPTS_PIPE )
    add_definitions( -pipe )
  endif( CXX_ACCEPTS_PIPE )

  # Following flag will cause warning on MacOSX, if enabled:
  #686-apple-darwin9-g++-4.0.1: -as-needed: linker input file unused because linking not done
  if( NOT APPLE)
    check_cxx_compiler_flag(-Wl,-as-needed CXX_ACCEPTS_AS_NEEDED )
    if( CXX_ACCEPTS_AS_NEEDED )
      add_definitions( -Wl,-as-needed )
    endif( CXX_ACCEPTS_AS_NEEDED )
  endif( NOT APPLE )

  # Display include dependencies
  # check_cxx_compiler_flag(-H CXX_ACCEPTS_H )
  # if( CXX_ACCEPTS_H )
  #  add_definitions( -H )
  # endif( CXX_ACCEPTS_H )

  # check_cxx_compiler_flag(-Wunreachable-code CXX_ACCEPTS_UNREACHABLE-CODE )
  # if( CXX_ACCEPTS_UNREACHABLE-CODE )
  #   add_definitions( -Wunreachable-code )
  # endif( CXX_ACCEPTS_UNREACHABLE-CODE )

endif( CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX )

if(MSVC)
  set (CMAKE_CXX_FLAGS "/EHa ${CMAKE_CXX_FLAGS}")
endif(MSVC)

## --------------------------------------------------
## Detect architecture
## --------------------------------------------------

if (UNIX AND NOT WIN32)
  if (CMAKE_SIZEOF_VOID_P MATCHES "8")
    message (STATUS "Architecture: 64bit Unix" )
    set(ATERM_FLAGS "-m64 -DAT_64BIT")
    add_definitions("-DAT_64BIT")
    # set(ARCHITECTURE "x86_64")
  endif (CMAKE_SIZEOF_VOID_P MATCHES "8")
  if (CMAKE_SIZEOF_VOID_P MATCHES "4")
    message (STATUS "Architecture: 32bit Unix" )
    set(ATERM_FLAGS "-m32")
    # set(ARCHITECTURE "i386")
  endif (CMAKE_SIZEOF_VOID_P MATCHES "4")
endif (UNIX AND NOT WIN32)

##---------------------------------------------------
## Remove "install_tools"
## install_tools contains the tools marked for symlink export
##---------------------------------------------------
if(APPLE)
  if(EXISTS "${CMAKE_BINARY_DIR}/install_tools" )
    FILE(REMOVE ${CMAKE_BINARY_DIR}/install_tools )
  endif(EXISTS "${CMAKE_BINARY_DIR}/install_tools" )
endif(APPLE)


##---------------------------------------------------
## Print CMake install prefix (if any)
##---------------------------------------------------

message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")


##---------------------------------------------------
## Print the way how libraries are built
##---------------------------------------------------

if(MSVC)
  option(BUILD_SHARED_LIBS "Enable/disable creation of shared libraries" OFF)
else(MSVC)
  option(BUILD_SHARED_LIBS "Enable/disable creation of shared libraries" ON)
endif(MSVC)

## SET mCRL2 Library Directory
# Unix
if(UNIX)
  set(MCRL2_LIB_DIR "lib/mcrl2")
endif(UNIX)

# Override MCRL2_LIB_DIR such that libraries are stored in
# bin directory, such that they can be found after an install
if(WIN32)
  set(MCRL2_LIB_DIR "bin")
endif(WIN32)

if(APPLE AND MCRL2_SINGLE_BUNDLE)
  set(MCRL2_BIN_DIR "MacOS")
else(APPLE AND MCRL2_SINGLE_BUNDLE)
  set(MCRL2_BIN_DIR "bin")
endif(APPLE AND MCRL2_SINGLE_BUNDLE)


if(BUILD_SHARED_LIBS)
  if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    if(NOT MINGW)
      check_cxx_compiler_flag(-fPIC CXX_ACCEPTS_FPIC )
      if( CXX_ACCEPTS_FPIC )
        add_definitions( -fPIC )
        set(BUILD_SHARED_LIBS_OPTION "-fPIC")
      endif( CXX_ACCEPTS_FPIC )
    endif(NOT MINGW)
  endif( CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)

  if(CMAKE_CXX_COMPILER MATCHES "icpc")
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag(-fPIC CXX_ACCEPTS_FPIC )
    if( CXX_ACCEPTS_FPIC )
      add_definitions( -fPIC )
      set(BUILD_SHARED_LIBS_OPTION "-fPIC")
    endif( CXX_ACCEPTS_FPIC )
  endif(CMAKE_CXX_COMPILER MATCHES "icpc")

  if(WIN32)
    if(MINGW OR MSYS)
      get_filename_component( MINGW_LIB_DIR ${CMAKE_CXX_COMPILER} PATH )

      # Install required MINGW DLLs
      install(
	FILES
        ${MINGW_LIB_DIR}/libgcc_s_dw2-1.dll
	${MINGW_LIB_DIR}/mingwm10.dll
        DESTINATION ${MCRL2_BIN_DIR}
      )

      if  (MCRL2_ENABLE_GUI_TOOLS)
        # Install compiled wxWidgets DLLs
        # Libraries are searched for in following order:
        # 1) relative path to wxWidgets_CONFIG_EXECUTABLE
        # 2) relative path to wxWidgets_wxrc_EXECUTABLE        (not implemented)
        # 3) to given wxWidgets_LIB_DIR                   (not implemented)
        # 4) to given wxWidgets_ROOT_DIR                  (not implemented)

        if( NOT ${wxWidgets_CONFIG_EXECUTABLE} MATCHES wxWidgets_wxrc_EXECUTABLE-NOTFOUND )
          get_filename_component(MINGW_wxWidgets_LIB_DIR ${wxWidgets_CONFIG_EXECUTABLE} PATH )
          STRING(REPLACE "${MCRL2_BIN_DIR}" "lib" MINGW_wxWidgets_LIB_DIR "${MINGW_wxWidgets_LIB_DIR}" )
        endif( NOT ${wxWidgets_CONFIG_EXECUTABLE} MATCHES wxWidgets_wxrc_EXECUTABLE-NOTFOUND )

    	# Copy wxWidgets files on install
	    if(MINGW_wxWidgets_LIB_DIR)
	      install(
             DIRECTORY
                   ${MINGW_wxWidgets_LIB_DIR}/
	           DESTINATION ${MCRL2_BIN_DIR}
	           PATTERN "dll"
             )
          else(MINGW_wxWidgets_LIB_DIR)
            message( STATUS "wxWidgets DLLs cannot be deterimed for a shared build." )
            message( STATUS "Copy DLLs manually." )
	      endif(MINGW_wxWidgets_LIB_DIR)
      endif(MCRL2_ENABLE_GUI_TOOLS )

    endif(MINGW OR MSYS)
  endif(WIN32)

else(BUILD_SHARED_LIBS)
  set(BUILD_SHARED_LIBS false)
endif(BUILD_SHARED_LIBS)
message(STATUS "BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}")


##-----------------------------------------------------
## Set runtime path variable RPATH for install targets
##-----------------------------------------------------

# use, i.e. don't skip the full RPATH for the build tree
set(CMAKE_SKIP_BUILD_RPATH false)

# when building, don't use the install RPATH already
# (but later on when installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH false)

# the RPATH to be used when installing (non-OSX, use CMAKE_INSTALL_NAME_DIR instead)
if(NOT CMAKE_INSTALL_RPATH)
  set(CMAKE_INSTALL_RPATH "$ORIGIN/../${MCRL2_LIB_DIR}")
endif(NOT CMAKE_INSTALL_RPATH)

# Mac OSX directory name for installed targets
# Installing prerequisite shared libraries is dealt with in "RelocateInstallTree.cmake"
set(CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/${MCRL2_LIB_DIR}")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH true)

##---------------------------------------------------
## Determine latest SVN revision
##---------------------------------------------------

set(MCRL2_SVN_LABEL "" CACHE STRING "Overwrite SVN revision with a custom string" )
if( MCRL2_SVN_LABEL STREQUAL "" )
  find_program(SVNVERSION
    svnversion
    /usr/local/bin
    /usr/pkg/bin
    /usr/bin
  )

  if(SVNVERSION)
    execute_process(COMMAND ${SVNVERSION} ${CMAKE_SOURCE_DIR}
      OUTPUT_VARIABLE SVN_REV
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
  else(SVNVERSION)
    set(SVN_REV "Unknown")
  endif(SVNVERSION)
else( MCRL2_SVN_LABEL STREQUAL "" )
  set( SVN_REV ${MCRL2_SVN_LABEL} )	
endif( MCRL2_SVN_LABEL STREQUAL "" )

message(STATUS "SVN revision: ${SVN_REV}")

##---------------------------------------------------
## Set Boost configuration
##---------------------------------------------------



if(MSVC OR MINGW OR MSYS)
  SET (Boost_USE_STATIC_LIBS ON)
        
  find_package(Boost COMPONENTS filesystem signals system serialization REQUIRED)
     # Disable boost::regex and use sregex instead.
     # using MSVC 2010, BoostPro 1.44, results in following link error:
     # mcrl2_utilities_command_line.lib(command_line_interface.obj) :
     # error LNK2019: unresolved external symbol "char __fastcall boost::re_detail::w32_toupper(char,unsigned long)" (?w32_toupper@re_detail@boost@@YIDDK@Z) referenced in function "public: char __thiscall boost::w32_regex_traits<char>::toupper(char)const " (?toupper@?$w32_regex_traits@D@boost@@QBEDD@Z)
     add_definitions( -DMCRL2_DISABLE_BOOST_REGEX )
else(MSVC OR MINGW OR MSYS)
  find_package(Boost COMPONENTS system serialization signals filesystem regex REQUIRED)
endif(MSVC OR MINGW OR MSYS)

if(NOT Boost_FOUND)
  message( STATUS "The mCRL2 toolset requires the installation of Boost version v1.35 or higher." )
  message( STATUS "See http://www.mcrl2.org/mcrl2/wiki/index.php/CMake_build_instructions for" )
  message( FATAL_ERROR "instructions on building mCRL2 with an external version of boost.")
endif(NOT Boost_FOUND)

if( Boost_VERSION LESS 103500 )
  message( STATUS "Boost version v1.35 or higher required." )
  message( STATUS "Current Boost version: ${Boost_VERSION}.")
  message( STATUS "See http://www.mcrl2.org/mcrl2/wiki/index.php/CMake_build_instructions for" )
  message( FATAL_ERROR "instructions on building mCRL2 with an external version of boost.")
endif( Boost_VERSION LESS 103500 )

message( STATUS "Boost version: ${Boost_VERSION}" )

if (Boost_FOUND)
   include_directories(${Boost_INCLUDE_DIRS})
   link_directories(${Boost_LIBRARY_DIRS})
endif (Boost_FOUND)


##---------------------------------------------------
## Explicity Set NDebug
##--------------------------------------------------

option(MCRL2_NDEBUG "Explicity set NDEBUG" OFF)
message(STATUS "MCRL2_NDEBUG: ${MCRL2_NDEBUG}")
if(MCRL2_NDEBUG)
  add_definitions(-DNDEBUG)
endif(MCRL2_NDEBUG)

##---------------------------------------------------
## Set wxWidgets configuration
##---------------------------------------------------

option(MCRL2_WITH_WXSTC "Enable/disable compilation of mcrl2xi with wxStyledTextControl support" OFF)
message(STATUS "MCRL2_WITH_WXSTC: ${MCRL2_WITH_WXSTC}")


option(MCRL2_ENABLE_GUI_TOOLS "Enable/disable creation of GUI tools" ON)

if(MCRL2_ENABLE_GUI_TOOLS)
# Find package for wxWidgets to work
find_package(wxWidgets)
endif(MCRL2_ENABLE_GUI_TOOLS)

if(wxWidgets_FOUND)

	if(MCRL2_WITH_WXSTC)			
    find_package(wxWidgets COMPONENTS base core adv gl xml html aui stc REQUIRED)
	else(MCRL2_WITH_WXSTC)			
    find_package(wxWidgets COMPONENTS base core adv gl xml html aui REQUIRED)
	endif(MCRL2_WITH_WXSTC)			

  INCLUDE(${wxWidgets_USE_FILE})

  IF( NOT ${wxWidgets_CONFIG_EXECUTABLE} MATCHES wxWidgets_wxrc_EXECUTABLE-NOTFOUND )
    #Find wxWidgets version
    execute_process(COMMAND ${wxWidgets_CONFIG_EXECUTABLE} --version
      OUTPUT_VARIABLE wxWidgets_VERSION
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if( wxWidgets_VERSION LESS 2.8.0 )

      message( STATUS "Warning: GUI tools are not built" )
      message( STATUS "The mCRL2 toolset requires wxWidgets version v2.8.0 or higher" )
      message( STATUS "Current wxWidgets version: ${wxWidgets_VERSION}")

    endif( wxWidgets_VERSION LESS 2.8.0 )

    message(STATUS "wxWidgets version: ${wxWidgets_VERSION}")

    ##---------------------------------------------------
    ## Detect OpenGL for wxWidgets Version prior to 2.9
    ##---------------------------------------------------
    if(wxWidgets_VERSION LESS 2.9.0)
      include(FindOpenGL)
      message(STATUS "OpenGL: ${OPENGL_FOUND}")
      if(NOT OPENGL_FOUND)
        set(MCRL2_BUILD_GUI_TOOLS false)
      endif(NOT OPENGL_FOUND)
    endif(wxWidgets_VERSION LESS 2.9.0)

  else( NOT ${wxWidgets_CONFIG_EXECUTABLE} MATCHES wxWidgets_wxrc_EXECUTABLE-NOTFOUND )

    message( STATUS "Make sure that wxWidgets (version 2.8.0 or higher) is installed and compiled" )

  endif( NOT ${wxWidgets_CONFIG_EXECUTABLE} MATCHES wxWidgets_wxrc_EXECUTABLE-NOTFOUND )

  set(MCRL2_BUILD_GUI_TOOLS true)

  # wxWidgets include directory
  include_directories(${wxWidgets_INCLUDE_DIRS})

else(wxWidgets_FOUND)

  message( STATUS "Warning: GUI tools are not built" )
  message( STATUS "The mCRL2 toolset requires the installation of wxWidgets 2.8.0 or higher")
  set(MCRL2_ENABLE_GUI_TOOLS false)

endif(wxWidgets_FOUND)

if( OPENGL_FOUND AND wxWidgets_FOUND )
  message(STATUS "MCRL2_ENABLE_GUI_TOOLS: ${MCRL2_ENABLE_GUI_TOOLS}")
endif( OPENGL_FOUND AND wxWidgets_FOUND )

if(NOT MCRL2_ENABLE_GUI_TOOLS )
  set(MCRL2_BUILD_GUI_TOOLS false)
  if(NOT MINGW OR NOT MSYS)
    if( CXX_ACCEPTS_PTHREAD )
      set(CMAKE_EXE_LINKER_FLAGS "-pthread ${CMAKE_EXE_LINKER_FLAGS}")
      set(CMAKE_SHARED_LINKER_FLAGS "-pthread ${CMAKE_SHARED_LINKER_FLAGS}")
      set(CMAKE_MODULE_LINKER_FLAGS "-pthread ${CMAKE_SHARED_LINKER_FLAGS}")
    endif( CXX_ACCEPTS_PTHREAD )
  endif(NOT MINGW OR NOT MSYS)

endif(NOT MCRL2_ENABLE_GUI_TOOLS  )

##---------------------------------------------------
##
##---------------------------------------------------

if ( WIN32 )
  if(MSVC)
    include_directories( build/workarounds/msvc )

    option(MCRL2_ENABLE_MULTICORE_COMPILATION "Enable/disable multi-core compilation" ON)
    if( MCRL2_ENABLE_MULTICORE_COMPILATION )
      set(CMAKE_CXX_FLAGS "/MP ${CMAKE_CXX_FLAGS}")
    endif( MCRL2_ENABLE_MULTICORE_COMPILATION )

  endif(MSVC)



  #add_definitions(-DWIN32)
  #add_definitions(-D__WIN32__)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_definitions(-D_SCL_SECURE_NO_DEPRECATE)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS)
  #add_definitions(-DWIN32_LEAN_AND_MEAN=1 )
  #add_definitions(-D_WIN32_WINNT=0x0501 )
  add_definitions(-DBOOST_ALL_NO_LIB=1)

  # The following definition is mCRL2 specific.
  # It prevents code being compiled for the innermost and jitty compiling rewriters
  # TODO: Should be prefixed with MCRL2_
  add_definitions(-DNO_DYNLOAD)
endif ( WIN32)

if ( APPLE )
  add_definitions( -D__DARWIN__)
  add_definitions( -D__WXMAC__)

  set(CMAKE_EXE_LINKER_FLAGS "-framework Carbon ${CMAKE_EXE_LINKER_FLAGS}")
  set(CMAKE_SHARED_LINKER_FLAGS "-framework Carbon -single_module ${CMAKE_SHARED_LINKER_FLAGS}")
  set(CMAKE_MODULE_LINKER_FLAGS "-framework Carbon ${CMAKE_SHARED_LINKER_FLAGS}")

  if( OPENGL_FOUND )
    include_directories( ${OPENGL_INCLUDE_DIR} )
    link_directories(${OPENGL_LIBRARIES})
    set(CMAKE_EXE_LINKER_FLAGS "-framework OpenGL ${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS "-framework OpenGL ${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS "-framework OpenGL ${CMAKE_SHARED_LINKER_FLAGS}")
  endif( OPENGL_FOUND )

endif (APPLE)

## Toggle man page generation
##---------------------------------------------------

if (UNIX)
  option(MCRL2_MAN_PAGES "Enable/disable creation of UNIX man pages" ON)
else (UNIX)
  option(MCRL2_MAN_PAGES "Enable/disable creation of UNIX man pages" OFF)
endif (UNIX)

message(STATUS "MCRL2_MAN_PAGES: ${MCRL2_MAN_PAGES}")

##---------------------------------------------------
## Toggle profile build
##---------------------------------------------------

if( UNIX )
  option(MCRL2_ENABLE_PROFILING "Enable/disable profiling support" OFF)
  if( MCRL2_ENABLE_PROFILING )
    add_definitions( -pg )
    set(CMAKE_EXE_LINKER_FLAGS "-pg ${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS "-pg ${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS "-pg ${CMAKE_SHARED_LINKER_FLAGS}")
  endif(MCRL2_ENABLE_PROFILING)
  message(STATUS "MCRL2_ENABLE_PROFILING: ${MCRL2_ENABLE_PROFILING}")
endif( UNIX )

##---------------------------------------------------
## Set properties for build
##---------------------------------------------------

set(MCRL2_TEST_LABEL "rev.${SVN_REV}-${CMAKE_BUILD_TYPE}")

##---------------------------------------------------
## 3rd party dependencies
##---------------------------------------------------

add_subdirectory ( 3rd-party/aterm )
add_subdirectory ( 3rd-party/svc )
if(MCRL2_ENABLE_GUI_TOOLS)
  add_subdirectory ( 3rd-party/tr)

  add_subdirectory ( 3rd-party/gl2ps )
endif(MCRL2_ENABLE_GUI_TOOLS)

##---------------------------------------------------
## Library dependencies
##---------------------------------------------------

  add_subdirectory ( libraries/data )
  add_subdirectory ( libraries/utilities )
  add_subdirectory ( libraries/core )
  add_subdirectory ( libraries/process )
  add_subdirectory ( libraries/lps )
  add_subdirectory ( libraries/lts )

##---------------------------------------------------
## Tool dependencies
##---------------------------------------------------

  add_subdirectory ( tools/besinfo )
  add_subdirectory ( tools/bespp )
  add_subdirectory ( tools/chi2mcrl2 )
  add_subdirectory ( tools/diagraphica )
  add_subdirectory ( tools/formulacheck )
  add_subdirectory ( tools/grapemcrl2 )
  add_subdirectory ( tools/lps2lts )
  add_subdirectory ( tools/lps2pbes )
  add_subdirectory ( tools/lps2torx )
  add_subdirectory ( tools/lpsactionrename )
  add_subdirectory ( tools/lpsbinary )
  add_subdirectory ( tools/lpsconfcheck )
  add_subdirectory ( tools/lpsconstelm )
  add_subdirectory ( tools/lpsinfo )
  add_subdirectory ( tools/lpsinvelm )
  add_subdirectory ( tools/lpsparelm )
  add_subdirectory ( tools/lpsparunfold )
  add_subdirectory ( tools/lpspp )
  add_subdirectory ( tools/lpsrewr )
  add_subdirectory ( tools/lpssumelm )
  add_subdirectory ( tools/lpssuminst )
  add_subdirectory ( tools/lpsuntime )
  add_subdirectory ( tools/lts2lps )
  add_subdirectory ( tools/ltsconvert )
  add_subdirectory ( tools/ltscompare )
  add_subdirectory ( tools/ltsinfo )
  add_subdirectory ( tools/ltsgraph )
  add_subdirectory ( tools/ltsview )
  add_subdirectory ( tools/lysa2mcrl2 )
  add_subdirectory ( tools/mcrl22lps )
  add_subdirectory ( tools/pbes2bool )
  add_subdirectory ( tools/pbesinfo )
  add_subdirectory ( tools/pbespp )
  add_subdirectory ( tools/pbesrewr )
  add_subdirectory ( tools/pnml2mcrl2 )
  add_subdirectory ( tools/tbf2lps )
  add_subdirectory ( tools/tracepp )
  add_subdirectory ( tools/txt2pbes )
  add_subdirectory ( tools/lpsxsim )
  add_subdirectory ( tools/txt2lps )
  add_subdirectory ( tools/mcrl2i )
  add_subdirectory ( tools/pbesconstelm )
  add_subdirectory ( tools/pbesparelm )
  add_subdirectory ( tools/mcrl2-gui )
  add_subdirectory ( tools/mcrl2xi )

##---------------------------------------------------
## Experimental tool dependencies
##---------------------------------------------------

option(MCRL2_ENABLE_EXPERIMENTAL "Enable/disable creation of experimental tools" OFF)
message(STATUS "MCRL2_ENABLE_EXPERIMENTAL: ${MCRL2_ENABLE_EXPERIMENTAL}" )

if(MCRL2_ENABLE_EXPERIMENTAL)
  add_subdirectory ( tools/besconvert )
  add_subdirectory ( tools/bessolve )
  add_subdirectory ( tools/lpsrealelm )
  add_subdirectory ( tools/lpsbisim2pbes )
  add_subdirectory ( tools/pbesabstract )
  add_subdirectory ( tools/pbes2bes )
  add_subdirectory ( tools/pbespareqelm )
  add_subdirectory ( tools/pbespgsolve )
  add_subdirectory ( tools/txt2bes )
endif(MCRL2_ENABLE_EXPERIMENTAL)

##---------------------------------------------------
## Deprecated tool dependencies
##---------------------------------------------------

option(MCRL2_ENABLE_DEPRECATED "Enable/disable creation of deprecated tools" OFF)
message(STATUS "MCRL2_ENABLE_DEPRECATED: ${MCRL2_ENABLE_DEPRECATED}" )

if(MCRL2_ENABLE_DEPRECATED)
  add_subdirectory ( tools/ltsmin )
endif(MCRL2_ENABLE_DEPRECATED)

##---------------------------------------------------
## Experimental tool dependencies
##---------------------------------------------------

option(MCRL2_COMPILE_EXAMPLES "Enable/disable creation of example tools" OFF)
message(STATUS "MCRL2_COMPILE_EXAMPLES: ${MCRL2_COMPILE_EXAMPLES}" )

if(MCRL2_COMPILE_EXAMPLES)
   add_subdirectory( libraries/atermpp/example)
   add_subdirectory( libraries/data/example)
   add_subdirectory( libraries/lps/example)
   add_subdirectory( libraries/utilities/example )
   add_subdirectory( libraries/pbes/example)
   add_subdirectory( libraries/bes/example)
endif(MCRL2_COMPILE_EXAMPLES)

##---------------------------------------------------
## Create "libraries/utilities/include/mcrl2/setup.h",
## required for the compiling rewriters
##---------------------------------------------------

IF( UNIX )
FILE(REMOVE ${CMAKE_CURRENT_SOURCE_DIR}/libraries/utilities/include/mcrl2/setup.h)

set(CC  ${CMAKE_C_COMPILER})
set(CXX ${CMAKE_CXX_COMPILER})

string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE)
if(CMAKE_BUILD_TYPE MATCHES "release")
  set(R_CFLAGS ${CMAKE_CXX_FLAGS_RELEASE})
endif(CMAKE_BUILD_TYPE MATCHES "release")
if(CMAKE_BUILD_TYPE MATCHES "debug")
  set(R_CFLAGS ${CMAKE_CXX_FLAGS_DEBUG})
endif(CMAKE_BUILD_TYPE MATCHES "debug")

if(CMAKE_C_COMPILER MATCHES "icc")
    set(R_CFLAGS "${R_CFLAGS} -Wno-unused -rdynamic")
else(CMAKE_C_COMPILER MATCHES "icc")
  check_cxx_compiler_flag(-Wno-unused C_ACCEPTS_WNO-UNUSED )
  if( C_ACCEPTS_WNO-UNUSED )
    set(R_CFLAGS "${R_CFLAGS} -Wno-unused")
  endif( C_ACCEPTS_WNO-UNUSED )

  check_cxx_compiler_flag(-rdynamic C_ACCEPTS_RDYNAMIC )
  if( C_ACCEPTS_RDYNAMIC )
    set(R_CFLAGS "${R_CFLAGS} -rdynamic")
  endif( C_ACCEPTS_RDYNAMIC )
endif(CMAKE_C_COMPILER MATCHES "icc")

set(R_CXXFLAGS "${R_CFLAGS} -std=c++98")
set(R_CFLAGS "${R_CFLAGS} -std=c99")
set(R_CPPFLAGS "-DNDEBUG")

if(NOT BUILD_SHARED_LIBS)
  set(R_CFLAGS "${R_CFLAGS} -fPIC")
endif(NOT BUILD_SHARED_LIBS)

set(R_SCFLAGS ${BUILD_SHARED_LIBS_OPTION})
set(R_SCXXFLAGS ${BUILD_SHARED_LIBS_OPTION})

if( NOT APPLE )
  set(R_SLDFLAGS "-shared")
else ( NOT APPLE )
  set(R_SLDFLAGS "-bundle -undefined dynamic_lookup")
endif( NOT APPLE )

set(IATERM_CPPFLAGS "${ATERM_FLAGS} -I\\\"${CMAKE_INSTALL_PREFIX}/include/aterm\\\"")
set(IATERM_LDFLAGS  "-L\\\"${CMAKE_INSTALL_PREFIX}/${MCRL2_LIB_DIR}\\\" -lATerm")

option(MCRL2_ENABLE_CADP_SUPPORT "Enable/disable CADP support" OFF)
message(STATUS "MCRL2_ENABLE_CADP_SUPPORT: ${MCRL2_ENABLE_CADP_SUPPORT}" )
if(MCRL2_ENABLE_CADP_SUPPORT )
  set(USE_BCG "")
else(MCRL2_ENABLE_CADP_SUPPORT)
  set( USE_BCG "//" )
endif(MCRL2_ENABLE_CADP_SUPPORT)

configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/build/autoconf/setup.h.in" "${CMAKE_CURRENT_BINARY_DIR}/libraries/utilities/include/mcrl2/setup.h" @ONLY  )

ENDIF( UNIX )

##---------------------------------------------------
## Install headers
## --------------------------------------------------

install(
  DIRECTORY
    3rd-party/aterm/include/
    libraries/atermpp/include/
    libraries/core/include/
    libraries/data/include/
    libraries/lps/include/
    libraries/lts/include/
    libraries/bes/include/
    libraries/pbes/include/
    libraries/process/include/
    libraries/trace/include/
    libraries/utilities/include/
  DESTINATION include
  PATTERN ".svn" EXCLUDE
)

##---------------------------------------------------
## Install examples
## --------------------------------------------------

install(
  DIRECTORY examples/
  DESTINATION share/doc/mcrl2/examples
  PATTERN ".svn" EXCLUDE
)

##---------------------------------------------------
## Make tests
##---------------------------------------------------

option(MCRL2_ENABLE_TEST_TARGETS "Enable/disable creation of test targets" OFF)
message(STATUS "MCRL2_ENABLE_TEST_TARGETS: ${MCRL2_ENABLE_TEST_TARGETS}" )

ENABLE_TESTING()
configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/CTestCustom.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/CTestCustom.cmake" )

  # Define macro for build_and_run_test_targets
  # This method compiles tests when invoked
  macro( build_and_run_test_target TARGET )
    ADD_TEST("${TARGET}" ${CMAKE_CTEST_COMMAND}
     --build-and-test
     "${CMAKE_CURRENT_SOURCE_DIR}"
     "${CMAKE_CURRENT_BINARY_DIR}"
     --build-noclean
     --build-nocmake
     --build-generator "${CMAKE_GENERATOR}"
     --build-target "${TARGET}"
     --build-makeprogram "${CMAKE_MAKE_PROGRAM}"
     --test-command "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}"
    )
    set_tests_properties("${TARGET}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
  endmacro( build_and_run_test_target TARGET )

  add_subdirectory( libraries/lps/test)
  add_subdirectory( libraries/bes/test)
  add_subdirectory( libraries/pbes/test)
  add_subdirectory( libraries/lts/test)
  add_subdirectory( libraries/data/test)
  add_subdirectory( libraries/core/test)
  add_subdirectory( libraries/utilities/test)
  add_subdirectory( libraries/process/test )
  add_subdirectory( libraries/atermpp/test )
  add_subdirectory( libraries/trace/test )

if( MCRL2_ENABLE_TEST_TARGETS )
   add_subdirectory( tools/lpsparunfold/test )
endif( MCRL2_ENABLE_TEST_TARGETS )

##---------------------------------------
## Tool tests
##---------------------------------------


  #-------------
  # abp.mcrl2
  #-------------

  # Conversion tools
  set(testdir "${CMAKE_CURRENT_BINARY_DIR}/mcrl2-testoutput")
  file( REMOVE_RECURSE ${testdir} )
  file( MAKE_DIRECTORY ${testdir} )
  add_test(mcrl22lps_version ${mcrl22lps_BINARY_DIR}/mcrl22lps --version)
  add_test(mcrl22lps_abp ${mcrl22lps_BINARY_DIR}/mcrl22lps -v -D ${CMAKE_SOURCE_DIR}/examples/academic/abp/abp.mcrl2 ${testdir}/abp.lps)
  add_test(lpsinfo_abp ${lpsinfo_BINARY_DIR}/lpsinfo ${testdir}/abp.lps)
  add_test(lpsconstelm_abp ${lpsconstelm_BINARY_DIR}/lpsconstelm -v ${testdir}/abp.lps ${testdir}/abp_celm.lps)
  add_test(lpsparelm_abp ${lpsparelm_BINARY_DIR}/lpsparelm -v ${testdir}/abp_celm.lps ${testdir}/abp_celm_pelm.lps)
  add_test(lpssuminst_abp ${lpssuminst_BINARY_DIR}/lpssuminst -v ${testdir}/abp_celm_pelm.lps ${testdir}/abp_celm_pelm_sinst.lps)
  add_test(lps2lts_abp ${lps2lts_BINARY_DIR}/lps2lts -v ${testdir}/abp_celm_pelm_sinst.lps ${testdir}/abp_celm_pelm_sinst.aut  )
  add_test(lps2lts_abp_fsm ${lps2lts_BINARY_DIR}/lps2lts -v ${testdir}/abp_celm_pelm_sinst.lps ${testdir}/abp_celm_pelm_sinst.fsm  )
  add_test(lps2lts_abp_dot ${lps2lts_BINARY_DIR}/lps2lts -v ${testdir}/abp_celm_pelm_sinst.lps ${testdir}/abp_celm_pelm_sinst.dot  )
  add_test(lps2lts_abp_svc ${lps2lts_BINARY_DIR}/lps2lts -v ${testdir}/abp_celm_pelm_sinst.lps ${testdir}/abp_celm_pelm_sinst.svc  )
  add_test(lps2lts_abp_lts ${lps2lts_BINARY_DIR}/lps2lts -v ${testdir}/abp_celm_pelm_sinst.lps ${testdir}/abp_celm_pelm_sinst.lts  )
  add_test(ltsinfo_abp     ${ltsinfo_BINARY_DIR}/ltsinfo ${testdir}/abp_celm_pelm_sinst.aut )
  add_test(ltsinfo_abp_fsm ${ltsinfo_BINARY_DIR}/ltsinfo ${testdir}/abp_celm_pelm_sinst.fsm )
  add_test(ltsinfo_abp_dot ${ltsinfo_BINARY_DIR}/ltsinfo ${testdir}/abp_celm_pelm_sinst.dot )
  add_test(ltsinfo_abp_svc ${ltsinfo_BINARY_DIR}/ltsinfo ${testdir}/abp_celm_pelm_sinst.svc )
  add_test(ltsinfo_abp_lts ${ltsinfo_BINARY_DIR}/ltsinfo ${testdir}/abp_celm_pelm_sinst.lts )
  add_test(lps2pbes_abp ${lps2pbes_BINARY_DIR}/lps2pbes -v -f${CMAKE_SOURCE_DIR}/examples/modal-formulas/nodeadlock.mcf ${testdir}/abp_celm_pelm_sinst.lps ${testdir}/abp_celm_pelm_sinst.pbes)
  add_test(pbes2bool_abp ${pbes2bool_BINARY_DIR}/pbes2bool -v ${testdir}/abp_celm_pelm_sinst.pbes)
  add_test(lysa2mcrl2_test  ${lysa2mcrl2_BINARY_DIR}/lysa2mcrl2 ${lysa2mcrl2_SOURCE_DIR}/wmf_01.tlysa ${testdir}/wmf_01.mcrl2  )
  add_test(tracepp_test ${tracepp_BINARY_DIR}/tracepp ${CMAKE_SOURCE_DIR}/examples/industrial/garage/movie.trc ${testdir}/movie.txt )
  add_test(pnml2mcrl2_philcol ${pnml2mcrl2_BINARY_DIR}/pnml2mcrl2 ${CMAKE_SOURCE_DIR}/examples/generated/philcol/philcol.pnml ${testdir}/philcol.mcrl2 )
  add_test(pnml2mcrl2_desel-esparza-5-2 ${pnml2mcrl2_BINARY_DIR}/pnml2mcrl2 ${pnml2mcrl2_SOURCE_DIR}/test/desel-esparza-5-2.pnml ${testdir}/desel-esparza-5-2.mcrl2)
  add_test(chi2mcrl2_test ${chi2mcrl2_BINARY_DIR}/chi2mcrl2 ${chi2mcrl2_SOURCE_DIR}/examples/industrialsystem/industrialsystem.chi ${testdir}/industrialsystem.mcrl2)
  add_test(lts2lps_test ${lts2lps_BINARY_DIR}/lts2lps ${testdir}/abp_celm_pelm_sinst.lts ${testdir}/abp_celm_pelm_sinst_lts.mcrl2)
  add_test(ltsconvert_bisim_test ${ltsconvert_BINARY_DIR}/ltsconvert -ebisim ${testdir}/abp_celm_pelm_sinst.lts ${testdir}/abp_celm_pelm_sinst_bisim.aut)



  # Simulation tools
  # add_test(lpssim_abp ${lpssim_BINARY_DIR}/lpssim ${testdir}/abp_celm_pelm_sinst.lps )
  # set_tests_properties(lpssim_abp PROPERTIES TIMEOUT 15 )

  # add_test(lpsxsim_abp ${lpsxsim_BINARY_DIR}/lpsxsim ${testdir}/abp_celm_pelm_sinst.lps )
  # set_tests_properties(lpsxsim_abp PROPERTIES TIMEOUT 15 )

  # Graphical tools
  # add_test(diagraphica_abp ${diagraphica_BINARY_DIR}/diagraphica ${testdir}/abp_celm_pelm_sinst.aut )
  # set_tests_properties(diagraphica_abp PROPERTIES TIMEOUT 15 )

  # add_test(ltsview_abp ${ltsview_BINARY_DIR}/ltsview ${testdir}/abp_celm_pelm_sinst.aut )
  # set_tests_properties(ltsview_abp PROPERTIES TIMEOUT 15 )

  # add_test(ltsgraph_abp ${ltsgraph_BINARY_DIR}/ltsgraph ${testdir}/abp_celm_pelm_sinst.aut )
  # set_tests_properties(ltsgraph_abp PROPERTIES TIMEOUT 15 )

  # add_test(grapemcrl2_smoke ${grapemcrl2_BINARY_DIR}/grapemcrl2)
  # set_tests_properties(grapemcrl2_smoke PROPERTIES TIMEOUT 15 )

  # Documentation tests
  if (MCRL2_MAN_PAGES)
    add_test(mcrl22lps_generate-man-page ${mcrl22lps_BINARY_DIR}/mcrl22lps --generate-man-page)
    set_tests_properties(
      mcrl22lps_generate-man-page
    PROPERTIES
      LABELS "${MCRL2_TEST_LABEL}"
    )
  endif(MCRL2_MAN_PAGES)
    add_test(mcrl22lps_generate-wiki-page ${mcrl22lps_BINARY_DIR}/mcrl22lps --generate-wiki-page)
    add_test(mcrl22lps_mcrl2-gui ${mcrl22lps_BINARY_DIR}/mcrl22lps --mcrl2-gui)


  set_tests_properties(
    mcrl22lps_abp
    mcrl22lps_generate-wiki-page
    mcrl22lps_mcrl2-gui
    lpsinfo_abp
    lpsconstelm_abp
    lpsparelm_abp
    lpssuminst_abp
    lps2lts_abp
    ltsinfo_abp
    pbes2bool_abp
    lps2pbes_abp
    lysa2mcrl2_test
    tracepp_test
    pnml2mcrl2_philcol
    pnml2mcrl2_desel-esparza-5-2
    chi2mcrl2_test
    lts2lps_test
    ltsconvert_bisim_test
  PROPERTIES
    LABELS "${MCRL2_TEST_LABEL}"
  )


include(CTest)

##---------------------------------------------------
## Header tests
##---------------------------------------------------
option(MCRL2_COMPILE_HEADER_TESTS "Enable/disable compile of library headers" OFF)
message(STATUS "MCRL2_COMPILE_HEADER_TESTS: ${MCRL2_COMPILE_HEADER_TESTS}" )
if(MCRL2_COMPILE_HEADER_TESTS)
  add_subdirectory( build/testing )
endif( MCRL2_COMPILE_HEADER_TESTS)

##---------------------------------------------------
## Additional make targets
##---------------------------------------------------

##---------------------------------------------------
## Target tags (useful editing with Vim)
##---------------------------------------------------

find_program(CTAGS
  ctags
  /usr/local/bin
  /usr/pkg/bin
  /usr/bin
)

if(CTAGS)
  add_custom_target(tags
    ${CTAGS} --languages=C,C++ --recurse=yes --extra=+q --fields=+i --totals=yes ${CMAKE_SOURCE_DIR}
  )
endif(CTAGS)

##---------------------------------------------------
## Target doxy
##---------------------------------------------------

if(UNIX)
  add_custom_target(doxy
     doc/doxy/generate_libref_website.sh
     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  )
endif(UNIX)

##---------------------------------------------------
## Target latex documentation
##---------------------------------------------------

if(UNIX)
  add_custom_target(tex
     doc/tex/generate_libdocs.py
     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  )
endif(UNIX)

##---------------------------------------------------
## Target quickbook documentation
##---------------------------------------------------

if(Boost_FOUND)
find_program(BOOST_BJAM
             NAMES bjam
             PATHS "${BOOST_INCLUDEDIR}/bin"
                     /usr/local/bin
                     /usr/pkg/bin
                     /usr/bin
            )
    if(BOOST_BJAM)
	    add_custom_target(quickbook
            ${BOOST_BJAM} html
	    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/doc)
    endif(BOOST_BJAM)
endif(Boost_FOUND)

##---------------------------------------------------
## Target parsers
##---------------------------------------------------

if(UNIX)

  add_custom_target(mcrl2parser
    flex -Pmcrl2 -omcrl2lexer.cpp mcrl2lexer.ll
    COMMAND bison -p mcrl2 --defines=../include/mcrl2/core/detail/mcrl2parser.h -o mcrl2parser.cpp mcrl2parser.yy
    COMMAND sed -i -e 's+include \"mcrl2parser.h\"+include \"mcrl2/core/detail/mcrl2parser.h\"+' mcrl2parser.cpp
    COMMAND sed -i -e '/isatty/d' mcrl2lexer.cpp
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/libraries/core/source
  )

  add_custom_target(chiparser
    flex -Pchi -ochilexer.cpp chilexer.ll
    COMMAND bison -p chi --defines=chiparser.h -o chiparser.cpp chiparser.yy
    COMMAND sed -i -e '/isatty/d' chilexer.cpp
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tools/chi2mcrl2
  )

  add_custom_target(lysaparser
    flex -Plysa -olysalexer.cpp lysalexer.ll
    COMMAND bison -p lysa --defines=lysaparser.h -o lysaparser.cpp lysaparser.yy
    COMMAND sed -i -e '/isatty/d' lysalexer.cpp
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tools/lysa2mcrl2
  )

  add_custom_target(liblts_fsmparser
    flex -Pfsm -oliblts_fsmlexer.cpp liblts_fsmlexer.ll
    COMMAND bison -p fsm --defines=../include/mcrl2/lts/detail/liblts_fsmparser.h -o liblts_fsmparser.cpp liblts_fsmparser.yy
    COMMAND sed -i -e 's+include \"liblts_fsmparser.h\"+include \"mcrl2/lts/detail/liblts_fsmparser.h\"+' liblts_fsmparser.cpp
    COMMAND sed -i -e '/isatty/d' liblts_fsmlexer.cpp
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/libraries/lts/source
  )

  add_custom_target(liblts_dotparser
    flex -Pdot -oliblts_dotlexer.cpp liblts_dotlexer.ll
    COMMAND bison -p dot --defines=../include/mcrl2/lts/detail/liblts_dotparser.h -o liblts_dotparser.cpp liblts_dotparser.yy
    COMMAND sed -i -e 's+include \"liblts_dotparser.h\"+include \"mcrl2/lts/detail/liblts_dotparser.h\"+' liblts_dotparser.cpp
    COMMAND sed -i -e '/isatty/d' liblts_dotlexer.cpp
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/libraries/lts/source
  )

  add_custom_target(parsers
    cp /usr/include/FlexLexer.h ${CMAKE_SOURCE_DIR}/build/workarounds
  )

  add_dependencies(parsers mcrl2parser chiparser lysaparser liblts_fsmparser liblts_dotparser)
endif(UNIX)


##---------------------------------------------------
## mCRL2 Sigle Bundle for MacOSX
##---------------------------------------------------

option(MCRL2_SINGLE_BUNDLE "Enable/disable creation of a single mCRL2.app" OFF)
IF(APPLE AND MCRL2_SINGLE_BUNDLE)
  if(BUILD_SHARED_LIBS)
    message( FATAL_ERROR "BUILD_SHARED_LIBS requires to be FALSE when compiling with MCRL2_SINGLE_BUNDLE set to TRUE" )
  endif(BUILD_SHARED_LIBS)

  if(NOT "${CMAKE_INSTALL_PREFIX}" STREQUAL "/")
    message( STATUS "CMAKE_INSTALL_PREFIX is not equal to \"/\". This will create an installer, which is non-standard." )
  endif(NOT "${CMAKE_INSTALL_PREFIX}" STREQUAL "/")

  SET(CMAKE_BUNDLE_NAME "mCRL2")

  SET(CMAKE_BUNDLE_LOCATION "${CMAKE_INSTALL_PREFIX}")
  # make sure CMAKE_INSTALL_PREFIX ends in /
  STRING(LENGTH "${CMAKE_INSTALL_PREFIX}" LEN)
  MATH(EXPR LEN "${LEN} -1" )
  STRING(SUBSTRING "${CMAKE_INSTALL_PREFIX}" ${LEN} 1 ENDCH)
  IF(NOT "${ENDCH}" STREQUAL "/")
    SET(CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/")
  ENDIF(NOT "${ENDCH}" STREQUAL "/")
  SET(CMAKE_INSTALL_PREFIX
    "${CMAKE_INSTALL_PREFIX}${CMAKE_BUNDLE_NAME}.app/Contents")

  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/build/macosx)

  # Install "install_tools" in share/installer
  # install_tools contains the tools marked for symlink export
  install(FILES ${CMAKE_BINARY_DIR}/install_tools DESTINATION share/installer)
  #Install Info.plist for single application bundle
  install(FILES ${CMAKE_SOURCE_DIR}/build/macosx/Info.plist DESTINATION . )
  #Install mcrl2-logo for single application bundle
  install(FILES ${CMAKE_SOURCE_DIR}/build/macosx/mcrl2-logo.icns DESTINATION Resources )

ENDIF(APPLE AND MCRL2_SINGLE_BUNDLE)

##---------------------------------------------------
## CPack configuration
##---------------------------------------------------

INCLUDE(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME "mcrl2")
if(MCRL2_SVN_LABEL STREQUAL "")
  set(CPACK_PACKAGE_VERSION "svn${SVN_REV}")
else(MCRL2_SVN_LABEL STREQUAL "")
  set(CPACK_PACKAGE_VERSION "${MCRL2_SVN_LABEL}")
endif(MCRL2_SVN_LABEL STREQUAL "")

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Toolset for modelling, validation and verification of concurrent systems and protocols.")
set(CPACK_PACKAGE_CONTACT "info@mcrl2.org")
set(CPACK_PACKAGE_INSTALL_DIRECTORY mCRL2) #USED FOR NSIS, Adding this line will overwrite standard path, which is: mcrl2-${CMAKE_BUILD_TYPE}svn{SVN_REV}

SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "mCRL2")
SET(CPACK_PACKAGE_VENDOR "TUe")

# Create Desktop link to mcrl2-gui
set(CPACK_CREATE_DESKTOP_LINKS mcrl2-gui)
set(CPACK_PACKAGE_EXECUTABLES "grapemcrl2;grapemcrl2" "ltsgraph;ltsgraph" "ltsview;ltsview" "diagraphica;diagraphica" "lpsxsim;lpsxsim" "mcrl2-gui;mcrl2-gui" "mcrl2xi;mcrl2xi")

  IF(APPLE AND MCRL2_SINGLE_BUNDLE)
    SET(CPACK_SET_DESTDIR TRUE)

    configure_file(${CMAKE_SOURCE_DIR}/postflight.sh.in ${CMAKE_CURRENT_BINARY_DIR}/postflight.sh)

    SET(CMAKE_POSTFLIGHT_SCRIPT
      ${CMAKE_CURRENT_BINARY_DIR}/postflight.sh)
    SET(CPACK_POSTUPGRADE_SCRIPT
      ${CMAKE_CURRENT_BINARY_DIR}/postflight.sh)
  ENDIF(APPLE AND MCRL2_SINGLE_BUNDLE)

# NSIS VARIABLES
SET(CPACK_NSIS_DISPLAY_NAME "mCRL2")
SET(CPACK_NSIS_PACKAGE_NAME "mCRL2")
SET(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}\\\\doc\\\\images\\\\mcrl2-install-logo.bmp")

#Extend PATH variable with mCRL2_INSTALL_DIR/bin if CPACK_NSIS_MODIFY_PATH = ON
SET(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}/${MCRL2_BIN_DIR}")
SET(CPACK_NSIS_MODIFY_PATH ON)

# README and COPYING require a .txt extention when be used with MacOSX's PackageMaker
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/COPYING" "${CMAKE_CURRENT_BINARY_DIR}/COPYING.txt" COPYONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/README"  "${CMAKE_CURRENT_BINARY_DIR}/README.txt" COPYONLY)

set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_BINARY_DIR}/COPYING.txt )
set(CPACK_RESOURCE_FILE_README  ${CMAKE_CURRENT_BINARY_DIR}/README.txt )

#Variables for RPM packaging
set(CPACK_RPM_PACKAGE_LICENSE "Boost Software License, Version 1.0")
set(CPACK_RPM_PACKAGE_GROUP "Devel")
set(CPACK_RPM_PACKAGE_DESCRIPTION ${CMAKE_SOURCE_DIR}/README)
set(CPACK_RPM_PACKAGE_VENDOR "Technische Universiteit Eindhoven (TU/e)")

include(CPack)
