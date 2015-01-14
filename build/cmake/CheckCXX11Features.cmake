# - Check which parts of the C++11 standard the compiler supports
# - Assumes that -std=c++0x or -std=c++11 has already been set for platforms
#   that need those flags.
#
# The script searches for tests in the CheckCXX11Features directory; for every
# file with one of the following names
#
#   cxx11-test-<testname>.cpp
#   cxx11-test-<testname>-fail.cpp
#   cxx11-test-<testname>-fail_compile.cpp
#
# it is tested that the the file exits with exit code 0, exits with non-zero
# exit code, or does not compile at all, respectively. If all tests for a
# given <testname> succeed, then the CMake variable HAS_CXX11_<TESTNAME> is
# set to "TRUE", else it is set to "FALSE". <testname> may not contain periods.

#=============================================================================
# Copyright 2011,2012 Rolf Eike Beer <eike@sf-mail.de>
# Copyright 2012 Andreas Weis
# Copyright 2014 Sjoerd Cranen
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

if(NOT CMAKE_CXX_COMPILER_LOADED)
    message(FATAL_ERROR "CheckCXX11Features module only works if language CXX is enabled")
endif()

set(CXX11_REQUIRED_FEATURES "auto" "is_sorted")

#
### Check for needed compiler flags
#
function(cxx11_check_feature FEATURE_NAME REQUIRED)
    string(TOUPPER "HAS_CXX11_${FEATURE_NAME}" RESULT_VAR)
    set(_LOG_NAME "\"${FEATURE_NAME}\"")
    if(NOT DEFINED ${RESULT_VAR})
        set(_bindir "${CMAKE_CURRENT_BINARY_DIR}")

        set(_SRCFILE_BASE ${CMAKE_CURRENT_LIST_DIR}/CheckCXX11Features/cxx11-test-${FEATURE_NAME})
        message(STATUS "Checking C++11 support for \"${FEATURE_NAME}\"")

        set(_SRCFILE "${_SRCFILE_BASE}.cpp")
        set(_SRCFILE_FAIL "${_SRCFILE_BASE}.fail.cpp")
        set(_SRCFILE_FAIL_COMPILE "${_SRCFILE_BASE}.fail_compile.cpp")

        if(CROSS_COMPILING)
            try_compile(${RESULT_VAR} "${_bindir}" "${_SRCFILE}"
                        COMPILE_DEFINITIONS "${CXX11_COMPILER_FLAGS}")
            if(${RESULT_VAR} AND EXISTS ${_SRCFILE_FAIL})
                try_compile(${RESULT_VAR} "${_bindir}" "${_SRCFILE_FAIL}"
                            COMPILE_DEFINITIONS "${CXX11_COMPILER_FLAGS}")
            endif()
        else()
            try_run(_RUN_RESULT_VAR _COMPILE_RESULT_VAR
                    "${_bindir}" "${_SRCFILE}"
                    COMPILE_DEFINITIONS "${CXX11_COMPILER_FLAGS}")
            if(_COMPILE_RESULT_VAR AND NOT _RUN_RESULT_VAR)
                set(${RESULT_VAR} TRUE)
            else()
                set(${RESULT_VAR} FALSE)
            endif()
            if(${RESULT_VAR} AND EXISTS ${_SRCFILE_FAIL})
                try_run(_RUN_RESULT_VAR _COMPILE_RESULT_VAR
                        "${_bindir}" "${_SRCFILE_FAIL}"
                         COMPILE_DEFINITIONS "${CXX11_COMPILER_FLAGS}")
                if(_COMPILE_RESULT_VAR AND _RUN_RESULT_VAR)
                    set(${RESULT_VAR} TRUE)
                else()
                    set(${RESULT_VAR} FALSE)
                endif()
            endif()
        endif()
        if(${RESULT_VAR} AND EXISTS ${_SRCFILE_FAIL_COMPILE})
            try_compile(_TMP_RESULT "${_bindir}" "${_SRCFILE_FAIL_COMPILE}"
                        COMPILE_DEFINITIONS "${CXX11_COMPILER_FLAGS}")
            if(_TMP_RESULT)
                set(${RESULT_VAR} FALSE)
            else()
                set(${RESULT_VAR} TRUE)
            endif()
        endif()

        if(${RESULT_VAR})
            message(STATUS "Checking C++11 support for \"${FEATURE_NAME}\": works")
        else()
            message(STATUS "Checking C++11 support for \"${FEATURE_NAME}\": not supported")
        endif()
        set(${RESULT_VAR} ${${RESULT_VAR}} CACHE INTERNAL "C++11 support for \"${FEATURE_NAME}\"")
    endif()

    if(${REQUIRED} AND NOT ${RESULT_VAR})
       message(FATAL_ERROR "No C++11 support for required functionality \"${FEATURE_NAME}\"")
    endif()
endfunction()

file(GLOB TEST_SOURCES "${CMAKE_SOURCE_DIR}/build/cmake/CheckCXX11Features/*.cpp")
add_custom_target(CXX11_TEST_SOURCES SOURCES ${TEST_SOURCES})
set(tests "")
foreach(test ${TEST_SOURCES})
  string(REGEX REPLACE ".*/cxx11-test-([^.]*)(.fail|.fail_compile)?.cpp" "\\1" test "/${test}")
  list(APPEND tests "${test}")
endforeach()
list(REMOVE_DUPLICATES tests)
foreach(test ${tests})
  list(FIND CXX11_REQUIRED_FEATURES "${test}" required)
  if(${required} LESS 0)
    cxx11_check_feature(${test} False)
  else()
    cxx11_check_feature(${test} True)
  endif()
endforeach()

if(HAS_CXX11_NULLPTR)
  add_definitions(-DMCRL2_HAVE_NULLPTR)
endif()
if(HAS_CXX11_UNORDERED_SET)
  add_definitions(-DMCRL2_HAVE_UNORDERED_SET)
endif()
