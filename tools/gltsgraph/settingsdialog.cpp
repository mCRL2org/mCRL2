#include "wx.hpp" // precompiled headers

#include "settingsdialog.h"
#include <wx/sizer.h>
#include "ids.h"

using namespace IDS;
BEGIN_EVENT_TABLE(SettingsDialog, wxDialog)
  EVT_SPINCTRL(myID_RADIUS_SPIN, SettingsDialog::onRadius)
  EVT_CHECKBOX(myID_CURVES_CHECK, SettingsDialog::onCurves)
END_EVENT_TABLE()

SettingsDialog::SettingsDialog(GLTSGraph* owner, wxWindow* parent)
  : wxDialog(parent, wxID_ANY, wxT("Visualization settings"), wxDefaultPosition,
             wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
{
  app = owner;
  
  int lflags = wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL;
  int rflags = wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxEXPAND | wxALL;


  wxFlexGridSizer* sizer = new wxFlexGridSizer(0, 2, 0, 0);

  wxSpinCtrl* radiusControl = new wxSpinCtrl(this, myID_RADIUS_SPIN, 
    wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 20, 10);

  sizer->Add(new wxStaticText(this, wxID_ANY, wxT("State radius")), 0, 
             lflags, 4);

  sizer->Add(radiusControl, 0, rflags, 3);
  
  wxCheckBox* allowCurves = new wxCheckBox(this, myID_CURVES_CHECK, 
    wxT("Edit transition curves"));
  
  sizer->Add(allowCurves);

  SetSizer(sizer);
  Fit();
  Layout();
  // TODO: Setup option fields + triggers
}

void SettingsDialog::onRadius(wxSpinEvent& evt)
{
  app->setRadius(evt.GetPosition());
}

void SettingsDialog::onCurves(wxCommandEvent& evt)
{
  app->setCurves(evt.IsChecked());
}
