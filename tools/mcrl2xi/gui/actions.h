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
#include "outputpanel.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/rewriter.h"
#include "subactions.h"

enum
{
  OPTION_EVAL = 900,
  OPTION_TC,
  OPTION_SOLVE,
  OPTION_SOLVE_ABORT,
  OPTION_SOLVE_CLEAR

};

class Options: public wxPanel
{

  public:

    Options(wxWindow* parent, wxWindowID id, xEditor* editor, outputpanel* output, mcrl2::data::rewriter::strategy rewrite_strategy);
    void OnEval(wxCommandEvent& /*event*/);
    void SolveExpr(wxCommandEvent& /*event*/);
    void OnTypeCheck(wxCommandEvent& /*event*/);
    void OnSize(wxSizeEvent& /*event*/);
  private:
//  wxTextCtrl *EvalExpr;
//  wxPanel *data_expression_panel;
    xEditor* p_editor;
    outputpanel* p_output;

    typeCheckSpec* tc;
    evalDataExpr* ev;
    solveDataExpr* sd;


    mcrl2::data::rewriter::strategy m_rewrite_strategy;

    DECLARE_EVENT_TABLE()
};

#endif
