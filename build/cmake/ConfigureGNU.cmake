# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://github.com/mCRL2org/mCRL2/blob/master/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
include(AddFlag)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(MCRL2_CLANGPP ON)
endif()
if("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
  set(MCRL2_CLANG ON)
endif()

##---------------------------------------------------
## Add option for profiling support
##---------------------------------------------------

option(MCRL2_ENABLE_PROFILING "Enable/disable profiling support" OFF)
if( MCRL2_ENABLE_PROFILING )
  add_definitions( -pg )
  set(CMAKE_EXE_LINKER_FLAGS "-pg ${CMAKE_EXE_LINKER_FLAGS}")
  set(CMAKE_SHARED_LINKER_FLAGS "-pg ${CMAKE_SHARED_LINKER_FLAGS}")
  set(CMAKE_MODULE_LINKER_FLAGS "-pg ${CMAKE_SHARED_LINKER_FLAGS}")
endif(MCRL2_ENABLE_PROFILING)

##---------------------------------------------------
## Set C compile flags
##---------------------------------------------------

try_add_c_flag(-std=c99)
try_add_c_flag(-Wall)
try_add_c_flag(-Wno-inline)
try_add_c_flag(-fno-strict-overflow)
try_add_c_flag(-pipe)
try_add_c_flag(-pedantic                 MAINTAINER)
try_add_c_flag(-W                        MAINTAINER)

# The following flags are not implemented in clang and therefore cause warnings.
if(NOT MCRL2_CLANG)
  try_add_c_flag(-fprofile-arcs            MAINTAINER)
  try_add_c_flag(-ftest-coverage           MAINTAINER)
endif()

# The following is only implemented in clang, but not on Apple.
if(MCRL2_CLANG AND NOT APPLE)
  # We need to add the proper flag to the linker before we try:
  set(CMAKE_REQUIRED_LIBRARIES "-fsanitize=address")
  try_add_c_flag(-fsanitize=address       MAINTAINER)
  try_add_c_flag(-fno-omit-frame-pointer  MAINTAINER)
  unset(CMAKE_REQUIRED_LIBRARIES)
endif()

##---------------------------------------------------
## Set C++ compile flags
##---------------------------------------------------

try_add_cxx_flag(-std=c++11)
if(NOT CXX_ACCEPTS_STD_CPP11)
  try_add_cxx_flag(-std=c++0x)
endif()

if(APPLE)
  try_add_cxx_flag(-stdlib=libc++)
endif()

if(NOT (CXX_ACCEPTS_STD_CPP11 OR CXX_ACCEPTS_STD_CPP0X))
  message(FATAL_ERROR "Your compiler does not support -std=c++11 or -std=c++0x. You should upgrade to a newer compiler version to build mCRL2")
endif()

try_add_cxx_flag(-Wall)
try_add_cxx_flag(-Wno-inline)
try_add_cxx_flag(-fno-strict-overflow)
try_add_cxx_flag(-pipe)
try_add_cxx_flag(-pedantic)

try_add_cxx_flag(-W                      MAINTAINER)
try_add_cxx_flag(-Wextra                 MAINTAINER)
try_add_cxx_flag(-Wunused-variable       MAINTAINER)
try_add_cxx_flag(-Wunused-parameter      MAINTAINER)
try_add_cxx_flag(-Wunused-function       MAINTAINER)
try_add_cxx_flag(-Wunused                MAINTAINER)
try_add_cxx_flag(-Wno-system-headers     MAINTAINER)
try_add_cxx_flag(-Woverloaded-virtual    MAINTAINER)
try_add_cxx_flag(-Wwrite-strings         MAINTAINER)
try_add_cxx_flag(-Wmissing-declarations  MAINTAINER)

# The following flags are not implemented in clang and therefore cause warnings.
if(NOT MCRL2_CLANGPP)
  try_add_cxx_flag(-fprofile-arcs            MAINTAINER)
  try_add_cxx_flag(-ftest-coverage           MAINTAINER)
endif()

# The following is only implemented in clang
if(MCRL2_CLANGPP AND NOT APPLE)
  # We need to add the proper flag to the linker before we try:
  set(CMAKE_REQUIRED_LIBRARIES "-fsanitize=address")
  try_add_cxx_flag(-fsanitize=address MAINTAINER)

  # Add a blacklist for the address and leak sanitizer to suppress reports for desired behaviour.
  get_filename_component(LEAKSANITIZER_SUPPRESS ${CMAKE_SOURCE_DIR}/build/leaksanitizer.suppress ABSOLUTE)
  try_add_cxx_flag("-fsanitize-blacklist=${LEAKSANITIZER_SUPPRESS}" MAINTAINER)
  unset(CMAKE_REQUIRED_LIBRARIES)
endif()

if(BUILD_SHARED_LIBS)
  check_cxx_compiler_flag(-fPIC CXX_ACCEPTS_FPIC )
  check_c_compiler_flag(-fPIC C_ACCEPTS_FPIC )
  if(CXX_ACCEPTS_FPIC AND C_ACCEPTS_FPIC)
    set(CMAKE_CXX_FLAGS "-fPIC ${CMAKE_CXX_FLAGS}")
    set(CMAKE_C_FLAGS "-fPIC ${CMAKE_C_FLAGS}")
    set(BUILD_SHARED_LIBS_OPTION "-fPIC")
  endif()
endif()

if(NOT WIN32)
  # Indicates that systems having a GNU compiler the rewriter can be compiled.
  # This flag is not available on MSVC because dynload is not available.
  # It's also not available on mingw for now.
  add_definitions(-DMCRL2_JITTYC_AVAILABLE)   
endif()

##---------------------------------------------------
## Set linker flags
##---------------------------------------------------

if(APPLE)
  # Set the size of the stack on apple to 4Gig, which is much larger than the default size. 
  if (NOT DEFINED MCRL2_OSX_STACK_SIZE)
   # Stack size set to 4GB for Mac, since terminal hard limit is 64MB
   # This number is the number of bytes in Hex
   set (MCRL2_OSX_STACK_SIZE "200000000" CACHE STRING "Maximum stack size to allow on MacOS, in number of bytes in hexadecimal (200000000 equals 4GB).")
 endif()
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-stack_size,${MCRL2_OSX_STACK_SIZE}")
  set(CMAKE_EXE_LINKER_FLAGS_MAINTAINER "")
else()
  set(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed")
  set(CMAKE_EXE_LINKER_FLAGS_MAINTAINER "-Wl,--warn-unresolved-symbols,--warn-once")
endif()

if(CXX_ACCEPTS_FPROFILE_ARCS)
  set(CMAKE_EXE_LINKER_FLAGS_MAINTAINER "${CMAKE_EXE_LINKER_FLAGS_MAINTAINER} -fprofile-arcs")
  set(CMAKE_SHARED_LINKER_FLAGS_MAINTAINER "${CMAKE_SHARED_LINKER_FLAGS_MAINTAINER} -fprofile-arcs")
endif()
if(CXX_ACCEPTS_FTEST_COVERAGE)
  set(CMAKE_SHARED_LINKER_FLAGS_MAINTAINER "${CMAKE_SHARED_LINKER_FLAGS_MAINTAINER} -ftest-coverage")
endif()
if(CXX_ACCEPTS_FSANITIZE_ADDRESS)
  set(CMAKE_EXE_LINKER_FLAGS_MAINTAINER "${CMAKE_EXE_LINKER_FLAGS_MAINTAINER} -fsanitize=address")
  set(CMAKE_SHARED_LINKER_FLAGS_MAINTAINER "${CMAKE_SHARED_LINKER_FLAGS_MAINTAINER} -fsanitize=address")
endif()
