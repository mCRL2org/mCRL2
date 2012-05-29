// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ids.h
/// \brief Defines IDs for wxWidget controls

#ifndef IDS_H
#define IDS_H
#include <wx/wx.h>

namespace IDs
{
enum
{
  myID_SAVEPIC = wxID_HIGHEST + 1,
  myID_SAVEVEC,
  myID_SAVETXT,
  // Tool identifiers
  myID_SELECT,
  myID_ZOOM,
  myID_PAN,
  myID_ROTATE,
  myID_INFO,
  myID_MARK,
  myID_SIM,
  // Drawing style identifiers
  myID_CONES_STYLE,
  myID_TUBES_STYLE,
  myID_FSM_STYLE,
  // Event identifiers
  myID_MARK_RADIOBUTTON,
  myID_MARK_ANYALL,
  myID_MARK_CLUSTER,
  myID_MARK_RULES,
  myID_ADD_RULE,
  myID_REMOVE_RULE,
  myID_MARK_TRANSITIONS,
  myID_PARAMETER_CHOICE,
  myID_DISPLAY_STATES,
  myID_DISPLAY_TRANSITIONS,
  myID_DISPLAY_BACKPOINTERS,
  myID_DISPLAY_WIREFRAME,
  myID_ITERATIVE,
  myID_CYCLIC,
  myID_ANY,
  myID_ZOOM_IN_ABOVE,
  myID_ZOOM_IN_BELOW,
  myID_ZOOM_OUT,
  myID_OPEN_TRACE,
  // Simulation event identifiers
  myID_SIM_START_BUTTON,
  myID_SIM_RESET_BUTTON,
  myID_SIM_STOP_BUTTON,
  myID_SIM_TRANSITIONS_VIEW,
  myID_SIM_TRIGGER_BUTTON,
  myID_SIM_UNDO_BUTTON,
  myID_SIM_STATE_VIEW,
  myID_SIM_BT_BUTTON,
  // Setting controls identifiers
  myID_LONG_INTERPOLATION,
  myID_NAV_SHOW_BACKPOINTERS,
  myID_NAV_SHOW_STATES,
  myID_NAV_SHOW_TRANSITIONS,
  myID_NAV_SMOOTH_SHADING,
  myID_NAV_LIGHTING,
  myID_NAV_TRANSPARENCY,
  myID_BACKGROUND_CLR,
  myID_DOWN_EDGE_CLR,
  myID_INTERPOLATE_CLR_1,
  myID_INTERPOLATE_CLR_2,
  myID_MARK_CLR,
  myID_STATE_CLR,
  myID_UP_EDGE_CLR,
  myID_SIM_CURR_CLR,
  myID_SIM_POS_CLR,
  myID_SIM_SEL_CLR,
  myID_SIM_PREV_CLR,
  myID_BRANCH_ROTATION,
  myID_STATE_SIZE,
  myID_CLUSTER_HEIGHT,
  myID_BRANCH_TILT,
  myID_QUALITY,
  myID_TRANSPARENCY,
  myID_SP_STATEPOS,
  myID_MP_STATEPOS
};
}
#endif
