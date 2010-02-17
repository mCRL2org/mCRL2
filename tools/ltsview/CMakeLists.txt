# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(ltsview)
cmake_minimum_required(VERSION 2.6)

if ( MCRL2_BUILD_GUI_TOOLS )

# ######### General setup ##########

# Sources:
add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE
  cluster.cpp
  conedb.cpp
  glcanvas.cpp
  infodialog.cpp
  lts.cpp
  ltsview.cpp
  mainframe.cpp
  markdialog.cpp
  markmanager.cpp
  markstateruledialog.cpp
  primitivefactory.cpp
  primitives.cpp
  savepicdialog.cpp
  savevecdialog.cpp
  settings.cpp
  settingsdialog.cpp
  simdialog.cpp
  simreader.cpp
  simulation.cpp
  state.cpp
  transition.cpp
  utils.cpp
  visobjectfactory.cpp
  visualizer.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lps/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lts/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/trace/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/aterm/include/aterm)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/svc/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/tr/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/gl2ps/include)

include_directories(${CMAKE_SOURCE_DIR}/build/precompile)

# actual target:
if( MONO_LIB )
  target_link_libraries(${PROJECT_NAME}
    mcrl2_monolib
    tr
    gl2ps
  )

  # ######### required for squadt connectivity ##########
  if(SQUADT_CONNECTIVITY)
    add_definitions("-DENABLE_SQUADT_CONNECTIVITY")
    include_directories(${CMAKE_SOURCE_DIR}/tools/squadt/libraries/tipi/include)
  endif(SQUADT_CONNECTIVITY)
  # #####################################################

else( MONO_LIB )
target_link_libraries(${PROJECT_NAME}
  ${wxWidgets_LIBRARIES}
  mcrl2_trace
  mcrl2_lts
  mcrl2_utilities_command_line
  mcrl2_utilities_wx
  tr
  gl2ps
)

# ######### required for squadt connectivity ##########
if(SQUADT_CONNECTIVITY)
  add_definitions("-DENABLE_SQUADT_CONNECTIVITY")
  include_directories(${CMAKE_SOURCE_DIR}/tools/squadt/libraries/tipi/include)
  target_link_libraries(${PROJECT_NAME}
    mcrl2_utilities_squadt
  )
endif(SQUADT_CONNECTIVITY)
# #####################################################

endif( MONO_LIB )

# add install target:
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# install documentation
if (MAN_PAGES)
  add_custom_command(
    OUTPUT ${PROJECT_NAME}.1
    COMMAND ${PROJECT_NAME} --generate-man-page > ${PROJECT_NAME}.1
    DEPENDS ${PROJECT_NAME}
    COMMENT "Generating man page for ${PROJECT_NAME}"
  )
  add_custom_target(${PROJECT_NAME}_man_page ALL DEPENDS ${PROJECT_NAME}.1)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.1 DESTINATION share/man/man1)
endif (MAN_PAGES)

endif ( MCRL2_BUILD_GUI_TOOLS )
