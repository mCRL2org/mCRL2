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


option(MCRL2_ENABLE_TEST_TARGETS "Enable/disable creation of test targets" OFF)
message(STATUS "MCRL2_ENABLE_TEST_TARGETS: ${MCRL2_ENABLE_TEST_TARGETS}" )

option(MCRL2_ENABLE_RELEASE_TEST_TARGETS "Enable/disable creation of test targets for toolset release" OFF)
message(STATUS "MCRL2_ENABLE_RELEASE_TEST_TARGETS: ${MCRL2_ENABLE_RELEASE_TEST_TARGETS}" )

ENABLE_TESTING()
configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/CTestCustom.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/CTestCustom.cmake" )

  # Define macro for build_and_run_test_targets
  # This method compiles tests when invoked
  macro( build_and_run_test_target TARGET )
    ADD_TEST("${TARGET}" ${CMAKE_CTEST_COMMAND}
     --build-and-test
     "${CMAKE_CURRENT_SOURCE_DIR}"
     "${CMAKE_CURRENT_BINARY_DIR}"
     --build-noclean
     --build-nocmake
     --build-generator "${CMAKE_GENERATOR}"
     --build-target "${TARGET}"
     --build-makeprogram "${CMAKE_MAKE_PROGRAM}"
     --build-project "${PROJECT_NAME}"
     --test-command "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}"
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

##---------------------------------------------------
## Experimental tool dependencies
##---------------------------------------------------

option(MCRL2_ENABLE_TEST_COMPILED_EXAMPLES "Enable/disable testing of compiled tool examples" OFF)
message(STATUS "MCRL2_ENABLE_TEST_COMPILED_EXAMPLES: ${MCRL2_ENABLE_TEST_COMPILED_EXAMPLES}" )

  macro( build_and_run_test_example_target TARGET )
    if(MCRL2_ENABLE_TEST_COMPILED_EXAMPLES)
      ADD_TEST("${TARGET}" ${CMAKE_CTEST_COMMAND}
       --build-and-test
       "${CMAKE_CURRENT_SOURCE_DIR}"
       "${CMAKE_CURRENT_BINARY_DIR}"
       --build-noclean
       --build-nocmake
       --build-generator "${CMAKE_GENERATOR}"
       --build-target "${TARGET}"
       --build-makeprogram "${CMAKE_MAKE_PROGRAM}"
       --test-command "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}"
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
  add_test(mcrl22lps_version ${mcrl22lps_BINARY_DIR}/mcrl22lps --version)
  add_test(mcrl22lps_abp ${mcrl22lps_BINARY_DIR}/mcrl22lps -v -D ${CMAKE_SOURCE_DIR}/examples/academic/abp/abp.mcrl2 ${testdir}/abp.lps)
  add_test(lpsinfo_abp ${lpsinfo_BINARY_DIR}/lpsinfo ${testdir}/abp.lps)
  set_tests_properties( lpsinfo_abp PROPERTIES DEPENDS mcrl22lps_abp )
  add_test(lpsconstelm_abp ${lpsconstelm_BINARY_DIR}/lpsconstelm -v ${testdir}/abp.lps ${testdir}/abp_celm.lps)
  set_tests_properties( lpsconstelm_abp PROPERTIES DEPENDS mcrl22lps_abp )
  add_test(lpsparelm_abp ${lpsparelm_BINARY_DIR}/lpsparelm -v ${testdir}/abp_celm.lps ${testdir}/abp_celm_pelm.lps)
  set_tests_properties( lpsparelm_abp PROPERTIES DEPENDS lpsconstelm_abp )
  add_test(lpssuminst_abp ${lpssuminst_BINARY_DIR}/lpssuminst -v ${testdir}/abp_celm_pelm.lps ${testdir}/abp_celm_pelm_sinst.lps)
  set_tests_properties( lpssuminst_abp PROPERTIES DEPENDS lpsparelm_abp )
  add_test(lps2lts_abp ${lps2lts_BINARY_DIR}/lps2lts -v ${testdir}/abp_celm_pelm_sinst.lps ${testdir}/abp_celm_pelm_sinst.aut  )
  set_tests_properties( lps2lts_abp PROPERTIES DEPENDS lpssuminst_abp )
  add_test(lps2lts_abp_fsm ${lps2lts_BINARY_DIR}/lps2lts -v ${testdir}/abp_celm_pelm_sinst.lps ${testdir}/abp_celm_pelm_sinst.fsm  )
  set_tests_properties( lps2lts_abp_fsm PROPERTIES DEPENDS lpssuminst_abp )
  add_test(lps2lts_abp_dot ${lps2lts_BINARY_DIR}/lps2lts -v ${testdir}/abp_celm_pelm_sinst.lps ${testdir}/abp_celm_pelm_sinst.dot  )
  set_tests_properties( lps2lts_abp_dot PROPERTIES DEPENDS lpssuminst_abp )
  add_test(lps2lts_abp_svc ${lps2lts_BINARY_DIR}/lps2lts -v ${testdir}/abp_celm_pelm_sinst.lps ${testdir}/abp_celm_pelm_sinst.svc  )
  set_tests_properties( lps2lts_abp_svc PROPERTIES DEPENDS lpssuminst_abp )
  add_test(lps2lts_abp_lts ${lps2lts_BINARY_DIR}/lps2lts -v ${testdir}/abp_celm_pelm_sinst.lps ${testdir}/abp_celm_pelm_sinst.lts  )
  set_tests_properties( lps2lts_abp_lts PROPERTIES DEPENDS lpssuminst_abp )
  add_test(ltsinfo_abp     ${ltsinfo_BINARY_DIR}/ltsinfo ${testdir}/abp_celm_pelm_sinst.aut )
  set_tests_properties( ltsinfo_abp PROPERTIES DEPENDS lps2lts_abp )
  add_test(ltsinfo_abp_fsm ${ltsinfo_BINARY_DIR}/ltsinfo ${testdir}/abp_celm_pelm_sinst.fsm )
  set_tests_properties( ltsinfo_abp_fsm PROPERTIES DEPENDS lps2lts_abp_fsm )
  add_test(ltsinfo_abp_dot ${ltsinfo_BINARY_DIR}/ltsinfo ${testdir}/abp_celm_pelm_sinst.dot )
  set_tests_properties( ltsinfo_abp_dot PROPERTIES DEPENDS lps2lts_abp_dot )
  add_test(ltsinfo_abp_svc ${ltsinfo_BINARY_DIR}/ltsinfo ${testdir}/abp_celm_pelm_sinst.svc )
  set_tests_properties( ltsinfo_abp_svc PROPERTIES DEPENDS lps2lts_abp_svc )
  add_test(ltsinfo_abp_lts ${ltsinfo_BINARY_DIR}/ltsinfo ${testdir}/abp_celm_pelm_sinst.lts )
  set_tests_properties( ltsinfo_abp_lts PROPERTIES DEPENDS lps2lts_abp_lts )
  add_test(lps2pbes_abp ${lps2pbes_BINARY_DIR}/lps2pbes -v -f${CMAKE_SOURCE_DIR}/examples/modal-formulas/nodeadlock.mcf ${testdir}/abp_celm_pelm_sinst.lps ${testdir}/abp_celm_pelm_sinst.pbes)
  set_tests_properties( lps2pbes_abp PROPERTIES DEPENDS lpssuminst_abp )
  add_test(pbes2bool_abp ${pbes2bool_BINARY_DIR}/pbes2bool -v ${testdir}/abp_celm_pelm_sinst.pbes)
  set_tests_properties( pbes2bool_abp PROPERTIES DEPENDS lps2pbes_abp )
  add_test(tracepp_test ${tracepp_BINARY_DIR}/tracepp ${CMAKE_SOURCE_DIR}/examples/industrial/garage/movie.trc ${testdir}/movie.txt )
  add_test(lts2lps_test ${lts2lps_BINARY_DIR}/lts2lps ${testdir}/abp_celm_pelm_sinst.lts ${testdir}/abp_celm_pelm_sinst_lts.mcrl2)
  set_tests_properties( lts2lps_test PROPERTIES DEPENDS lps2lts_abp_lts )
  add_test(ltsconvert_bisim_test ${ltsconvert_BINARY_DIR}/ltsconvert -ebisim ${testdir}/abp_celm_pelm_sinst.lts ${testdir}/abp_celm_pelm_sinst_bisim.aut)
  set_tests_properties( ltsconvert_bisim_test PROPERTIES DEPENDS lps2lts_abp_lts )

  if( MCRL2_ENABLE_EXPERIMENTAL )
    add_test(pbesinst_abp ${pbesinst_BINARY_DIR}/pbesinst -v ${testdir}/abp_celm_pelm_sinst.pbes ${testdir}/abp_celm_pelm_sinst_pinst.pbes)
    set_tests_properties( pbesinst_abp PROPERTIES DEPENDS lps2pbes_abp )
  endif( MCRL2_ENABLE_EXPERIMENTAL )

  # Simulation tools
  # add_test(lpssim_abp ${lpssim_BINARY_DIR}/lpssim ${testdir}/abp_celm_pelm_sinst.lps )
  # set_tests_properties(lpssim_abp PROPERTIES TIMEOUT 15 )

  # add_test(lpsxsim_abp ${lpsxsim_BINARY_DIR}/lpsxsim ${testdir}/abp_celm_pelm_sinst.lps )
  # set_tests_properties(lpsxsim_abp PROPERTIES TIMEOUT 15 )

  # Graphical tools
  # add_test(diagraphica_abp ${diagraphica_BINARY_DIR}/diagraphica ${testdir}/abp_celm_pelm_sinst.aut )
  # set_tests_properties(diagraphica_abp PROPERTIES TIMEOUT 15 )

  # add_test(ltsview_abp ${ltsview_BINARY_DIR}/ltsview ${testdir}/abp_celm_pelm_sinst.aut )
  # set_tests_properties(ltsview_abp PROPERTIES TIMEOUT 15 )

  # add_test(ltsgraph_abp ${ltsgraph_BINARY_DIR}/ltsgraph ${testdir}/abp_celm_pelm_sinst.aut )
  # set_tests_properties(ltsgraph_abp PROPERTIES TIMEOUT 15 )

  # add_test(grapemcrl2_smoke ${grapemcrl2_BINARY_DIR}/grapemcrl2)
  # set_tests_properties(grapemcrl2_smoke PROPERTIES TIMEOUT 15 )

  # Documentation tests
  if (MCRL2_MAN_PAGES)
    add_test(mcrl22lps_generate-man-page ${mcrl22lps_BINARY_DIR}/mcrl22lps --generate-man-page)
    set_tests_properties(
      mcrl22lps_generate-man-page
    PROPERTIES
      LABELS "${MCRL2_TEST_LABEL}"
    )
  endif(MCRL2_MAN_PAGES)
    add_test(mcrl22lps_generate-wiki-page ${mcrl22lps_BINARY_DIR}/mcrl22lps --generate-wiki-page)
    add_test(mcrl22lps_mcrl2-gui ${mcrl22lps_BINARY_DIR}/mcrl22lps --mcrl2-gui)


  set_tests_properties(
    mcrl22lps_abp
    mcrl22lps_generate-wiki-page
    mcrl22lps_mcrl2-gui
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
 run_release_tests( "${CMAKE_SOURCE_DIR}/examples/language/sets_bags.mcrl2"	 "lpsbinary;lts2lps;lts2pbes")
 run_release_tests( "${CMAKE_SOURCE_DIR}/examples/language/delta.mcrl2"			 "lpsparunfold")
 run_release_tests( "${CMAKE_SOURCE_DIR}/examples/language/rational.mcrl2"		 "pbes2bool;pbesrewr;lts2lps;besinfo;bespp;lpsbinary;pbes2bes;besconvert;bessolve;pbesinst;lts2pbes;pbespgsolve;txt2bes;lpsrealelm;lps2lts")
 run_release_tests( "${CMAKE_SOURCE_DIR}/examples/language/lambda.mcrl2"			 "lpsparunfold")
 run_release_tests( "${CMAKE_SOURCE_DIR}/examples/language/divide2_500.mcrl2" "lpsbinary;lpsrealelm")

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

