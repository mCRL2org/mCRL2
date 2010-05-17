# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ########## Project setup ##########
project(grapemcrl2)
cmake_minimum_required(VERSION 2.6)

if ( MCRL2_BUILD_GUI_TOOLS )

add_definitions(-DTIXML_USE_TICPP)

# ######### General setup ##########

set(MACOSX_BUNDLE_ICON_FILE grapemcrl2.icns )

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/grapeapp/help/grapehelp.zip" "${CMAKE_CURRENT_BINARY_DIR}/grapehelp.zip" COPYONLY)

add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE
  grapeapp/grape_clipboard.cpp
  grapeapp/grape_frame.cpp
  grapeapp/grape_frame_events.cpp
  grapeapp/grape_glcanvas.cpp
  grapeapp/grape_icons.cpp
  grapeapp/grape_listbox.cpp
  grapeapp/grape_logpanel.cpp
  grapeapp/grape_menubar.cpp
  grapeapp/grape_toolbar.cpp
  grapeapp/main.cpp

  grapeapp/events/event_architecturediagram.cpp
  grapeapp/events/event_architecturereference.cpp
  grapeapp/events/event_base.cpp
  grapeapp/events/event_channelcommunication.cpp
  grapeapp/events/event_channel.cpp
  grapeapp/events/event_comment.cpp
  grapeapp/events/event_diagram.cpp
  grapeapp/events/event_edit.cpp
  grapeapp/events/event_file.cpp
  grapeapp/events/event_help.cpp
  grapeapp/events/event_initialdesignator.cpp
  grapeapp/events/event_input.cpp
  grapeapp/events/event_nonterminatingtransition.cpp
  grapeapp/events/event_object.cpp
  grapeapp/events/event_processdiagram.cpp
  grapeapp/events/event_processreference.cpp
  grapeapp/events/event_referencestate.cpp
  grapeapp/events/event_select.cpp
  grapeapp/events/event_state.cpp
  grapeapp/events/event_terminatingtransition.cpp
  grapeapp/events/event_transition.cpp

  grapeapp/visuals/geometric.cpp
  grapeapp/visuals/visualarchitecture_reference.cpp
  grapeapp/visuals/visualchannel_communication.cpp
  grapeapp/visuals/visualchannel.cpp
  grapeapp/visuals/visualcomment.cpp
  grapeapp/visuals/visualinitial_designator.cpp
  grapeapp/visuals/visualnonterminating_transition.cpp
  grapeapp/visuals/visualpreamble.cpp
  grapeapp/visuals/visualprocess_reference.cpp
  grapeapp/visuals/visualreference_state.cpp
  grapeapp/visuals/visualstate.cpp
  grapeapp/visuals/visualterminating_transition.cpp

  grapeapp/dialogs/channelcommunicationdialog.cpp
  grapeapp/dialogs/channeldialog.cpp
  grapeapp/dialogs/labeldialog.cpp
  grapeapp/dialogs/parameterdialog.cpp
  grapeapp/dialogs/preambledialog.cpp
  grapeapp/dialogs/referencedialog.cpp
  grapeapp/dialogs/textdialog.cpp

  libgrape/action.cpp
  libgrape/architecturediagram.cpp
  libgrape/architecturereference.cpp
  libgrape/channelcommunication.cpp
  libgrape/channel.cpp
  libgrape/comment.cpp
  libgrape/communication.cpp
  libgrape/compoundreference.cpp
  libgrape/compoundstate.cpp
  libgrape/coordinate.cpp
  libgrape/dataexpression.cpp
  libgrape/datatypespecification.cpp
  libgrape/decl.cpp
  libgrape/diagram.cpp
  libgrape/initialdesignator.cpp
  libgrape/label.cpp
  libgrape/nonterminatingtransition.cpp
  libgrape/object.cpp
  libgrape/preamble.cpp
  libgrape/processdiagram.cpp
  libgrape/processreference.cpp
  libgrape/referencestate.cpp
  libgrape/sortexpression.cpp
  libgrape/specification.cpp
  libgrape/state.cpp
  libgrape/terminatingtransition.cpp
  libgrape/transition.cpp
  libgrape/var.cpp
  libgrape/varupdate.cpp
  libgrape/xml.cpp
  libgrape/xmlopen.cpp

  mcrl2gen/mcrl2gen.cpp
  mcrl2gen/mcrl2gen_validate.cpp

  ${MACOSX_BUNDLE_ICON_FILE}
)

IF (APPLE)
 SET_SOURCE_FILES_PROPERTIES(${MACOSX_BUNDLE_ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
ENDIF (APPLE)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/grapeapp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/grapeapp/events)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/grapeapp/visuals)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/grapeapp/dialogs)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libgrape)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/mcrl2gen)

include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lps/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/aterm/include/aterm)
include_directories(${CMAKE_SOURCE_DIR}/build/precompile)
include_directories(${CMAKE_SOURCE_DIR}/build/workarounds)

if( MONO_LIB )
  target_link_libraries(${PROJECT_NAME}
    mcrl2_monolib
  )
else( MONO_LIB )
target_link_libraries(${PROJECT_NAME}
  ${wxWidgets_LIBRARIES}
  mcrl2_data
  mcrl2_core
  mcrl2_utilities_command_line
  mcrl2_utilities_wx
)

endif( MONO_LIB )

# install grape help file
install(FILES grapeapp/help/grapehelp.zip DESTINATION share/doc/mcrl2/grape)

# install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_BIN_DIR})

# Relocate install tree
include(${CMAKE_SOURCE_DIR}/scripts/RelocateInstallTree.cmake)

# Generate man pages
include(${CMAKE_SOURCE_DIR}/scripts/GenerateManPages.cmake)

endif ( MCRL2_BUILD_GUI_TOOLS )

