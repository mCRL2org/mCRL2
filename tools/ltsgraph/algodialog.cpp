// Author(s): Carst Tankink and Ali Deniz Aladagli
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file algodialog.cpp
/// \brief Implementation of algorithm dialog

#include "wx.hpp" // precompiled headers

#include "algodialog.h"
#include <wx/sizer.h>
#include <wx/notebook.h>
#include "ids.h"

using namespace IDS;
BEGIN_EVENT_TABLE(AlgoDialog, wxPanel)
  EVT_SPINCTRL(myID_RADIUS_SPIN, AlgoDialog::onRadius)
  EVT_CHECKBOX(myID_CURVES_CHECK, AlgoDialog::onCurves)
  EVT_CHECKBOX(myID_TRANS_CHECK, AlgoDialog::onTransLabels)
  EVT_CHECKBOX(myID_STATE_CHECK, AlgoDialog::onStateLabels)
  EVT_COLOURPICKER_CHANGED(myID_COLOUR, AlgoDialog::onColour)
END_EVENT_TABLE()

AlgoDialog::AlgoDialog(LTSGraph* owner, wxWindow* parent)
  : wxPanel(parent, wxID_ANY, wxDefaultPosition,
            wxDefaultSize
            //,wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER
           )

{
  app = owner;

  size_t nrAlgos = app->getNumberOfAlgorithms();

  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

  wxPanel* panel_optimize = new wxPanel(this,wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_DOUBLE);

  wxBoxSizer* innerSizer1 = new wxBoxSizer(wxVERTICAL);

  if (nrAlgos > 1)
  {
    wxNotebook* nb = new wxNotebook(
      panel_optimize, wxID_ANY, wxDefaultPosition,
      wxDefaultSize, wxNB_TOP);

    for (size_t i = 0; i < nrAlgos; ++i)
    {
      LayoutAlgorithm* algo = app->getAlgorithm(i);
      wxPanel* algoWindow = new wxPanel(nb, wxID_ANY);

      algo->setupPane(algoWindow);

      //TODO: Get algorithm name
      nb->AddPage(algoWindow, wxT("Spring embedder"));
    }
    innerSizer1->Add(nb, 0, wxEXPAND|wxALL, 5);
  }
  else
  {
    LayoutAlgorithm* algo = app->getAlgorithm(0);

    wxPanel* algoWindow = new wxPanel(panel_optimize, wxID_ANY);

    algo->setupPane(algoWindow);
    PushEventHandler(algo);

    innerSizer1->Add(algoWindow, 0, wxEXPAND|wxALL, 5);
  }

  sizer->Add(panel_optimize, 0, wxEXPAND|wxALL, 5);

  /* Curves */

  int lflags = wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL;
  int rflags = wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxEXPAND | wxALL;

  wxFlexGridSizer* settingsSizer = new wxFlexGridSizer(0, 1, 0, 0);
  settingsSizer->AddGrowableCol(0);

  wxFlexGridSizer* radiusSizer = new wxFlexGridSizer(0, 2, 0, 0);
  radiusSizer->AddGrowableCol(0);

  wxSpinCtrl* radiusControl = new wxSpinCtrl(this, myID_RADIUS_SPIN,
      wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 20, 10);

  radiusSizer->Add(new wxStaticText(this, wxID_ANY, wxT("State radius")), 0,
                   lflags, 4);

  radiusSizer->Add(radiusControl, 0, rflags, 3);

  settingsSizer->Add(radiusSizer, wxEXPAND|wxALL);



  wxFlexGridSizer* colourSizer = new wxFlexGridSizer(0, 2, 0, 0);
  colourSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Brush colour")), 0,
                   lflags, 4);

  wxColourPickerCtrl* colourControl = new wxColourPickerCtrl(this, myID_COLOUR,
      *wxRED);
  colourSizer->Add(colourControl, 0, rflags, 3);

  settingsSizer->Add(colourSizer);
  wxCheckBox* showTransLabels = new wxCheckBox(this, myID_TRANS_CHECK,
      wxT("Show transition labels"));
  showTransLabels->SetValue(true);
  wxCheckBox* showStateLabels = new wxCheckBox(this, myID_STATE_CHECK,
      wxT("Show state labels"));
  showStateLabels->SetValue(true);

  settingsSizer->Add(showTransLabels);
  settingsSizer->Add(showStateLabels);

  wxCheckBox* allowCurves = new wxCheckBox(this, myID_CURVES_CHECK,
      wxT("Edit transition curves"));

  settingsSizer->Add(allowCurves);

  sizer->Add(settingsSizer, 0, wxEXPAND|wxALL, 5);

  SetSizer(sizer);
  Fit();
  Layout();
}

void AlgoDialog::onRadius(wxSpinEvent& evt)
{
  app->setRadius(evt.GetPosition());
}

void AlgoDialog::onCurves(wxCommandEvent& evt)
{
  app->setCurves(evt.IsChecked());
}

void AlgoDialog::onTransLabels(wxCommandEvent& evt)
{
  app->setTransLabels(evt.IsChecked());
}

void AlgoDialog::onStateLabels(wxCommandEvent& evt)
{
  app->setStateLabels(evt.IsChecked());
}

void AlgoDialog::onColour(wxColourPickerEvent& evt)
{
  app->setBrushColour(evt.GetColour());
}

