#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H
#include <wx/wx.h>
#include <wx/spinctrl.h>
#include "mcrl2/utilities/colorbutton.h"
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
    wxCheckBox*	   ntrCheck;
    wxColorButton* bgButton;  // Background colour
    wxColorButton* deButton;  // Down edge (transition) colour
    wxColorButton* i1Button;  // First interpolation colour
    wxColorButton* i2Button;  // Second interpolation colour
    wxColorButton* mkButton;  // Mark colour
    wxColorButton* ndButton;  // Node (state) colour
    wxColorButton* ueButton;  // Up edge (backpointer) colour
    wxColorButton* scButton;  // Simulation Current colour
    wxColorButton* spButton;  // Simulation Possible colour
    wxColorButton* ssButton;  // Simulation Selected colour
    wxColorButton* shButton;  // Simulation History colour
    wxSpinCtrl*	   brSpin;
    wxSpinCtrl*    nsSpin;
    wxSpinCtrl*	   obSpin;
    wxSpinCtrl*	   qlSpin;
    wxSpinCtrl*	   trSpin;
    void setupClrPanel(wxPanel* panel);
    void setupParPanel(wxPanel* panel);
    void setupSimPanel(wxPanel* panel);
    void setupPfmPanel(wxPanel* panel);
    wxColour RGB_to_wxC(Utils::RGB_Color c) const;
    Utils::RGB_Color wxC_to_RGB(wxColour c) const;
    DECLARE_EVENT_TABLE()
};

#endif
