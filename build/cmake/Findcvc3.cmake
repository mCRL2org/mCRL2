# Authors: Frank Stappers 
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

FIND_PROGRAM(cvc3_EXECUTABLE
  NAMES
  cvc3	
)

IF( cvc3_EXECUTABLE )
	EXECUTE_PROCESS(COMMAND ${cvc3_EXECUTABLE} -version
    OUTPUT_VARIABLE cvc3_VERSION
    )

	STRING(REGEX REPLACE "^(.*)?CVC3 version ([.0-9]+).*"
					"\\2" cvc3_VERSION "${cvc3_VERSION}")

ENDIF(cvc3_EXECUTABLE )


INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(cvc3 REQUIRED_VARS cvc3_EXECUTABLE 
				                               VERSION_VAR cvc3_VERSION )
MARK_AS_ADVANCED( cvc3_EXECUTABLE )

set( cvc3_FOUND ${CVC3_FOUND} )

