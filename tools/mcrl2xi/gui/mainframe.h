// Author(s): Frank Stappers 
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mainframe.h

#ifndef MCRL2XI_MAINFRAME_H_
#define MCRL2XI_MAINFRAME_H_

#include "wx/app.h"
#include "wx/log.h"
#include "wx/frame.h"
#include "wx/panel.h"

#include "wx/menu.h"
#include "wx/msgdlg.h"

#include "wx/textctrl.h"
#include "wx/sizer.h"
#include <wx/aui/aui.h>

#include "editor.h"
#include "actions.h"
#include "outputpanel.h"


// Define main frame
class MainFrame: public wxFrame {

public:
	MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size, mcrl2::data::rewriter::strategy rewrite_strategy);
	~MainFrame();
	void OnQuit(wxCommandEvent& /*event*/) ;
	void OnOpenFile(wxCommandEvent& /*event*/) ;
  void LoadFile(wxString filename);
  void OnSaveFile(wxCommandEvent& /*event*/);
  void OnSaveFileAs(wxCommandEvent& /*event*/);
  void OnToggleOutputPanel(wxCommandEvent& /*event*/);
  void OnToggleOptionsPanel(wxCommandEvent& /*event*/);
  void OnClosePane(wxAuiManagerEvent& event );
  void OnResetLayout(wxCommandEvent& /*event*/);
  void OnEdit (wxCommandEvent &event);
  void UpdateFocus(wxCommandEvent& event);
  void SetStatus(wxCommandEvent& event);
  void OnNewFile(wxCommandEvent& /*event*/);
  void OnEvaluate(wxCommandEvent& e);
  void OnTypeCheck(wxCommandEvent& e);
  void OnWrapmode(wxCommandEvent& /*event*/);
private:

	wxMenu *m_PanelMenu;
  wxMenu *editMenu;
  wxMenu *viewMenu;
	wxAuiManager m_mgr;
	wxString m_default_perspective;
  xEditor *editor;
  Options *options;
  outputpanel *output;
  xStcEditor *focussed_editor;

  DECLARE_EVENT_TABLE()
};

#endif /* MCRL2XI_MAINFRAME_H_ */
