// Author(s): Frank Stappers 
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file configpanel.h


#ifndef MCRL2_GUI_OUTPUTCONFIGPANEL_H_
#define MCRL2_GUI_OUTPUTCONFIGPANEL_H_

#include <gui/tooloutputlistbox.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/gbsizer.h>

#define ID_BT_GO_BACK 1600
#define ID_BT_RUN 1601

class OutputConfigPanel: public wxPanel {
public:
  OutputConfigPanel(wxWindow *parent, wxWindowID id, const wxPoint& pos =
        wxDefaultPosition, const wxSize& size = wxDefaultSize) :
	wxPanel( parent, id, pos, size )
	{
		m_parent = parent;

    wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);

		p_output = new ToolOutPutTextCtrl(this, id, wxDefaultPosition, wxDefaultSize);
		vbox->Add( p_output, 1, wxEXPAND|wxALL, 5);

    wxGridBagSizer *fgs = new wxGridBagSizer(5, 5);

		wxButton *bt_run    = new wxButton(this, ID_BT_RUN, wxT("Re-Run"), wxDefaultPosition, wxDefaultSize);
		wxButton *bt_save = new wxButton(this, wxID_SAVE, wxT("Save Output"), wxDefaultPosition, wxDefaultSize);
		wxButton *bt_back = new wxButton(this, ID_BT_GO_BACK, wxT("<<< Back"), wxDefaultPosition, wxDefaultSize);
		wxButton *bt_abort = new wxButton(this, wxID_ABORT, wxT("Abort"), wxDefaultPosition, wxDefaultSize);

		fgs->Add( bt_run, wxGBPosition(0,0));
		fgs->Add( bt_save, wxGBPosition(0,1));
		fgs->Add( bt_back, wxGBPosition(0,2));
		fgs->Add( bt_abort, wxGBPosition(0,3));

    vbox->Add( fgs, 0, wxALIGN_CENTER|wxALL, 5);
		vbox->SetSizeHints(this);
    this->SetSizer(vbox);
		this->Layout();
  }

  ToolOutPutTextCtrl* GetOutput(){
		return p_output;
	}

	void SetRunCognizance( wxWindow *p){
		p_output->SetRunCognizance( p );
	}	

  void
  OnGoBackToConfiguration(wxCommandEvent &/*event*/)
  {
    ((wxAuiNotebook *) (p_output->GetParent()->GetParent()))->SetSelection(0);
  }

  void
  OnRunAndClear(wxCommandEvent &/*event*/)
  {
    p_output->Clear();
    p_output->Run();
  }

  void
  OnSave(wxCommandEvent &/*event*/)
  {
    p_output->Save();
  }

  void OnAbort(wxCommandEvent& event){
	  m_parent->GetEventHandler()->ProcessEvent (event);
  }

private:
  ToolOutPutTextCtrl *p_output;	
  wxWindow *m_parent;

DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(OutputConfigPanel, wxPanel)
  EVT_BUTTON(ID_BT_GO_BACK, OutputConfigPanel::OnGoBackToConfiguration)
  EVT_BUTTON(ID_BT_RUN,     OutputConfigPanel::OnRunAndClear)
  EVT_BUTTON(wxID_SAVE,    OutputConfigPanel::OnSave)
  EVT_BUTTON(wxID_ABORT,   OutputConfigPanel::OnAbort)
END_EVENT_TABLE ()

#endif
