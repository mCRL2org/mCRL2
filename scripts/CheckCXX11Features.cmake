# - Check which parts of the C++11 standard the compiler supports
# - Assumes that -std=c++0x or -std=c++11 has already been set for platforms
#   that need those flags.
#
# When found it will set the following variables
#
#  HAS_CXX11_AUTO               - auto keyword
#  HAS_CXX11_AUTO_RET_TYPE      - function declaration with deduced return types
#  HAS_CXX11_CLASS_OVERRIDE     - override and final keywords for classes and methods
#  HAS_CXX11_CONSTEXPR          - constexpr keyword
#  HAS_CXX11_CSTDINT_H          - cstdint header
#  HAS_CXX11_DECLTYPE           - decltype keyword
#  HAS_CXX11_FUNC               - __func__ preprocessor constant
#  HAS_CXX11_INITIALIZER_LIST   - initializer list
#  HAS_CXX11_LAMBDA             - lambdas
#  HAS_CXX11_LIB_REGEX          - regex library
#  HAS_CXX11_LONG_LONG          - long long signed & unsigned types
#  HAS_CXX11_NULLPTR            - nullptr
#  HAS_CXX11_RVALUE_REFERENCES  - rvalue references
#  HAS_CXX11_SIZEOF_MEMBER      - sizeof() non-static members
#  HAS_CXX11_STATIC_ASSERT      - static_assert()
#  HAS_CXX11_VARIADIC_TEMPLATES - variadic templates
#  HAS_CXX11_UNIQUE_PTR         - std::unique_ptr

#=============================================================================
# Copyright 2011,2012 Rolf Eike Beer <eike@sf-mail.de>
# Copyright 2012 Andreas Weis
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

if (NOT CMAKE_CXX_COMPILER_LOADED)
    message(FATAL_ERROR "CheckCXX11Features module only works if language CXX is enabled")
endif ()

cmake_minimum_required(VERSION 2.8.3)

#
### Check for needed compiler flags
#
function(cxx11_check_feature FEATURE_NAME RESULT_VAR REQUIRED)
    set(_LOG_NAME "\"${FEATURE_NAME}\"")
    if (NOT DEFINED ${RESULT_VAR})
        set(_bindir "${CMAKE_CURRENT_BINARY_DIR}")

        set(_SRCFILE_BASE ${CMAKE_CURRENT_LIST_DIR}/CheckCXX11Features/cxx11-test-${FEATURE_NAME})
        message(STATUS "Checking C++11 support for ${_LOG_NAME}")

        set(_SRCFILE "${_SRCFILE_BASE}.cpp")
        set(_SRCFILE_FAIL "${_SRCFILE_BASE}_fail.cpp")
        set(_SRCFILE_FAIL_COMPILE "${_SRCFILE_BASE}_fail_compile.cpp")

        if (CROSS_COMPILING)
            try_compile(${RESULT_VAR} "${_bindir}" "${_SRCFILE}"
                        COMPILE_DEFINITIONS "${CXX11_COMPILER_FLAGS}")
            if (${RESULT_VAR} AND EXISTS ${_SRCFILE_FAIL})
                try_compile(${RESULT_VAR} "${_bindir}" "${_SRCFILE_FAIL}"
                            COMPILE_DEFINITIONS "${CXX11_COMPILER_FLAGS}")
            endif (${RESULT_VAR} AND EXISTS ${_SRCFILE_FAIL})
        else (CROSS_COMPILING)
            try_run(_RUN_RESULT_VAR _COMPILE_RESULT_VAR
                    "${_bindir}" "${_SRCFILE}"
                    COMPILE_DEFINITIONS "${CXX11_COMPILER_FLAGS}")
            if (_COMPILE_RESULT_VAR AND NOT _RUN_RESULT_VAR)
                set(${RESULT_VAR} TRUE)
            else (_COMPILE_RESULT_VAR AND NOT _RUN_RESULT_VAR)
                set(${RESULT_VAR} FALSE)
            endif (_COMPILE_RESULT_VAR AND NOT _RUN_RESULT_VAR)
            if (${RESULT_VAR} AND EXISTS ${_SRCFILE_FAIL})
                try_run(_RUN_RESULT_VAR _COMPILE_RESULT_VAR
                        "${_bindir}" "${_SRCFILE_FAIL}"
                         COMPILE_DEFINITIONS "${CXX11_COMPILER_FLAGS}")
                if (_COMPILE_RESULT_VAR AND _RUN_RESULT_VAR)
                    set(${RESULT_VAR} TRUE)
                else (_COMPILE_RESULT_VAR AND _RUN_RESULT_VAR)
                    set(${RESULT_VAR} FALSE)
                endif (_COMPILE_RESULT_VAR AND _RUN_RESULT_VAR)
            endif (${RESULT_VAR} AND EXISTS ${_SRCFILE_FAIL})
        endif (CROSS_COMPILING)
        if (${RESULT_VAR} AND EXISTS ${_SRCFILE_FAIL_COMPILE})
            try_compile(_TMP_RESULT "${_bindir}" "${_SRCFILE_FAIL_COMPILE}"
                        COMPILE_DEFINITIONS "${CXX11_COMPILER_FLAGS}")
            if (_TMP_RESULT)
                set(${RESULT_VAR} FALSE)
            else (_TMP_RESULT)
                set(${RESULT_VAR} TRUE)
            endif (_TMP_RESULT)
        endif (${RESULT_VAR} AND EXISTS ${_SRCFILE_FAIL_COMPILE})

        if (${RESULT_VAR})
            message(STATUS "Checking C++11 support for ${_LOG_NAME}: works")
        else (${RESULT_VAR})
            message(STATUS "Checking C++11 support for ${_LOG_NAME}: not supported")
        endif (${RESULT_VAR})
        set(${RESULT_VAR} ${${RESULT_VAR}} CACHE INTERNAL "C++11 support for ${_LOG_NAME}")
    endif (NOT DEFINED ${RESULT_VAR})

    if (${REQUIRED} AND NOT ${RESULT_VAR})
       message(FATAL_ERROR "No C++11 support for required functionality ${_LOG_NAME}")
    endif()
endfunction(cxx11_check_feature)

cxx11_check_feature("__func__" HAS_CXX11_FUNC False)
cxx11_check_feature("auto" HAS_CXX11_AUTO True) # Auto is assumed to be supported
cxx11_check_feature("auto_ret_type" HAS_CXX11_AUTO_RET_TYPE False)
cxx11_check_feature("class_override_final" HAS_CXX11_CLASS_OVERRIDE False)
cxx11_check_feature("constexpr" HAS_CXX11_CONSTEXPR False)
cxx11_check_feature("cstdint" HAS_CXX11_CSTDINT_H False)
cxx11_check_feature("decltype" HAS_CXX11_DECLTYPE False)
cxx11_check_feature("initializer_list" HAS_CXX11_INITIALIZER_LIST False)
cxx11_check_feature("lambda" HAS_CXX11_LAMBDA False)
cxx11_check_feature("long_long" HAS_CXX11_LONG_LONG False)
cxx11_check_feature("nullptr" HAS_CXX11_NULLPTR False)
cxx11_check_feature("regex" HAS_CXX11_LIB_REGEX False)
cxx11_check_feature("rvalue-references" HAS_CXX11_RVALUE_REFERENCES False)
cxx11_check_feature("sizeof_member" HAS_CXX11_SIZEOF_MEMBER False)
cxx11_check_feature("static_assert" HAS_CXX11_STATIC_ASSERT False)
cxx11_check_feature("variadic_templates" HAS_CXX11_VARIADIC_TEMPLATES False)
cxx11_check_feature("unique_ptr" HAS_CXX11_UNIQUE_PTR False)
cxx11_check_feature("is_sorted" HAS_CXX11_IS_SORTED True) # Assumed to be supported
cxx11_check_feature("unordered_set" HAS_CXX11_UNORDERED_SET False)

if(HAS_CXX11_NULLPTR)
  add_definitions(-DMCRL2_HAVE_NULLPTR)
endif()
if(HAS_CXX11_UNORDERED_SET)
  add_definitions(-DMCRL2_HAVE_UNORDERED_SET)
endif()
