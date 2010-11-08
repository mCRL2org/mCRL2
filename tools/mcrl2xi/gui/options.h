// Author(s): Frank Stappers 
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file options.h

#ifndef MCRL2XI_OPTIONS_H_
#define MCRL2XI_OPTIONS_H_

#include <wx/wx.h>
#include <wx/gbsizer.h>
#include <wx/textctrl.h>
#include "editor.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/rewriter.h"

enum {
	OPTION_EVAL = 900
};

class Options: public wxPanel {

public:

  Options(wxWindow *parent, wxWindowID id, xEditor *editor, wxTextCtrl *output, mcrl2::data::rewriter::strategy rewrite_strategy);
	void OnEval(wxCommandEvent& /*event*/);
	void OnSize(wxSizeEvent& /*event*/);
	void UpdateFocus( wxCommandEvent& event );
private:
  wxTextCtrl *EvalExpr;
	xEditor *p_editor;
  wxTextCtrl *p_output;

  mcrl2::data::rewriter::strategy m_rewrite_strategy;

DECLARE_EVENT_TABLE()
};

#endif
