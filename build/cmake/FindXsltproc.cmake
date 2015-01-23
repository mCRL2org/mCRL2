# Authors: Frank Stappers 
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# Documentation requires xsltproc

include(FindPackageHandleStandardArgs)

find_program(XSLTPROC_EXECUTABLE NAMES xsltproc QUIET)

if(XSLTPROC_EXECUTABLE)
  execute_process(
    COMMAND ${XSLTPROC_EXECUTABLE} --version
    OUTPUT_VARIABLE XSLTPROC_VERSION)
  string(REGEX MATCH "^([^\r\n]*)" XSLTPROC_VERSION "${XSLTPROC_VERSION}")
endif()

find_package_handle_standard_args(Xsltproc REQUIRED_VARS XSLTPROC_EXECUTABLE VERSION_VAR XSLTPROC_VERSION)
mark_as_advanced(XSLTPROC_EXECUTABLE)
