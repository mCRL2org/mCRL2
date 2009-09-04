// Author(s): Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ids.h
/// \brief Identifiers

#ifndef IDS_H
#define IDS_H
#include <wx/wx.h>

namespace IDS
{
  enum {
    myID_MENU_EXPORT = wxID_HIGHEST + 1,
    myID_IMPORT,
    myID_DISPLAY_STATE_LBL,
    myID_DISPLAY_TRANS_LBL,
    myID_DLG_INFO,
    myID_DLG_ALGO,
    myID_TOGGLE_POSITIONING,
    myID_TOGGLE_VECTOR,
	myID_TOGGLE_3D,

    myID_NS_SLIDER,
    myID_ES_SLIDER,
    myID_NL_SLIDER,
    myID_GF_SLIDER,
    myID_TP_SLIDER,
    myID_START_OPTI,
    myID_STOP_OPTI,
    myID_RADIUS_SPIN,
    myID_CURVES_CHECK,
    myID_TRANS_CHECK,
    myID_STATE_CHECK,
    myID_COLOUR,
    myID_SELECT,
	myID_ROTATE,
	myID_PAN,
	myID_ZOOM,
	myID_RESET_ALL,
	myID_RESET_ROTATE,
	myID_RESET_PAN,
	myID_SHOW_SYSTEM,
	myID_NONE
  };

  enum PickState {TRANSITION, SELF_LOOP, STATE, LABEL, SELF_LABEL};
}

#endif //IDS_H
