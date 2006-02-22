#ifndef SPINCTRLFLOAT_H
#define SPINCTRLFLOAT_H
#include <string>
#include <wx/wx.h>
#include <wx/spinbutt.h>
#include "utils.h"
using namespace std;
using namespace Utils;

class wxSpinCtrlFloat : public wxPanel
{
  public:
    wxSpinCtrlFloat(wxWindow* parent, wxWindowID id, float p_Min, float p_Max,
	float p_Rate, float p_Init, const wxPoint& pos = wxDefaultPosition,
	const wxSize& size = wxDefaultSize);

    float GetValue();
    void SetValue(float v);
protected:
    void OnEnter(wxCommandEvent & event);
    void OnSpin(wxSpinEvent & event);
    wxTextCtrl* m_TextCtrl;
    wxSpinButton* m_SpinButton;
	
    float m_Rate;
    int m_Min;
    int m_Max;
    int m_ID;
      
    DECLARE_EVENT_TABLE()
};


#endif
