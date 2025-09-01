# Copyright (c) 2012 - 2017, Lars Bilke
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software without
#    specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# https://github.com/bilke/cmake-modules/blob/master/CodeCoverage.cmake
#
# CHANGES:
#
# 2012-01-31, Lars Bilke
# - Enable Code Coverage
#
# 2013-09-17, Joakim Söderberg
# - Added support for Clang.
# - Some additional usage instructions.
#
# 2016-02-03, Lars Bilke
# - Refactored functions to use named parameters
#
# 2017-06-02, Lars Bilke
# - Merged with modified version from github.com/ufz/ogs
#

include(CMakeParseArguments)
include(ProcessorCount)

# Check prereqs
find_program( GCOV_PATH gcov )
find_program( LCOV_PATH  NAMES lcov lcov.bat lcov.exe lcov.perl)
find_program( GENHTML_PATH NAMES genhtml genhtml.perl genhtml.bat )

mark_as_advanced(
  GCOV_PATH
  LCOV_PATH
  GENHTML_PATH
)

if(NOT GCOV_PATH)
  message(FATAL_ERROR "gcov not found! Aborting...")
endif() # NOT GCOV_PATH

if(NOT LCOV_PATH)
  message(FATAL_ERROR "lcov not found! Aborting...")
endif() # NOT LCOV_PATH

if(NOT GENHTML_PATH)
  message(FATAL_ERROR "genhtml not found! Aborting...")
endif() # NOT GENHTML_PATH

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "(Apple)?[Cc]lang")
  if("${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS 3)
    message(FATAL_ERROR "Test coverage requires Clang version 3.0.0 or greater. Aborting...")
  endif()
elseif(NOT CMAKE_COMPILER_IS_GNUCXX)
  message(FATAL_ERROR "Test coverage requires gcc or clang. Aborting...")
endif()


if(NOT CMAKE_BUILD_TYPE MATCHES "(Debug)")
  message(WARNING "Code coverage results with an optimised (non-Debug) build may be misleading.")
endif()
if(NOT MCRL2_ENABLE_TESTS)
  message(WARNING "Code coverage for tests is turned on while tests are disabled.")
endif()

# The try_add_cxx_flag function is not really working.. For now just set flag manually.
set(COVERAGE_COMPILER_FLAGS "-g -O0 --coverage -fprofile-arcs -fprofile-update=atomic -ftest-coverage"
  CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COVERAGE_COMPILER_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COVERAGE_COMPILER_FLAGS}")

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
  link_libraries(gcov)
else()
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

ProcessorCount(NumProc)
if(NOT NumProc EQUAL 0)
  set(PROCESSOR_ARG -j${NumProc})
endif()

set(coverage_target mcrl2_coverage)

# Setup target
add_custom_target(${coverage_target}

  # Cleanup lcov
  COMMAND ${LCOV_PATH} --directory . --zerocounters
  # Create baseline to make sure untouched files show up in the report
  COMMAND ${LCOV_PATH} --capture --initial --directory .
    --ignore-errors mismatch,unused
    --output ${coverage_target}.base

  # Run tests
  # ctest exits with a non-zero exit code when at least one test fails. To make
  # sure that the rest of the commands are still executed, we use || true.
  COMMAND ctest --output-on-failure ${PROCESSOR_ARG} -L librarytest || true

  # Capturing lcov counters and generating report
  COMMAND ${LCOV_PATH} --capture --directory .
    --ignore-errors mismatch,unused,source
    --output-file ${coverage_target}.info

  # add baseline counters
  COMMAND ${LCOV_PATH} -a ${coverage_target}.base -a ${coverage_target}.info --output-file ${coverage_target}.total
  COMMAND ${LCOV_PATH} --remove "*.g" --remove "*/3rd-party/*" --remove "*toolset_version_const.h" --remove "*jittyc_*.cpp" --remove ${coverage_target}.total --output-file ${PROJECT_BINARY_DIR}/${coverage_target}.info.cleaned
  COMMAND ${GENHTML_PATH} --legend -o ${coverage_target} ${PROJECT_BINARY_DIR}/${coverage_target}.info.cleaned
  # COMMAND ${CMAKE_COMMAND} -E remove ${coverage_target}.base ${coverage_target}.total ${PROJECT_BINARY_DIR}/${coverage_target}.info.cleaned

  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  DEPENDS ${Coverage_DEPENDENCIES}
  COMMENT "Generating code coverage statistics..."
  USES_TERMINAL
)

# Show info where to find the report
add_custom_command(TARGET ${coverage_target} POST_BUILD
  COMMAND ;
  COMMENT "Coverage report saved in ./${coverage_target}/index.html."
)
