// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file subactions.h

#ifndef MCRL2XI_EVALDATAEXPRESSION_H_
#define MCRL2XI_EVALDATAEXPRESSION_H_
#include "xstceditor.h"
#include "outputpanel.h"
#include "wx/textctrl.h"
#include <wx/aui/auibook.h>
#include "mytextctrl.h"

class typeCheckSpec: public wxPanel
{
  public:
    typeCheckSpec(wxWindow* parent, wxWindowID id);
  private:
    void OnTypeCheck(wxCommandEvent& e);
    wxWindow* p_parent;

    DECLARE_EVENT_TABLE()
};

class evalDataExpr: public wxPanel
{
  public:
    evalDataExpr(wxWindow* parent, wxWindowID id);
    wxString getDataExprVal();
  private:
    void OnEval(wxCommandEvent& e);
    wxWindow* p_parent;
    myTextControl* EvalExpr;

    DECLARE_EVENT_TABLE()
};

class solveDataExpr: public wxPanel
{
  public:
    solveDataExpr(wxWindow* parent, wxWindowID id);
    myTextControl* getSolutionWindow();
    wxString getDataExprSolve();
    bool getStopSolving();
  private:
    void OnStopSolve(wxCommandEvent& /*e*/);
    void OnSolve(wxCommandEvent& e);
    wxWindow* p_parent;
    myTextControl* p_solutions;
    myTextControl* SolveExpr;
    wxButton* solveBtn;
    wxButton* solveStopBtn;
    bool StopSolving;

    DECLARE_EVENT_TABLE()
};


#endif
