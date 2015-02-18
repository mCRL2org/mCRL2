include(GenerateToolTests)
find_package(PythonModules 2.4 COMPONENTS argparse)

generate_tool_tests(${CMAKE_SOURCE_DIR}/examples/academic/abp/abp.mcrl2 "")

if(MCRL2_EXTRA_TOOL_TESTS)
  set(MCRL2_DIR ${CMAKE_SOURCE_DIR}/examples/language) 
  generate_tool_tests(${MCRL2_DIR}/list.mcrl2 "")
  generate_tool_tests(${MCRL2_DIR}/funccomp.mcrl2 "")
  # lpsrealelm cannot deal with "P / 2" in the following
  generate_tool_tests(${MCRL2_DIR}/divide2_10.mcrl2 "lpsrealelm")
  # Disable lts2*, because LTS contains a "Terminate" actions, which 
  # is not declared in the mCRL2 spec
  generate_tool_tests(${MCRL2_DIR}/tau.mcrl2 "lts2lps;lts2pbes")
  generate_tool_tests(${MCRL2_DIR}/par.mcrl2 "lts2lps;lts2pbes")
  # Disable lpsparunfold when LPS has no process parameters
  generate_tool_tests(${MCRL2_DIR}/struct.mcrl2 "lpsparunfold")
  generate_tool_tests(${MCRL2_DIR}/forall.mcrl2 "lpsparunfold")
  generate_tool_tests(${MCRL2_DIR}/exists.mcrl2 "lpsparunfold")
  generate_tool_tests(${MCRL2_DIR}/small3.mcrl2 "lpsparunfold")
  generate_tool_tests(${MCRL2_DIR}/small2.mcrl2 "lpsparunfold")  
  generate_tool_tests(${MCRL2_DIR}/upcast.mcrl2 "lpsparunfold") 
  generate_tool_tests(${MCRL2_DIR}/delta0.mcrl2 "lpsparunfold")
  generate_tool_tests(${MCRL2_DIR}/lambda.mcrl2 "lpsparunfold")
  generate_tool_tests(${MCRL2_DIR}/delta.mcrl2 "lpsparunfold")
  generate_tool_tests(${MCRL2_DIR}/time.mcrl2 "lpsparunfold;lpsrealelm")
  # LPS and PBES instantiation time out on most platforms. All tests
  # that depend on the LTS or BES are also disabled.
  generate_tool_tests(${MCRL2_DIR}/sets_bags.mcrl2 "lpsbinary;lts2lps;lts2pbes;lps2lts;ltsinfo;ltsconvert;ltscompare;pbes2bool;pbes2bes;pbesinst;besconvert;bespp;besinfo;txt2bes;bessolve;pbespgsolve") 
  generate_tool_tests(${MCRL2_DIR}/rational.mcrl2 "pbes2bool;pbesrewr;lts2lps;besinfo;bespp;lpsbinary;pbes2bes;besconvert;bessolve;pbesinst;lts2pbes;pbespgsolve;txt2bes;lpsrealelm;lps2lts;ltsinfo;ltsconvert;ltscompare")
endif()

#
# Python test drivers are assumed to have the following behaviour:
#
#   testdrv.py --names
#
# prints all test names (on separate lines) defined by the test driver.
# The test names are assumed to be numbered consecutively, starting from 0.
#
#   testdrv.py --run NUM
#
# runs the test numbered NUM (if it exists) and returns exit code 0 if the
# test succeeds, or 1 if it fails.
#
include(./dependencies.cmake)

set(python_dirs "random" "regression")
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/workspace)
foreach(dir ${python_dirs})
  set(num 0)
  set(testrunner ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/test.py)
  execute_process(COMMAND ${PYTHON_EXECUTABLE} ${testrunner} --names
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/workspace
                  OUTPUT_VARIABLE tests
                  OUTPUT_STRIP_TRAILING_WHITESPACE
                  RESULT_VARIABLE testnames_error)
  if(testnames_error)
    message(SEND_ERROR "${testrunner} did not succesfully list its test targets. Please contact an mCRL2 developer to fix this.")
  endif()
  string(REPLACE "\n" ";" tests "${tests}")
  string(REPLACE "\r" "" tests "${tests}")
  foreach(testname ${tests})
    if(MCRL2_PYTHON_TESTS)
      add_test(NAME ${testname}
               WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/workspace
               COMMAND ${PYTHON_EXECUTABLE} ${testrunner} --run ${num} --toolpath ${MCRL2_STAGE_PATH}/${MCRL2_RUNTIME_PATH} --verbose)
    else()
      add_test(NAME ${testname}
               COMMAND ${CMAKE_COMMAND} -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH} -Dmode=FATAL_ERROR -P ${CMAKE_CURRENT_SOURCE_DIR}/dependencies.cmake)
    endif()
    set_tests_properties(${testname} PROPERTIES LABELS ${dir})
    math(EXPR num "${num} + 1")
  endforeach()
endforeach()
