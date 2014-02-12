# Authors: Frank Stappers 
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# Random tests and documentation require Python

include(FindPythonInterp)

if( PYTHONINTERP_FOUND )
execute_process(COMMAND ${PYTHON_EXECUTABLE} --version
    ERROR_VARIABLE PYTHONINTERP_VERSION
    ERROR_STRIP_TRAILING_WHITESPACE
)			
message( STATUS "${PYTHONINTERP_VERSION}" )
endif( PYTHONINTERP_FOUND )

# Following script is based on a solution of Mark Moll:
# http://www.cmake.org/pipermail/cmake/2011-January/041666.html 
#
# Usage: "find_python_module(PyQt4 REQUIRED)"

function(find_python_module module)
	string(TOUPPER ${module} module_upper)
	if(NOT MCRL2_PY_${module_upper})
		if(ARGC GREATER 1 AND ARGV1 STREQUAL "REQUIRED")
			set(${module}_FIND_REQUIRED TRUE)
		endif()
		# A module's location is usually a directory, but for binary modules
		# it's a .so file.
		execute_process(COMMAND "${PYTHON_EXECUTABLE}" "-c" 
			"import re, ${module}; print re.compile('/__init__.py.*').sub('',${module}.__file__)"
			RESULT_VARIABLE _${module}_status 
			OUTPUT_VARIABLE _${module}_location
			ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
		if(NOT _${module}_status)
			set(MCRL2_PY_${module_upper} "${_${module}_location}" CACHE STRING 
				"Location of Python module ${module}" FORCE)
		endif(NOT _${module}_status)
	endif(NOT MCRL2_PY_${module_upper})
	find_package_handle_standard_args(MCRL2_PY_${module} DEFAULT_MSG MCRL2_PY_${module_upper})
endfunction(find_python_module)

# Find required Python modules
if( PYTHONINTERP_FOUND )
  #Find sphinx. When found sets MCRL2_PY_SPHINX
  find_python_module(sphinx)
  if ( MCRL2_PY_SPHINX )
    message( STATUS "Python Sphinx-module found" )
  endif( MCRL2_PY_SPHINX )

  #Find argparse. When found sets MCRL2_PY_ARGPARSE
  find_python_module(argparse)
  if ( MCRL2_PY_ARGPARSE )
    message( STATUS "Python Argparse-module found" )
  endif( MCRL2_PY_ARGPARSE )
endif( PYTHONINTERP_FOUND )


