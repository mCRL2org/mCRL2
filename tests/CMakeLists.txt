find_package(PythonRequirements)

# The workspace is used for temporary files (it is the directory where the test script is executed from).
set(WORKSPACE ${CMAKE_CURRENT_BINARY_DIR}/workspace)
file(MAKE_DIRECTORY ${WORKSPACE})

#
# Python test drivers are assumed to have the following behaviour random_testing.py --names
# prints all test names (on separate lines) that the test script can run.
# For each <name> a target is created named random_<name> that runs 10 consecutive
# tests with --repetitions 10 choosing the tests with the --pattern <name> option.
#
foreach(type "random" "regression")
  set(testrunner ${CMAKE_CURRENT_SOURCE_DIR}/${type}/${type}_testing.py)
  
  set(EXTRA_ARGS "")
  if(MCRL2_ENABLE_EXPERIMENTAL)
    set(EXTRA_ARGS "--experimental")
  endif()
  if("${type}" STREQUAL "random")
    set(EXTRA_ARGS "${EXTRA_ARGS};-r 10")
  endif()

  execute_process(COMMAND ${Python_EXECUTABLE} ${testrunner} --names ${EXTRA_ARGS} --python ${Python_EXECUTABLE}
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

    set(targetname "${type}_${testname}")

    add_test(NAME ${targetname}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/workspace
             COMMAND ${Python_EXECUTABLE} ${testrunner} ${EXTRA_ARGS} --toolpath ${MCRL2_TOOL_PATH} --pattern ${testname} --python ${Python_EXECUTABLE})
    set_tests_properties(${targetname} PROPERTIES LABELS ${type})
  endforeach()
endforeach()

#
# Add a ctest test for the tooltests
#
set(TOOLTEST_ARGS "-m8")
if(MCRL2_ENABLE_JITTYC)
  set(TOOLTEST_ARGS "${TOOLTEST_ARGS};--jittyc")
endif()

if(CVC3_EXECUTABLE)
  set(TOOLTEST_ARGS "${TOOLTEST_ARGS};--cvc3")
endif()

if(MCRL2_ENABLE_EXPERIMENTAL)
  set(TOOLTEST_ARGS "${TOOLTEST_ARGS};--experimental")
endif()

add_test(NAME tooltests
  COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/tool_testing.py
    --toolpath ${MCRL2_TOOL_PATH}
    -m 4
    ${TOOLTEST_ARGS}
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/workspace
)

set(EXAMPLETEST_ARGS "")
if(MCRL2_ENABLE_JITTYC)
  set(EXAMPLETEST_ARGS "${EXAMPLETEST_ARGS};--jittyc")
endif()

add_test(NAME exampletests
  COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/run_examples.py
    --toolpath ${MCRL2_TOOL_PATH}
    --timeout 15
    -m 8
    ${EXAMPLETEST_ARGS}
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/workspace
)
