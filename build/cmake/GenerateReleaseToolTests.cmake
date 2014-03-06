# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# This file will generate for each inputed file a series of tests
# Usage:
#  include(${CMAKE_SOURCE_DIR}/scripts/GenerateReleaseToolTests.cmake)
#  set(files "PATH2FILE_1; ... ; PATH2FILE_X"  )
#  run_release_tests( ${files} )

if(NOT WIN32)
  set(_JITTYC "-rjittyc")
endif()

set(tagIN "!IN")
set(tagOUT "!OUT")
set(_test_file_dependencies "")

function(add_tool_test TOOL)
  set(test_name "tooltest_${TOOL}_")
  set(test_args "")
  set(test_IN "")
  set(test_OUT "")
  foreach(arg ${ARGN})
    if(arg STREQ ${tagSAVE})
      set(test_SAVE TRUE)
      set(save_next "SAVE")
    elseif(arg STREQ ${tagIN} OR arg STREQ ${tagOUT})
      set(save_next test_{arg})
    else()
      set(test_name ${test_name}_${arg})
      list(APPEND test_args ${arg})
      if(save_next)
        list(APPEND ${save_next} ${arg})
      endif()
    endif()
  endforeach()

  add_test(${test_name} COMMAND ${TOOL} ${test_args})

  set(test_deps "")
  foreach(in_file ${test_IN})
    if(NOT EXISTS ${in_file})
      set(even TRUE)
      set(found FALSE)
      foreach(item ${_test_file_dependencies})
        if(even)
          if(item STREQ in_file)
            set(found TRUE)
          endif()
          set(even FALSE)
        else()
          if(found)
            list(APPEND test_deps ${item})
            break()
          endif()
          set(even TRUE)
        endif()
      endforeach()
      if(NOT found)
        message(FATAL_ERROR "Tool test ${test_name} is using an invalid input file: ${in_file}.")
      endif()
    endif()
  endforeach()

  if(test_deps)
    set_tests_properties(${test_name} PROPERTIES DEPENDS ${test_deps})
  endif()

  if(test_OUT)
    foreach(out_file ${test_OUT})
      list(APPEND _test_file_dependencies "${out_file}" "${test_name}")
    endforeach()
    set(_test_file_dependencies ${_test_file_dependencies} PARENT_SCOPE)
  endif()
endfunction()

# mcrl22lps
function(gen_mcrl22lps_release_tests MCRL2FILE LPSFILE)
  set(ARGUMENTS "-a" "-b" "-c" "-e" "-f" "-g" "-lregular" "-lregular2" "-lstack" "-m" 
                "-n" "--no-constelm" "-o" "-rjitty" "-rjittyp" ${_JITTYC}
                "--timings" "-w" "-z")
  foreach(arglist ${ARGUMENTS})
    add_tool_test(mcrl22lps ${arglist} ${tagIN} ${MCRL2FILE})
  endforeach()
  add_tool_test(mcrl22lps -D ${tagIN} ${MCRL2FILE} ${tagOUT} ${LPSFILE})
endfunction()

# lpsinfo
function(gen_lpsinfo_release_tests LPSFILE)
	add_tool_test(lpsinfo ${tagIN} ${LPSFILE})
endfunction()

# lpspp
function(gen_lpspp_release_tests LPSFILE TXTFILE)
	add_tool_test(lpspp -fdefault ${tagIN} ${LPSFILE} ${tagOUT} ${TXTFILE})
	add_tool_test(lpspp -finternal ${tagIN} ${LPSFILE})
endfunction()

# lps2lts
function(gen_lps2lts_release_tests LPSFILE LTSFILE AUTFILE SVCFILE FSMFILE DOTFILE ACTIONS)
  set(ARGUMENTS "-b10" "-ctau" "-D" "--error-trace" "--init-tsize=10" "-l10" "--no-info"
                "-rjitty" "-rjittyp" ${_JITTYC} "-sd" "-sb" "-sp" "-sq;-l100" "-sr;-l100"; 
                "--verbose;--suppress" "--todo-max=10" "-u" "-yno")
  if(ACTIONS)
    list(GET ACTIONS 0 ACTION)
    list(APPEND ARGUMENTS "-a${ACTIONS}" "-c${ACTION}")
  endif()
  foreach(arglist ${ARGUMENTS})
    add_tool_test(lps2lts ${arglist} ${tagIN} ${LPSFILE})
  endforeach()
  add_tool_test(lps2lts ${tagIN} ${LPSFILE} ${tagOUT} ${LTSFILE})
  add_tool_test(lps2lts ${tagIN} ${LPSFILE} ${tagOUT} ${AUTFILE})
  add_tool_test(lps2lts ${tagIN} ${LPSFILE} ${tagOUT} ${SVCFILE})
  add_tool_test(lps2lts ${tagIN} ${LPSFILE} ${tagOUT} ${FSMFILE})
  add_tool_test(lps2lts ${tagIN} ${LPSFILE} ${tagOUT} ${DOTFILE})
endfunction()

# lpsconstelm
function(gen_lpsconstelm_release_tests LPSFILE)
  set(ARGUMENTS "" "-c" "-f" "-s" "-t" "-rjitty" "-rjittyp" ${_JITTYC})
  foreach(arglist ${ARGUMENTS})
    add_tool_test(lpsconstelm ${arglist} ${tagIN} ${LPSFILE})
  endforeach()
endfunction()

# lpsrewr
function(gen_lpsrewr_release_tests LPSFILE)
  set(ARGUMENTS "-rjitty" "-rjittyp" ${_JITTYC})
  foreach(arglist ${ARGUMENTS})
    add_tool_test(lpsrewr ${arglist} ${tagIN} ${LPSFILE})
  endforeach()
endfunction()

# lpsparelm
function(gen_lpsparelm_release_tests LPSFILE)
	add_tool_test(lpsparelm ${tagIN} ${LPSFILE})
endfunction()

# lpssumelm
function(gen_lpssumelm_release_tests LPSFILE)
	add_tool_test(lpssumelm ${tagIN} ${LPSFILE})
	add_tool_test(lpssumelm -c ${LPSFILE})
endfunction()

# lpsactionrename
function(gen_lpsactionrename_release_tests LPSFILE RENFILE)
  set(ARGUMENTS "" "-m" "-o" "-ppa" "-ptc" "-P" "-rjitty" "-rjittyp" ${_JITTYC})
  foreach(arglist ${ARGUMENTS})
    add_tool_test(lpsactionrename -f${RENFILE} ${arglist} ${tagIN} ${LPSFILE})
  endforeach()
endfunction()

# lpsuntime
function(gen_lpsuntime_release_tests LPSFILE)
	add_tool_test(lpsuntime ${tagIN} ${LPSFILE})
endfunction()

# lpsinvelm
function(gen_lpsinvelm_release_tests LPSFILE TESTDIR)
  set(ARGUMENTS "-c" "-e" "-l" "-n" "-p" "-rjitty" "-t10" ${_JITTYC})
  if(cvc3_FOUND)
    list(APPEND ARGUMENTS "-zcvc")
  endif()
  foreach(arglist ${ARGUMENTS})
    add_tool_test(lpsinvelm -i${TESTDIR}/true.txt ${arglist} ${tagIN} ${LPSFILE})
  endforeach()
  add_tool_test(lpsinvelm -i${TESTDIR}/false.txt -y ${tagIN} ${LPSFILE})
endfunction()

# lpsconfcheck
function(gen_lpsconfcheck_release_tests LPSFILE TESTDIR)
  set(TRUEFILE ${TESTDIR}/true.txt)
  set(FALSEFILE ${TESTDIR}/true.txt)
  set(DOTFILE ${TESTDIR}/DOTFILE.dot)
  set(ARGUMENTS "" "-a" "-c" "-g" "-i${TRUEFILE}" "-i${FALSEFILE}"
                "-p${DOTFILE};-i${TRUEFILE}" "-m" "-n" "-o"
                "-rjitty" ${_JITTYC} "-s10;-i${TRUEFILE}" "-t10;-i${TRUEFILE}")
  if(cvc3_FOUND)
    list(APPEND ARGUMENTS "-zcvc;-i${TRUEFILE}")
  endif()
  foreach(arglist ${ARGUMENTS})
    add_tool_test(lpsconfcheck ${arglist} ${tagIN} ${LPSFILE})
  endforeach()
endfunction()

# lpsbinary
function(gen_lpsbinary_release_tests LPSFILE)
  set(ARGUMENTS "" "-rjitty" "-rjittyp" ${_JITTYC})
  foreach(arglist ${ARGUMENTS})
    add_tool_test(lpsbinary ${arglist} ${tagIN} ${LPSFILE})
  endforeach()
endfunction()

# lpsparunfold
function(gen_lpsparunfold_release_tests LPSFILE)
  set(ARGUMENTS "-l" "-n10" "-rjitty" "-rjittyp" ${_JITTYC})
  foreach(arglist ${ARGUMENTS})
    add_tool_test(lpsparunfold -sNat ${arglist} ${tagIN} ${LPSFILE})
  endforeach()
  add_tool_test(lpsparunfold -i0 ${tagIN} ${LPSFILE})
endfunction()

# lpssuminst
function(gen_lpssuminst_release_tests LPSFILE)
  set(ARGUMENTS "" "-rjitty" "-rjittyp" ${_JITTYC} "-t")
  foreach(arglist ${ARGUMENTS})
    add_tool_test(lpsparunfold -f ${arglist} ${tagIN} ${LPSFILE})
  endforeach()
endfunction()

# ltsinfo
function(gen_ltsinfo_release_tests LTSFILE)
	add_tool_test(ltsinfo ${tagIN} ${LTSFILE})
endfunction()

# lts2lps
function(gen_lts2lps_release_tests MCRL2FILE LTSFILE)
  add_tool_test(lts2lps -m ${tagIN} "${MCRL2FILE}" ${tagIN} ${LTSFILE})
endfunction()

# ltsconvert
function(gen_ltsconvert_release_tests LTSFILE ACTIONS)
  set(ARGUMENTS "" "-D" "-ebsisim" "-ebranching-bisim" "-edpbranching-bisim" "-esim" "-etau-star"
                "-etrace" "-eweak-trace" "-n" "--no-reach")
  if(ACTIONS)
    list(APPEND ARGUMENTS "--tau=${ACTIONS}")
  endif()
  foreach(arglist ${ARGUMENTS})
    add_tool_test(ltsconvert -oaut ${arglist} ${tagIN} ${LTSFILE})
  endforeach()
endfunction()

# ltscompare
function(gen_ltscompare_release_tests LTSFILE ACTIONS)
  set(ARGUMENTS "-c;-ebranching-bisim" "-c;-edpbranching-bisim"  "-c;-esim" "-c;-etrace" "-c;-eweak-trace"
                "-psim" "-pweak-trace")
  if(ACTIONS)
    list(APPEND ARGUMENTS "-ebisim;--tau=${ACTIONS}")
  endif()
  foreach(arglist ${ARGUMENTS})
    add_tool_test(ltscompare ${arglist} ${tagIN} ${LTSFILE} ${tagIN} ${LTSFILE})
  endforeach()
endfunction()

# lps2pbes
function(gen_lps2pbes_release_tests LPSFILE MCFFILES)
  get_filename_component(LPS_BASE ${LPSFILE} NAME_WE)
  foreach(MCF ${MCFFILES})
	  get_filename_component(MCF_BASE ${MCF} NAME_WE)
    add_tool_test(lps2pbes -f ${tagIN} ${MCF} ${LPSFILE} ${tagOUT} ${LPS_BASE}_${MCF_BASE}.pbes)
    add_tool_test(lps2pbes -tf ${tagIN} ${MCF} ${LPSFILE})
  endforeach(MCF)
endfunction()

# lts2pbes
macro( add_lts2pbes_release_test INPUT ARGS EXT)
  set( TRIMMED_ARGS "" )

  FOREACH( i ${ARGS} )

    if( ${i} MATCHES "^-f")
      set(TRIMMED_ARGS "${TRIMMED_ARGS}-f${INPUT}" )
    else( ${i} MATCHES "^-f")
      set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
    endif( ${i} MATCHES "^-f")
  ENDFOREACH( )

 set( POST_FIX_TEST "${BASENAME_TEST}-ARGS${TRIMMED_ARGS}" )


  ADD_TEST(NAME "lts2pbes_${POST_FIX_TEST}_${EXT}" COMMAND lts2pbes ${ARGS} ${testdir}/${BASENAME_TEST}.${EXT} -m${testdir}/${BASENAME_TEST}.mcrl2 ${testdir}/dummy.pbes )
  set_tests_properties("lts2pbes_${POST_FIX_TEST}_${EXT}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
  set_tests_properties("lts2pbes_${POST_FIX_TEST}_${EXT}" PROPERTIES DEPENDS "lps2lts_${BASENAME_TEST}-ARGS_${EXT}" )

endmacro( add_lts2pbes_release_test INPUT ARGS EXT)

macro( gen_lts2pbes_release_tests )
  foreach(MCF ${SET_OF_MCF} )
    foreach(EXT ${LTS_EXTS} )
      get_filename_component( mcf_name ${MCF} NAME_WE)
	add_lts2pbes_release_test( "${mcf_name}" "-f${MCF}" "${EXT}" )
    endforeach(EXT ${LTS_EXTS} )
  endforeach(MCF ${SET_OF_MCF})
endmacro( gen_lts2pbes_release_tests )

####################
## Macro pbesinfo  ##
####################

macro( add_pbesinfo_release_test ARGS)
	set( TRIMMED_ARGS "" )

  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${BASENAME_TEST}-ARGS${TRIMMED_ARGS}" )

	foreach(MCF ${SET_OF_MCF} )
    get_filename_component( mcf_name ${MCF} NAME_WE)

    ADD_TEST(NAME "pbesinfo_${POST_FIX_TEST}_${mcf_name}" COMMAND pbesinfo ${ARGS} ${testdir}/${BASENAME_TEST}_${mcf_name}.pbes  )
	  set_tests_properties("pbesinfo_${POST_FIX_TEST}_${mcf_name}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
	  set_tests_properties("pbesinfo_${POST_FIX_TEST}_${mcf_name}" PROPERTIES DEPENDS "lps2pbes_${BASENAME_TEST}-ARGS-f${mcf_name}" )
	endforeach(MCF ${SET_OF_MCF})

endmacro( add_pbesinfo_release_test ARGS)

macro( gen_pbesinfo_release_tests )
					add_pbesinfo_release_test(  "" )
					add_pbesinfo_release_test(  "-f" )
endmacro( gen_pbesinfo_release_tests )

###################
## Macro pbespp  ##
###################

macro( add_pbespp_release_test ARGS SAVE)
	set( TRIMMED_ARGS "" )

  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${BASENAME_TEST}-ARGS${TRIMMED_ARGS}" )

	foreach(MCF ${SET_OF_MCF} )
    get_filename_component( mcf_name ${MCF} NAME_WE)
  	if( NOT ${SAVE} )
      ADD_TEST(NAME "pbespp_${POST_FIX_TEST}_${mcf_name}" COMMAND pbespp ${ARGS} ${testdir}/${BASENAME_TEST}_${mcf_name}.pbes ${testdir}/${BASENAME_TEST}_${mcf_name}_pbes.txt )
  	else( NOT ${SAVE} )
      ADD_TEST(NAME "pbespp_${POST_FIX_TEST}_${mcf_name}" COMMAND pbespp ${ARGS} ${testdir}/${BASENAME_TEST}_${mcf_name}.pbes )
  	endif( NOT ${SAVE} )
	  set_tests_properties("pbespp_${POST_FIX_TEST}_${mcf_name}" PROPERTIES DEPENDS "lps2pbes_${BASENAME_TEST}-ARGS-f${mcf_name}" )
	endforeach()

endmacro( add_pbespp_release_test ARGS SAVE)

macro( gen_pbespp_release_tests )
					add_pbespp_release_test(  "-fdefault" "SAVE")
					add_pbespp_release_test(  "-finternal" "")
endmacro( gen_pbespp_release_tests )

#########################
## Macro pbesconstelm  ##
#########################

macro( add_pbesconstelm_release_test ARGS)
	set( TRIMMED_ARGS "" )

  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${BASENAME_TEST}-ARGS${TRIMMED_ARGS}" )

	foreach(MCF ${SET_OF_MCF} )
    get_filename_component( mcf_name ${MCF} NAME_WE)

    ADD_TEST(NAME "pbesconstelm_${POST_FIX_TEST}_${mcf_name}" COMMAND pbesconstelm ${ARGS} ${testdir}/${BASENAME_TEST}_${mcf_name}.pbes ${testdir}/dummy.pbes )
	  set_tests_properties("pbesconstelm_${POST_FIX_TEST}_${mcf_name}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
    set_tests_properties("pbesconstelm_${POST_FIX_TEST}_${mcf_name}" PROPERTIES DEPENDS "lps2pbes_${BASENAME_TEST}-ARGS-f${mcf_name}" )
	endforeach()
endmacro( add_pbesconstelm_release_test ARGS)

macro( gen_pbesconstelm_release_tests )
	add_pbesconstelm_release_test(  "-c" )
	add_pbesconstelm_release_test(  "-e" )
	add_pbesconstelm_release_test(  "-psimplify" )
	add_pbesconstelm_release_test(  "-pquantifier-all" )
	add_pbesconstelm_release_test(  "-pquantifier-finite" )
	add_pbesconstelm_release_test(  "-ppfnf" )
	add_pbesconstelm_release_test(  "-rjitty" )
	add_pbesconstelm_release_test(  "-rjittyp" )
	if( NOT WIN32)
		add_pbesconstelm_release_test(  "-rjittyc" )
	endif( NOT WIN32)
endmacro( gen_pbesconstelm_release_tests )

#######################
## Macro pbesparelm  ##
#######################

macro( add_pbesparelm_release_test )
	set( TRIMMED_ARGS "" )

  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${BASENAME_TEST}-ARGS${TRIMMED_ARGS}" )

	foreach(MCF ${SET_OF_MCF} )
    get_filename_component( mcf_name ${MCF} NAME_WE)
    ADD_TEST(NAME "pbesparelm_${POST_FIX_TEST}_${mcf_name}" COMMAND pbesparelm ${ARGS} ${testdir}/${BASENAME_TEST}_${mcf_name}.pbes ${testdir}/dummy.pbes )
	  set_tests_properties("pbesparelm_${POST_FIX_TEST}_${mcf_name}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
    set_tests_properties("pbesparelm_${POST_FIX_TEST}_${mcf_name}" PROPERTIES DEPENDS "lps2pbes_${BASENAME_TEST}-ARGS-f${mcf_name}" )
	endforeach()
endmacro( add_pbesparelm_release_test )

macro( gen_pbesparelm_release_tests )
					add_pbesparelm_release_test(  )
endmacro( gen_pbesparelm_release_tests )

#####################
## Macro pbesrewr  ##
#####################

macro( add_pbesrewr_release_test ARGS)
	set( TRIMMED_ARGS "" )

  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${BASENAME_TEST}-ARGS${TRIMMED_ARGS}" )

	foreach(MCF ${SET_OF_MCF} )
    get_filename_component( mcf_name ${MCF} NAME_WE)
    ADD_TEST(NAME "pbesrewr_${POST_FIX_TEST}_${mcf_name}" COMMAND pbesrewr ${ARGS} ${testdir}/${BASENAME_TEST}_${mcf_name}.pbes ${testdir}/dummy.pbes )
	  set_tests_properties("pbesrewr_${POST_FIX_TEST}_${mcf_name}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
    set_tests_properties("pbesrewr_${POST_FIX_TEST}_${mcf_name}" PROPERTIES DEPENDS "lps2pbes_${BASENAME_TEST}-ARGS-f${mcf_name}" )
	endforeach()
endmacro( add_pbesrewr_release_test ARGS)

macro( gen_pbesrewr_release_tests )
		add_pbesrewr_release_test( "-psimplify" )
		add_pbesrewr_release_test( "-pquantifier-all" )
		add_pbesrewr_release_test( "-pquantifier-finite" )
		add_pbesrewr_release_test( "-ppfnf" )
		add_pbesrewr_release_test( "-rjitty" )
		add_pbesrewr_release_test( "-rjittyp" )
		if( NOT WIN32)
			add_pbesrewr_release_test( "-rjittyc" )
		endif( NOT WIN32)
endmacro( gen_pbesrewr_release_tests )

######################
## Macro pbes2bool  ##
######################

macro( add_pbes2bool_release_test ARGS)
	set( TRIMMED_ARGS "" )

  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${BASENAME_TEST}-ARGS${TRIMMED_ARGS}" )

	foreach(MCF ${SET_OF_MCF} )
    get_filename_component( mcf_name ${MCF} NAME_WE)
    ADD_TEST(NAME "pbes2bool_${POST_FIX_TEST}_${mcf_name}" COMMAND pbes2bool ${ARGS} ${testdir}/${BASENAME_TEST}_${mcf_name}.pbes )
	  set_tests_properties("pbes2bool_${POST_FIX_TEST}_${mcf_name}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
    set_tests_properties("pbes2bool_${POST_FIX_TEST}_${mcf_name}" PROPERTIES DEPENDS "lps2pbes_${BASENAME_TEST}-ARGS-f${mcf_name}" )
	endforeach()
endmacro( add_pbes2bool_release_test ARGS)

macro( gen_pbes2bool_release_tests )
					add_pbes2bool_release_test( "-c" )
					add_pbes2bool_release_test( "-H" )
					add_pbes2bool_release_test( "-psimplify" )
					add_pbes2bool_release_test( "-pquantifier-all" )
					add_pbes2bool_release_test( "-pquantifier-finite" )
					add_pbes2bool_release_test( "-ppfnf" )
					add_pbes2bool_release_test( "-rjitty" )
					add_pbes2bool_release_test( "-rjittyp" )
					if( NOT WIN32)
  					add_pbes2bool_release_test( "-rjittyc" )
					endif( NOT WIN32)
					add_pbes2bool_release_test( "-s0" )
					add_pbes2bool_release_test( "-s1" )
					add_pbes2bool_release_test( "-s2" )
					add_pbes2bool_release_test( "-s3" )
					add_pbes2bool_release_test( "-t" )
					add_pbes2bool_release_test( "-u" )
endmacro( gen_pbes2bool_release_tests )

######################
## Macro txt2lps  ##
######################

macro( add_txt2lps_release_test ARGS)
	set( TRIMMED_ARGS "" )

  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${BASENAME_TEST}-ARGS${TRIMMED_ARGS}" )

  ADD_TEST(NAME "txt2lps_${POST_FIX_TEST}" COMMAND txt2lps ${ARGS} ${testdir}/${BASENAME_TEST}_lps.txt ${testdir}/dummy.lps )
	set_tests_properties("txt2lps_${POST_FIX_TEST}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
  set_tests_properties("txt2lps_${POST_FIX_TEST}" PROPERTIES DEPENDS "lpspp_${BASENAME_TEST}-ARGS-fdefault" )
endmacro( add_txt2lps_release_test ARGS)

macro( gen_txt2lps_release_tests )
					add_txt2lps_release_test( "" )
endmacro( gen_txt2lps_release_tests )

######################
## Macro txt2pbes  ##
######################

macro( add_txt2pbes_release_test ARGS)
	set( TRIMMED_ARGS "" )

  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${BASENAME_TEST}-ARGS${TRIMMED_ARGS}" )

	foreach(MCF ${SET_OF_MCF} )
    get_filename_component( mcf_name ${MCF} NAME_WE)

    ADD_TEST(NAME "txt2pbes_${POST_FIX_TEST}_${mcf_name}" COMMAND txt2pbes ${ARGS} ${testdir}/${BASENAME_TEST}_${mcf_name}_pbes.txt ${testdir}/dummy.pbes )
	  set_tests_properties("txt2pbes_${POST_FIX_TEST}_${mcf_name}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
    set_tests_properties("txt2pbes_${POST_FIX_TEST}_${mcf_name}" PROPERTIES DEPENDS "pbespp_${BASENAME_TEST}-ARGS-fdefault_${mcf_name}" )
	endforeach()
endmacro( add_txt2pbes_release_test ARGS)

macro( gen_txt2pbes_release_tests )
  add_txt2pbes_release_test( "" )
endmacro( gen_txt2pbes_release_tests )

######################
## Macro pbes2bes  ##
######################

macro( add_pbes2bes_release_test ARGS SAVE)
	set( TRIMMED_ARGS "" )

  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${BASENAME_TEST}-ARGS${TRIMMED_ARGS}" )

	foreach(MCF ${SET_OF_MCF} )
    get_filename_component( mcf_name ${MCF} NAME_WE)

		if( NOT ${SAVE} )
      ADD_TEST(NAME "pbes2bes_${POST_FIX_TEST}_${mcf_name}"
							COMMAND pbes2bes ${ARGS}
							${testdir}/${BASENAME_TEST}_${mcf_name}.pbes
							${testdir}/${BASENAME_TEST}_${mcf_name}.bes)
		else( NOT ${SAVE} )
      ADD_TEST(NAME "pbes2bes_${POST_FIX_TEST}_${mcf_name}"
							COMMAND pbes2bes ${ARGS}
							${testdir}/${BASENAME_TEST}_${mcf_name}.pbes
							${testdir}/dummy.bes)
		endif( NOT ${SAVE} )

	  set_tests_properties("pbes2bes_${POST_FIX_TEST}_${mcf_name}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
    set_tests_properties("pbes2bes_${POST_FIX_TEST}_${mcf_name}" PROPERTIES DEPENDS "lps2pbes_${BASENAME_TEST}-ARGS-f${mcf_name}" )
	endforeach()
endmacro( add_pbes2bes_release_test ARGS)

macro( gen_pbes2bes_release_tests )
	add_pbes2bes_release_test( "" "SAVE" )
	add_pbes2bes_release_test( "-H"  "")
	add_pbes2bes_release_test( "-opbes"  "")
	add_pbes2bes_release_test( "-ocwi"  "")
	add_pbes2bes_release_test( "-obes"  "")

	add_pbes2bes_release_test( "-psimplify" "" )
	add_pbes2bes_release_test( "-pquantifier-all"  "")
	add_pbes2bes_release_test( "-pquantifier-finite"  "")
	add_pbes2bes_release_test( "-ppfnf"  "")
	add_pbes2bes_release_test( "-rjitty"  "")
	add_pbes2bes_release_test( "-rjittyp"  "")
	if( NOT WIN32)
		add_pbes2bes_release_test( "-rjittyc"  "")
	endif( NOT WIN32)
	add_pbes2bes_release_test( "-s0"  "")
	add_pbes2bes_release_test( "-s1"  "")
	add_pbes2bes_release_test( "-s2"  "")
	add_pbes2bes_release_test( "-s3"  "")
	add_pbes2bes_release_test( "-t"  "")
	add_pbes2bes_release_test( "-u"  "")
endmacro( gen_pbes2bes_release_tests )

######################
## Macro besconvert  ##
######################

macro( add_besconvert_release_test ARGS)
	set( TRIMMED_ARGS "" )
  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${POST_FIX_TEST}-ARGS${TRIMMED_ARGS}" )

	foreach(MCF ${SET_OF_MCF} )
    get_filename_component( mcf_name ${MCF} NAME_WE)

    ADD_TEST(NAME "besconvert_${POST_FIX_TEST}" COMMAND besconvert ${ARGS} ${testdir}/${INPUT} ${testdir}/dummy.bes )
	  set_tests_properties("besconvert_${POST_FIX_TEST}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
   ### TODO:    set_tests_properties("besconvert_${POST_FIX_TEST}" PROPERTIES DEPENDS "" )
	endforeach()
endmacro( add_besconvert_release_test ARGS)

macro( gen_besconvert_release_tests )
  FOREACH( i ${SET_OF_PBES_FILES} )
					add_besconvert_release_test( "${i}" "-ebisim" )
					add_besconvert_release_test( "${i}" "-estuttering" )
  ENDFOREACH( )
endmacro( gen_besconvert_release_tests )

####################
## Macro besinfo  ##
####################

macro( add_besinfo_release_test ARGS)
	set( TRIMMED_ARGS "" )

  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${BASENAME_TEST}-ARGS${TRIMMED_ARGS}" )

	foreach(MCF ${SET_OF_MCF} )
    get_filename_component( mcf_name ${MCF} NAME_WE)

    ADD_TEST(NAME "besinfo_${POST_FIX_TEST}_${mcf_name}" COMMAND besinfo ${ARGS} ${testdir}/${BASENAME_TEST}_${mcf_name}.bes  )
	  set_tests_properties("besinfo_${POST_FIX_TEST}_${mcf_name}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
	  set_tests_properties("besinfo_${POST_FIX_TEST}_${mcf_name}" PROPERTIES DEPENDS "pbes2bes_${BASENAME_TEST}-ARGS_${mcf_name}" )
	endforeach(MCF ${SET_OF_MCF})

endmacro( add_besinfo_release_test ARGS)

macro( gen_besinfo_release_tests )
					add_besinfo_release_test(  "" )
					add_besinfo_release_test(  "-f" )
endmacro( gen_besinfo_release_tests )

###################
## Macro bespp  ##
###################

macro( add_bespp_release_test ARGS SAVE)
	set( TRIMMED_ARGS "" )

  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${BASENAME_TEST}-ARGS${TRIMMED_ARGS}" )

	foreach(MCF ${SET_OF_MCF} )
    get_filename_component( mcf_name ${MCF} NAME_WE)
  	if( NOT ${SAVE} )
      ADD_TEST(NAME "bespp_${POST_FIX_TEST}_${mcf_name}" COMMAND bespp ${ARGS} ${testdir}/${BASENAME_TEST}_${mcf_name}.bes ${testdir}/${BASENAME_TEST}_${mcf_name}_bes.txt )
  	else( NOT ${SAVE} )
      ADD_TEST(NAME "bespp_${POST_FIX_TEST}_${mcf_name}" COMMAND bespp ${ARGS} ${testdir}/${BASENAME_TEST}_${mcf_name}.bes )
  	endif( NOT ${SAVE} )
	  set_tests_properties("bespp_${POST_FIX_TEST}_${mcf_name}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
	  set_tests_properties("bespp_${POST_FIX_TEST}_${mcf_name}" PROPERTIES DEPENDS "pbes2bes_${BASENAME_TEST}-ARGS_${mcf_name}" )
	endforeach()

endmacro( add_bespp_release_test ARGS SAVE)

macro( gen_bespp_release_tests )
					add_bespp_release_test(  "-fdefault" "SAVE")
endmacro( gen_bespp_release_tests )

###################
## Macro bessolve  ##
###################

macro( add_bessolve_release_test ARGS )
	set( TRIMMED_ARGS "" )

  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${BASENAME_TEST}-ARGS${TRIMMED_ARGS}" )

	foreach(MCF ${SET_OF_MCF} )
    get_filename_component( mcf_name ${MCF} NAME_WE)
    ADD_TEST(NAME "bessolve_${POST_FIX_TEST}_${mcf_name}" COMMAND bessolve ${ARGS} ${testdir}/${BASENAME_TEST}_${mcf_name}.bes )
	  set_tests_properties("bessolve_${POST_FIX_TEST}_${mcf_name}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
	  set_tests_properties("bessolve_${POST_FIX_TEST}_${mcf_name}" PROPERTIES DEPENDS "pbes2bes_${BASENAME_TEST}-ARGS_${mcf_name}" )
	endforeach()

endmacro( add_bessolve_release_test ARGS )

macro( gen_bessolve_release_tests )
					add_bessolve_release_test(  "-sgauss" )
					add_bessolve_release_test(  "-sspm" )
endmacro( gen_bessolve_release_tests )

###################
## Macro besconvert  ##
###################

macro( add_besconvert_release_test ARGS )
	set( TRIMMED_ARGS "" )

  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${BASENAME_TEST}-ARGS${TRIMMED_ARGS}" )

	foreach(MCF ${SET_OF_MCF} )
    get_filename_component( mcf_name ${MCF} NAME_WE)
    ADD_TEST(NAME "besconvert_${POST_FIX_TEST}_${mcf_name}" COMMAND besconvert ${ARGS} ${testdir}/${BASENAME_TEST}_${mcf_name}.bes )
	  set_tests_properties("besconvert_${POST_FIX_TEST}_${mcf_name}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
	  set_tests_properties("besconvert_${POST_FIX_TEST}_${mcf_name}" PROPERTIES DEPENDS "pbes2bes_${BASENAME_TEST}-ARGS_${mcf_name}" )
	endforeach()

endmacro( add_besconvert_release_test ARGS )

macro( gen_besconvert_release_tests )
					add_besconvert_release_test(  "-ebisim" )
					add_besconvert_release_test(  "-estuttering" )
endmacro( gen_besconvert_release_tests )

###################
## Macro pbesabstract  ##
###################

macro( add_pbesabstract_release_test ARGS )
	set( TRIMMED_ARGS "" )

  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${BASENAME_TEST}-ARGS${TRIMMED_ARGS}" )

	foreach(MCF ${SET_OF_MCF} )
    get_filename_component( mcf_name ${MCF} NAME_WE)
    ADD_TEST(NAME "pbesabstract_${POST_FIX_TEST}_${mcf_name}" COMMAND pbesabstract ${ARGS} ${testdir}/${BASENAME_TEST}_${mcf_name}.pbes ${testdir}/dummy.pbes )
	  set_tests_properties("pbesabstract_${POST_FIX_TEST}_${mcf_name}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
	  set_tests_properties("pbesabstract_${POST_FIX_TEST}_${mcf_name}" PROPERTIES DEPENDS "lps2pbes_${BASENAME_TEST}-ARGS-f${mcf_name}" )
	endforeach()

endmacro( add_pbesabstract_release_test ARGS )

macro( gen_pbesabstract_release_tests )
					add_pbesabstract_release_test(  "-a1" )
					add_pbesabstract_release_test(  "-a0" )
endmacro( gen_pbesabstract_release_tests )

###################
## Macro pbesinst  ##
###################

macro( add_pbesinst_release_test ARGS )
  set( TRIMMED_ARGS "" )

  foreach( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
    string(REPLACE "*" "_" TRIMMED_ARGS ${TRIMMED_ARGS})
    string(REPLACE "\"" "_" TRIMMED_ARGS ${TRIMMED_ARGS})
    string(REPLACE "(" "_" TRIMMED_ARGS ${TRIMMED_ARGS})
    string(REPLACE ")" "_" TRIMMED_ARGS ${TRIMMED_ARGS})
  endforeach()

  set( POST_FIX_TEST "${BASENAME_TEST}-ARGS${TRIMMED_ARGS}" )

  set(EXT "pbes")
  if( "${ARGS}" STREQUAL "-obes" )
    set( EXT "bes" )
  endif( "${ARGS}" STREQUAL "-obes" )

  if( "${ARGS}" STREQUAL "-ocwi" )
    set( EXT "cwi" )
  endif( "${ARGS}" STREQUAL "-ocwi" )

  foreach(MCF ${SET_OF_MCF} )
    get_filename_component( mcf_name ${MCF} NAME_WE)
    ADD_TEST(NAME "pbesinst_${POST_FIX_TEST}_${mcf_name}" COMMAND pbesinst ${ARGS} ${testdir}/${BASENAME_TEST}_${mcf_name}.pbes ${testdir}/dummy.${EXT} )
    set_tests_properties("pbesinst_${POST_FIX_TEST}_${mcf_name}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
    set_tests_properties("pbesinst_${POST_FIX_TEST}_${mcf_name}" PROPERTIES DEPENDS "lps2pbes_${BASENAME_TEST}-ARGS-f${mcf_name}" )
  endforeach()
endmacro( add_pbesinst_release_test ARGS )

macro( gen_pbesinst_release_tests )
  add_pbesinst_release_test(  "" )
  add_pbesinst_release_test(  "-opbes" )
  add_pbesinst_release_test(  "-obes" )
  add_pbesinst_release_test(  "-ocwi" )
  add_pbesinst_release_test(  "-slazy" )
  add_pbesinst_release_test(  "-sfinite;-f\"*(*:Bool)\"" )
  add_pbesinst_release_test(  "-rjitty" )
  add_pbesinst_release_test(  "-rjittyp" )
  if( NOT WIN32 )
    add_pbesinst_release_test(  "-rjittyc" )
  endif( NOT WIN32 )
endmacro( gen_pbesinst_release_tests )

###################
## Macro pbespareqelm  ##
###################

macro( add_pbespareqelm_release_test ARGS )
	set( TRIMMED_ARGS "" )

  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${BASENAME_TEST}-ARGS${TRIMMED_ARGS}" )

	foreach(MCF ${SET_OF_MCF} )
    get_filename_component( mcf_name ${MCF} NAME_WE)
    ADD_TEST(NAME "pbespareqelm_${POST_FIX_TEST}_${mcf_name}" COMMAND pbespareqelm ${ARGS} ${testdir}/${BASENAME_TEST}_${mcf_name}.pbes ${testdir}/dummy.pbes )
	  set_tests_properties("pbespareqelm_${POST_FIX_TEST}_${mcf_name}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
	  set_tests_properties("pbespareqelm_${POST_FIX_TEST}_${mcf_name}" PROPERTIES DEPENDS "lps2pbes_${BASENAME_TEST}-ARGS-f${mcf_name}" )
	endforeach()

endmacro( add_pbespareqelm_release_test ARGS )

macro( gen_pbespareqelm_release_tests )
					add_pbespareqelm_release_test(  "" )
					add_pbespareqelm_release_test(  "-i" )
					add_pbespareqelm_release_test(  "-psimplify" )
					add_pbespareqelm_release_test(  "-pquantifier-all" )
					add_pbespareqelm_release_test(  "-pquantifier-finite" )
					add_pbespareqelm_release_test(  "-ppfnf" )
					add_pbespareqelm_release_test(  "-rjitty" )
					add_pbespareqelm_release_test(  "-rjittyp" )
          if( NOT WIN32 )
					  add_pbespareqelm_release_test(  "-rjittyc" )
          endif( NOT WIN32 )
endmacro( gen_pbespareqelm_release_tests )

###################
## Macro pbespgsolve  ##
###################

macro( add_pbespgsolve_release_test ARGS )
	set( TRIMMED_ARGS "" )

  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${BASENAME_TEST}-ARGS${TRIMMED_ARGS}" )

	foreach(MCF ${SET_OF_MCF} )
    get_filename_component( mcf_name ${MCF} NAME_WE)
    ADD_TEST(NAME "pbespgsolve_${POST_FIX_TEST}_${mcf_name}" COMMAND pbespgsolve ${ARGS} ${testdir}/${BASENAME_TEST}_${mcf_name}.pbes )
	  set_tests_properties("pbespgsolve_${POST_FIX_TEST}_${mcf_name}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
	  set_tests_properties("pbespgsolve_${POST_FIX_TEST}_${mcf_name}" PROPERTIES DEPENDS "lps2pbes_${BASENAME_TEST}-ARGS-f${mcf_name}" )
	endforeach()

endmacro( add_pbespgsolve_release_test ARGS )

macro( gen_pbespgsolve_release_tests )
  add_pbespgsolve_release_test(  "" )
  add_pbespgsolve_release_test(  "-c" )
  add_pbespgsolve_release_test(  "-C" )
  add_pbespgsolve_release_test(  "-L" )
  add_pbespgsolve_release_test(  "-e" )
  add_pbespgsolve_release_test(  "-sspm" )
  add_pbespgsolve_release_test(  "-saltspm" )
  add_pbespgsolve_release_test(  "-srecursive" )
  add_pbespgsolve_release_test(  "-rjitty" )
  add_pbespgsolve_release_test(  "-rjittyp" )
  if( NOT WIN32 )
    add_pbespgsolve_release_test(  "-rjittyc" )
  endif( NOT WIN32 )
endmacro( gen_pbespgsolve_release_tests )

###################
## Macro lpsbisim2pbes  ##
###################

macro( add_lpsbisim2pbes_release_test ARGS )
	set( TRIMMED_ARGS "" )

  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${BASENAME_TEST}-ARGS${TRIMMED_ARGS}" )

  ADD_TEST(NAME "lpsbisim2pbes_${POST_FIX_TEST}" COMMAND lpsbisim2pbes ${ARGS} ${testdir}/${BASENAME_TEST}.lps ${testdir}/${BASENAME_TEST}.lps ${testdir}/${BASENAME_TEST}.pbes )
	set_tests_properties("lpsbisim2pbes_${POST_FIX_TEST}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
  set_tests_properties("lpsbisim2pbes_${POST_FIX_TEST}" PROPERTIES DEPENDS "mcrl22lps_${BASENAME_TEST}-ARGS-D" )

endmacro( add_lpsbisim2pbes_release_test ARGS )

macro( gen_lpsbisim2pbes_release_tests )
					add_lpsbisim2pbes_release_test(  "-bstrong-bisim" )
					add_lpsbisim2pbes_release_test(  "-bweak-bisim" )
					add_lpsbisim2pbes_release_test(  "-bbranching-bisim" )
					add_lpsbisim2pbes_release_test(  "-bbranching-sim" )
					add_lpsbisim2pbes_release_test(  "-n;-bstrong-bisim" )
endmacro( gen_lpsbisim2pbes_release_tests )

######################
## Macro txt2bes  ##
######################

macro( add_txt2bes_release_test ARGS)
	set( TRIMMED_ARGS "" )

  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${BASENAME_TEST}-ARGS${TRIMMED_ARGS}" )

	foreach(MCF ${SET_OF_MCF} )
    get_filename_component( mcf_name ${MCF} NAME_WE)

    ADD_TEST(NAME "txt2bes_${POST_FIX_TEST}_${mcf_name}"
						COMMAND txt2bes ${ARGS}
						${testdir}/${BASENAME_TEST}_${mcf_name}_bes.txt
						${testdir}/dummy.pbes )
	  set_tests_properties("txt2bes_${POST_FIX_TEST}_${mcf_name}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
    set_tests_properties("txt2bes_${POST_FIX_TEST}_${mcf_name}" PROPERTIES DEPENDS "bespp_${BASENAME_TEST}-ARGS-fdefault_${mcf_name}" )
	endforeach()
endmacro( add_txt2bes_release_test ARGS)

macro( gen_txt2bes_release_tests )
  add_txt2bes_release_test( "" )
endmacro( gen_txt2bes_release_tests )

########################
## Macro lpsrealelm  ##
########################

macro( add_lpsrealelm_release_test ARGS)
	set( TRIMMED_ARGS "" )

  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${BASENAME_TEST}-ARGS${TRIMMED_ARGS}" )

  ADD_TEST(NAME "lpsrealelm_${POST_FIX_TEST}" COMMAND lpsrealelm ${ARGS} ${testdir}/${BASENAME_TEST}.lps  ${testdir}/dummy.lps )
	set_tests_properties("lpsrealelm_${POST_FIX_TEST}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
  set_tests_properties("lpsrealelm_${POST_FIX_TEST}" PROPERTIES DEPENDS "mcrl22lps_${BASENAME_TEST}-ARGS-D" )
endmacro( add_lpsrealelm_release_test ARGS)

macro( gen_lpsrealelm_release_tests )
  add_lpsrealelm_release_test( "")
  add_lpsrealelm_release_test( "--max=10" )
  add_lpsrealelm_release_test( "-rjitty" )
  add_lpsrealelm_release_test( "-rjittyp" )
	if( NOT WIN32 )
    add_lpsrealelm_release_test(  "-rjittyc" )
	endif( NOT WIN32 )
endmacro( gen_lpsrealelm_release_tests )

##############################
## tool testcase generation ##
##############################

message( STATUS  "Preparing release tool tests" )
# Set lts different lts output formats
set(LTS_EXTS "lts;aut;fsm" )
if(${MCRL2_ENABLE_CADP_SUPPORT})
  set(LTS_EXTS "${LTS_EXTS};bcg")
endif(${MCRL2_ENABLE_CADP_SUPPORT})

# Set location of mcf formula's
set(SET_OF_MCF "${CMAKE_SOURCE_DIR}/examples/modal-formulas/nodeadlock.mcf;${CMAKE_SOURCE_DIR}/examples/modal-formulas/nolivelock.mcf")

set(testdir "${CMAKE_BINARY_DIR}/mcrl2-testoutput")

# input files for lpsconfcheck and lpsinvelm
write_file(${testdir}/true.txt "true" )
write_file(${testdir}/false.txt "false" )

function( run_release_tests SET_OF_MCRL2_FILES SET_OF_DISABLED_TESTS )
FOREACH( i ${SET_OF_MCRL2_FILES} )
	message( STATUS  "+ Generating tool release tests for: ${i}" )

	get_filename_component( BASENAME_TEST ${i} NAME_WE )

	# message( STATUS  "+ Generating rename file: ${BASENAME_TEST}_rename.txt" )

	 file(STRINGS ${i} output NEWLINE_CONSUME )
	 #message( ${output} )

   # remove comment from files
	 string(REGEX REPLACE "%[^\n]*" "" output "${output}" )
	 string(REGEX REPLACE "\n" " " output "${output}" )


  	# Take line that matches "act" keyword
    string(REGEX MATCH "act[ ]+([^\;]+)" output "${output}" )

		# If actions are declared create rename files for lpsactionrename and actions for hiding
		if( NOT "${output}" STREQUAL "" )

  	# Take first action
    string(REGEX REPLACE "act +(.*)" "\\1" fst_act "${output}")

		  #Meanwile, write line of the first declared actions to file.
	    #This action is used for hiding (e.g. ltscompare)
      string(REGEX MATCH "^[^:\;]+" fst_line_act "${fst_act}")
			#Remove spacing
      string(REGEX REPLACE " " "" fst_line_act "${fst_line_act}")
		  #Write line of the first declared actions to file.
	    #This action is used for hiding (e.g. ltscompare,ltsconvert)
		  write_file(${testdir}/${BASENAME_TEST}_hide_action.txt "${fst_line_act}")

		# And now take the first action
    string(REGEX MATCH "^[^,:\;]+" fst_act "${fst_act}")
    string(REGEX REPLACE " " "" fst_act "${fst_act}")
  	# Find corresponding sorts
  	if( "${output}" MATCHES ":" )

    string(REGEX REPLACE ".*:(.*)" "\\1" fst_act_sorts "${output}")

  	#Create a list: replace # by ; and trim spaces
  	string(REGEX REPLACE "#" ";" fst_act_sorts "${fst_act_sorts}")
  	string(REGEX REPLACE " " "" fst_act_sorts "${fst_act_sorts}")
    #Convert string to list
  	set(fst_act_sorts ${fst_act_sorts} )

  	set(lpsactionrename_vardecl "")
  	set(lpsactionrename_varlist "")
  	foreach(j ${fst_act_sorts}  )
      set(lpsactionrename_vardecl "${lpsactionrename_vardecl}\n  v${cnt}: ${j};" )
  		set(lpsactionrename_varlist "${lpsactionrename_varlist},v${cnt}")
      set(cnt "${cnt}'")
    endforeach()
		if( NOT "${lpsactionrename_vardecl}" STREQUAL "" )
     set(lpsactionrename_vardecl "\nvar${lpsactionrename_vardecl}")
		endif( NOT "${lpsactionrename_vardecl}" STREQUAL "" )

    string( REGEX REPLACE "^," "(" lpsactionrename_varlist ${lpsactionrename_varlist} )
  	set( lpsactionrename_varlist "${lpsactionrename_varlist})" )
  	#    message( ${lpsactionrename_varlist} )
    else( "${output}" MATCHES ":" )

  	endif( "${output}" MATCHES ":" )

  	# Generate random post_fix
  	string(RANDOM
  					 LENGTH 4
  					 ALPHABET abcdefghijklmnopgrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWZ
  					 lpsactionrename_postfix )

    write_file(${testdir}/${BASENAME_TEST}_rename.txt "act ${fst_act}${lpsactionrename_postfix};${lpsactionrename_vardecl}\nrename\n  ${fst_act}${lpsactionrename_varlist} => ${fst_act}${lpsactionrename_postfix};" )
		endif( NOT "${output}" STREQUAL "" )

		#Copy files from examples directory to testdir (Required for "lts2lps" -m argument)
		configure_file(${i} ${testdir}/${BASENAME_TEST}.mcrl2 COPYONLY)


	list(FIND SET_OF_DISABLED_TESTS "mcrl22lps" index_find)
  if( index_find LESS 0 )
	  gen_mcrl22lps_release_tests( )
	endif()

	list(FIND SET_OF_DISABLED_TESTS "lps2lts" index_find)
  if( index_find LESS 0 )
    gen_lps2lts_release_tests()
	endif()

	list(FIND SET_OF_DISABLED_TESTS "lpsrewr" index_find)
  if( index_find LESS 0 )
    gen_lpsrewr_release_tests()
	endif()

	list(FIND SET_OF_DISABLED_TESTS "lpsinfo" index_find)
  if( index_find LESS 0 )
    gen_lpsinfo_release_tests()
	endif()

	list(FIND SET_OF_DISABLED_TESTS "lpspp" index_find)
  if( index_find LESS 0 )
    gen_lpspp_release_tests()
	endif()

	list(FIND SET_OF_DISABLED_TESTS "lpsconstelm" index_find)
  if( index_find LESS 0 )
    gen_lpsconstelm_release_tests()
	endif()

	list(FIND SET_OF_DISABLED_TESTS "lpsparelm" index_find)
  if( index_find LESS 0 )
    gen_lpsparelm_release_tests()
	endif()

	list(FIND SET_OF_DISABLED_TESTS "lpssumelm" index_find)
  if( index_find LESS 0 )
    gen_lpssumelm_release_tests()
	endif()

	list(FIND SET_OF_DISABLED_TESTS "lpsactionrename" index_find)
  if( index_find LESS 0 )
    gen_lpsactionrename_release_tests()
	endif()

	list(FIND SET_OF_DISABLED_TESTS "lpsuntime" index_find)
  if( index_find LESS 0 )
    gen_lpsuntime_release_tests()
	endif()

	list(FIND SET_OF_DISABLED_TESTS "lpsinvelm" index_find)
  if( index_find LESS 0 )
    gen_lpsinvelm_release_tests()
	endif()

	list(FIND SET_OF_DISABLED_TESTS "lpsconfcheck" index_find)
  if( index_find LESS 0 )
    gen_lpsconfcheck_release_tests()
	endif()

	list(FIND SET_OF_DISABLED_TESTS "lpsbinary" index_find)
  if( index_find LESS 0 )
    gen_lpsbinary_release_tests()
	endif()

	list(FIND SET_OF_DISABLED_TESTS "lpsparunfold" index_find)
  if( index_find LESS 0 )
      gen_lpsparunfold_release_tests()
	endif()

	list(FIND SET_OF_DISABLED_TESTS "lpssuminst" index_find)
  if( index_find LESS 0 )
	  gen_lpssuminst_release_tests()
	endif()

	list(FIND SET_OF_DISABLED_TESTS "ltsinfo" index_find)
  if( index_find LESS 0 )
    gen_ltsinfo_release_tests()
	endif()

	list(FIND SET_OF_DISABLED_TESTS "lts2lps" index_find)
  if( index_find LESS 0 )
    gen_lts2lps_release_tests()
	endif()

	list(FIND SET_OF_DISABLED_TESTS "ltsconvert" index_find)
  if( index_find LESS 0 )
    gen_ltsconvert_release_tests()
	endif()

	list(FIND SET_OF_DISABLED_TESTS "ltscompare" index_find)
  if( index_find LESS 0 )
    gen_ltscompare_release_tests()
	endif()

	list(FIND SET_OF_DISABLED_TESTS "lps2pbes" index_find)
  if( index_find LESS 0 )
    gen_lps2pbes_release_tests()
	endif()

	list(FIND SET_OF_DISABLED_TESTS "pbesinfo" index_find)
  if( index_find LESS 0 )
    gen_pbesinfo_release_tests()
	endif()

	list(FIND SET_OF_DISABLED_TESTS "pbespp" index_find)
  if( index_find LESS 0 )
    gen_pbespp_release_tests()
	endif()

	list(FIND SET_OF_DISABLED_TESTS "pbesconstelm" index_find)
  if( index_find LESS 0 )
    gen_pbesconstelm_release_tests()
	endif()

	list(FIND SET_OF_DISABLED_TESTS "pbesparelm" index_find)
  if( index_find LESS 0 )
    gen_pbesparelm_release_tests()
	endif()

	list(FIND SET_OF_DISABLED_TESTS "pbesrewr" index_find)
  if( index_find LESS 0 )
    gen_pbesrewr_release_tests()
	endif()
	list(FIND SET_OF_DISABLED_TESTS "pbes2bool" index_find)
  if( index_find LESS 0 )
    gen_pbes2bool_release_tests()
	endif()

	list(FIND SET_OF_DISABLED_TESTS "txt2lps" index_find)
  if( index_find LESS 0 )
    gen_txt2lps_release_tests()
	endif()

	list(FIND SET_OF_DISABLED_TESTS "txt2pbes" index_find)
  if( index_find LESS 0 )
    gen_txt2pbes_release_tests()
	endif()

	list(FIND SET_OF_DISABLED_TESTS "pbes2bes" index_find)
  if( index_find LESS 0 )
    gen_pbes2bes_release_tests()
	endif()

	list(FIND SET_OF_DISABLED_TESTS "besinfo" index_find)
  if( index_find LESS 0 )
    gen_besinfo_release_tests()
	endif()

	list(FIND SET_OF_DISABLED_TESTS "bespp" index_find)
  if( index_find LESS 0 )
    gen_bespp_release_tests()
	endif()

  list(FIND SET_OF_DISABLED_TESTS "pbespgsolve" index_find)
    if( index_find LESS 0 )
      gen_pbespgsolve_release_tests()
  endif()

  list(FIND SET_OF_DISABLED_TESTS "lts2pbes" index_find)
    if( index_find LESS 0 )
      gen_lts2pbes_release_tests()
  endif()

  ##
  ## Experimental tests
  ##
  if(MCRL2_ENABLE_EXPERIMENTAL)
	  list(FIND SET_OF_DISABLED_TESTS "lpsrealelm" index_find)
    if( index_find LESS 0 )
      gen_lpsrealelm_release_tests()
	  endif()

	  list(FIND SET_OF_DISABLED_TESTS "besconvert" index_find)
    if( index_find LESS 0 )
      gen_besconvert_release_tests()
	  endif()

		list(FIND SET_OF_DISABLED_TESTS "bessolve" index_find)
    if( index_find LESS 0 )
      gen_bessolve_release_tests()
	  endif()

		list(FIND SET_OF_DISABLED_TESTS "pbesabstract" index_find)
    if( index_find LESS 0 )
      gen_pbesabstract_release_tests()
	  endif()

		list(FIND SET_OF_DISABLED_TESTS "pbesinst" index_find)
    if( index_find LESS 0 )
      gen_pbesinst_release_tests()
	  endif()

		list(FIND SET_OF_DISABLED_TESTS "pbespareqelm" index_find)
    if( index_find LESS 0 )
      gen_pbespareqelm_release_tests()
	  endif()


		list(FIND SET_OF_DISABLED_TESTS "lpsbisim2pbes" index_find)
    if( index_find LESS 0 )
      gen_lpsbisim2pbes_release_tests()
	  endif()

	  list(FIND SET_OF_DISABLED_TESTS "txt2bes" index_find)
    if( index_find LESS 0 )
      gen_txt2bes_release_tests()
	  endif()

  endif(MCRL2_ENABLE_EXPERIMENTAL)
ENDFOREACH()

endfunction()
