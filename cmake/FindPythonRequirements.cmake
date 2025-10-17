# This script can be used to find Python and specific modules.

find_package(Python 3.10.0)

if(Python_FOUND)
  # Execute Python and try to import the module, if this fails then the module cannot be found.
  execute_process(COMMAND ${Python_EXECUTABLE} "check_requirements.py" WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR} RESULT_VARIABLE RESULT)
  if(NOT ${RESULT} EQUAL 0)
      message(FATAL_ERROR "Python dependencies are not correctly installed, or of insufficient version. Please run 'pip3 install -r requirements.txt' in the mCRL2 source directory.")
  endif()
else()
  message(FATAL_ERROR "Could not find a suitable python3 interpreter. Ensure that variable Python_EXECUTABLE is set to a python3 interpreter.")
endif()
