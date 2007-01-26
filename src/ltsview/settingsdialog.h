#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H
#include <wx/wx.h>
#include <wx/spinctrl.h>
#include "colorbutton.h"
#include "glcanvas.h"
#include "settings.h"
#include "utils.h"

class SettingsDialog: public wxDialog {
  public:
    SettingsDialog(wxWindow* parent,GLCanvas* glc,Settings* ss);
    void onButton(wxCommandEvent& event);
    void onCheck(wxCommandEvent& event);
    void onSpin(wxSpinEvent& event);
  private:
    GLCanvas*      glCanvas;
    Settings*      settings;
    wxCheckBox*	   liCheck;
    wxCheckBox*	   nsbCheck;
    wxCheckBox*	   nssCheck;
    wxCheckBox*	   nstCheck;
    wxCheckBox*	   nshCheck;
    wxCheckBox*	   nliCheck;
    wxColorButton* bgButton;
    wxColorButton* deButton;
    wxColorButton* i1Button;
    wxColorButton* i2Button;
    wxColorButton* mkButton;
    wxColorButton* ndButton;
    wxColorButton* ueButton;
    wxSpinCtrl*	   brSpin;
    wxSpinCtrl*    elSpin;
    wxSpinCtrl*	   ibSpin;
    wxSpinCtrl*    nsSpin;
    wxSpinCtrl*	   obSpin;
    wxSpinCtrl*	   qlSpin;
    wxSpinCtrl*	   trSpin;
    void setupClrPanel(wxPanel* panel);
    void setupParPanel(wxPanel* panel);
    void setupPfmPanel(wxPanel* panel);
    wxColour RGB_to_wxC(Utils::RGB_Color c) const;
    Utils::RGB_Color wxC_to_RGB(wxColour c) const;
    DECLARE_EVENT_TABLE()
};

#endif
