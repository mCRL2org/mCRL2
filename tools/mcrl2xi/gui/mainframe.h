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
#include "options.h"
#include "outputpanel.h"

// IDs for the controls and the menu commands
enum {
	Exec_PerspectiveReset = 300,

	Exec_ToggleOutputPanel,
	Exec_ToggleOptionsPanel

};

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_UPDATE_EDITOR_FOCUS, 7777)
    DECLARE_EVENT_TYPE(wxEVT_SETSTATUSTEXT, 7777)
END_DECLARE_EVENT_TYPES()

DEFINE_EVENT_TYPE(wxEVT_UPDATE_EDITOR_FOCUS)
DEFINE_EVENT_TYPE(wxEVT_SETSTATUSTEXT)

#define EVT_UPDATE_EDITOR_FOCUS(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_UPDATE_EDITOR_FOCUS, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
        (wxObject *) NULL \
    ),

#define EVT_SETSTATUSTEXT(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_SETSTATUSTEXT, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
        (wxObject *) NULL \
    ),

wxString mcrl2_files(wxT("LPS files (*.lps)|*.lps|mCRL2 files (*.mcrl2)|*.mcrl2|TXT files (*.txt)|*.txt"));

// Define main frame
class MainFrame: public wxFrame {

public:
	MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size, mcrl2::data::rewriter::strategy rewrite_strategy) :
		wxFrame((wxFrame *) NULL, wxID_ANY, title, pos, size){

#ifdef __WXMAC__
		// required since ABOUT is not the default id of the about menu
		wxApp::s_macAboutMenuItemId = Exec_About;
#endif

		// create a menu bar
		wxMenu *menuFile = new wxMenu(wxEmptyString, wxMENU_TEAROFF);
		menuFile->Append(wxID_OPEN, wxT("&Open file \tCtrl-O"),
				wxT("Open an existing file"));
		menuFile->Append(wxID_SAVE, wxT("Save \tCtrl-S"), wxT(""));
		menuFile->Append(wxID_SAVEAS, wxT("Save As"), wxT(""));
		menuFile->AppendSeparator();
    menuFile->Append(wxID_CLOSE, wxT("E&xit\tAlt-X"),
        wxT("Quit the program"));

		editMenu = new wxMenu;

		editMenu->Append(wxID_UNDO, wxT("&Undo \tCtrl-Z"), wxT("Undo edit step"));
		editMenu->Append(wxID_REDO, wxT("&Redo \tCtrl-Y"), wxT("Redo edit step"));
		editMenu->AppendSeparator();
    editMenu->Append(wxID_CUT, wxT("Cu&t \tCtrl-X"), wxT("Cut selection to clipboard"));
		editMenu->Append(wxID_COPY, wxT("&Copy \tCtrl-C"), wxT("Copy selection to clipboard"));
		editMenu->Append(wxID_PASTE, wxT("&Paste \tCtrl-V"), wxT("Paste clipboard to editor"));
    editMenu->Append(wxID_CLEAR, wxT("&Delete \tDel"), wxT("Delete selection"));
    editMenu->AppendSeparator();

    editMenu->Append(wxID_SELECTALL, wxT("Select All \tCtrl-A"), wxT("Select all items tems in focused window"));

    editMenu->Enable(wxID_SELECTALL, true);
    editMenu->Enable(wxID_COPY, true);
    editMenu->Enable(wxID_PASTE, true);
    editMenu->Enable(wxID_CLEAR, true);
    editMenu->Enable(wxID_PASTE, true);
    editMenu->Enable(wxID_UNDO, true);
    editMenu->Enable(wxID_REDO, true);
    editMenu->Enable(wxID_CUT, true);

		wxMenu *helpMenu = new wxMenu(wxEmptyString, wxMENU_TEAROFF);
		helpMenu->Append(wxID_ABOUT, wxT("&About\tF1"),
				wxT("Show about dialog"));

		m_PanelMenu = new wxMenu(wxEmptyString, wxMENU_TEAROFF);
		m_PanelMenu->AppendCheckItem( Exec_ToggleOutputPanel, wxT("Output Panel"));
		m_PanelMenu->AppendCheckItem( Exec_ToggleOptionsPanel, wxT("Options Panel"));

	  m_PanelMenu->Check(Exec_ToggleOutputPanel, true);
		m_PanelMenu->Check(Exec_ToggleOptionsPanel, true);

		wxMenu *windowMenu = new wxMenu(wxEmptyString, wxMENU_TEAROFF);
		windowMenu->AppendSubMenu(m_PanelMenu, wxT("&Dockable panels"),
            wxT("Toggle panel visibility"));
		windowMenu->AppendSeparator();
		windowMenu->Append(Exec_PerspectiveReset, wxT("&Reset Perspective"),
				wxT("Reset Perspective"));

		// add menus to the menu bar
		wxMenuBar *menuBar = new wxMenuBar();
		menuBar->Append(menuFile, wxT("&File"));
		menuBar->Append(editMenu, wxT("&Edit"));
		menuBar->Append(windowMenu, wxT("&Window"));
		menuBar->Append(helpMenu, wxT("&Help"));


		// Attach menu bar
		SetMenuBar(menuBar);

		m_mgr.SetManagedWindow(this);

		output = new outputpanel( this );

    editor = new xEditor(this, wxID_ANY, output );
    options = new Options(this, wxID_ANY, editor, output, rewrite_strategy);

		m_mgr.AddPane(editor , wxCENTER, wxT("Data specification"));
		m_mgr.AddPane(options , wxRIGHT, wxT("Control"));
		m_mgr.AddPane(output , wxBOTTOM, wxT("Output"));

    // tell the manager to "commit" all the changes just made
		m_mgr.Update();

		m_default_perspective = m_mgr.SavePerspective();

#if wxUSE_STATUSBAR
		// create a status
		CreateStatusBar();
		SetStatusText(wxT("Welcome to mCRL2xi"));
#endif // wxUSE_STATUSBAR
		Centre();
		SetMinSize(wxSize(800, 600));
    editor->SetFocus();

	}
	;

	~MainFrame() {
		// deinitialize the frame manager
		m_mgr.UnInit();
	}
	;

	void OnQuit(wxCommandEvent& /*event*/) {
		Close(true);
	}
	;

	void OnOpenFile(wxCommandEvent& /*event*/) {
    wxFileDialog * openFileDialog = new wxFileDialog(this, wxT("Choose a file"), wxEmptyString , wxEmptyString,
        mcrl2_files
);

    if (openFileDialog->ShowModal() == wxID_OK){
        wxString fileName = openFileDialog->GetPath();
        editor->LoadFile(fileName);
    }
	};

  void LoadFile(wxString filename){
    editor->LoadFile(filename);
  };

  void OnSaveFile(wxCommandEvent& /*event*/){
    wxString fileName = editor->GetFileInUse();
    if ( fileName == wxEmptyString){

      wxFileDialog * openFileDialog = new wxFileDialog(this, wxT("Save file as"),
          wxEmptyString,
          wxEmptyString,
          mcrl2_files,
          wxFD_SAVE | wxFD_OVERWRITE_PROMPT
          );
      if (openFileDialog->ShowModal() == wxID_OK){
         fileName = openFileDialog->GetPath();
      }
    }
    if( fileName != wxEmptyString ){
      editor->SaveFile( editor->GetFileInUse() );
    }
  };

  void OnSaveFileAs(wxCommandEvent& /*event*/){
     wxFileDialog * openFileDialog = new wxFileDialog(this, wxT("Save file as"),
        wxEmptyString,
        wxEmptyString,
        mcrl2_files,
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT
        );
    if (openFileDialog->ShowModal() == wxID_OK){
      editor->SaveFile( openFileDialog->GetPath() );
    }
  };

  void OnToggleOutputPanel(wxCommandEvent& /*event*/) {
     if (m_mgr.GetPane(output).IsShown()){
       m_mgr.GetPane(output).Hide();
     } else{
       m_mgr.GetPane(output).Show();
     }
     m_mgr.Update();
     m_PanelMenu->Check(Exec_ToggleOutputPanel, m_mgr.GetPane(output).IsShown() );
  };

  void OnToggleOptionsPanel(wxCommandEvent& /*event*/) {
    if (m_mgr.GetPane(options).IsShown()){
      m_mgr.GetPane(options).Hide();
    } else{
      m_mgr.GetPane(options).Show();
    }
    m_mgr.Update();
    m_PanelMenu->Check(Exec_ToggleOptionsPanel, m_mgr.GetPane(options).IsShown() );
  };

  void OnClosePane(wxAuiManagerEvent& event ){
    //Closing File Browser Pane
    if ( event.pane == &m_mgr.GetPane(output) )
    {
      m_PanelMenu->Check(Exec_ToggleOutputPanel, false );
    }

    //Closing Executed Commands Pane
    if ( event.pane == &m_mgr.GetPane(options) )
    {
      m_PanelMenu->Check(Exec_ToggleOptionsPanel, false );
    }
  };

  void OnResetLayout(wxCommandEvent& /*event*/) {
     m_mgr.LoadPerspective(m_default_perspective);
     m_PanelMenu->Check(Exec_ToggleOutputPanel, m_mgr.GetPane(output).IsShown() );
     m_PanelMenu->Check(Exec_ToggleOptionsPanel, m_mgr.GetPane(options).IsShown() );
  };

  void OnEdit (wxCommandEvent &event) {
      if (focussed_editor) focussed_editor->GetEventHandler()->ProcessEvent (event);
  };

  void UpdateFocus(wxCommandEvent& event){
    if( event.GetClientData() == NULL){
      focussed_editor = NULL;
      editMenu->Enable(wxID_SELECTALL, false);
      editMenu->Enable(wxID_COPY, false);
      editMenu->Enable(wxID_PASTE, false);
      editMenu->Enable(wxID_CLEAR, false);
      editMenu->Enable(wxID_PASTE, false);
      editMenu->Enable(wxID_UNDO, false);
      editMenu->Enable(wxID_REDO, false);
      editMenu->Enable(wxID_CUT, false);
    } else {
      focussed_editor = (xStcEditor*) event.GetClientData();
      editMenu->Enable(wxID_SELECTALL, true);
      editMenu->Enable(wxID_COPY, true);
      editMenu->Enable(wxID_PASTE, true);
      editMenu->Enable(wxID_CLEAR, true);
      editMenu->Enable(wxID_PASTE, true);
      editMenu->Enable(wxID_UNDO, true);
      editMenu->Enable(wxID_REDO, true);
      editMenu->Enable(wxID_CUT, true);
    }
  };

  void SetStatus(wxCommandEvent& event){
    wxString *s = (wxString*) event.GetClientData();
    SetStatusText(*s);

  }

private:

	wxMenu *m_PanelMenu;
  wxMenu *editMenu;

	wxAuiManager m_mgr;

	wxString m_default_perspective;

  xEditor *editor;
  Options *options;
  wxTextCtrl *output;

  xStcEditor *focussed_editor;

DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_MENU(wxID_CLOSE, MainFrame::OnQuit)
EVT_MENU(wxID_OPEN, MainFrame::OnOpenFile)
EVT_MENU(Exec_ToggleOutputPanel, MainFrame::OnToggleOutputPanel)
EVT_MENU(Exec_ToggleOptionsPanel, MainFrame::OnToggleOptionsPanel)
EVT_MENU(Exec_PerspectiveReset, MainFrame::OnResetLayout)

EVT_MENU(wxID_COPY, MainFrame::OnEdit)
EVT_MENU(wxID_SELECTALL, MainFrame::OnEdit)
EVT_MENU(wxID_PASTE, MainFrame::OnEdit)
EVT_MENU(wxID_CUT, MainFrame::OnEdit)
EVT_MENU(wxID_UNDO, MainFrame::OnEdit)
EVT_MENU(wxID_REDO, MainFrame::OnEdit)
EVT_MENU(wxID_CLEAR, MainFrame::OnEdit)


EVT_MENU(wxID_SAVE, MainFrame::OnSaveFile)
EVT_MENU(wxID_SAVEAS, MainFrame::OnSaveFileAs)

EVT_UPDATE_EDITOR_FOCUS(wxID_ANY, MainFrame::UpdateFocus)
EVT_SETSTATUSTEXT(wxID_ANY, MainFrame::SetStatus)

EVT_AUI_PANE_CLOSE(MainFrame::OnClosePane)

END_EVENT_TABLE()

#endif /* MCRL2XI_MAINFRAME_H_ */
