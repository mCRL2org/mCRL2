# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

##---------------------------------------------------
## File for generating test targets
##---------------------------------------------------

##---------------------------------------------------
## Make tests
##---------------------------------------------------

## Set properties for testing
set(MCRL2_TEST_LABEL "rev.${MCRL2_VERSION}-${CMAKE_BUILD_TYPE}")

option(MCRL2_SKIP_LONG_TESTS "Disable execution of tests that take a long time to complete" OFF)
message(STATUS "MCRL2_SKIP_LONG_TESTS: ${MCRL2_SKIP_LONG_TESTS}")
mark_as_advanced(MCRL2_SKIP_LONG_TESTS)

if(MCRL2_SKIP_LONG_TESTS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMCRL2_SKIP_LONG_TESTS")
endif(MCRL2_SKIP_LONG_TESTS)

option(MCRL2_ENABLE_TEST_TARGETS "Enable/disable creation of test targets" OFF)
message(STATUS "MCRL2_ENABLE_TEST_TARGETS: ${MCRL2_ENABLE_TEST_TARGETS}" )

option(MCRL2_ENABLE_RELEASE_TEST_TARGETS "Enable/disable creation of test targets for toolset release" OFF)
message(STATUS "MCRL2_ENABLE_RELEASE_TEST_TARGETS: ${MCRL2_ENABLE_RELEASE_TEST_TARGETS}" )

ENABLE_TESTING()
configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/CTestCustom.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/CTestCustom.cmake" )

  # Define macro for build_and_run_test_targets
  # This method compiles tests when invoked
  macro( build_and_run_test_target TARGET )
    if(NOT CMAKE_CFG_INTDIR STREQUAL ".")
      set(_configuration --build-config "$<CONFIGURATION>")
    endif()
    ADD_TEST(NAME "${TARGET}" COMMAND ${CMAKE_CTEST_COMMAND}
     --build-and-test
     "${CMAKE_CURRENT_SOURCE_DIR}"
     "${CMAKE_CURRENT_BINARY_DIR}"
     --build-noclean
     --build-nocmake
     --build-generator "${CMAKE_GENERATOR}"
     --build-makeprogram "${CMAKE_MAKE_PROGRAM}"
     --build-exe-dir "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
     --build-target "${TARGET}"
     --build-project "${PROJECT_NAME}"
     ${_configuration}
     --test-command "${TARGET}"
    )
    set_tests_properties("${TARGET}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
  endmacro( build_and_run_test_target TARGET )

  add_subdirectory( libraries/lps/test)
  add_subdirectory( libraries/bes/test)
  add_subdirectory( libraries/pbes/test)
  add_subdirectory( libraries/lts/test)
  add_subdirectory( libraries/data/test)
  add_subdirectory( libraries/core/test)
  add_subdirectory( libraries/utilities/test)
  add_subdirectory( libraries/process/test )
  add_subdirectory( libraries/atermpp/test )
  add_subdirectory( libraries/trace/test )
  add_subdirectory( libraries/modal_formula/test )

##---------------------------------------------------
## Experimental tool dependencies
##---------------------------------------------------

option(MCRL2_ENABLE_TEST_COMPILED_EXAMPLES "Enable/disable testing of compiled tool examples" OFF)
message(STATUS "MCRL2_ENABLE_TEST_COMPILED_EXAMPLES: ${MCRL2_ENABLE_TEST_COMPILED_EXAMPLES}" )

  macro( build_and_run_test_example_target TARGET )
    if(NOT CMAKE_CFG_INTDIR STREQUAL ".")
      set(_configuration --build-config "$<CONFIGURATION>")
    endif()
    if(${ARGC} GREATER 1)
      set(_command "${TARGET}" "${ARGN}")
    else()
      set(_command "${TARGET}")
    endif()
    if(MCRL2_ENABLE_TEST_COMPILED_EXAMPLES)
      ADD_TEST(NAME "${TARGET}" COMMAND ${CMAKE_CTEST_COMMAND}
       --build-and-test
       "${CMAKE_CURRENT_SOURCE_DIR}"
       "${CMAKE_CURRENT_BINARY_DIR}"
       --build-noclean
       --build-nocmake
       --build-generator "${CMAKE_GENERATOR}"
       --build-target "${TARGET}"
       --build-makeprogram "${CMAKE_MAKE_PROGRAM}"
       --build-exe-dir "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
       ${_configuration}
       --test-command ${_command}
      )
      set_tests_properties("${TARGET}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
    endif(MCRL2_ENABLE_TEST_COMPILED_EXAMPLES)	
  endmacro( build_and_run_test_example_target TARGET )

  add_subdirectory( libraries/atermpp/example)
  add_subdirectory( libraries/data/example)
  add_subdirectory( libraries/lps/example)
  add_subdirectory( libraries/utilities/example )
  add_subdirectory( libraries/pbes/example)
  add_subdirectory( libraries/bes/example)

if( MCRL2_ENABLE_TEST_TARGETS )
   add_subdirectory( tools/lpsparunfold/test )
endif( MCRL2_ENABLE_TEST_TARGETS )

##---------------------------------------
## Tool tests
##---------------------------------------


  #-------------
  # abp.mcrl2
  #-------------

  # Conversion tools
  set(testdir "${CMAKE_CURRENT_BINARY_DIR}/mcrl2-testoutput")
  file( REMOVE_RECURSE ${testdir} )
  file( MAKE_DIRECTORY ${testdir} )
  add_test(NAME mcrl22lps_version COMMAND mcrl22lps --version)
  add_test(NAME mcrl22lps_abp COMMAND mcrl22lps -v -D ${CMAKE_SOURCE_DIR}/examples/academic/abp/abp.mcrl2 ${testdir}/abp.lps)
  add_test(NAME lpsinfo_abp COMMAND lpsinfo ${testdir}/abp.lps)
  set_tests_properties( lpsinfo_abp PROPERTIES DEPENDS mcrl22lps_abp )
  add_test(NAME lpsconstelm_abp COMMAND lpsconstelm -v ${testdir}/abp.lps ${testdir}/abp_celm.lps)
  set_tests_properties( lpsconstelm_abp PROPERTIES DEPENDS mcrl22lps_abp )
  add_test(NAME lpsparelm_abp COMMAND lpsparelm -v ${testdir}/abp_celm.lps ${testdir}/abp_celm_pelm.lps)
  set_tests_properties( lpsparelm_abp PROPERTIES DEPENDS lpsconstelm_abp )
  add_test(NAME lpssuminst_abp COMMAND lpssuminst -v ${testdir}/abp_celm_pelm.lps ${testdir}/abp_celm_pelm_sinst.lps)
  set_tests_properties( lpssuminst_abp PROPERTIES DEPENDS lpsparelm_abp )
  add_test(NAME lps2lts_abp COMMAND lps2lts -v ${testdir}/abp_celm_pelm_sinst.lps ${testdir}/abp_celm_pelm_sinst.aut  )
  set_tests_properties( lps2lts_abp PROPERTIES DEPENDS lpssuminst_abp )
  add_test(NAME lps2lts_abp_fsm COMMAND lps2lts -v ${testdir}/abp_celm_pelm_sinst.lps ${testdir}/abp_celm_pelm_sinst.fsm  )
  set_tests_properties( lps2lts_abp_fsm PROPERTIES DEPENDS lpssuminst_abp )
  add_test(NAME lps2lts_abp_dot COMMAND lps2lts -v ${testdir}/abp_celm_pelm_sinst.lps ${testdir}/abp_celm_pelm_sinst.dot  )
  set_tests_properties( lps2lts_abp_dot PROPERTIES DEPENDS lpssuminst_abp )
  add_test(NAME lps2lts_abp_lts COMMAND lps2lts -v ${testdir}/abp_celm_pelm_sinst.lps ${testdir}/abp_celm_pelm_sinst.lts  )
  set_tests_properties( lps2lts_abp_lts PROPERTIES DEPENDS lpssuminst_abp )
  add_test(NAME ltsinfo_abp COMMAND ltsinfo ${testdir}/abp_celm_pelm_sinst.aut )
  set_tests_properties( ltsinfo_abp PROPERTIES DEPENDS lps2lts_abp )
  add_test(NAME ltsinfo_abp_fsm COMMAND ltsinfo ${testdir}/abp_celm_pelm_sinst.fsm )
  set_tests_properties( ltsinfo_abp_fsm PROPERTIES DEPENDS lps2lts_abp_fsm )
  add_test(NAME ltsinfo_abp_lts COMMAND ltsinfo ${testdir}/abp_celm_pelm_sinst.lts )
  set_tests_properties( ltsinfo_abp_lts PROPERTIES DEPENDS lps2lts_abp_lts )
  add_test(NAME lps2pbes_abp COMMAND lps2pbes -v -f${CMAKE_SOURCE_DIR}/examples/modal-formulas/nodeadlock.mcf ${testdir}/abp_celm_pelm_sinst.lps ${testdir}/abp_celm_pelm_sinst.pbes)
  set_tests_properties( lps2pbes_abp PROPERTIES DEPENDS lpssuminst_abp )
  add_test(NAME pbes2bool_abp COMMAND pbes2bool -v ${testdir}/abp_celm_pelm_sinst.pbes)
  set_tests_properties( pbes2bool_abp PROPERTIES DEPENDS lps2pbes_abp )
  add_test(NAME tracepp_test COMMAND tracepp ${CMAKE_SOURCE_DIR}/examples/industrial/garage/movie.trc ${testdir}/movie.txt )
  add_test(NAME lts2lps_test COMMAND lts2lps ${testdir}/abp_celm_pelm_sinst.lts ${testdir}/abp_celm_pelm_sinst_lts.mcrl2)
  set_tests_properties( lts2lps_test PROPERTIES DEPENDS lps2lts_abp_lts )
  add_test(NAME ltsconvert_bisim_test COMMAND ltsconvert -ebisim ${testdir}/abp_celm_pelm_sinst.lts ${testdir}/abp_celm_pelm_sinst_bisim.aut)
  set_tests_properties( ltsconvert_bisim_test PROPERTIES DEPENDS lps2lts_abp_lts )

  if( MCRL2_ENABLE_EXPERIMENTAL )
    add_test(NAME pbesinst_abp COMMAND pbesinst -v ${testdir}/abp_celm_pelm_sinst.pbes ${testdir}/abp_celm_pelm_sinst_pinst.pbes)
    set_tests_properties( pbesinst_abp PROPERTIES DEPENDS lps2pbes_abp )
  endif( MCRL2_ENABLE_EXPERIMENTAL )

  # Documentation tests
  if (MCRL2_MAN_PAGES)
    add_test(NAME mcrl22lps_generate-man-page COMMAND mcrl22lps --generate-man-page)
    set_tests_properties(
      mcrl22lps_generate-man-page
    PROPERTIES
      LABELS "${MCRL2_TEST_LABEL}"
    )
  endif(MCRL2_MAN_PAGES)
  add_test(NAME mcrl22lps_generate-xml COMMAND mcrl22lps --generate-xml)


  set_tests_properties(
    mcrl22lps_abp
    mcrl22lps_generate-xml
    lpsinfo_abp
    lpsconstelm_abp
    lpsparelm_abp
    lpssuminst_abp
    lps2lts_abp
    ltsinfo_abp
    pbes2bool_abp
    lps2pbes_abp
    tracepp_test
    lts2lps_test
    ltsconvert_bisim_test
  PROPERTIES
    LABELS "${MCRL2_TEST_LABEL}"
  )


##---------------------------------------
## Tool release tests
##---------------------------------------
include(${CMAKE_SOURCE_DIR}/scripts/GenerateReleaseToolTests.cmake)

# Usage run_release_tests( "/path/to/mcrl2file" "list of tests to disable" )
run_release_tests( "${CMAKE_SOURCE_DIR}/examples/academic/abp/abp.mcrl2" "")

if( MCRL2_ENABLE_RELEASE_TEST_TARGETS )
 run_release_tests( "${CMAKE_SOURCE_DIR}/examples/language/tau.mcrl2"				 "lts2lps;lts2pbes")  #txt2lps, because LTS contains a "Terminate" actions, which is not declared in "tau.mcrl2". Applies to aut, dot, fsm
 run_release_tests( "${CMAKE_SOURCE_DIR}/examples/language/small2.mcrl2"			 "lpsparunfold")  #Lps has no process parameters
 run_release_tests( "${CMAKE_SOURCE_DIR}/examples/language/upcast.mcrl2"			 "lpsparunfold")  #Lps has no process parameters
 run_release_tests( "${CMAKE_SOURCE_DIR}/examples/language/list.mcrl2"				 "")
 run_release_tests( "${CMAKE_SOURCE_DIR}/examples/language/funccomp.mcrl2"		 "lpsparunfold") # rinner + lpsconfcheck, lpsinvelm = SEGFAULT
 run_release_tests( "${CMAKE_SOURCE_DIR}/examples/language/struct.mcrl2"			 "lpsparunfold") #Lps has no process parameters
 run_release_tests( "${CMAKE_SOURCE_DIR}/examples/language/forall.mcrl2"			 "lpsparunfold") #Lps has no process parameters
 run_release_tests( "${CMAKE_SOURCE_DIR}/examples/language/exists.mcrl2"			 "lpsparunfold") #Lps has no process parameters
 run_release_tests( "${CMAKE_SOURCE_DIR}/examples/language/small3.mcrl2"			 "lpsparunfold")  #Lps has no process parameters
 run_release_tests( "${CMAKE_SOURCE_DIR}/examples/language/par.mcrl2"				 "lts2lps;lts2pbes")  #txt2lps, because LTS contains a "Terminate" actions, which is not declared in "tau.mcrl2". Applies to aut, dot, fsm
 run_release_tests( "${CMAKE_SOURCE_DIR}/examples/language/time.mcrl2"				 "lpsparunfold;lpsrealelm")
 run_release_tests( "${CMAKE_SOURCE_DIR}/examples/language/delta0.mcrl2"			 "lpsparunfold")
 run_release_tests( "${CMAKE_SOURCE_DIR}/examples/language/sets_bags.mcrl2"	 "lpsbinary;lts2lps;lts2pbes;lps2lts;ltsinfo;ltsconvert;ltscompare;pbes2bool;pbes2bes;pbesinst;besconvert;bespp;besinfo;txt2bes;bessolve;pbespgsolve") #lps2lts and pbes2bool because they cause timeouts on most platforms, ltsinfo, ltsconvert and ltscompare because they depend on the lts.
 run_release_tests( "${CMAKE_SOURCE_DIR}/examples/language/delta.mcrl2"			 "lpsparunfold")
 run_release_tests( "${CMAKE_SOURCE_DIR}/examples/language/rational.mcrl2"		 "pbes2bool;pbesrewr;lts2lps;besinfo;bespp;lpsbinary;pbes2bes;besconvert;bessolve;pbesinst;lts2pbes;pbespgsolve;txt2bes;lpsrealelm;lps2lts;ltsinfo;ltsconvert;ltscompare")
 run_release_tests( "${CMAKE_SOURCE_DIR}/examples/language/lambda.mcrl2"			 "lpsparunfold")
 run_release_tests( "${CMAKE_SOURCE_DIR}/examples/language/divide2_10.mcrl2" "lpsbinary;lpsrealelm")

endif( MCRL2_ENABLE_RELEASE_TEST_TARGETS )


# Random test generation
option(MCRL2_ENABLE_RANDOM_TEST_TARGETS "Enable/disable random test generation" OFF)
message(STATUS "MCRL2_ENABLE_RANDOM_TEST_TARGETS: ${MCRL2_ENABLE_RANDOM_TEST_TARGETS}" )

if( MCRL2_ENABLE_RANDOM_TEST_TARGETS AND PYTHONINTERP_FOUND )
  include(${CMAKE_SOURCE_DIR}/scripts/GenerateRandomTests.cmake)
endif( MCRL2_ENABLE_RANDOM_TEST_TARGETS AND PYTHONINTERP_FOUND )

include(CTest)

##---------------------------------------------------
## Header tests
##---------------------------------------------------
option(MCRL2_COMPILE_HEADER_TESTS "Enable/disable compile of library headers" OFF)
message(STATUS "MCRL2_COMPILE_HEADER_TESTS: ${MCRL2_COMPILE_HEADER_TESTS}" )
if(MCRL2_COMPILE_HEADER_TESTS)
  add_subdirectory( build/testing )
endif( MCRL2_COMPILE_HEADER_TESTS)

