// Author(s): Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file settingsdialog.cpp
/// \brief Settings dialog.

#include "wx.hpp" // precompiled headers

#include "settingsdialog.h"
#include <wx/sizer.h>
#include "ids.h"

using namespace IDS;
BEGIN_EVENT_TABLE(SettingsDialog, wxDialog)
  EVT_SPINCTRL(myID_RADIUS_SPIN, SettingsDialog::onRadius)
  EVT_CHECKBOX(myID_CURVES_CHECK, SettingsDialog::onCurves)
  EVT_COLOURPICKER_CHANGED(myID_COLOUR, SettingsDialog::onColour)
  EVT_RADIOBOX(myID_TOOL_SELECT, SettingsDialog::onTool)
END_EVENT_TABLE()

SettingsDialog::SettingsDialog(LTSGraph* owner, wxWindow* parent)
  : wxDialog(parent, wxID_ANY, wxT("Visualization settings"), wxDefaultPosition,
             wxDefaultSize, wxDEFAULT_DIALOG_STYLE)
{
  app = owner;
  
  int lflags = wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL;
  int rflags = wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxEXPAND | wxALL;


  wxFlexGridSizer* sizer = new wxFlexGridSizer(0, 1, 0, 0);
  sizer->AddGrowableCol(0);

  wxFlexGridSizer* radiusSizer = new wxFlexGridSizer(0, 2, 0, 0);
  radiusSizer->AddGrowableCol(0);

  wxSpinCtrl* radiusControl = new wxSpinCtrl(this, myID_RADIUS_SPIN, 
    wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 20, 10);
  
  radiusSizer->Add(new wxStaticText(this, wxID_ANY, wxT("State radius")), 0, 
             lflags, 4);

  radiusSizer->Add(radiusControl, 0, rflags, 3);
  
  sizer->Add(radiusSizer, wxEXPAND|wxALL);

  wxString choices[2] = {wxT("Selection"), wxT("State colouring")};

  wxRadioBox* toolSelect = new wxRadioBox(this, myID_TOOL_SELECT,
    wxT("Tool"), wxDefaultPosition, wxDefaultSize, 2, 
    choices );
  
  sizer->Add(toolSelect, wxEXPAND|wxALL);
  
  wxFlexGridSizer* colourSizer = new wxFlexGridSizer(0, 2, 0, 0);
  colourSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Brush colour")), 0, 
              lflags, 4);
  
  wxColourPickerCtrl* colourControl = new wxColourPickerCtrl(this, myID_COLOUR);
  colourSizer->Add(colourControl, 0, rflags, 3);
  
  sizer->Add(colourSizer);
  wxCheckBox* allowCurves = new wxCheckBox(this, myID_CURVES_CHECK, 
    wxT("Edit transition curves"));
  
  sizer->Add(allowCurves);

  SetSizer(sizer);
  Fit();
  Layout();
}

void SettingsDialog::onRadius(wxSpinEvent& evt)
{
  app->setRadius(evt.GetPosition());
}

void SettingsDialog::onCurves(wxCommandEvent& evt)
{
  app->setCurves(evt.IsChecked());
}

void SettingsDialog::onColour(wxColourPickerEvent& evt)
{
  app->setBrushColour(evt.GetColour());
}

void SettingsDialog::onTool(wxCommandEvent &evt)
{
  app->setTool(evt.GetInt() == 1);
}
