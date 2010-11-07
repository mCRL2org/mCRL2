// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mainframe.cpp

#include "mainframe.h"

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_MENU(Exec_Quit, MainFrame::OnQuit)
EVT_MENU(Exec_OpenFile, MainFrame::OnOpenFile)
END_EVENT_TABLE()


MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size, mcrl2::data::rewriter::strategy rewrite_strategy) :
    wxFrame((wxFrame *) NULL, wxID_ANY, title, pos, size){

#ifdef __WXMAC__
    // required since ABOUT is not the default id of the about menu
    wxApp::s_macAboutMenuItemId = Exec_About;
#endif

    // create a menu bar
    wxMenu *menuFile = new wxMenu(wxEmptyString, wxMENU_TEAROFF);
    menuFile->Append(Exec_OpenFile, wxT("&Open file \tCtrl-O"),
        wxT("Open an existing file"));
    menuFile->Append(Exec_Quit, wxT("E&xit\tAlt-X"),
        wxT("Quit the program"));
    menuFile->Append(Exec_Save2File, wxT("Save \tCtrl-S"), wxT(""));
    menuFile->Append(Exec_Save2FileAs, wxT("Save As"), wxT(""));

    editMenu = new wxMenu;

    editMenu->Append(Exec_SelectAll, wxT("Select All \tCtrl-A"), wxT("Select all items tems in focused window"));
    editMenu->Append(Exec_Copy2ClipBoard, wxT("Copy \tCtrl-C"), wxT("Copy focused selection to clipboard"));


    editMenu->Enable(Exec_SelectAll, false);
    editMenu->Enable(Exec_Copy2ClipBoard, false);
    editMenu->Enable(Exec_Save2File, false);
    editMenu->Enable(Exec_ClearLog, false);

    wxMenu *helpMenu = new wxMenu(wxEmptyString, wxMENU_TEAROFF);
    helpMenu->Append(wxID_ABOUT, wxT("&About\tF1"),
        wxT("Show about dialog"));

    m_PanelMenu = new wxMenu(wxEmptyString, wxMENU_TEAROFF);
    /*m_PanelMenu->AppendCheckItem( Exec_ToggleFileBrowserPanel, wxT("File Browser"));
    m_PanelMenu->AppendCheckItem( Exec_ToggleExecutedCommandsPanel, wxT("Executed Commands"));

    m_PanelMenu->Check(Exec_ToggleFileBrowserPanel, true);
    m_PanelMenu->Check(Exec_ToggleExecutedCommandsPanel, true);*/

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

    output = new outputpanel( this ); //new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1, 100), wxTE_MULTILINE);

    editor = new xEditor(this, wxID_ANY, output );
    options = new Options(this, wxID_ANY, editor, output, rewrite_strategy);

    m_mgr.AddPane(editor , wxCENTER, wxT("Data specification"));
    m_mgr.AddPane(options , wxRIGHT, wxT("Control"));
    m_mgr.AddPane(output , wxBOTTOM, wxT("Output"));

    // tell the manager to "commit" all the changes just made
    m_mgr.Update();

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

MainFrame::~MainFrame() {
  // deinitialize the frame manager
  m_mgr.UnInit();
}
;

void MainFrame::OnQuit(wxCommandEvent& /*event*/) {
    Close(true);
  }
  ;

void MainFrame::OnOpenFile(wxCommandEvent& /*event*/) {
  wxFileDialog * openFileDialog = new wxFileDialog(this, wxT("Choose a file"), wxEmptyString , wxEmptyString, wxT("LPS files (*.lps)|*.lps|TXT and mCRL2 files (*.txt;*.mcrl2)|*.txt;*.mcrl2")
);
  if (openFileDialog->ShowModal() == wxID_OK){
      wxString fileName = openFileDialog->GetPath();
      editor->LoadFile(fileName);
  }
};
