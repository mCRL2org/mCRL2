# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(mcrl2_monolib)
cmake_minimum_required(VERSION 2.6)

# ######### General setup ##########
include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include )
include_directories(${CMAKE_SOURCE_DIR}/libraries/trace/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lps/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/pbes/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lts/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include/ )
include_directories(${CMAKE_SOURCE_DIR}/libraries/process/include/)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/build/workarounds/)
include_directories(${CMAKE_SOURCE_DIR}/build/precompile)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/aterm/include/aterm/)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/ticpp/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/svc/include)
include_directories(${CMAKE_SOURCE_DIR}/tools/squadt/libraries/tipi/include/)
#The following include directory is required by out-of-source builds for finding setup.h
include_directories(${CMAKE_BINARY_DIR}/libraries/utilities/include)


if(SQUADT_CONNECTIVITY)
  
  add_definitions("-DENABLE_SQUADT_CONNECTIVITY")

  set(MCRL2_UTILITIES_SQUADT
    ${CMAKE_SOURCE_DIR}/libraries/utilities/source/squadt_interface.cpp
  )

endif(SQUADT_CONNECTIVITY)


if(MCRL2_BUILD_GUI_TOOLS)

  set(MCRL2_UTILITIES_WX
    ${CMAKE_SOURCE_DIR}/libraries/utilities/source/spinctrlfloat.cpp
    ${CMAKE_SOURCE_DIR}/libraries/utilities/source/colorbutton.cpp
    ${CMAKE_SOURCE_DIR}/libraries/utilities/source/font_renderer.cpp
  )
 
  set(MCRL2_SIMULATOR_GUI
    ${CMAKE_SOURCE_DIR}/tools/xsim/simulatorgui.cpp
  )

endif(MCRL2_BUILD_GUI_TOOLS)


if(SQUADT_CONNECTIVITY OR MCRL2_BUILD_GUI_TOOLS)

  set(SQUADT_TIPI
    ${CMAKE_SOURCE_DIR}/tools/squadt/libraries/tipi/source/implementations.cpp
    ${CMAKE_SOURCE_DIR}/tools/squadt/libraries/tipi/source/controller/communicator.cpp
    ${CMAKE_SOURCE_DIR}/tools/squadt/libraries/tipi/source/tool/communicator.cpp
  )

endif(SQUADT_CONNECTIVITY OR MCRL2_BUILD_GUI_TOOLS)


add_library( ${PROJECT_NAME}
  ${CMAKE_SOURCE_DIR}/libraries/core/source/typecheck.cpp
  ${CMAKE_SOURCE_DIR}/libraries/core/source/parse.cpp
  ${CMAKE_SOURCE_DIR}/libraries/core/source/struct_core.cpp
  ${CMAKE_SOURCE_DIR}/libraries/core/source/mcrl2parser.cpp
  ${CMAKE_SOURCE_DIR}/libraries/core/source/mcrl2lexer.cpp
  ${CMAKE_SOURCE_DIR}/libraries/core/source/regfrmtrans.cpp
  ${CMAKE_SOURCE_DIR}/libraries/core/source/print.cpp
  ${CMAKE_SOURCE_DIR}/libraries/core/source/messaging.cpp
  ${CMAKE_SOURCE_DIR}/libraries/core/source/aterm_ext.cpp
  ${CMAKE_SOURCE_DIR}/libraries/core/source/numeric_string.cpp
  ${CMAKE_SOURCE_DIR}/libraries/core/source/alpha.cpp
  ${CMAKE_SOURCE_DIR}/libraries/core/source/print_c.cpp
  ${CMAKE_SOURCE_DIR}/libraries/core/source/text_utility.cpp
  ${CMAKE_SOURCE_DIR}/libraries/data/source/implementations.cpp
  ${CMAKE_SOURCE_DIR}/libraries/data/source/detail/rewrite/with_prover.cpp
  ${CMAKE_SOURCE_DIR}/libraries/data/source/detail/rewrite/inner.cpp
  ${CMAKE_SOURCE_DIR}/libraries/data/source/detail/rewrite/jitty.cpp
  ${CMAKE_SOURCE_DIR}/libraries/data/source/detail/rewrite/innerc.cpp
  ${CMAKE_SOURCE_DIR}/libraries/data/source/detail/rewrite/rewrite.cpp
  ${CMAKE_SOURCE_DIR}/libraries/data/source/detail/rewrite/jittyc.cpp
  ${CMAKE_SOURCE_DIR}/libraries/lps/source/implementations.cpp
  ${CMAKE_SOURCE_DIR}/libraries/lts/source/liblts_scc.cpp 
  ${CMAKE_SOURCE_DIR}/libraries/lts/source/liblts_sim.cpp
  ${CMAKE_SOURCE_DIR}/libraries/lts/source/liblts_aut.cpp
  ${CMAKE_SOURCE_DIR}/libraries/lts/source/liblts_det.cpp
  ${CMAKE_SOURCE_DIR}/libraries/lts/source/liblts_dotlexer.cpp
  ${CMAKE_SOURCE_DIR}/libraries/lts/source/liblts_svc.cpp
  ${CMAKE_SOURCE_DIR}/libraries/lts/source/liblts_fsmparser.cpp
  ${CMAKE_SOURCE_DIR}/libraries/lts/source/liblts_bisim.cpp
  ${CMAKE_SOURCE_DIR}/libraries/lts/source/sim_hashtable.cpp
  ${CMAKE_SOURCE_DIR}/libraries/lts/source/liblts_dotparser.cpp
  ${CMAKE_SOURCE_DIR}/libraries/lts/source/liblts_eq.cpp
  ${CMAKE_SOURCE_DIR}/libraries/lts/source/liblts_dot.cpp
  ${CMAKE_SOURCE_DIR}/libraries/lts/source/liblts.cpp
  ${CMAKE_SOURCE_DIR}/libraries/lts/source/liblts_bcg.cpp
  ${CMAKE_SOURCE_DIR}/libraries/lts/source/liblts_tau_star.cpp
  ${CMAKE_SOURCE_DIR}/libraries/lts/source/tree_set.cpp
  ${CMAKE_SOURCE_DIR}/libraries/lts/source/liblts_fsm.cpp
  ${CMAKE_SOURCE_DIR}/libraries/lts/source/liblts_fsmlexer.cpp
  ${CMAKE_SOURCE_DIR}/libraries/trace/source/trace.cpp
  ${CMAKE_SOURCE_DIR}/libraries/utilities/source/command_line_interface.cpp
  ${CMAKE_SOURCE_DIR}/libraries/utilities/source/toolset_revision.cpp

  ${CMAKE_SOURCE_DIR}/tools/xsim/simulator.cpp
  ${CMAKE_SOURCE_DIR}/tools/xsim/simbase.cpp 

  ${MCRL2_UTILITIES_SQUADT}
  ${MCRL2_UTILITIES_WX}
  ${MCRL2_SIMULATOR_GUI}
  ${SQUADT_TIPI}
)

set_source_files_properties(${CMAKE_SOURCE_DIR}/libraries/utilities/source/toolset_revision.cpp
  PROPERTIES COMPILE_DEFINITIONS MCRL2_REVISION="${SVN_REV} - ${CMAKE_BUILD_TYPE}"
)


target_link_libraries(${PROJECT_NAME}
  svc 
  ticpp
  aterm
)


if(USE_BOOST_INTERNAL)
  target_link_libraries(${PROJECT_NAME}
    boost
  ) 
else(USE_BOOST_INTERNAL)
  target_link_libraries(${PROJECT_NAME}
    ${Boost_LIBRARIES}     
  ) 
endif(USE_BOOST_INTERNAL)


if(MCRL2_BUILD_GUI_TOOLS)
  target_link_libraries(${PROJECT_NAME}
    ${wxWidgets_LIBRARIES}
  )
endif(MCRL2_BUILD_GUI_TOOLS)


install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR})
