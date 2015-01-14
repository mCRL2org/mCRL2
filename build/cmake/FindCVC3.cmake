# Authors: Frank Stappers 
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

include(FindPackageHandleStandardArgs)

find_program(CVC3_EXECUTABLE NAMES cvc3)

if(CVC3_EXECUTABLE)
  execute_process(
    COMMAND ${CVC3_EXECUTABLE} -version
    OUTPUT_VARIABLE CVC3_VERSION)
  string(REGEX REPLACE "^(.*)?CVC3 version ([.0-9]+).*" "\\2" CVC3_VERSION "${CVC3_VERSION}")
endif()

find_package_handle_standard_args(CVC3 REQUIRED_VARS CVC3_EXECUTABLE VERSION_VAR CVC3_VERSION)
mark_as_advanced(CVC3_EXECUTABLE)

