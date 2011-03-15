// Author(s): Frank Stappers
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file settings.h

#ifndef MCRL2XI_SETTINGS_H_
#define MCRL2XI_SETTINGS_H_

#include "wx/wx.h"

namespace mcrl2xi
{

static wxString mcrl2_spec_keywords(wxT("sort cons map var eqn act proc init struct glob "));
static wxString mcrl2_proc_keywords(wxT("delta tau sum block allow hide rename comm "));
static wxString mcrl2_sort_keywords(wxT("Bool Pos Nat Int Real Set List Bag "));
static wxString mcrl2_data_keywords(wxT("true false whr end lambda forall exists div mod in if "));
static wxString mcrl2_operator_keywords(wxT("min max succ pred abs floor ceil round exp A2B head tail rhead rtail count Set2Bag Bag2Set "));

// IDs for the controls and the menu commands
enum
{
  Exec_PerspectiveReset = 300,

  Exec_ToggleOutputPanel,
  Exec_ToggleOptionsPanel,
  myID_EVALEXPR,
  myID_TYPECHECKSPEC,
  myID_WRAPMODE
};

static wxString mcrl2_files = wxT("mCRL2 files (*.mcrl2)|*.mcrl2|LPS files (*.lps)|*.lps|TXT files (*.txt)|*.txt");

}

#endif /* SETTINGS_H_ */
