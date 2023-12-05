find_package(PythonModules COMPONENTS yaml psutil REQUIRED)

# The workspace is used for temporary files (it is the directory where the test script is executed from).
set(WORKSPACE ${CMAKE_CURRENT_BINARY_DIR}/workspace)
file(MAKE_DIRECTORY ${WORKSPACE})

#
# Python test drivers are assumed to have the following behaviour random_testing.py --names
# prints all test names (on separate lines) that the test script can run.
# For each <name> a target is created named random_<name> that runs 100 consecutive
# tests with --repetitions 100 choosing the tests with the --pattern <name> option.
#
foreach(type "random" "regression")
  set(testrunner ${CMAKE_CURRENT_SOURCE_DIR}/${type}/${type}_testing.py)
  execute_process(COMMAND ${Python_EXECUTABLE} ${testrunner} --names
    WORKING_DIRECTORY ${WORKSPACE}
    OUTPUT_VARIABLE available_tests
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE testnames_error)
  if(testnames_error)
    message(SEND_ERROR "${testrunner} did not succesfully list its test targets. Please contact an mCRL2 developer to fix this.")
  endif()

  string(REPLACE "\n" ";" available_tests "${available_tests}")
  string(REPLACE "\r" "" available_tests "${available_tests}")

  foreach(testname ${available_tests})
    set(REPETITIONS "")
    if("${type}" STREQUAL "random")
      set(REPETITIONS "-r 10")
    endif()

    set(targetname "${type}_${testname}")

    add_test(NAME ${targetname}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/workspace
             COMMAND ${Python_EXECUTABLE} ${testrunner} ${REPETITIONS} --toolpath ${MCRL2_TOOL_PATH} --pattern ${testname})
    set_tests_properties(${targetname} PROPERTIES LABELS ${type})
  endforeach()
endforeach()

#
# Add a ctest test for the tooltests
#
add_test(NAME tooltests
  COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tool/tool_testing.py
    --toolpath ${MCRL2_TOOL_PATH}
    -m8
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/workspace
)