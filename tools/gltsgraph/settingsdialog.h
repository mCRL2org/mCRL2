#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/clrpicker.h>
#include "gltsgraph.h"

class SettingsDialog: public wxDialog {
  public:
    SettingsDialog(GLTSGraph* app, wxWindow* parent);
    

  private:
    GLTSGraph* app;
    void onRadius(wxSpinEvent& evt);
    void onCurves(wxCommandEvent& evt);
    void onColour(wxColourPickerEvent& evt);
    void onTool(wxCommandEvent& evt);
  
  DECLARE_EVENT_TABLE()
};



#endif // SETTINGS_DIALOG_H
