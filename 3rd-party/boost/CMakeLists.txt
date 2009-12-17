# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(boost)
cmake_minimum_required(VERSION 2.6)

# ######### General setup ##########

if (BUILD_SHARED_LIBS)
  add_definitions(-DBOOST_ALL_DYN_LINK)
endif (BUILD_SHARED_LIBS)

if(WIN32)
add_library(${PROJECT_NAME}
#  libs/detail/utf8_codecvt_facet.cpp
  libs/signals/src/trackable.cpp
  libs/signals/src/signal_base.cpp
  libs/signals/src/named_slot_map.cpp
  libs/signals/src/connection.cpp
  libs/signals/src/slot.cpp
# libs/program_options/src/variables_map.cpp
# libs/program_options/src/config_file.cpp
# libs/program_options/src/options_description.cpp
# libs/program_options/src/parsers.cpp
# libs/program_options/src/cmdline.cpp
# libs/program_options/src/positional_options.cpp
# libs/program_options/src/convert.cpp
# libs/program_options/src/value_semantic.cpp
# libs/program_options/src/winmain.cpp
# libs/program_options/src/utf8_codecvt_facet.cpp
  libs/filesystem/src/operations.cpp
  libs/filesystem/src/portability.cpp
  libs/filesystem/src/path.cpp
  libs/filesystem/src/utf8_codecvt_facet.cpp
#  libs/thread/src/pthread/thread.cpp
#  libs/thread/src/pthread/exceptions.cpp
#  libs/thread/src/tss_null.cpp
  libs/thread/src/win32/tss_pe.cpp
  libs/thread/src/win32/tss_dll.cpp
  libs/thread/src/win32/thread.cpp
  libs/thread/src/win32/exceptions.cpp
  libs/system/src/error_code.cpp
  # MSVC requires a tss_cleanup_implemented implementation
  # Empty implementation is provided in the workarounds
  ${CMAKE_SOURCE_DIR}/build/workarounds/msvc/boost/tss_null.cpp
)
endif(WIN32)

if(UNIX)
add_library(${PROJECT_NAME}
#  libs/detail/utf8_codecvt_facet.cpp
  libs/signals/src/trackable.cpp
  libs/signals/src/signal_base.cpp
  libs/signals/src/named_slot_map.cpp
  libs/signals/src/connection.cpp
  libs/signals/src/slot.cpp
# libs/program_options/src/variables_map.cpp
# libs/program_options/src/config_file.cpp
# libs/program_options/src/options_description.cpp
# libs/program_options/src/parsers.cpp
# libs/program_options/src/cmdline.cpp
# libs/program_options/src/positional_options.cpp
# libs/program_options/src/convert.cpp
# libs/program_options/src/value_semantic.cpp
# libs/program_options/src/winmain.cpp
# libs/program_options/src/utf8_codecvt_facet.cpp
  libs/filesystem/src/operations.cpp
  libs/filesystem/src/portability.cpp
  libs/filesystem/src/path.cpp
  libs/filesystem/src/utf8_codecvt_facet.cpp
  libs/thread/src/pthread/thread.cpp
  libs/thread/src/pthread/exceptions.cpp
  libs/thread/src/pthread/once.cpp
#  libs/thread/src/tss_null.cpp
  libs/system/src/error_code.cpp
)
endif(UNIX)

# add install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR})
