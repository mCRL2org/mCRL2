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

set(TESTDIR "${CMAKE_BINARY_DIR}/mcrl2-testoutput")

if(NOT WIN32)
  set(_JITTYC "-rjittyc")
endif()

set(tagIN "!IN")
set(tagOUT "!OUT")
if(NOT MCRL2_ENABLE_EXPERIMENTAL)
  set(_skip_tests "complps2pbes;lpsrealelm;pbesabsinthe;besconvert;lpsrealzone;symbolic_exploration;lpssymbolicbisim;pbespareqelm" CACHE INTERNAL "Used internally by GenerateToolTests.cmake" FORCE)
else()
  set(_skip_tests "" CACHE INTERNAL "Used internally by GenerateToolTests.cmake" FORCE)
endif()

function(sanitize STR OUTPUT)
  string(REGEX REPLACE "^-" "" STR ${STR})
  string(REGEX REPLACE "[*\\/().]" "_" STR ${STR})
  set(${OUTPUT} ${STR} PARENT_SCOPE)
endfunction()

function(add_tool_test TOOL)
  list(FIND _skip_tests ${TOOL} _skip_test)
  if(_skip_test GREATER -1)
    return()
  endif()
  set(test_name "tooltest_${TOOL}")
  set(test_args "")
  set(test_IN "")
  set(test_OUT "")
  set(save_next)
  foreach(arg ${ARGN})
    if(arg STREQUAL ${tagIN})
      set(save_next test_IN)
    elseif(arg STREQUAL ${tagOUT})
      set(save_next test_OUT)
    else()
      sanitize("${arg}" arg_sane)
      set(test_name ${test_name}_${arg_sane})
      list(APPEND test_args ${arg})
      if(save_next)
        list(APPEND ${save_next} ${arg})
      endif()
      set(save_next)
    endif()
  endforeach()

  add_test(NAME ${test_name} WORKING_DIRECTORY ${TESTDIR} COMMAND ${TOOL} ${test_args})
  set_tests_properties(${test_name} PROPERTIES LABELS "tooltest")

  set(test_deps "")
  foreach(in_file ${test_IN})
    list(FIND _tooltest_source_files "${in_file}" index)
    if(index LESS 0)
      set(even TRUE)
      set(found FALSE)
      foreach(item ${_tooltest_file_dependencies})
        if(even)
          if(item STREQUAL in_file)
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
        message(FATAL_ERROR "Tool test ${test_name} is using an invalid input file: '${in_file}'")
      endif()
    endif()
  endforeach()

  if(test_deps)
    set_tests_properties(${test_name} PROPERTIES DEPENDS "${test_deps}")
  endif()

  if(test_OUT)
    foreach(out_file ${test_OUT})
      list(APPEND _tooltest_file_dependencies "${out_file}" "${test_name}")
    endforeach()
    set(_tooltest_file_dependencies "${_tooltest_file_dependencies}" CACHE INTERNAL "Used internally by GenerateToolTests.cmake" FORCE)
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
function(gen_lps2lts_release_tests LPSFILE LTSFILES ACTIONS)
  set(ARGUMENTS "-b10" "-ctau" "-D" "--error-trace" "--init-tsize=10" "-l10" "--no-info"
                "-rjitty" "-rjittyp" ${_JITTYC} "-sd" "-sb" "-sp" "-sq\;-l100" "-sr\;-l100"
                "--verbose\;--suppress" "--todo-max=10" "-u" "-yno")
  if(ACTIONS)
    list(GET ACTIONS 0 ACTION)
    list(APPEND ARGUMENTS "-a${ACTIONS}" "-c${ACTION}")
  endif()
  foreach(arglist ${ARGUMENTS})
    add_tool_test(lps2lts ${arglist} ${tagIN} ${LPSFILE})
  endforeach()
  foreach(ltsfile ${LTSFILES})
    add_tool_test(lps2lts ${tagIN} ${LPSFILE} ${tagOUT} ${ltsfile})
  endforeach()
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
	add_tool_test(lpssumelm -c ${tagIN} ${LPSFILE})
endfunction()

# lpsactionrename
function(gen_lpsactionrename_release_tests LPSFILE RENFILE)
  set(ARGUMENTS "" "-m" "-o" "-t" "-rjitty" "-rjittyp" ${_JITTYC})
  foreach(arglist ${ARGUMENTS})
    add_tool_test(lpsactionrename -f${RENFILE} ${arglist} ${tagIN} ${LPSFILE})
  endforeach()
endfunction()

# lpsuntime
function(gen_lpsuntime_release_tests LPSFILE)
	add_tool_test(lpsuntime ${tagIN} ${LPSFILE})
endfunction()

# lpsinvelm
function(gen_lpsinvelm_release_tests LPSFILE TESTDIR TRUEFILE FALSEFILE)
  set(ARGUMENTS "-c" "-e" "-l" "-n" "-pinvelm_test" "-rjitty" "-t10" ${_JITTYC})
  if(cvc3_FOUND)
    list(APPEND ARGUMENTS "-zcvc")
  endif()
  foreach(arglist ${ARGUMENTS})
    add_tool_test(lpsinvelm -i${TRUEFILE} ${arglist} ${tagIN} ${LPSFILE})
  endforeach()
  add_tool_test(lpsinvelm -i${FALSEFILE} -n -e ${tagIN} ${LPSFILE})
endfunction()

# lpsconfcheck
function(gen_lpsconfcheck_release_tests LPSFILE TESTDIR TRUEFILE FALSEFILE DOTFILE)
  set(ARGUMENTS "" "-a" "-c" "-g" "-i${TRUEFILE}" "-i${FALSEFILE}"
                "-p${DOTFILE}\;-i${TRUEFILE}" "-m" "-n" "-o"
                "-rjitty" ${_JITTYC} "-s10\;-i${TRUEFILE}" "-t10\;-i${TRUEFILE}")
  if(cvc3_FOUND)
    list(APPEND ARGUMENTS "-zcvc\;-i${TRUEFILE}")
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
    add_tool_test(lpssuminst -f ${arglist} ${tagIN} ${LPSFILE})
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
  list(APPEND ARGUMENTS "-c\;-ebranching-bisim" "-c\;-edpbranching-bisim"  "-c\;-esim" "-c\;-etrace" "-c\;-eweak-trace"
                "-psim" "-pweak-trace")
  if(ACTIONS)
    list(APPEND ARGUMENTS "-ebisim\;--tau=${ACTIONS}")
  endif()
  foreach(arglist ${ARGUMENTS})
    add_tool_test(ltscompare ${arglist} ${tagIN} ${LTSFILE} ${tagIN} ${LTSFILE})
  endforeach()
endfunction()

# lps2pbes
function(gen_lps2pbes_release_tests LPSFILE MCFFILE PBESFILE)
  add_tool_test(lps2pbes -f ${tagIN} ${MCFFILE} ${tagIN} ${LPSFILE} ${tagOUT} ${PBESFILE})
  add_tool_test(lps2pbes -tf ${tagIN} ${MCFFILE} ${tagIN} ${LPSFILE})
endfunction()

# lts2pbes
function(gen_lts2pbes_release_tests LTSFILE MCFFILE MCRL2FILE)
  add_tool_test(lts2pbes -f ${tagIN} ${MCFFILE} -m ${tagIN} ${MCRL2FILE} ${tagIN} ${LTSFILE} ${tagOUT})
endfunction()

# pbesinfo
function(gen_pbesinfo_release_tests PBESFILE)
  add_tool_test(pbesinfo ${tagIN} ${PBESFILE})
  add_tool_test(pbesinfo -f ${tagIN} ${PBESFILE})
endfunction()

# pbespp
function(gen_pbespp_release_tests PBESFILE TXTFILE)
  add_tool_test(pbespp "-finternal" ${tagIN} ${PBESFILE})
  add_tool_test(pbespp "-fdefault" ${tagIN} ${PBESFILE} ${tagOUT} ${TXTFILE})
endfunction()

# pbesconstelm
function(gen_pbesconstelm_release_tests PBESFILE)
  set(ARGUMENTS "-c" "-e" "-psimplify" "-pquantifier-all" "-pquantifier-finite" "-ppfnf"
                "-rjitty" "-rjittyp" ${_JITTYC})
  foreach(arglist ${ARGUMENTS})
    add_tool_test(pbesconstelm ${arglist} ${tagIN} ${PBESFILE})
  endforeach()
endfunction()

# pbesparelm
function(gen_pbesparelm_release_tests PBESFILE)
  add_tool_test(pbesparelm ${tagIN} ${PBESFILE})
endfunction()

# pbesrewr
function(gen_pbesrewr_release_tests PBESFILE)
  set(ARGUMENTS "-psimplify" "-pquantifier-all" "-pquantifier-finite" "-ppfnf"
                "-rjitty" "-rjittyp" ${_JITTYC})
  foreach(arglist ${ARGUMENTS})
    add_tool_test(pbesrewr ${arglist} ${tagIN} ${PBESFILE})
  endforeach()
endfunction()

# pbes2bool
function(gen_pbes2bool_release_tests PBESFILE)
  set(ARGUMENTS "-c" "-rjitty" "-rjittyp" ${_JITTYC} "-s0" "-s1" "-s2" "-s3" "-u"
                "-zbreadth-first" "-zdepth-first" "--erase=none" "--erase=some" "--erase=all" "-glf" "-gspm")
  foreach(arglist ${ARGUMENTS})
    add_tool_test(pbes2bool ${arglist} ${tagIN} ${PBESFILE})
  endforeach()
endfunction()

# txt2lps
function(gen_txt2lps_release_tests TXTFILE)
  add_tool_test(txt2lps ${tagIN} ${TXTFILE})
endfunction()

# txt2pbes
function(gen_txt2pbes_release_tests TXTFILE)
  add_tool_test(txt2pbes ${tagIN} ${TXTFILE})
endfunction()

# pbes2bes
function(gen_pbes2bes_release_tests PBESFILE BESFILE)
  set(ARGUMENTS "-opbes" "-otext" "-obes" "-opgsolver" "-rjitty" "-rjittyp" ${_JITTYC}
                "-s0" "-s1" "-s2" "-s3" "-u" "-zb" "-zd" "--erase=none" "--erase=some" "--erase=all")
  add_tool_test(pbes2bes ${arglist} ${tagIN} ${PBESFILE} ${tagOUT} ${BESFILE})
  foreach(arglist ${ARGUMENTS})
    add_tool_test(pbes2bes ${arglist} ${tagIN} ${PBESFILE})
  endforeach()
endfunction()

# besinfo
function(gen_besinfo_release_tests BESFILE)
  add_tool_test(besinfo ${tagIN} ${BESFILE})
  add_tool_test(besinfo -f ${tagIN} ${BESFILE})
endfunction()

# bespp
function(gen_bespp_release_tests BESFILE TXTFILE)
  add_tool_test(bespp -fdefault ${tagIN} ${BESFILE} ${tagOUT} ${TXTFILE})
endfunction()

# bessolve
function(gen_bessolve_release_tests BESFILE)
  add_tool_test(bessolve -sgauss ${tagIN} ${BESFILE})
  add_tool_test(bessolve -sspm ${tagIN} ${BESFILE})
endfunction()

# besconvert
function(gen_besconvert_release_tests BESFILE)
  add_tool_test(besconvert -ebisim ${tagIN} ${BESFILE})
  add_tool_test(besconvert -estuttering ${tagIN} ${BESFILE})
endfunction()

# pbesinst
function(gen_pbesinst_release_tests PBESFILE)
  set(ARGUMENTS "" "-opbes" "-obes" "-otext" "-opgsolver" "-slazy" "-sfinite\;-f\"*(*:Bool)\"" "-rjitty" "-rjittyp" ${_JITTYC})
  foreach(arglist ${ARGUMENTS})
    add_tool_test(pbesinst ${arglist} ${tagIN} ${PBESFILE})
  endforeach()
endfunction()

# pbespareqelm
function(gen_pbespareqelm_release_tests PBESFILE)
  set(ARGUMENTS "" "-I" "-psimplify" "-pquantifier-all" "-pquantifier-finite" "-ppfnf" "-rjitty" "-rjittyp" ${_JITTYC})
  foreach(arglist ${ARGUMENTS})
    add_tool_test(pbespareqelm ${arglist} ${tagIN} ${PBESFILE})
  endforeach()
endfunction()

# pbespgsolve
function(gen_pbespgsolve_release_tests PBESFILE)
  set(ARGUMENTS "" "-c" "-C" "-L" "-e" "-sspm" "-saltspm" "-srecursive" "-rjitty" "-rjittyp" ${_JITTYC})
  foreach(arglist ${ARGUMENTS})
    add_tool_test(pbespgsolve ${arglist} ${tagIN} ${PBESFILE})
  endforeach()
endfunction()

# lpsbisim2pbes
function(gen_lpsbisim2pbes_release_tests LPSFILE)
  add_tool_test(lpsbisim2pbes -bstrong-bisim ${tagIN} ${LPSFILE} ${tagIN} ${LPSFILE})
  add_tool_test(lpsbisim2pbes -bweak-bisim ${tagIN} ${LPSFILE} ${tagIN} ${LPSFILE})
  add_tool_test(lpsbisim2pbes -bbranching-bisim ${tagIN} ${LPSFILE} ${tagIN} ${LPSFILE})
  add_tool_test(lpsbisim2pbes -bbranching-sim ${tagIN} ${LPSFILE} ${tagIN} ${LPSFILE})
  add_tool_test(lpsbisim2pbes -bstrong-bisim -n ${tagIN} ${LPSFILE} ${tagIN} ${LPSFILE})
endfunction()

# txt2bes
function(gen_txt2bes_release_tests TXTFILE)
  add_tool_test(txt2bes ${tagIN} ${TXTFILE})
endfunction()

# lpsrealelm
function(gen_lpsrealelm_release_tests LPSFILE)
  set(ARGUMENTS "" "--max=10" "-rjitty" "-rjittyp" ${_JITTYC})
  foreach(arglist ${ARGUMENTS})
    add_tool_test(lpsrealelm ${arglist} ${tagIN} ${LPSFILE})
  endforeach()
endfunction()

##############################
## tool testcase generation ##
##############################

function(get_actions MCRL2FILE ACTION_NAMES ACTION_TYPE)
  file(READ ${MCRL2FILE} spec)
  string(REGEX REPLACE "%[^\n]*\n?" "" spec "${spec}")
  if ("${spec}" MATCHES "(^|;)[ \r\n\t]*act[ \t\r\n]+([^;]+);")
    set(actlist ${CMAKE_MATCH_2})
    if (${actlist} MATCHES "([^ \t\n\r]*)[ \t\n\r]*:(.*)")
      set(actlist ${CMAKE_MATCH_1})
      set(typelist ${CMAKE_MATCH_2})
      string(REGEX REPLACE "[ \t\r\n]*#[ \t\r\n]*" ";" typelist ${typelist})
    endif()
    string(REGEX REPLACE "[ \t\r\n]*,[ \t\r\n]*" ";" actlist ${actlist})
  endif()
  set(${ACTION_NAMES} ${actlist} PARENT_SCOPE)
  set(${ACTION_TYPE} ${typelist} PARENT_SCOPE)
endfunction()

function(join GLUE OUTPUT_STRING)
  set(result "")
  if(ARGN)
    set(glue "")
    foreach(s ${ARGN})
      set(result "${result}${glue}${s}")
      set(glue ${GLUE})
    endforeach()
  endif()
  set(${OUTPUT_STRING} ${result} PARENT_SCOPE)
endfunction()

function(generate_tool_tests MCRL2FILES SKIP_TESTS)
  set(_old_skip_tests ${_skip_tests})
  list(APPEND _skip_tests ${SKIP_TESTS})
  set(_skip_tests ${_skip_tests} CACHE INTERNAL "Used internally by GenerateToolTests.cmake" FORCE)
  foreach(MCRL2FILE ${MCRL2FILES})
		# Copy mCRL2 spec from examples directory to testdir (Required for "lts2lps" -m argument)
    get_filename_component(BASE ${MCRL2FILE} NAME_WE)
		configure_file(${MCRL2FILE} ${TESTDIR}/${BASE}.mcrl2 COPYONLY)
    configure_file(${CMAKE_SOURCE_DIR}/examples/modal-formulas/nodeadlock.mcf ${TESTDIR}/nodeadlock.mcf COPYONLY)
    configure_file(${CMAKE_SOURCE_DIR}/examples/modal-formulas/nolivelock.mcf ${TESTDIR}/nolivelock.mcf COPYONLY)

    set(MCRL2FILE ${BASE}.mcrl2)
    set(RENAME_FILE ${BASE}_rename_spec.txt)
    set(HIDE_FILE ${BASE}_hide_spec.txt)
    set(TRUE_FILE true.txt)
    set(FALSE_FILE false.txt)
    set(MCFFILES nodeadlock.mcf nolivelock.mcf)
    set(_tooltest_source_files ${MCRL2FILE} ${RENAME_FILE} ${HIDE_FILE} ${TRUE_FILE} ${FALSE_FILE} ${MCFFILES} CACHE INTERNAL "Used internally by GenerateToolTests.cmake" FORCE)

    set(LPSFILE ${BASE}.lps)
    set(LPSTXTFILE ${BASE}.lps.txt)
    set(LTSFILE ${BASE}.lts)
    set(AUTFILE ${BASE}.aut)
    set(FSMFILE ${BASE}.fsm)
    set(DOTFILE ${BASE}.dot)
    if(${MCRL2_ENABLE_CADP_SUPPORT})
      set(BCGFILE ${BASE}.bcg)
    endif(${MCRL2_ENABLE_CADP_SUPPORT})
    set(LTSFILES ${LTSFILE} ${AUTFILE} ${FSMFILE} ${BCGFILE})
    set(LPSCONFCHECK_DOTFILE ${BASE}.lpsconfcheck.dot)
    set(PBESFILES ${BASE}.nodeadlock.pbes
                  ${BASE}.nolivelock.pbes)
    set(BESFILES ${BASE}.nodeadlock.bes
                 ${BASE}.nolivelock.bes)
    set(BESTXTFILES ${BASE}.nodeadlock.bes.txt
                    ${BASE}.nolivelock.bes.txt)
    set(PBESTXTFILES ${BASE}.nodeadlock.pbes.txt
                     ${BASE}.nolivelock.pbes.txt)

    get_actions(${TESTDIR}/${MCRL2FILE} ACTION_NAMES ACTION_TYPES)

    if(ACTION_NAMES)
      # Create hide-specification
      join("," COMMASEP_ACTIONS ${ACTION_NAMES})
      write_file(${TESTDIR}/${HIDE_FILE} ${COMMASEP_ACTIONS})
      # Create rename-specification
      list(GET ACTION_NAMES 0 actname)
      if(ACTION_TYPES)
        set(varnames)
        set(vardecl "var ")
        set(index 0)
        foreach(tp ${ACTION_TYPES})
          set(varnames ${varnames} v${index})
          set(vardecl "${vardecl}v${index}: ${tp}\;\n    ")
          math(EXPR index "${index} + 1")
        endforeach()
        join(", " actparams ${varnames})
        set(actparams "(${actparams})")
      endif()
      set(RENAME_SPEC "
          act ${actname}_renamed_by_tooltest\;
          ${vardecl}
          rename ${actname}${actparams} => ${actname}_renamed_by_tooltest\;")
      write_file(${TESTDIR}/${RENAME_FILE} ${RENAME_SPEC})
      # Create input files for lpsconfcheck and lpsinvelm
      write_file(${TESTDIR}/${TRUE_FILE} "true")
      write_file(${TESTDIR}/${FALSE_FILE} "false")
    endif()

    gen_mcrl22lps_release_tests(${MCRL2FILE} ${LPSFILE})
    gen_lpsinfo_release_tests(${LPSFILE})
    gen_lpspp_release_tests(${LPSFILE} ${LPSTXTFILE})
    gen_lps2lts_release_tests(${LPSFILE} "${LTSFILES};${DOTFILE}" "${COMMASEP_ACTIONS}")
    gen_lpsconstelm_release_tests(${LPSFILE})
    gen_lpsrewr_release_tests(${LPSFILE})
    gen_lpsparelm_release_tests(${LPSFILE})
    gen_lpssumelm_release_tests(${LPSFILE})
    if(EXISTS ${TESTDIR}/${RENAME_FILE})
      gen_lpsactionrename_release_tests(${LPSFILE} ${RENAME_FILE})
    endif()
    gen_lpsuntime_release_tests(${LPSFILE})
    gen_lpsinvelm_release_tests(${LPSFILE} ${TESTDIR} ${TRUE_FILE} ${FALSE_FILE})
    gen_lpsconfcheck_release_tests(${LPSFILE} ${TESTDIR} ${TRUE_FILE} ${FALSE_FILE} ${LPSCONFCHECK_DOTFILE})
    gen_lpsbinary_release_tests(${LPSFILE})
    gen_lpsparunfold_release_tests(${LPSFILE})
    gen_lpssuminst_release_tests(${LPSFILE})
    foreach(file ${LTSFILES})
      gen_ltsinfo_release_tests(${file})
    endforeach()
    gen_ltscompare_release_tests(${LTSFILE} "${COMMASEP_ACTIONS}")
    gen_lpsbisim2pbes_release_tests(${LPSFILE})
    gen_lpsrealelm_release_tests(${LPSFILE})
    gen_txt2lps_release_tests(${LPSTXTFILE})
    set(index 0)
    foreach(MCFFILE ${MCFFILES})
      list(GET PBESFILES ${index} PBESFILE)
      list(GET BESFILES ${index} BESFILE)
      list(GET PBESTXTFILES ${index} PBESTXTFILE)
      list(GET BESTXTFILES ${index} BESTXTFILE)
      math(EXPR index "${index} + 1")
      gen_lps2pbes_release_tests(${LPSFILE} ${MCFFILE} ${PBESFILE})
      gen_lts2pbes_release_tests(${LTSFILE} ${MCFFILE} ${MCRL2FILE})
      gen_pbesinfo_release_tests(${PBESFILE})
      gen_pbespp_release_tests(${PBESFILE} ${PBESTXTFILE})
      gen_pbesconstelm_release_tests(${PBESFILE})
      gen_pbesparelm_release_tests(${PBESFILE})
      gen_pbesrewr_release_tests(${PBESFILE})
      gen_pbes2bool_release_tests(${PBESFILE})
      gen_pbes2bes_release_tests(${PBESFILE} ${BESFILE})
      gen_bespp_release_tests(${BESFILE} ${BESTXTFILE})
      gen_besconvert_release_tests(${BESFILE})
      gen_besinfo_release_tests(${BESFILE})
      gen_bessolve_release_tests(${BESFILE})
      gen_pbesinst_release_tests(${PBESFILE})
      gen_pbespareqelm_release_tests(${PBESFILE})
      gen_txt2pbes_release_tests(${PBESTXTFILE})
      gen_txt2bes_release_tests(${BESTXTFILE})
    endforeach()
  endforeach()
  set(_skip_tests ${_old_skip_tests} CACHE INTERNAL "Used internally by GenerateToolTests.cmake" FORCE)
endfunction()
