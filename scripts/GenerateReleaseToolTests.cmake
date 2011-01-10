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

function( run_release_tests SET_OF_MCRL2_FILES )
set(testdir "${CMAKE_SOURCE_DIR}/mcrl2-testoutput")

# Set lts different lts output formats
set(LTS_EXTS "lts;aut;svc;dot;fsm;bcg" )
# Set location of mcf formula's
set(SET_OF_MCF "${CMAKE_SOURCE_DIR}/examples/modal-formulas/nodeadlock.mcf;${CMAKE_SOURCE_DIR}/examples/modal-formulas/nolivelock.mcf")

set(SET_OF_LPS_FILES "" )
set(SET_OF_LTS_FILES "" )
set(SET_OF_LTSCONVERT_TESTS "" )
set(SET_OF_TXTLPS_FILES "" )
set(SET_OF_TXTPBES_FILES "" )

##################### 
## Macro mcrl22lps ## 
#####################

macro( add_mcrl22lps_release_test INPUT ARGS OUTPUTFILE_WITHOUT_EXTENSION)
	set( TRIMMED_ARGS "" )		

	get_filename_component( POST_FIX_TEST ${INPUT} NAME_WE )

  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${POST_FIX_TEST}-ARGS${TRIMMED_ARGS}" )

	if( NOT ${OUTPUTFILE_WITHOUT_EXTENSION} )
	  set(SET_OF_LPS_FILES "${SET_OF_LPS_FILES};${OUTPUTFILE_WITHOUT_EXTENSION}.lps"	)
    set(SET_OF_MCRL22LPS_TESTS "mcrl22lps_${POST_FIX_TEST};${SET_OF_MCRL22LPS_TESTS}" )
    ADD_TEST("mcrl22lps_${POST_FIX_TEST}" ${mcrl22lps_BINARY_DIR}/mcrl22lps ${ARGS} ${INPUT} ${testdir}/${OUTPUTFILE_WITHOUT_EXTENSION}.lps)
	else( NOT ${OUTPUTFILE_WITHOUT_EXTENSION} )
    ADD_TEST("mcrl22lps_${POST_FIX_TEST}" ${mcrl22lps_BINARY_DIR}/mcrl22lps ${ARGS} ${INPUT} ${testdir}/dummy.lps)
  endif( NOT ${OUTPUTFILE_WITHOUT_EXTENSION} )

	set_tests_properties("mcrl22lps_${POST_FIX_TEST}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
endmacro( add_mcrl22lps_release_test INPUT ARGS)

macro( gen_mcrl22lps_release_tests )
  FOREACH( i ${SET_OF_MCRL2_FILES} )

	get_filename_component(save ${i} NAME_WE )
	# message( ${save} )

  add_mcrl22lps_release_test( "${i}" "-a" "")
  add_mcrl22lps_release_test( "${i}" "-b" "")
  add_mcrl22lps_release_test( "${i}" "-c" "")
  add_mcrl22lps_release_test( "${i}" "-D" "${save}")
  add_mcrl22lps_release_test( "${i}" "-e" "")
  add_mcrl22lps_release_test( "${i}" "-f" "")
  add_mcrl22lps_release_test( "${i}" "-g" "")
  add_mcrl22lps_release_test( "${i}" "-lregular" "")
  add_mcrl22lps_release_test( "${i}" "-lregular2" "")
  add_mcrl22lps_release_test( "${i}" "-lstack" "")
  add_mcrl22lps_release_test( "${i}" "-m" "")
  add_mcrl22lps_release_test( "${i}" "-n" "")
  add_mcrl22lps_release_test( "${i}" "--no-constelm" "")
  add_mcrl22lps_release_test( "${i}" "-o" "")
  add_mcrl22lps_release_test( "${i}" "-rjitty" "")
  add_mcrl22lps_release_test( "${i}" "-rjittyp" "")
  add_mcrl22lps_release_test( "${i}" "-rinner" "")
  add_mcrl22lps_release_test( "${i}" "-rinnerp" "")
	if( NOT WIN32 )
    add_mcrl22lps_release_test( "${i}" "-rjittyc" "")
    add_mcrl22lps_release_test( "${i}" "-rinnerc" "")
	endif( NOT WIN32 )
  add_mcrl22lps_release_test( "${i}" "--timings" "")
  add_mcrl22lps_release_test( "${i}" "-w" "")
  add_mcrl22lps_release_test( "${i}" "-z" "")
  ENDFOREACH( )
endmacro( gen_mcrl22lps_release_tests )

####################
## Macro lpsinfo  ##
####################

macro( add_lpsinfo_release_test INPUT ARGS)
	set( TRIMMED_ARGS "" )			
	get_filename_component( POST_FIX_TEST ${INPUT} NAME_WE )
		
  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${POST_FIX_TEST}-ARGS${TRIMMED_ARGS}" )

  ADD_TEST("lpsinfo_${POST_FIX_TEST}" ${lpsinfo_BINARY_DIR}/lpsinfo ${ARGS} ${testdir}/${INPUT}  )
	set_tests_properties("lpsinfo_${POST_FIX_TEST}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
  foreach( i ${SET_OF_MCRL22LPS_TESTS} )
  	set_tests_properties("lpsinfo_${POST_FIX_TEST}" PROPERTIES DEPENDS "${i}" )
	endforeach()	
endmacro( add_lpsinfo_release_test INPUT ARGS)

macro( gen_lpsinfo_release_tests )
  FOREACH( i ${SET_OF_LPS_FILES} )
					#					message( "${i}" )
					add_lpsinfo_release_test( "${i}" "" )
  ENDFOREACH( )
endmacro( gen_lpsinfo_release_tests )

##################
## Macro lpspp  ##
##################

macro( add_lpspp_release_test INPUT ARGS OUTPUTFILE_WITHOUT_EXTENSION)
	set( TRIMMED_ARGS "" )			
	get_filename_component( POST_FIX_TEST ${INPUT} NAME_WE )
		
  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${POST_FIX_TEST}-ARGS${TRIMMED_ARGS}" )

	if( NOT ${OUTPUTFILE_WITHOUT_EXTENSION} )
    ADD_TEST("lpspp_${POST_FIX_TEST}" ${lpspp_BINARY_DIR}/lpspp ${ARGS} ${testdir}/${INPUT} ${testdir}/${OUTPUTFILE_WITHOUT_EXTENSION}.txt )
	  set(SET_OF_TXTLPS_FILES "${SET_OF_TXTLPS_FILES};${OUTPUTFILE_WITHOUT_EXTENSION}.txt")
    set(SET_OF_LPSPP_TESTS "${SET_OF_LPSPP_TESTS};lpspp_${POST_FIX_TEST}")
	else( NOT ${OUTPUTFILE_WITHOUT_EXTENSION} )
    ADD_TEST("lpspp_${POST_FIX_TEST}" ${lpspp_BINARY_DIR}/lpspp ${ARGS} ${testdir}/${INPUT} )
	endif( NOT ${OUTPUTFILE_WITHOUT_EXTENSION} )

	set_tests_properties("lpspp_${POST_FIX_TEST}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
  foreach( i ${SET_OF_MCRL22LPS_TESTS} )
    set_tests_properties("lpspp_${POST_FIX_TEST}" PROPERTIES DEPENDS "${i}" )
	endforeach()	
  
endmacro( add_lpspp_release_test INPUT ARGS)

macro( gen_lpspp_release_tests )
  FOREACH( i ${SET_OF_LPS_FILES} )
					add_lpspp_release_test( "${i}" "-fdefault" "${i}")
					add_lpspp_release_test( "${i}" "-fdebug" "")
					add_lpspp_release_test( "${i}" "-finternal" "")
					add_lpspp_release_test( "${i}" "-finternal-debug" "")
  ENDFOREACH( )
endmacro( gen_lpspp_release_tests )

################### 
## Macro lps2lts ## 
###################

macro( add_lps2lts_release_test INPUT ARGS OUTPUTFILE_WITHOUT_EXTENSION)
	set( TRIMMED_ARGS "" )			
	get_filename_component( POST_FIX_TEST ${INPUT} NAME_WE )
		
  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${POST_FIX_TEST}-ARGS${TRIMMED_ARGS}" )

	foreach(EXT ${LTS_EXTS} )
		if( NOT ${OUTPUTFILE_WITHOUT_EXTENSION} )
						#						message( "${OUTPUTFILE_WITHOUT_EXTENSION}.${EXT}" )
      ADD_TEST("lps2lts_${POST_FIX_TEST}_${EXT}" ${lps2lts_BINARY_DIR}/lps2lts ${ARGS} ${testdir}/${INPUT} ${testdir}/${OUTPUTFILE_WITHOUT_EXTENSION}.${EXT} )
			set(SET_OF_LTS_FILES "${SET_OF_LTS_FILES};${OUTPUTFILE_WITHOUT_EXTENSION}.${EXT}"	)
      set(SET_OF_LPS2LTS_TESTS "lps2lts_${POST_FIX_TEST}_${EXT};${SET_OF_LPS2LTS_TESTS}" )
		else( NOT ${OUTPUTFILE_WITHOUT_EXTENSION} )
      ADD_TEST("lps2lts_${POST_FIX_TEST}_${EXT}" ${lps2lts_BINARY_DIR}/lps2lts ${ARGS} ${testdir}/${INPUT} ${testdir}/dummy.aut )
		endif( NOT ${OUTPUTFILE_WITHOUT_EXTENSION} )

		set_tests_properties("lps2lts_${POST_FIX_TEST}_${EXT}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
    foreach( i ${SET_OF_MCRL22LPS_TESTS})
      set_tests_properties("lps2lts_${POST_FIX_TEST}_${EXT}" PROPERTIES DEPENDS "${i}" )
		endforeach()

	endforeach(EXT ${LTS_EXTS})				
endmacro( add_lps2lts_release_test INPUT ARGS OUTPUTFILE_WITHOUT_EXTENSION)

macro( gen_lps2lts_release_tests )
  FOREACH( i ${SET_OF_LPS_FILES} )

					get_filename_component( save ${i} NAME_WE)

					add_lps2lts_release_test( "${i}" "" "${save}" )
					add_lps2lts_release_test( "${i}" "-atau" "")
					add_lps2lts_release_test( "${i}" "-b10" "")
					add_lps2lts_release_test( "${i}" "-ctau" "")
					add_lps2lts_release_test( "${i}" "-D" "")
					add_lps2lts_release_test( "${i}" "--error-trace" "")
					add_lps2lts_release_test( "${i}" "-ftree" "")
					add_lps2lts_release_test( "${i}" "--init-tsize=10" "")
					add_lps2lts_release_test( "${i}" "-l10" "")
					add_lps2lts_release_test( "${i}" "--no-info" "")
					add_lps2lts_release_test( "${i}" "-rjitty" "")
					add_lps2lts_release_test( "${i}" "-rjittyp" "")
					add_lps2lts_release_test( "${i}" "-rinner" "")
					if( NOT WIN32)
  					add_lps2lts_release_test( "${i}" "-rjittyc" "")
	  				add_lps2lts_release_test( "${i}" "-rinnerc" "")
					endif( NOT WIN32)
					add_lps2lts_release_test( "${i}" "-rinnerp" "")
					add_lps2lts_release_test( "${i}" "-sd" "")
					add_lps2lts_release_test( "${i}" "-sb" "")
					add_lps2lts_release_test( "${i}" "-sp" "")
					add_lps2lts_release_test( "${i}" "-sq;-l100" "" )
					add_lps2lts_release_test( "${i}" "-sr;-l100" "")
					add_lps2lts_release_test( "${i}" "--verbose;--suppress" "")
					add_lps2lts_release_test( "${i}" "--todo-max=10" "")
					add_lps2lts_release_test( "${i}" "-u" "")
					add_lps2lts_release_test( "${i}" "-yno" "")
  ENDFOREACH( )
endmacro( gen_lps2lts_release_tests )

########################
## Macro lpsconstelm  ##
########################

macro( add_lpsconstelm_release_test INPUT ARGS)
	set( TRIMMED_ARGS "" )			
	get_filename_component( POST_FIX_TEST ${INPUT} NAME_WE )
		
  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${POST_FIX_TEST}-ARGS${TRIMMED_ARGS}" )

  ADD_TEST("lpsconstelm_${POST_FIX_TEST}" ${lpsconstelm_BINARY_DIR}/lpsconstelm ${ARGS} ${testdir}/${INPUT}  ${testdir}/dummy.lps )
	set_tests_properties("lpsconstelm_${POST_FIX_TEST}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
  set_tests_properties("lpsconstelm_${POST_FIX_TEST}" PROPERTIES DEPENDS "${SET_OF_MCRL22LPS_TESTS}" )
endmacro( add_lpsconstelm_release_test INPUT ARGS)

macro( gen_lpsconstelm_release_tests )
  FOREACH( i ${SET_OF_LPS_FILES} )
					add_lpsconstelm_release_test( "${i}" "" )
					add_lpsconstelm_release_test( "${i}" "-c" )
					add_lpsconstelm_release_test( "${i}" "-f" )
					add_lpsconstelm_release_test( "${i}" "-s" )
					add_lpsconstelm_release_test( "${i}" "-t" )
					add_lpsconstelm_release_test( "${i}" "-rjitty" )
					add_lpsconstelm_release_test( "${i}" "-rjittyp" )
					add_lpsconstelm_release_test( "${i}" "-rinner" )
					if( NOT WIN32 )
					  add_lpsconstelm_release_test( "${i}" "-rjittyc" )
					  add_lpsconstelm_release_test( "${i}" "-rinnerc" )
					endif( NOT WIN32 )
					add_lpsconstelm_release_test( "${i}" "-rinnerp" )
	ENDFOREACH( )
endmacro( gen_lpsconstelm_release_tests )

####################
## Macro lpsrewr  ##
####################

macro( add_lpsrewr_release_test INPUT ARGS)
	set( TRIMMED_ARGS "" )			
	get_filename_component( POST_FIX_TEST ${INPUT} NAME_WE )
		
  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${POST_FIX_TEST}-ARGS${TRIMMED_ARGS}" )

  ADD_TEST("lpsrewr_${POST_FIX_TEST}" ${lpsrewr_BINARY_DIR}/lpsrewr ${ARGS} ${testdir}/${INPUT}  ${testdir}/dummy.lps )
	set_tests_properties("lpsrewr_${POST_FIX_TEST}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
  set_tests_properties("lpsrewr_${POST_FIX_TEST}" PROPERTIES DEPENDS "${SET_OF_MCRL22LPS_TESTS}" )
endmacro( add_lpsrewr_release_test INPUT ARGS)

macro( gen_lpsrewr_release_tests )
  FOREACH( i ${SET_OF_LPS_FILES} )
					add_lpsrewr_release_test( "${i}" "-rjitty" )
					add_lpsrewr_release_test( "${i}" "-rjittyp" )
					add_lpsrewr_release_test( "${i}" "-rinner" )
					if( NOT WIN32 )
  					add_lpsrewr_release_test( "${i}" "-rjittyc" )
	  				add_lpsrewr_release_test( "${i}" "-rinnerc" )
					endif( NOT WIN32)
					add_lpsrewr_release_test( "${i}" "-rinnerp" )
	ENDFOREACH( )
endmacro( gen_lpsrewr_release_tests )

########################
## Macro lpsparelm  ##
########################

macro( add_lpsparelm_release_test INPUT ARGS)
	set( TRIMMED_ARGS "" )			
	get_filename_component( POST_FIX_TEST ${INPUT} NAME_WE )
		
  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${POST_FIX_TEST}-ARGS${TRIMMED_ARGS}" )

  ADD_TEST("lpsparelm_${POST_FIX_TEST}" ${lpsparelm_BINARY_DIR}/lpsparelm ${ARGS} ${testdir}/${INPUT}  ${testdir}/dummy.lps )
	set_tests_properties("lpsparelm_${POST_FIX_TEST}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
  set_tests_properties("lpsparelm_${POST_FIX_TEST}" PROPERTIES DEPENDS "${SET_OF_MCRL22LPS_TESTS}" )
endmacro( add_lpsparelm_release_test INPUT ARGS)

macro( gen_lpsparelm_release_tests )
  FOREACH( i ${SET_OF_LPS_FILES} )
					add_lpsparelm_release_test( "${i}" "" )
	ENDFOREACH( )
endmacro( gen_lpsparelm_release_tests )

######################
## Macro lpssumelm  ##
######################

macro( add_lpssumelm_release_test INPUT ARGS)
	set( TRIMMED_ARGS "" )			
	get_filename_component( POST_FIX_TEST ${INPUT} NAME_WE )
		
  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${POST_FIX_TEST}-ARGS${TRIMMED_ARGS}" )

  ADD_TEST("lpssumelm_${POST_FIX_TEST}" ${lpssumelm_BINARY_DIR}/lpssumelm ${ARGS} ${testdir}/${INPUT}  ${testdir}/dummy.lps )
	set_tests_properties("lpssumelm_${POST_FIX_TEST}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
  set_tests_properties("lpssumelm_${POST_FIX_TEST}" PROPERTIES DEPENDS "${SET_OF_MCRL22LPS_TESTS}" )
endmacro( add_lpssumelm_release_test INPUT ARGS)

macro( gen_lpssumelm_release_tests )
  FOREACH( i ${SET_OF_LPS_FILES} )
					add_lpssumelm_release_test( "${i}" "" )
	ENDFOREACH( )
endmacro( gen_lpssumelm_release_tests )

############################
## Macro lpsactionrename  ##
############################

macro( add_lpsactionrename_release_test INPUT ARGS)
	set( TRIMMED_ARGS "" )			
	get_filename_component( POST_FIX_TEST ${INPUT} NAME_WE )
	
	FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${POST_FIX_TEST}-ARGS${TRIMMED_ARGS}" )

  ADD_TEST("lpsactionrename_${POST_FIX_TEST}" ${lpsactionrename_BINARY_DIR}/lpsactionrename ${ARGS} ${testdir}/${INPUT}  ${testdir}/dummy.lps )
	set_tests_properties("lpsactionrename_${POST_FIX_TEST}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
  set_tests_properties("lpsactionrename_${POST_FIX_TEST}" PROPERTIES DEPENDS "${SET_OF_MCRL22LPS_TESTS}" )
endmacro( add_lpsactionrename_release_test INPUT ARGS)
 
write_file(${testdir}/rename.txt "rename tau => delta;" )

macro( gen_lpsactionrename_release_tests )
  FOREACH( i ${SET_OF_LPS_FILES} )
					add_lpsactionrename_release_test( "${i}" "-f${testdir}/rename.txt" )
					add_lpsactionrename_release_test( "${i}" "-f${testdir}/rename.txt;-m" )
					add_lpsactionrename_release_test( "${i}" "-f${testdir}/rename.txt;-o" )
					add_lpsactionrename_release_test( "${i}" "-f${testdir}/rename.txt;-ppa" )
					add_lpsactionrename_release_test( "${i}" "-f${testdir}/rename.txt;-ptc" )
					add_lpsactionrename_release_test( "${i}" "-f${testdir}/rename.txt;-pdi" )
					add_lpsactionrename_release_test( "${i}" "-f${testdir}/rename.txt;-P" )
					add_lpsactionrename_release_test( "${i}" "-f${testdir}/rename.txt;-rjitty" )
					add_lpsactionrename_release_test( "${i}" "-f${testdir}/rename.txt;-rjittyp" )
					add_lpsactionrename_release_test( "${i}" "-f${testdir}/rename.txt;-rinner" )
					if( NOT WIN32)
					  add_lpsactionrename_release_test( "${i}" "-f${testdir}/rename.txt;-rjittyc" )
					  add_lpsactionrename_release_test( "${i}" "-f${testdir}/rename.txt;-rinnerc" )
					endif( NOT WIN32)
					add_lpsactionrename_release_test( "${i}" "-f${testdir}/rename.txt;-rinnerp" )
	ENDFOREACH( )
endmacro( gen_lpsactionrename_release_tests )

######################
## Macro lpsuntime  ##
######################

macro( add_lpsuntime_release_test INPUT ARGS)
	set( TRIMMED_ARGS "" )			
	get_filename_component( POST_FIX_TEST ${INPUT} NAME_WE )

  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${POST_FIX_TEST}-ARGS${TRIMMED_ARGS}" )

  ADD_TEST("lpsuntime_${POST_FIX_TEST}" ${lpsuntime_BINARY_DIR}/lpsuntime ${ARGS} ${testdir}/${INPUT}  ${testdir}/dummy.lps )
	set_tests_properties("lpsuntime_${POST_FIX_TEST}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
  set_tests_properties("lpsuntime_${POST_FIX_TEST}" PROPERTIES DEPENDS "${SET_OF_MCRL22LPS_TESTS}" )
endmacro( add_lpsuntime_release_test INPUT ARGS)
 
macro( gen_lpsuntime_release_tests )
  FOREACH( i ${SET_OF_LPS_FILES} )
					add_lpsuntime_release_test( "${i}" "" )
	ENDFOREACH( )
endmacro( gen_lpsuntime_release_tests )

######################
## Macro lpsinvelm  ##
######################

macro( add_lpsinvelm_release_test INPUT ARGS)
	set( TRIMMED_ARGS "" )			
	get_filename_component( POST_FIX_TEST ${INPUT} NAME_WE )

  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${POST_FIX_TEST}-ARGS${TRIMMED_ARGS}" )

  write_file(${testdir}/true.txt "true" )
  write_file(${testdir}/false.txt "false" )

  ADD_TEST("lpsinvelm_${POST_FIX_TEST}" ${lpsinvelm_BINARY_DIR}/lpsinvelm ${ARGS} ${testdir}/${INPUT}  ${testdir}/dummy.lps )
	set_tests_properties("lpsinvelm_${POST_FIX_TEST}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
  set_tests_properties("lpsinvelm_${POST_FIX_TEST}" PROPERTIES DEPENDS "${SET_OF_MCRL22LPS_TESTS}" )
endmacro( add_lpsinvelm_release_test INPUT ARGS)
 
macro( gen_lpsinvelm_release_tests )
  FOREACH( i ${SET_OF_LPS_FILES} )
					add_lpsinvelm_release_test( "${i}" "-i${testdir}/true.txt" )
					add_lpsinvelm_release_test( "${i}" "-c;-i${testdir}/true.txt" )
					add_lpsinvelm_release_test( "${i}" "-e;-i${testdir}/true.txt" )
					add_lpsinvelm_release_test( "${i}" "-l;-i${testdir}/true.txt" )
					add_lpsinvelm_release_test( "${i}" "-n;-i${testdir}/true.txt" )
					add_lpsinvelm_release_test( "${i}" "-p${testdir}/DOTFILE.dot;-i${testdir}/true.txt" )
					add_lpsinvelm_release_test( "${i}" "-rjitty;-i${testdir}/true.txt" )
					add_lpsinvelm_release_test( "${i}" "-rjittyp;-i${testdir}/true.txt" )
					add_lpsinvelm_release_test( "${i}" "-rinner;-i${testdir}/true.txt" )
					if( NOT WIN32 )
					  add_lpsinvelm_release_test( "${i}" "-rjittyc;-i${testdir}/true.txt" )
					  add_lpsinvelm_release_test( "${i}" "-rinnerc;-i${testdir}/true.txt" )
					endif( NOT WIN32 )
					add_lpsinvelm_release_test( "${i}" "-rinnerp;-i${testdir}/true.txt" )
					add_lpsinvelm_release_test( "${i}" "-s10;-i${testdir}/true.txt" )
					add_lpsinvelm_release_test( "${i}" "-t10;-i${testdir}/true.txt" )
					add_lpsinvelm_release_test( "${i}" "-y;-i${testdir}/false.txt" )
					add_lpsinvelm_release_test( "${i}" "-zario;-i${testdir}/true.txt" )
					add_lpsinvelm_release_test( "${i}" "-zcvc;-i${testdir}/true.txt" )
	ENDFOREACH( )
endmacro( gen_lpsinvelm_release_tests )

#########################
## Macro lpsconfcheck  ##
#########################

macro( add_lpsconfcheck_release_test INPUT ARGS)
	set( TRIMMED_ARGS "" )			
	get_filename_component( POST_FIX_TEST ${INPUT} NAME_WE )

  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${POST_FIX_TEST}-ARGS${TRIMMED_ARGS}" )

	write_file(${testdir}/true.txt "true" )
  write_file(${testdir}/false.txt "false" )

  ADD_TEST("lpsconfcheck_${POST_FIX_TEST}" ${lpsconfcheck_BINARY_DIR}/lpsconfcheck ${ARGS} ${testdir}/${INPUT}  ${testdir}/dummy.lps )
	set_tests_properties("lpsconfcheck_${POST_FIX_TEST}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
  set_tests_properties("lpsconfcheck_${POST_FIX_TEST}" PROPERTIES DEPENDS "${SET_OF_MCRL22LPS_TESTS}" )
endmacro( add_lpsconfcheck_release_test INPUT ARGS)
 
macro( gen_lpsconfcheck_release_tests )
  FOREACH( i ${SET_OF_LPS_FILES} )
					add_lpsconfcheck_release_test( "${i}" "" )
					add_lpsconfcheck_release_test( "${i}" "-a" )
					add_lpsconfcheck_release_test( "${i}" "-c" )
					add_lpsconfcheck_release_test( "${i}" "-g" )
					add_lpsconfcheck_release_test( "${i}" "-i${testdir}/true.txt" )
					add_lpsconfcheck_release_test( "${i}" "-i${testdir}/false.txt" )
					add_lpsconfcheck_release_test( "${i}" "-p${testdir}/DOTFILE.dot;-i${testdir}/true.txt" )
					add_lpsconfcheck_release_test( "${i}" "-m" )
					add_lpsconfcheck_release_test( "${i}" "-n" )
					add_lpsconfcheck_release_test( "${i}" "-o" )
					add_lpsconfcheck_release_test( "${i}" "-rjitty" )
					add_lpsconfcheck_release_test( "${i}" "-rjittyp" )
					add_lpsconfcheck_release_test( "${i}" "-rinner" )
					if( NOT WIN32)
					  add_lpsconfcheck_release_test( "${i}" "-rinnerc" )
					  add_lpsconfcheck_release_test( "${i}" "-rjittyc" )
					endif( NOT WIN32)
					add_lpsconfcheck_release_test( "${i}" "-rinnerp" )
					add_lpsconfcheck_release_test( "${i}" "-s10;-i${testdir}/true.txt" )
					add_lpsconfcheck_release_test( "${i}" "-t10;-i${testdir}/true.txt" )
					add_lpsconfcheck_release_test( "${i}" "-y;-i${testdir}/false.txt" )
					add_lpsconfcheck_release_test( "${i}" "-zario;-i${testdir}/true.txt" )
					add_lpsconfcheck_release_test( "${i}" "-zcvc;-i${testdir}/true.txt" )
	ENDFOREACH( )
endmacro( gen_lpsconfcheck_release_tests )

#########################
## Macro lpsbinary  ##
#########################

macro( add_lpsbinary_release_test INPUT ARGS)
	set( TRIMMED_ARGS "" )			
	get_filename_component( POST_FIX_TEST ${INPUT} NAME_WE )

  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${POST_FIX_TEST}-ARGS${TRIMMED_ARGS}" )

  ADD_TEST("lpsbinary_${POST_FIX_TEST}" ${lpsbinary_BINARY_DIR}/lpsbinary ${ARGS} ${testdir}/${INPUT}  ${testdir}/dummy.lps )
	set_tests_properties("lpsbinary_${POST_FIX_TEST}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
  set_tests_properties("lpsbinary_${POST_FIX_TEST}" PROPERTIES DEPENDS "${SET_OF_MCRL22LPS_TESTS}" )
endmacro( add_lpsbinary_release_test INPUT ARGS)
 
macro( gen_lpsbinary_release_tests )
  FOREACH( i ${SET_OF_LPS_FILES} )
					add_lpsbinary_release_test( "${i}" "" )
					add_lpsbinary_release_test( "${i}" "-rjitty" )
					add_lpsbinary_release_test( "${i}" "-rjittyp" )
					add_lpsbinary_release_test( "${i}" "-rinner" )
          if( NOT WIN32)
         		add_lpsbinary_release_test( "${i}" "-rjittyc" )
	  				add_lpsbinary_release_test( "${i}" "-rinnerc" )
					endif( NOT WIN32)
					add_lpsbinary_release_test( "${i}" "-rinnerp" )
	ENDFOREACH( )
endmacro( gen_lpsbinary_release_tests )

#########################
## Macro lpsparunfold  ##
#########################

macro( add_lpsparunfold_release_test INPUT ARGS)
	set( TRIMMED_ARGS "" )			
	get_filename_component( POST_FIX_TEST ${INPUT} NAME_WE )

  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${POST_FIX_TEST}-ARGS${TRIMMED_ARGS}" )

  ADD_TEST("lpsparunfold_${POST_FIX_TEST}" ${lpsparunfold_BINARY_DIR}/lpsparunfold ${ARGS} ${testdir}/${INPUT}  ${testdir}/dummy.lps )
	set_tests_properties("lpsparunfold_${POST_FIX_TEST}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
  set_tests_properties("lpsparunfold_${POST_FIX_TEST}" PROPERTIES DEPENDS "${SET_OF_MCRL22LPS_TESTS}" )
endmacro( add_lpsparunfold_release_test INPUT ARGS)
 
macro( gen_lpsparunfold_release_tests )
  FOREACH( i ${SET_OF_LPS_FILES} )
					add_lpsparunfold_release_test( "${i}" "-i0" )
					add_lpsparunfold_release_test( "${i}" "-i1" )
					add_lpsparunfold_release_test( "${i}" "-sNat;-l" )
					add_lpsparunfold_release_test( "${i}" "-sNat;-n10" )
					add_lpsparunfold_release_test( "${i}" "-sNat;-rjitty" )
					add_lpsparunfold_release_test( "${i}" "-sNat;-rjittyp" )
					add_lpsparunfold_release_test( "${i}" "-sNat;-rinner" )
					if( NOT WIN32)
					add_lpsparunfold_release_test( "${i}" "-sNat;-rjittyc" )
					add_lpsparunfold_release_test( "${i}" "-sNat;-rinnerc" )
					endif( NOT WIN32)
					add_lpsparunfold_release_test( "${i}" "-sNat;-rinnerp" )
	ENDFOREACH( )
endmacro( gen_lpsparunfold_release_tests )

#########################
## Macro lpssuminst  ##
#########################

macro( add_lpssuminst_release_test INPUT ARGS)
	set( TRIMMED_ARGS "" )			
	get_filename_component( POST_FIX_TEST ${INPUT} NAME_WE )

  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${POST_FIX_TEST}-ARGS${TRIMMED_ARGS}" )

  ADD_TEST("lpssuminst_${POST_FIX_TEST}" ${lpssuminst_BINARY_DIR}/lpssuminst ${ARGS} ${testdir}/${INPUT}  ${testdir}/dummy.lps )
	set_tests_properties("lpssuminst_${POST_FIX_TEST}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
  set_tests_properties("lpssuminst_${POST_FIX_TEST}" PROPERTIES DEPENDS "${SET_OF_MCRL22LPS_TESTS}" )
endmacro( add_lpssuminst_release_test INPUT ARGS)
 
macro( gen_lpssuminst_release_tests )
  FOREACH( i ${SET_OF_LPS_FILES} )
					add_lpssuminst_release_test( "${i}" "-f" )
					add_lpssuminst_release_test( "${i}" "-f;-rjitty" )
					add_lpssuminst_release_test( "${i}" "-f;-rjittyp" )
					add_lpssuminst_release_test( "${i}" "-f;-rinner" )
					if( NOT WIN32)
					  add_lpssuminst_release_test( "${i}" "-f;-rjittyc" )
					  add_lpssuminst_release_test( "${i}" "-f;-rinnerc" )
					endif( NOT WIN32)
					add_lpssuminst_release_test( "${i}" "-f;-rinnerp" )
					add_lpssuminst_release_test( "${i}" "-f;-t" )
	ENDFOREACH( )
endmacro( gen_lpssuminst_release_tests )

####################
## Macro ltsinfo  ##
####################

macro( add_ltsinfo_release_test INPUT ARGS)
	set( TRIMMED_ARGS "" )			
  set( POST_FIX_TEST "${INPUT}" )
		
  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${POST_FIX_TEST}-ARGS${TRIMMED_ARGS}" )

  ADD_TEST("ltsinfo_${POST_FIX_TEST}" ${ltsinfo_BINARY_DIR}/ltsinfo ${ARGS} ${testdir}/${INPUT}  )
	set_tests_properties("ltsinfo_${POST_FIX_TEST}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")

	foreach( i ${SET_OF_LPS2LTS_TESTS} )
	  set_tests_properties("ltsinfo_${POST_FIX_TEST}" PROPERTIES DEPENDS "${i}" )
	endforeach()
endmacro( add_ltsinfo_release_test INPUT ARGS)

macro( gen_ltsinfo_release_tests )
  FOREACH( i ${SET_OF_LTS_FILES} )
					#										message( "${i}" )
					add_ltsinfo_release_test( "${i}" "" )
  ENDFOREACH( )
endmacro( gen_ltsinfo_release_tests )

####################
## Macro lts2lps  ##
####################

macro( add_lts2lps_release_test INPUT ARGS)
	set( TRIMMED_ARGS "" )			
  set( POST_FIX_TEST "${INPUT}" )
		
  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${POST_FIX_TEST}-ARGS${TRIMMED_ARGS}" )


	  ADD_TEST("lts2lps_${POST_FIX_TEST}" ${lts2lps_BINARY_DIR}/lts2lps ${ARGS} ${testdir}/${INPUT} ${testdir}/dummy.lps )
	  set_tests_properties("lts2lps_${POST_FIX_TEST}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
		foreach( i ${SET_OF_LPS2LTS_TESTS} ) 
	    set_tests_properties("lts2lps_${POST_FIX_TEST}" PROPERTIES DEPENDS "${i}" )
    endforeach()
endmacro( add_lts2lps_release_test INPUT ARGS)

macro( gen_lts2lps_release_tests )
  FOREACH( i ${SET_OF_LTS_FILES} )
	  get_filename_component( ext ${i} EXT )
	  get_filename_component( name ${i} NAME_WE )

		# Split on extension of target
	  if( ext STREQUAL ".lts" )
	  				add_lts2lps_release_test( "${i}" "" )
		endif( ext STREQUAL ".lts" )
	  add_lts2lps_release_test( "${i}" "-m${testdir}/${name}.mcrl2" )

  ENDFOREACH( )
endmacro( gen_lts2lps_release_tests )

#######################
## Macro ltsconvert  ##
#######################

macro( add_ltsconvert_release_test INPUT ARGS)
	set( TRIMMED_ARGS "" )			
  set( POST_FIX_TEST "${INPUT}" )
		
  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${POST_FIX_TEST}-ARGS${TRIMMED_ARGS}" )

	get_filename_component( ext ${INPUT} EXT )

  ADD_TEST("ltsconvert_${POST_FIX_TEST}" ${ltsconvert_BINARY_DIR}/ltsconvert ${ARGS} ${testdir}/${INPUT} ${testdir}/${POST_FIX_TEST}${ext})
  set_tests_properties("ltsconvert_${POST_FIX_TEST}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
	foreach( i ${SET_OF_LPS2LTS_TESTS})
    set_tests_properties("ltsconvert_${POST_FIX_TEST}" PROPERTIES DEPENDS "${i}" )
	endforeach()
	set(SET_OF_LTSCONVERT_TESTS "${SET_OF_LTSCONVERT_TESTS};ltsconvert_${POST_FIX_TEST}${ext}" )

endmacro( add_ltsconvert_release_test INPUT ARGS)

macro( gen_ltsconvert_release_tests )
  FOREACH( i ${SET_OF_LTS_FILES} )
	  get_filename_component( ext ${i} EXT )
	  get_filename_component( name ${i} NAME_WE )

	  add_ltsconvert_release_test( "${i}" "" )
	  add_ltsconvert_release_test( "${i}" "-D" )
	  add_ltsconvert_release_test( "${i}" "-ebisim" )
	  add_ltsconvert_release_test( "${i}" "-ebranching-bisim" )
	  add_ltsconvert_release_test( "${i}" "-edpbranching-bisim" )
	  add_ltsconvert_release_test( "${i}" "-esim" )
	  add_ltsconvert_release_test( "${i}" "-etrace" )
	  add_ltsconvert_release_test( "${i}" "-eweak-trace" )
	  add_ltsconvert_release_test( "${i}" "-n" )
	  add_ltsconvert_release_test( "${i}" "--no-reach" )
	  add_ltsconvert_release_test( "${i}" "--tau=tau,delta" )

  ENDFOREACH( )
endmacro( gen_ltsconvert_release_tests )

#######################
## Macro ltscompare  ##
#######################

macro( add_ltscompare_release_test INPUT ARGS)
	set( TRIMMED_ARGS "" )			
  set( POST_FIX_TEST "${INPUT}" )
		
  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${POST_FIX_TEST}-ARGS${TRIMMED_ARGS}" )

	get_filename_component( ext ${INPUT} EXT )

  ADD_TEST("ltscompare_${POST_FIX_TEST}" ${ltscompare_BINARY_DIR}/ltscompare ${ARGS} ${testdir}/${INPUT} ${testdir}/${INPUT}-ARGS${ext})
  set_tests_properties("ltscompare_${POST_FIX_TEST}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
	set_tests_properties("ltscompare_${POST_FIX_TEST}" PROPERTIES DEPENDS "${SET_OF_LTSCONVERT_TESTS}${ext}" )

endmacro( add_ltscompare_release_test INPUT ARGS)

macro( gen_ltscompare_release_tests )
  FOREACH( i ${SET_OF_LTS_FILES} )
	  get_filename_component( ext ${i} EXT )
	  get_filename_component( name ${i} NAME_WE )

	  add_ltscompare_release_test( "${i}" "-c;-ebranching-bisim" )
	  add_ltscompare_release_test( "${i}" "-c;-edpbranching-bisim" )
	  add_ltscompare_release_test( "${i}" "-c;-esim" )
	  add_ltscompare_release_test( "${i}" "-c;-etrace" )
	  add_ltscompare_release_test( "${i}" "-c;-eweak-trace" )
	  add_ltscompare_release_test( "${i}" "-psim" )
	  add_ltscompare_release_test( "${i}" "-pweak-trace" )
	  add_ltscompare_release_test( "${i}" "-ebisim;--tau=tau" )

  ENDFOREACH( )
endmacro( gen_ltscompare_release_tests )

################### 
## Macro lps2pbes ## 
###################

macro( add_lps2pbes_release_test INPUT ARGS OUTPUTFILE_WITHOUT_EXTENSION)
	set( TRIMMED_ARGS "" )			
	get_filename_component( POST_FIX_TEST ${INPUT} NAME_WE )
		
  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${POST_FIX_TEST}-ARGS${TRIMMED_ARGS}" )


		if( NOT ${OUTPUTFILE_WITHOUT_EXTENSION} )
      ADD_TEST("lps2pbes_${POST_FIX_TEST}" ${lps2pbes_BINARY_DIR}/lps2pbes ${ARGS} ${testdir}/${INPUT} ${testdir}/${OUTPUTFILE_WITHOUT_EXTENSION}.pbes )
			set(SET_OF_PBES_FILES "${SET_OF_PBES_FILES};${OUTPUTFILE_WITHOUT_EXTENSION}.pbes"	)
      set(SET_OF_LPS2PBES_TESTS "${SET_OF_LPS2PBES_TESTS};lps2pbes_${POST_FIX_TEST}" )
		else( NOT ${OUTPUTFILE_WITHOUT_EXTENSION} )
      ADD_TEST("lps2pbes_${POST_FIX_TEST}" ${lps2pbes_BINARY_DIR}/lps2pbes ${ARGS} ${testdir}/${INPUT} ${testdir}/dummy.pbes )
		endif( NOT ${OUTPUTFILE_WITHOUT_EXTENSION} )

		set_tests_properties("lps2pbes_${POST_FIX_TEST}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
    set_tests_properties("lps2pbes_${POST_FIX_TEST}" PROPERTIES DEPENDS "${SET_OF_MCRL22LPS_TESTS}" )


endmacro( add_lps2pbes_release_test INPUT ARGS OUTPUTFILE_WITHOUT_EXTENSION)

macro( gen_lps2pbes_release_tests )
  FOREACH( i ${SET_OF_LPS_FILES} )
	  get_filename_component( save ${i} NAME_WE)

	  foreach(MCF ${SET_OF_MCF} )
	        get_filename_component( mcf_name ${MCF} NAME_WE)
					add_lps2pbes_release_test( "${i}" "-f${MCF}" "${save}_${mcf_name}" )
					add_lps2pbes_release_test( "${i}" "-tf${MCF}" "" )
	  endforeach(MCF ${SET_OF_MCF})

	ENDFOREACH( )
endmacro( gen_lps2pbes_release_tests )

####################
## Macro pbesinfo  ##
####################

macro( add_pbesinfo_release_test INPUT ARGS)
	set( TRIMMED_ARGS "" )			
	get_filename_component( POST_FIX_TEST ${INPUT} NAME_WE )
		
  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${POST_FIX_TEST}-ARGS${TRIMMED_ARGS}" )

  ADD_TEST("pbesinfo_${POST_FIX_TEST}" ${pbesinfo_BINARY_DIR}/pbesinfo ${ARGS} ${testdir}/${INPUT}  )
	set_tests_properties("pbesinfo_${POST_FIX_TEST}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
	foreach( i ${SET_OF_LPS2PBES_TESTS})
	  set_tests_properties("pbesinfo_${POST_FIX_TEST}" PROPERTIES DEPENDS "${i}" )
	endforeach()
endmacro( add_pbesinfo_release_test INPUT ARGS)

macro( gen_pbesinfo_release_tests )
  FOREACH( i ${SET_OF_PBES_FILES} )
					#					message( "${i}" )
					add_pbesinfo_release_test( "${i}" "" )
					add_pbesinfo_release_test( "${i}" "-f" )
  ENDFOREACH( )
endmacro( gen_pbesinfo_release_tests )

###################
## Macro pbespp  ##
###################

macro( add_pbespp_release_test INPUT ARGS OUTPUTFILE_WITHOUT_EXTENSION)
	set( TRIMMED_ARGS "" )			
	get_filename_component( POST_FIX_TEST ${INPUT} NAME_WE )
		
  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${POST_FIX_TEST}-ARGS${TRIMMED_ARGS}" )

	if( NOT ${OUTPUTFILE_WITHOUT_EXTENSION} )
    ADD_TEST("pbespp_${POST_FIX_TEST}" ${pbespp_BINARY_DIR}/pbespp ${ARGS} ${testdir}/${INPUT} ${testdir}/${OUTPUTFILE_WITHOUT_EXTENSION}.txt )
	  set(SET_OF_TXTPBES_FILES "${SET_OF_TXTPBES_FILES};${OUTPUTFILE_WITHOUT_EXTENSION}.txt")
    set(SET_OF_PBESPP_TESTS "${SET_OF_PBESPP_TESTS};lpspp_${POST_FIX_TEST}")
	else( NOT ${OUTPUTFILE_WITHOUT_EXTENSION} )
    ADD_TEST("pbespp_${POST_FIX_TEST}" ${pbespp_BINARY_DIR}/pbespp ${ARGS} ${testdir}/${INPUT} )
	endif( NOT ${OUTPUTFILE_WITHOUT_EXTENSION} )

	set_tests_properties("pbespp_${POST_FIX_TEST}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
  set_tests_properties("pbespp_${POST_FIX_TEST}" PROPERTIES DEPENDS "${SET_OF_LPS2BPES_TESTS}" )
endmacro( add_pbespp_release_test INPUT ARGS OUTPUTFILE_WITHOUT_EXTENSION)

macro( gen_pbespp_release_tests )
  FOREACH( i ${SET_OF_PBES_FILES} )
					add_pbespp_release_test( "${i}" "-fdefault" "${i}")
					add_pbespp_release_test( "${i}" "-fdebug" "")
					add_pbespp_release_test( "${i}" "-finternal" "")
					add_pbespp_release_test( "${i}" "-finternal-debug" "")
  ENDFOREACH( )
endmacro( gen_pbespp_release_tests )

#########################
## Macro pbesconstelm  ##
#########################

macro( add_pbesconstelm_release_test INPUT ARGS)
	set( TRIMMED_ARGS "" )			
	get_filename_component( POST_FIX_TEST ${INPUT} NAME_WE )
		
  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${POST_FIX_TEST}-ARGS${TRIMMED_ARGS}" )

  ADD_TEST("pbesconstelm_${POST_FIX_TEST}" ${pbesconstelm_BINARY_DIR}/pbesconstelm ${ARGS} ${testdir}/${INPUT} ${testdir}/dummy.pbes )
	set_tests_properties("pbesconstelm_${POST_FIX_TEST}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
  set_tests_properties("pbesconstelm_${POST_FIX_TEST}" PROPERTIES DEPENDS "${SET_OF_LPS2BPES_TESTS}" )
endmacro( add_pbesconstelm_release_test INPUT ARGS)

macro( gen_pbesconstelm_release_tests )
  FOREACH( i ${SET_OF_PBES_FILES} )
					add_pbesconstelm_release_test( "${i}" "-c" )
					add_pbesconstelm_release_test( "${i}" "-e" )
					add_pbesconstelm_release_test( "${i}" "-psimplify" )
					add_pbesconstelm_release_test( "${i}" "-pquantifier-all" )
					add_pbesconstelm_release_test( "${i}" "-pquantifier-finite" )
					add_pbesconstelm_release_test( "${i}" "-ppfnf" )
					add_pbesconstelm_release_test( "${i}" "-rjitty" )
					add_pbesconstelm_release_test( "${i}" "-rjittyp" )
					add_pbesconstelm_release_test( "${i}" "-rinner" )
					if( NOT WIN32)
  					add_pbesconstelm_release_test( "${i}" "-rjittyc" )
	  				add_pbesconstelm_release_test( "${i}" "-rinnerc" )
					endif( NOT WIN32)
					add_pbesconstelm_release_test( "${i}" "-rinnerp" )
  ENDFOREACH( )
endmacro( gen_pbesconstelm_release_tests )

#######################
## Macro pbesparelm  ##
#######################

macro( add_pbesparelm_release_test INPUT ARGS)
	set( TRIMMED_ARGS "" )			
	get_filename_component( POST_FIX_TEST ${INPUT} NAME_WE )
		
  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${POST_FIX_TEST}-ARGS${TRIMMED_ARGS}" )

  ADD_TEST("pbesparelm_${POST_FIX_TEST}" ${pbesparelm_BINARY_DIR}/pbesparelm ${ARGS} ${testdir}/${INPUT} ${testdir}/dummy.pbes )
	set_tests_properties("pbesparelm_${POST_FIX_TEST}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
  set_tests_properties("pbesparelm_${POST_FIX_TEST}" PROPERTIES DEPENDS "${SET_OF_LPS2BPES_TESTS}" )
endmacro( add_pbesparelm_release_test INPUT ARGS)

macro( gen_pbesparelm_release_tests )
  FOREACH( i ${SET_OF_PBES_FILES} )
					add_pbesparelm_release_test( "${i}" "" )
  ENDFOREACH( )
endmacro( gen_pbesparelm_release_tests )

#####################
## Macro pbesrewr  ##
#####################

macro( add_pbesrewr_release_test INPUT ARGS)
	set( TRIMMED_ARGS "" )			
	get_filename_component( POST_FIX_TEST ${INPUT} NAME_WE )
		
  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${POST_FIX_TEST}-ARGS${TRIMMED_ARGS}" )

  ADD_TEST("pbesrewr_${POST_FIX_TEST}" ${pbesrewr_BINARY_DIR}/pbesrewr ${ARGS} ${testdir}/${INPUT} ${testdir}/dummy.pbes )
	set_tests_properties("pbesrewr_${POST_FIX_TEST}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
  set_tests_properties("pbesrewr_${POST_FIX_TEST}" PROPERTIES DEPENDS "${SET_OF_LPS2BPES_TESTS}" )
endmacro( add_pbesrewr_release_test INPUT ARGS)

macro( gen_pbesrewr_release_tests )
  FOREACH( i ${SET_OF_PBES_FILES} )
					add_pbesrewr_release_test( "${i}" "-psimplify" )
					add_pbesrewr_release_test( "${i}" "-pquantifier-all" )
					add_pbesrewr_release_test( "${i}" "-pquantifier-finite" )
					add_pbesrewr_release_test( "${i}" "-ppfnf" )
					add_pbesrewr_release_test( "${i}" "-rjitty" )
					add_pbesrewr_release_test( "${i}" "-rjittyp" )
					add_pbesrewr_release_test( "${i}" "-rinner" )
					if( NOT WIN32)
  					add_pbesrewr_release_test( "${i}" "-rjittyc" )
	  				add_pbesrewr_release_test( "${i}" "-rinnerc" )
					endif( NOT WIN32)
					add_pbesrewr_release_test( "${i}" "-rinnerp" )
  ENDFOREACH( )
endmacro( gen_pbesrewr_release_tests )

######################
## Macro pbes2bool  ##
######################

macro( add_pbes2bool_release_test INPUT ARGS)
	set( TRIMMED_ARGS "" )			
	get_filename_component( POST_FIX_TEST ${INPUT} NAME_WE )
		
  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${POST_FIX_TEST}-ARGS${TRIMMED_ARGS}" )

  ADD_TEST("pbes2bool_${POST_FIX_TEST}" ${pbes2bool_BINARY_DIR}/pbes2bool ${ARGS} ${testdir}/${INPUT} ${testdir}/dummy.pbes2bool )
	set_tests_properties("pbes2bool_${POST_FIX_TEST}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
  set_tests_properties("pbes2bool_${POST_FIX_TEST}" PROPERTIES DEPENDS "${SET_OF_LPS2BPES_TESTS}" )
endmacro( add_pbes2bool_release_test INPUT ARGS)

macro( gen_pbes2bool_release_tests )
  FOREACH( i ${SET_OF_PBES_FILES} )
					add_pbes2bool_release_test( "${i}" "-c" )
					add_pbes2bool_release_test( "${i}" "-H" )
					add_pbes2bool_release_test( "${i}" "-onone" )
					add_pbes2bool_release_test( "${i}" "-ovasy" )
					add_pbes2bool_release_test( "${i}" "-opbes" )
					add_pbes2bool_release_test( "${i}" "-ocwi" )
					add_pbes2bool_release_test( "${i}" "-obes" )
					add_pbes2bool_release_test( "${i}" "-psimplify" )
					add_pbes2bool_release_test( "${i}" "-pquantifier-all" )
					add_pbes2bool_release_test( "${i}" "-pquantifier-finite" )
					add_pbes2bool_release_test( "${i}" "-ppfnf" )
					add_pbes2bool_release_test( "${i}" "-rjitty" )
					add_pbes2bool_release_test( "${i}" "-rjittyp" )
					add_pbes2bool_release_test( "${i}" "-rinner" )
					if( NOT WIN32)
  					add_pbes2bool_release_test( "${i}" "-rjittyc" )
	  				add_pbes2bool_release_test( "${i}" "-rinnerc" )
					endif( NOT WIN32)
					add_pbes2bool_release_test( "${i}" "-rinnerp" )
					add_pbes2bool_release_test( "${i}" "-s0" )
					add_pbes2bool_release_test( "${i}" "-s1" )
					add_pbes2bool_release_test( "${i}" "-s2" )
					add_pbes2bool_release_test( "${i}" "-s3" )
					add_pbes2bool_release_test( "${i}" "-t" )
					add_pbes2bool_release_test( "${i}" "-u" )
  ENDFOREACH( )
endmacro( gen_pbes2bool_release_tests )

######################
## Macro txt2lps  ##
######################

macro( add_txt2lps_release_test INPUT ARGS)
	set( TRIMMED_ARGS "" )			
	get_filename_component( POST_FIX_TEST ${INPUT} NAME_WE )
		
  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${POST_FIX_TEST}-ARGS${TRIMMED_ARGS}" )

  ADD_TEST("txt2lps_${POST_FIX_TEST}" ${txt2lps_BINARY_DIR}/txt2lps ${ARGS} ${testdir}/${INPUT} ${testdir}/dummy.lps )
	set_tests_properties("txt2lps_${POST_FIX_TEST}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
	foreach( i ${SET_OF_LPSPP_TESTS} )
    set_tests_properties("txt2lps_${POST_FIX_TEST}" PROPERTIES DEPENDS "${i}" )
	endforeach()
endmacro( add_txt2lps_release_test INPUT ARGS)

macro( gen_txt2lps_release_tests )
  FOREACH( i ${SET_OF_TXTLPS_FILES} )
					add_txt2lps_release_test( "${i}" "" )
  ENDFOREACH( )
endmacro( gen_txt2lps_release_tests )

######################
## Macro txt2pbes  ##
######################

macro( add_txt2pbes_release_test INPUT ARGS)
	set( TRIMMED_ARGS "" )			
	get_filename_component( POST_FIX_TEST ${INPUT} NAME_WE )
		
  FOREACH( i ${ARGS} )
    set(TRIMMED_ARGS "${TRIMMED_ARGS}${i}" )
	ENDFOREACH( )
	set( POST_FIX_TEST "${POST_FIX_TEST}-ARGS${TRIMMED_ARGS}" )

  ADD_TEST("txt2pbes_${POST_FIX_TEST}" ${txt2pbes_BINARY_DIR}/txt2pbes ${ARGS} ${testdir}/${INPUT} ${testdir}/dummy.pbes )
	set_tests_properties("txt2pbes_${POST_FIX_TEST}" PROPERTIES LABELS "${MCRL2_TEST_LABEL}")
	foreach( i ${SET_OF_PBESPP_TESTS} )
    set_tests_properties("txt2pbes_${POST_FIX_TEST}" PROPERTIES DEPENDS "${i}" )
	endforeach()
endmacro( add_txt2pbes_release_test INPUT ARGS)

macro( gen_txt2pbes_release_tests )
  FOREACH( i ${SET_OF_TXTPBES_FILES} )
					add_txt2pbes_release_test( "${i}" "" )
  ENDFOREACH( )
endmacro( gen_txt2pbes_release_tests )


## tool testcase generation 
gen_mcrl22lps_release_tests()
gen_lps2lts_release_tests()
gen_lpsrewr_release_tests()
gen_lpsinfo_release_tests()
gen_lpspp_release_tests()
gen_lpsconstelm_release_tests()
gen_lpsparelm_release_tests()
gen_lpssumelm_release_tests()
gen_lpsactionrename_release_tests()
gen_lpsuntime_release_tests()
gen_lpsinvelm_release_tests()
gen_lpsconfcheck_release_tests()
gen_lpsbinary_release_tests()
gen_lpsparunfold_release_tests()
gen_lpssuminst_release_tests()
gen_ltsinfo_release_tests()
gen_lts2lps_release_tests()
gen_ltsconvert_release_tests()
gen_ltscompare_release_tests()
gen_lps2pbes_release_tests()
gen_pbesinfo_release_tests()
gen_pbespp_release_tests()
gen_pbesconstelm_release_tests()
gen_pbesparelm_release_tests()
gen_pbesrewr_release_tests()
gen_pbes2bool_release_tests()
gen_txt2lps_release_tests()
gen_txt2pbes_release_tests()

endfunction()
# examples/academic/cabp/cabp.mcrl2
# examples/academic/goback/goback.mcrl2
# examples/academic/dining/dining3.mcrl2
# examples/academic/dining/dining3_schedule_seq.mcrl2
# examples/academic/dining/dining3_seq.mcrl2
# examples/academic/dining/dining8.mcrl2
# examples/academic/dining/dining_10000.mcrl2
# examples/academic/dining/dining3_cs_seq.mcrl2
# examples/academic/dining/dining3_cs.mcrl2
# examples/academic/dining/dining_1000.mcrl2
# examples/academic/dining/dining3_ns.mcrl2
# examples/academic/dining/dining_100.mcrl2
# examples/academic/dining/dining3_ns_seq.mcrl2
# examples/academic/dining/dining_10.mcrl2
# examples/academic/dining/dining3_schedule.mcrl2
# examples/academic/commprot/commprot.mcrl2
# examples/academic/bke/bke.mcrl2
# examples/academic/mpsu/mpsu.mcrl2
# examples/academic/abp_bw/abp_bw.mcrl2
# examples/academic/abp/abp.mcrl2
# examples/academic/bakery/bakery.mcrl2
# examples/academic/parallel/parallel.mcrl2
# examples/academic/producer_consumer/producer_consumer.mcrl2
# examples/academic/scheduler/scheduler.mcrl2
# examples/academic/tree/tree.mcrl2
# examples/academic/onebit/onebit.mcrl2
# examples/academic/block/block.mcrl2
# examples/academic/cellular_automata/cellular_automata.mcrl2
# examples/academic/allow/allow.mcrl2
# examples/academic/swp/swp_with_tanenbaums_bug.mcrl2
# examples/academic/swp/swp_fgpbp.mcrl2
# examples/academic/swp/swp_lists.mcrl2
# examples/academic/swp/swp_func.mcrl2
# examples/academic/trains/trains.mcrl2
# examples/academic/leader/leader.mcrl2

