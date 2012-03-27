# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

project(mcrl2_bes)
cmake_minimum_required(VERSION 2.6)

# Trick to add header files to projects in IDEs like Visual Studio and XCode
FILE(GLOB HEADERS "include/mcrl2/bes/*.h")
FILE(GLOB DETAIL_HEADERS "include/mcrl2/bes/detail/*.h")
add_custom_target(${PROJECT_NAME}_headers
  SOURCES ${HEADERS} ${DETAIL_HEADERS}
)

# ##### Start generate parser tables ######

# Silence warnings while compiling generated C code (pg-syntax.c is the only
# C code in this library)
if(MSVC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /w") # Disable all warnings
else(MSVC)
  if (CMAKE_C_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w") # Disable all warnings
  else (CMAKE_C_COMPILER_ID STREQUAL "Clang")
    # Assuming GCC: disable only the missing-field-initializer warning.
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-missing-field-initializers")
  endif (CMAKE_C_COMPILER_ID STREQUAL "Clang")
endif(MSVC)

add_custom_command(
  OUTPUT ${CMAKE_SOURCE_DIR}/libraries/bes/source/pg_syntax.c
  COMMAND make_dparser -A -H1 -ipg -o${CMAKE_CURRENT_SOURCE_DIR}/source/pg_syntax.c ${CMAKE_SOURCE_DIR}/doc/specs/pg-syntax.g
  DEPENDS make_dparser ${CMAKE_SOURCE_DIR}/doc/specs/pg-syntax.g
)

include_directories(
  ${CMAKE_SOURCE_DIR}/3rd-party/dparser
)

add_library(${PROJECT_NAME}
  source/pg_syntax.c
)

target_link_libraries(
  ${PROJECT_NAME}
  dparser
)

# add install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR})

# ##### End generate parser tables ######

#target_link_libraries(${PROJECT_NAME}
#  pg_syntax
#  dparser
#)
