// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mainframe.cpp

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
#include "mainframe.h"
#include "settings.h"

#include <wx/fdrepdlg.h>

#include "icons/mcrl2xi.xpm"

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
  EVT_MENU(wxID_CLOSE, MainFrame::OnQuit)
  EVT_MENU(wxID_NEW, MainFrame::OnNewFile)
  EVT_MENU(wxID_OPEN, MainFrame::OnOpenFile)
  EVT_MENU(mcrl2xi::Exec_ToggleOutputPanel, MainFrame::OnToggleOutputPanel)
  EVT_MENU(mcrl2xi::Exec_ToggleOptionsPanel, MainFrame::OnToggleOptionsPanel)
  EVT_MENU(mcrl2xi::Exec_PerspectiveReset, MainFrame::OnResetLayout)
  EVT_MENU(wxID_COPY, MainFrame::OnEdit)
  EVT_MENU(wxID_SELECTALL, MainFrame::OnEdit)
  EVT_MENU(wxID_PASTE, MainFrame::OnEdit)
  EVT_MENU(wxID_CUT, MainFrame::OnEdit)
  EVT_MENU(wxID_UNDO, MainFrame::OnEdit)
  EVT_MENU(wxID_REDO, MainFrame::OnEdit)
  EVT_MENU(wxID_CLEAR, MainFrame::OnEdit)
  EVT_MENU(wxID_SAVE, MainFrame::OnSaveFile)
  EVT_MENU(wxID_SAVEAS, MainFrame::OnSaveFileAs)
  EVT_MENU(mcrl2xi::myID_EVALEXPR, MainFrame::OnEvaluate)
  EVT_MENU(mcrl2xi::myID_TYPECHECKSPEC, MainFrame::OnTypeCheck)
  EVT_MENU(mcrl2xi::myID_WRAPMODE, MainFrame::OnWrapmode)

  EVT_MENU(wxID_FIND, MainFrame::OnOpenFind)

//EVT_MENU(wxID_REPLACE, MainFrame::ShowReplaceDialog)
  EVT_FIND(wxID_ANY, MainFrame::OnFind)
  EVT_FIND_NEXT(wxID_ANY, MainFrame::OnFind)
  EVT_FIND_REPLACE(wxID_ANY, MainFrame::OnReplace)
  EVT_FIND_REPLACE_ALL(wxID_ANY, MainFrame::OnReplaceAll)
  EVT_FIND_CLOSE(wxID_ANY, MainFrame::OnFindClose)


  EVT_UPDATE_EDITOR_FOCUS(wxID_ANY, MainFrame::UpdateEdtFocus)
  EVT_MYTXTCTRL_FOCUS(wxID_ANY, MainFrame::UpdateTxtFocus)

  EVT_SETSTATUSTEXT(wxID_ANY, MainFrame::SetStatus)
  EVT_AUI_PANE_CLOSE(MainFrame::OnClosePane)



END_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size, mcrl2::data::rewriter::strategy rewrite_strategy) :
  wxFrame((wxFrame*) NULL, wxID_ANY, title, pos, size)
{

  SetIcon(wxIcon(mcrl2xi_xpm));

#ifdef __WXMAC__
  // required since ABOUT is not the default id of the about menu
  wxApp::s_macAboutMenuItemId = wxID_ABOUT;
#endif

  // create a menu bar
  wxMenu* menuFile = new wxMenu(wxEmptyString, wxMENU_TEAROFF);
  menuFile->Append(wxID_NEW, wxT("&New file \tCtrl-N"),
                   wxT("Create a new specification"));
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
  editMenu->AppendSeparator();
  editMenu->Append(mcrl2xi::myID_EVALEXPR, wxT("&Evaluate \tCtrl-E"), wxT("Evaluate data expression"));
  editMenu->Append(mcrl2xi::myID_TYPECHECKSPEC, wxT("&Type check \tCtrl-T"), wxT("Parse and type check specification"));


  editMenu->Enable(wxID_SELECTALL, true);
  editMenu->Enable(wxID_COPY, true);
  editMenu->Enable(wxID_PASTE, true);
  editMenu->Enable(wxID_CLEAR, true);
  editMenu->Enable(wxID_PASTE, true);
  editMenu->Enable(wxID_UNDO, true);
  editMenu->Enable(wxID_REDO, true);
  editMenu->Enable(wxID_CUT, true);

  viewMenu = new wxMenu;
  viewMenu->Append(wxID_FIND, wxT("&Find \tCtrl-F"), wxT("Find text")) ;
  viewMenu->AppendSeparator();
  viewMenu->AppendCheckItem(mcrl2xi::myID_WRAPMODE, wxT("&Wrap mode \tCtrl+U"),
                            wxT("Wrap lines in editor"));

  wxMenu* helpMenu = new wxMenu(wxEmptyString, wxMENU_TEAROFF);
  helpMenu->Append(wxID_ABOUT, wxT("&About\tF1"),
                   wxT("Show about dialog"));

  m_PanelMenu = new wxMenu(wxEmptyString, wxMENU_TEAROFF);
  m_PanelMenu->AppendCheckItem(mcrl2xi::Exec_ToggleOptionsPanel, wxT("Actions"));
  m_PanelMenu->AppendCheckItem(mcrl2xi::Exec_ToggleOutputPanel, wxT("Output Panel"));


  m_PanelMenu->Check(mcrl2xi::Exec_ToggleOutputPanel, true);
  m_PanelMenu->Check(mcrl2xi::Exec_ToggleOptionsPanel, true);

  wxMenu* windowMenu = new wxMenu(wxEmptyString, wxMENU_TEAROFF);
  windowMenu->AppendSubMenu(m_PanelMenu, wxT("&Dockable panels"),
                            wxT("Toggle panel visibility"));
  windowMenu->AppendSeparator();
  windowMenu->Append(mcrl2xi::Exec_PerspectiveReset, wxT("&Reset Perspective"),
                     wxT("Reset Perspective"));

  // add menus to the menu bar
  wxMenuBar* menuBar = new wxMenuBar();
  menuBar->Append(menuFile, wxT("&File"));
  menuBar->Append(editMenu, wxT("&Edit"));
  menuBar->Append(viewMenu, wxT("&View"));
  menuBar->Append(windowMenu, wxT("&Window"));
  menuBar->Append(helpMenu, wxT("&Help"));


  // Attach menu bar
  SetMenuBar(menuBar);

  m_mgr.SetManagedWindow(this);

  output = new outputpanel(this);

  editor = new xEditor(this, wxID_ANY, output);
  options = new Options(this, wxID_ANY, editor, output, rewrite_strategy);

  m_mgr.AddPane(editor , wxCENTER, wxT("Data specification"));
  m_mgr.AddPane(options , wxRIGHT, wxT("Actions"));
  m_mgr.AddPane(output , wxBOTTOM, wxT("Output"));

  m_mgr.GetPane(output).BestSize(-1, 100);

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

  // Set downward search as default
  findData.SetFlags( wxFR_DOWN );

}
;

MainFrame::~MainFrame()
{
  // deinitialize the frame manager
  m_mgr.UnInit();
#if wxCHECK_VERSION(2, 9, 0)
  this->PopEventHandler(true);
#endif
}
;

void MainFrame::OnQuit(wxCommandEvent& /*event*/)
{
  Close(true);
}
;

void MainFrame::OnOpenFile(wxCommandEvent& /*event*/)
{
  wxFileDialog* openFileDialog = new wxFileDialog(this, wxT("Choose a file"), wxEmptyString , wxEmptyString,
      mcrl2xi::mcrl2_files
                                                 );

  if (openFileDialog->ShowModal() == wxID_OK)
  {
    wxString fileName = openFileDialog->GetPath();
    editor->LoadFile(fileName);
  }
};

void MainFrame::OnNewFile(wxCommandEvent& /*event*/)
{
  editor->AddEmptyPage();
};

void MainFrame::LoadFile(wxString filename)
{
  editor->LoadFile(filename);
};

void MainFrame::OnSaveFile(wxCommandEvent& /*event*/)
{
  if (editor->GetPageCount() > 0)
  {
    wxString fileName = editor->GetFileInUse();
    if (fileName == wxEmptyString)
    {

      wxFileDialog* saveFileDialog = new wxFileDialog(this, wxT("Save file as"),
          wxEmptyString,
          wxEmptyString,
          mcrl2xi::mcrl2_files,
          wxFD_SAVE | wxFD_OVERWRITE_PROMPT
                                                     );
      if (saveFileDialog->ShowModal() == wxID_OK)
      {
        fileName = saveFileDialog->GetPath();
      }
    }
    if (fileName != wxEmptyString)
    {
      editor->SaveFile(fileName);
    }
  }
};

void MainFrame::OnSaveFileAs(wxCommandEvent& /*event*/)
{
  if (editor->GetPageCount() > 0)
  {
    wxFileDialog* openFileDialog = new wxFileDialog(this, wxT("Save file as"),
        wxEmptyString,
        wxEmptyString,
        mcrl2xi::mcrl2_files,
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT
                                                   );
    if (openFileDialog->ShowModal() == wxID_OK)
    {
      editor->SaveFile(openFileDialog->GetPath());
    }
  }
};

void MainFrame::OnEvaluate(wxCommandEvent& e)
{
  options->OnEval(e);
};

void MainFrame::OnTypeCheck(wxCommandEvent& e)
{
  options->OnTypeCheck(e);
};

void MainFrame::OnToggleOutputPanel(wxCommandEvent& /*event*/)
{
  if (m_mgr.GetPane(output).IsShown())
  {
    m_mgr.GetPane(output).Hide();
  }
  else
  {
    m_mgr.GetPane(output).Show();
  }
  m_mgr.Update();
  m_PanelMenu->Check(mcrl2xi::Exec_ToggleOutputPanel, m_mgr.GetPane(output).IsShown());
};

void MainFrame::OnToggleOptionsPanel(wxCommandEvent& /*event*/)
{
  if (m_mgr.GetPane(options).IsShown())
  {
    m_mgr.GetPane(options).Hide();
  }
  else
  {
    m_mgr.GetPane(options).Show();
  }
  m_mgr.Update();
  m_PanelMenu->Check(mcrl2xi::Exec_ToggleOptionsPanel, m_mgr.GetPane(options).IsShown());
};

void MainFrame::OnClosePane(wxAuiManagerEvent& event)
{
  //Closing File Browser Pane
  if (event.pane == &m_mgr.GetPane(output))
  {
    m_PanelMenu->Check(mcrl2xi::Exec_ToggleOutputPanel, false);
  }

  //Closing Executed Commands Pane
  if (event.pane == &m_mgr.GetPane(options))
  {
    m_PanelMenu->Check(mcrl2xi::Exec_ToggleOptionsPanel, false);
  }
};

void MainFrame::OnResetLayout(wxCommandEvent& /*event*/)
{
  m_mgr.LoadPerspective(m_default_perspective);
  m_PanelMenu->Check(mcrl2xi::Exec_ToggleOutputPanel, m_mgr.GetPane(output).IsShown());
  m_PanelMenu->Check(mcrl2xi::Exec_ToggleOptionsPanel, m_mgr.GetPane(options).IsShown());
};

void MainFrame::OnEdit(wxCommandEvent& event)
{
  if (focussed_editor)
  {
    focussed_editor->GetEventHandler()->ProcessEvent(event);
  }
  if (focussed_txtCtrl)
  {
    focussed_txtCtrl->GetEventHandler()->ProcessEvent(event);
  }
};

void MainFrame::UpdateEdtFocus(wxCommandEvent& event)
{
  if (event.GetClientData() == NULL)
  {
    focussed_editor = NULL;

  }
  else
  {
    focussed_editor = (xStcEditor*) event.GetClientData();
    focussed_editor_for_find = focussed_editor;
    viewMenu->Check(mcrl2xi::myID_WRAPMODE, focussed_editor->GetWrapMode() != 0);
  }
  UpdateEditMenu();
};

void MainFrame::UpdateTxtFocus(wxCommandEvent& event)
{
  if (event.GetClientData() == NULL)
  {
    focussed_txtCtrl = NULL;
  }
  else
  {
    focussed_txtCtrl = (myTextControl*) event.GetClientData();
  }
  UpdateEditMenu();
};


void MainFrame::UpdateEditMenu()
{
  if (focussed_editor || focussed_txtCtrl)
  {
    editMenu->Enable(wxID_SELECTALL, true);
    editMenu->Enable(wxID_COPY, true);
    editMenu->Enable(wxID_PASTE, true);
    editMenu->Enable(wxID_CLEAR, true);
    editMenu->Enable(wxID_PASTE, true);
    editMenu->Enable(wxID_UNDO, true);
    editMenu->Enable(wxID_REDO, true);
    editMenu->Enable(wxID_CUT, true);
  }
  else
  {
    editMenu->Enable(wxID_SELECTALL, false);
    editMenu->Enable(wxID_COPY, false);
    editMenu->Enable(wxID_PASTE, false);
    editMenu->Enable(wxID_CLEAR, false);
    editMenu->Enable(wxID_PASTE, false);
    editMenu->Enable(wxID_UNDO, false);
    editMenu->Enable(wxID_REDO, false);
    editMenu->Enable(wxID_CUT, false);
  }
}

void MainFrame::SetStatus(wxCommandEvent& event)
{
  wxString* s = (wxString*) event.GetClientData();
  SetStatusText(*s);
};

void MainFrame::OnWrapmode(wxCommandEvent& /*event*/)
{
  if (focussed_editor)
  {
    focussed_editor->SetWrapMode(focussed_editor->GetWrapMode() == 0?
                                 focussed_editor->getWrapWordCode():
                                 focussed_editor->getWrapNoneCode());
  };
};

void MainFrame::OnOpenFind(wxCommandEvent& /* event */)
{

  focussed_editor_for_find = focussed_editor;
  m_dlgReplace = new wxFindReplaceDialog
  (
    this,
    &findData,
    wxT("Find and replace dialog"),
    wxFR_REPLACEDIALOG   
  );
  m_dlgReplace->Show(true);

};

void MainFrame::OnFindClose(wxFindDialogEvent& /* event */)
{
  m_dlgReplace->Destroy();
  m_dlgReplace = NULL;
};

void MainFrame::OnFind(wxFindDialogEvent& event)
{
  if (focussed_editor_for_find)
  {

    int i;

    if (focussed_editor_for_find->GetSelectionStart() != focussed_editor_for_find->GetSelectionEnd())
    {
      i = focussed_editor_for_find->FindText(focussed_editor_for_find->GetSelectionStart(),
                                             focussed_editor_for_find->GetSelectionEnd(), event.GetFindString());
    }
    else
    {

      /* fwd */
      if (findData.GetFlags() == wxFR_DOWN)
      {
        i = focussed_editor_for_find->FindText(focussed_editor_for_find->GetSelectionStart(), focussed_editor_for_find->GetTextLength() , event.GetFindString());
      }
      else
      {
        /* rev */
        i = focussed_editor_for_find->FindText(focussed_editor_for_find->GetSelectionStart(), 0 , event.GetFindString());
      }
    }

    if (i != -1)
    {
      focussed_editor_for_find->SetInsertionPoint(i);

      m_dlgReplace->Destroy();
      m_dlgReplace = NULL;

      focussed_editor_for_find->SetFocus();
    }
    else
    {
      if (findData.GetFlags() == wxFR_DOWN)
      {
        wxMessageBox(wxT("No match found downward."));
      }
      else
      {
        wxMessageBox(wxT("No match found upward."));
      }
    }
  }
};


void MainFrame::OnReplace(wxFindDialogEvent& event)
{
  if (focussed_editor_for_find)
  {

    int i;

    if (focussed_editor_for_find->GetSelectionStart() != focussed_editor_for_find->GetSelectionEnd())
    {
      i = focussed_editor_for_find->FindText(focussed_editor_for_find->GetSelectionStart(),
                                             focussed_editor_for_find->GetSelectionEnd(), event.GetFindString());
    }
    else
    {

      /* fwd */
      if (findData.GetFlags() == wxFR_DOWN)
      {
        i = focussed_editor_for_find->FindText(focussed_editor_for_find->GetSelectionStart(), focussed_editor_for_find->GetTextLength() , event.GetFindString());
      }
      else
      {
        /* rev */
        i = focussed_editor_for_find->FindText(focussed_editor_for_find->GetSelectionStart(), 0 , event.GetFindString());
      }
    }

    if (i != -1)
    {

      focussed_editor_for_find->Replace(i, i + event.GetFindString().Length(), event.GetReplaceString() );;

      m_dlgReplace->Destroy();
      m_dlgReplace = NULL;

      focussed_editor_for_find->SetInsertionPoint(i);
      focussed_editor_for_find->SetFocus();
    }
    else
    {
      if (findData.GetFlags() == wxFR_DOWN)
      {
        wxMessageBox(wxT("No match found downward."));
      }
      else
      {
        wxMessageBox(wxT("No match found upward."));
      }
    }
  }
};

void MainFrame::OnReplaceAll(wxFindDialogEvent& event)
{
  if (focussed_editor_for_find)
  {

    int begin, end;

    if (focussed_editor_for_find->GetSelectionStart() != focussed_editor_for_find->GetSelectionEnd())
    {
      begin = focussed_editor_for_find->GetSelectionStart();
      end   = focussed_editor_for_find->GetSelectionEnd();
    }
    else
    {
      begin = 0; 
      end   = focussed_editor_for_find->GetTextLength();
    }

    int i = focussed_editor_for_find->FindText(begin, end , event.GetFindString());
    if( i != -1 ) 
    {
      int count = 0;
      while( i != -1 )
      {
        focussed_editor_for_find->Replace(i, i + event.GetFindString().Length(), event.GetReplaceString() );
        end = end - event.GetFindString().Length() +  event.GetReplaceString().Length();
        i = focussed_editor_for_find->FindText(i, end , event.GetFindString());
        count++;
      }

      m_dlgReplace->Destroy();
      m_dlgReplace = NULL;
      wxMessageBox(wxT("Replaced ") + wxString::Format(wxT("%d"), count) + wxT(" occurrences."));

      focussed_editor_for_find->SetFocus();

    }
    else
    {
      wxMessageBox(wxT("No match found"));
    }
  }
};

