// Author(s): Bas Ploeger and Carst Tankink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ids.h
/// \brief Add your file description here.

#ifndef IDS_H
#define IDS_H
#include <wx/wx.h>

namespace IDs
{
  enum { myID_SAVEPIC = wxID_HIGHEST + 1, 
    // Tool identifiers
    myID_SELECT,
    myID_ZOOM,
    myID_PAN,
    myID_ROTATE,
    // Drawing style identifiers
    myID_CONES_STYLE,
    myID_TUBES_STYLE,
    myID_FSM_STYLE,
    // Event identifiers
    myID_MARK_RADIOBUTTON,
    myID_MARK_ANYALL,
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
    myID_START_FORCE_DIRECTED,
    myID_STOP_FORCE_DIRECTED,
    myID_RESET_STATE_POSITIONS,
    // Simulation event identifiers
    myID_SIM_START_BUTTON,
    myID_SIM_RESET_BUTTON,
    myID_SIM_STOP_BUTTON,
    myID_SIM_TRANSITIONS_VIEW,
    myID_SIM_TRIGGER_BUTTON,
    myID_SIM_UNDO_BUTTON,
    myID_SIM_STATE_VIEW, myID_SIM_BT_BUTTON,
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
    myID_BRANCH_TILT,
    myID_QUALITY,
    myID_TRANSPARENCY,
    myID_TRANSITION_ATTRACTION,
    myID_TRANSITION_LENGTH,
    myID_STATE_REPULSION
  };
}
#endif
