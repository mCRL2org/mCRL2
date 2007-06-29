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
    myID_SELECT, myID_ZOOM, myID_PAN, myID_ROTATE, 
    // Drawing style identifiers
    myID_CONES_STYLE, myID_TUBES_STYLE, 
    // Event identifiers
    myID_MARK_RADIOBUTTON, myID_MARK_ANYALL, myID_MARK_RULES, myID_ADD_RULE, 
    myID_REMOVE_RULE, myID_MARK_TRANSITIONS, myID_PARAMETER_CHOICE, 
    myID_DISPLAY_STATES, myID_DISPLAY_TRANSITIONS, myID_DISPLAY_BACKPOINTERS, 
    myID_DISPLAY_WIREFRAME, myID_ITERATIVE, myID_CYCLIC, myID_ANY, 
    // Simulation event identifiers
    myID_SIM_START_BUTTON, myID_SIM_RESET_BUTTON, myID_SIM_STOP_BUTTON, 
    myID_SIM_TRANSITIONS_VIEW, myID_SIM_TRIGGER_BUTTON, myID_SIM_UNDO_BUTTON, 
    myID_SIM_STATE_VIEW,
    };
}
#endif
