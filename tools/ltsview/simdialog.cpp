// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file simdialog.cpp
/// \brief Source file for simulation dialog class

#include "wx.hpp" // precompiled headers

#include "simdialog.h"
#include <vector>
#include <wx/listctrl.h>
#include "ids.h"
#include "mediator.h"
#include "state.h"
#include "transition.h"

using namespace IDs;
using namespace std;

BEGIN_EVENT_TABLE(SimDialog, wxDialog)
  EVT_BUTTON(myID_SIM_START_BUTTON, SimDialog::onSimStartButton)
  EVT_BUTTON(myID_SIM_RESET_BUTTON, SimDialog::onSimResetButton)
  EVT_BUTTON(myID_SIM_STOP_BUTTON, SimDialog::onSimStopButton)

  EVT_LIST_ITEM_SELECTED(myID_SIM_TRANSITIONS_VIEW,
                         SimDialog::onSimTransitionSelected)
  EVT_LIST_ITEM_ACTIVATED(myID_SIM_TRANSITIONS_VIEW,
                          SimDialog::onSimTransitionActivated)
  EVT_CHAR(SimDialog::onKeyDown)
  EVT_BUTTON(myID_SIM_TRIGGER_BUTTON, SimDialog::onSimTriggerButton)
  EVT_BUTTON(myID_SIM_UNDO_BUTTON, SimDialog::onSimUndoButton)
  EVT_BUTTON(myID_SIM_BT_BUTTON, SimDialog::onGenerateBackTraceButton)
END_EVENT_TABLE()

SimDialog::SimDialog(wxWindow* parent, Mediator* owner)
  : wxDialog(parent, wxID_ANY, wxT("Simulation"), wxDefaultPosition,
             wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER),
    simReader(NULL)
{
  mediator = owner;
  // Container for all elements in tab.
  wxFlexGridSizer* simSizer = new wxFlexGridSizer(3, 1, 0, 0);
  simSizer->AddGrowableCol(0);
  simSizer->AddGrowableRow(1);
  simSizer->AddGrowableRow(2);

  int flags = wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL;
  int border = 3;

  // Buttons for general simulation control: start, backtrace, reset, stop
  wxFlexGridSizer* simButtonSizer = new wxFlexGridSizer(2, 2, 0, 0);
  simButtonSizer->AddGrowableCol(0);
  simButtonSizer->AddGrowableCol(1);

  simStartButton = new wxButton(this, myID_SIM_START_BUTTON, wxT("Start"));
  simStartButton->Disable();

  // TODO: Rephrase button label?
  simBTButton = new wxButton(this, myID_SIM_BT_BUTTON, wxT("Backtrace"));
  simBTButton->Disable();

  simResetButton = new wxButton(this, myID_SIM_RESET_BUTTON,
                                          wxT("Reset"));
  simResetButton->Disable();

  simStopButton  = new wxButton(this, myID_SIM_STOP_BUTTON,
                                          wxT("Stop"));
  simStopButton->Disable();

  simButtonSizer->Add(simStartButton, 0, flags, border);
  simButtonSizer->Add(simStopButton,  0, flags, border);
  simButtonSizer->Add(simBTButton, 0, flags, border);
  simButtonSizer->Add(simResetButton, 0, flags, border);


  simSizer->Add(simButtonSizer, 1, flags, border);

  // List of transitions and buttons to fire these transitions and go back one
  // step
  wxStaticBoxSizer* simTransSizer = new wxStaticBoxSizer(wxVERTICAL, this,
    wxT("Transitions"));
  int listViewStyle = wxLC_REPORT|wxSUNKEN_BORDER|wxLC_HRULES|wxLC_VRULES|
                      wxLC_SINGLE_SEL;

  simTransView = new wxListView(this, myID_SIM_TRANSITIONS_VIEW,
    wxDefaultPosition, wxSize(200, 100), listViewStyle);
  //simTransView->SetMaxSize(wxSize(200, 100));
  simTransView->InsertColumn(0, wxT("Action"), wxLIST_FORMAT_LEFT, 120);
  simTransView->InsertColumn(1, wxT("State change"), wxLIST_FORMAT_LEFT);
  simTransView->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER|wxLIST_AUTOSIZE);

  simTransSizer->Add(simTransView, 1, flags|wxEXPAND, border);

  wxBoxSizer* triggerUndoSizer = new wxBoxSizer(wxHORIZONTAL);
  simTriggerButton = new wxButton(this, myID_SIM_TRIGGER_BUTTON,
                                         wxT("Trigger"));
  simTriggerButton->Disable();
  triggerUndoSizer->Add(simTriggerButton, 0, flags, border);

  simUndoButton = new wxButton(this, myID_SIM_UNDO_BUTTON, wxT("Undo"));
  simUndoButton->Disable();
  triggerUndoSizer->Add(simUndoButton, 0, flags, border);
  simTransSizer->Add(triggerUndoSizer, 0, flags, border);

  simSizer->Add(simTransSizer, 0, wxEXPAND|wxALL, border);


  // Information about current state
  /*wxStaticBoxSizer* simStateSizer = new wxStaticBoxSizer(wxVERTICAL, panel,
                                                         wxT("Current state"));
  simStateView = new wxListView(panel, myID_SIM_STATE_VIEW,
                                         wxDefaultPosition, wxSize(200, 100),
                                         listViewStyle);
  simStateView->InsertColumn(0, wxT("Parameter"), wxLIST_FORMAT_LEFT, 120);
  simStateView->InsertColumn(1, wxT("Value"), wxLIST_FORMAT_LEFT);
  simStateView->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER|wxLIST_AUTOSIZE);

  simStateSizer->Add(simStateView, 1, flags|wxEXPAND, border);
  simSizer->Add(simStateSizer, 0, wxEXPAND|wxALL, border);*/

  SetSizer(simSizer);
  Fit();
  Layout();

  // Now the panel has been laid out, we can get fill up the columns
  simTransView->SetColumnWidth(1, simTransView->GetClientSize().GetWidth() -
                               simTransView->GetColumnWidth(0));
  /*simStateView->SetColumnWidth(1, simStateView->GetClientSize().GetWidth() -
                               simStateView->GetColumnWidth(0));*/
}

// Simulation event handlers implementations
void SimDialog::onSimStartButton(wxCommandEvent& event) {
  mediator->startSim();
}

void SimDialog::onSimResetButton(wxCommandEvent& event) {
  sim->resetSim();
}

void SimDialog::onSimStopButton(wxCommandEvent& event) {
  sim->stop();
}

void SimDialog::onSimTransitionSelected(wxListEvent& event) {
  // Get index of transition that was selected
  int trans = event.GetIndex();

  // Choose trans to be the next transition
  sim->chooseTrans(trans);

}

void SimDialog::onSimTransitionActivated(wxListEvent& event) {
  onSimTransitionSelected(event);
  sim->followTrans();
}

void SimDialog::onKeyDown(wxKeyEvent& event)
{
  // Workaround: Catch the space
  if (event.GetKeyCode() == WXK_SPACE)
  {
  }
  else
  {
    event.Skip();
  }
}

void SimDialog::onSimTriggerButton(wxCommandEvent& event) {
  sim->followTrans();
}

void SimDialog::onSimUndoButton(wxCommandEvent& event) {
  sim->undoStep();
}

void SimDialog::onGenerateBackTraceButton(wxCommandEvent& event)
{
  mediator->generateBackTrace();
}

void SimDialog::refresh() {
  if (sim != NULL) {
    // There is a simulation, so we can request information from it.
    if (!sim->getStarted()) {
      // The simulation has not yet been started, enable the start button.
      simStartButton->Enable();
      simResetButton->Disable();
      simStopButton->Disable();
      simBTButton->Disable();

      // Clear the list view
      simTransView->DeleteAllItems();
    }
    else {
      // The simluation has been started, disable start button, enable stop and
      // reset buttons
      simStartButton->Disable();

      simResetButton->Enable();
      simStopButton->Enable();
      simBTButton->Enable();


      // Refresh the transition list
      simTransView->DeleteAllItems();

      vector<Transition*> posTrans = sim->getPosTrans();

      State* currState = sim->getCurrState();

      // Get the possible transitions
      for(size_t i = 0; i < posTrans.size(); ++i) {
        int labelId = posTrans[i]->getLabel();
        string label = mediator->getActionLabel(labelId);

        simTransView->InsertItem(i, wxString(label.c_str(), wxConvLocal));

        // Determine the state change this action will effectuate.
        State* nextState = posTrans[i]->getEndState();

        wxString stateChange = wxT("");
        if ((nextState != NULL) && (currState != NULL)) {
          for(int j = 0; j < mediator->getNumberOfParams(); ++j) {
            string nextVal = mediator->getParValue(nextState,j);
            if (mediator->getParValue(currState,j) != nextVal) {
              stateChange += wxString(mediator->getParName(j).c_str(),
                               wxConvLocal) +
                            wxT(":=") +
                            wxString(nextVal.c_str(), wxConvLocal) +
                            wxT(",");
            }
          }
          // Remove last comma. There always is one since an empty transition
          // does not exist
          stateChange.RemoveLast();
        }
        // Add stateChange value to the list
        simTransView->SetItem(i, 1, stateChange);
      }

      // Display selected transition
      int chosenTrans = sim->getChosenTransi();

      if(chosenTrans != -1) {
        simTransView->Select(chosenTrans);
      }

      // Trigger and undo buttons
      if(chosenTrans != -1) {
        simTriggerButton->Enable();
      }
      else {
        simTriggerButton->Disable();
      }

      if(sim->getTransHis().size() != 0) {
        simUndoButton->Enable();
      }
      else {
        simUndoButton->Disable();
      }
    }
  }
  Layout();
}

void SimDialog::selChange() {
  // There always is a simulation to inform us of a selection change.
  int j = sim->getChosenTransi();

  if(j != -1)
  {
    simTransView->Select(j);
    simTriggerButton->Enable();
  }
  else
  {
    simTriggerButton->Disable();
  }
}
