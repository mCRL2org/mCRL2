// --- spinctrlfloat.h ------------------------------------------------
// (c) 2006 -  S.W.C. Ploeger, D. Reniers  - Eindhoven Univ Technology
// ---------------------------  *  ------------------------------------


#ifndef SPINCTRLFLOAT_H
#define SPINCTRLFLOAT_H

#include <cmath>
#include <string>
#include <wx/wx.h>
#include <wx/spinbutt.h>
#include <wx/spinctrl.h>
#include "utils.h"
using namespace std;

class wxSpinCtrlFloat : public wxPanel
{
  public:
    wxSpinCtrlFloat(
        wxWindow* parent, 
        wxWindowID id, 
        double p_Min, 
        double p_Max,
	    double p_Rate, 
        double p_Init, 
        const wxPoint& pos = wxDefaultPosition,
	    const wxSize& size = wxDefaultSize );

    double GetValue();
    void SetValue(float v);
protected:
    void OnEnter(wxCommandEvent & event);
    void OnSpin(wxSpinEvent & event);
    wxTextCtrl* m_TextCtrl;
    wxSpinButton* m_SpinButton;
	
    double m_Rate;
    int m_Min;
    int m_Max;
    int m_ID;
      
    DECLARE_EVENT_TABLE()
};


#endif
