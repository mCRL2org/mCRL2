// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file simdialog.h
/// \brief Header file for simulation dialog class

#ifndef SIMDIALOG_H
#define SIMDIALOG_H
#include <wx/wx.h>
#include "simreader.h"

class Mediator;
class wxListView;
class wxListEvent;

class SimDialog: public wxDialog, public simReader
{
  public:
    SimDialog(wxWindow* parent, Mediator* mediator);

    // Event handlers
    void onSimStartButton(wxCommandEvent& event);
    void onSimResetButton(wxCommandEvent& event);
    void onSimStopButton(wxCommandEvent& event);
    void onSimTransitionSelected(wxListEvent& event);
    void onSimTransitionActivated(wxListEvent& event);
    void onKeyDown(wxKeyEvent& event);
    void onSimTriggerButton(wxCommandEvent& event);
    void onSimUndoButton(wxCommandEvent& event);
    void onGenerateBackTraceButton(wxCommandEvent& event);


    // Implemented for simReader interface
    virtual void refresh();
    virtual void selChange();
  private:
    // Buttons for simulation
    wxButton* simStartButton;
    wxButton* simBTButton;
    wxButton* simResetButton;
    wxButton* simStopButton;
    wxButton* simTriggerButton;
    wxButton* simUndoButton;

    // List views used in simulation
    wxListView* simTransView; // Transition information

    Mediator* mediator;

    DECLARE_EVENT_TABLE()
};

#endif //SIMDIALOG_H
