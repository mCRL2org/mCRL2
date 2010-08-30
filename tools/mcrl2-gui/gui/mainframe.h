/*
 * mainframe.h
 *
 *  Created on: Jun 3, 2010
 *      Author: fstapper
 */

#ifndef MCRL2_GUI_MAINFRAME_H_
#define MCRL2_GUI_MAINFRAME_H_

#include "wx/app.h"
#include "wx/log.h"
#include "wx/frame.h"
#include "wx/panel.h"

#include "wx/timer.h"

#include "wx/utils.h"
#include "wx/menu.h"

#include "wx/msgdlg.h"
#include "wx/textdlg.h"
#include "wx/filedlg.h"
#include "wx/choicdlg.h"

#include "wx/button.h"
#include "wx/textctrl.h"
#include "wx/listbox.h"
#include "gui/projectcontrol.h"
#include "gui/preferences.h"
#include "wx/sizer.h"

#include <wx/aui/aui.h>

#include "wx/txtstrm.h"
#include "wx/numdlg.h"
#include "wx/textdlg.h"
#include "wx/ffile.h"

#include "wx/process.h"

#include "wx/mimetype.h"

// IDs for the controls and the menu commands
enum {
	// menu items
	Exec_Quit = 100,
	Exec_Kill,
	Exec_ClearLog,
	Exec_SyncExec = 200,
	Exec_AsyncExec,
	Exec_Shell,
	Exec_POpen,
	Exec_NewFile,
	Exec_RenameFile,
	Exec_DeleteFile,
	Exec_Refresh,
	Exec_OpenFile,
	Exec_CopyFile,
	Exec_Redirect,
	Exec_Copy2ClipBoard,
	Exec_Save2File,
	Exec_SelectAll,
	Exec_About = 300,
	Exec_PerspectiveReset,
	Exec_Preferences,

	Exec_ToggleFileBrowserPanel,
	Exec_ToggleExecutedCommandsPanel,

	// control ids
	Exec_Btn_Send = 1000,
	Exec_Btn_SendFile,
	Exec_Btn_Get,
	Exec_Btn_Close
};

static const wxChar *DIALOG_TITLE = wxT("mCRL2-gui");

// Define main frame
class MainFrame: public wxFrame {

public:
	MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size,
			const std::vector<Tool>& tool_catalog, const std::multimap<
					std::string, std::string>& extention_tool_mapping) :
		wxFrame((wxFrame *) NULL, wxID_ANY, title, pos, size), // m_timerIdleWakeUp (this),
				m_tool_catalog(tool_catalog), m_extention_tool_mapping(
						extention_tool_mapping) {
		m_pidLast = 0;

#ifdef __WXMAC__
		// required since ABOUT is not the default id of the about menu
		wxApp::s_macAboutMenuItemId = Exec_About;
#endif

		// create a menu bar
		wxMenu *menuFile = new wxMenu(wxEmptyString, wxMENU_TEAROFF);
		menuFile->Append(Exec_NewFile, wxT("&New file \tCtrl-N"),
				wxT("Add a new file"));
		menuFile->AppendSeparator();
		menuFile->Append(Exec_OpenFile, wxT("&Edit selected file \tCtrl-E"),
						wxT("Edit selected file"));
		menuFile->Append(Exec_CopyFile, wxT("&Copy selected file \tF6"),
		    wxT("Copy selected file on disk"));
		menuFile->Append(Exec_RenameFile, wxT("&Rename selected file \tF2"),
				wxT("Rename a file"));
		menuFile->Append(Exec_DeleteFile, wxT("&Delete selected file"),
				wxT("Delete a file"));
		menuFile->AppendSeparator();
		menuFile->Append(Exec_Refresh, wxT("&Refresh file browser content \tF5"),
						wxT("Refresh file browser content"));
		menuFile->AppendSeparator();
		menuFile->Append(Exec_Quit, wxT("E&xit\tAlt-X"),
				wxT("Quit the program"));

		editMenu = new wxMenu;

		editMenu->Append(Exec_SelectAll, wxT("Select All \tCtrl-A"), wxT("Select all items tems in focused window"));
		editMenu->Append(Exec_Copy2ClipBoard, wxT("Copy \tCtrl-C"), wxT("Copy focused selection to clipboard"));
    editMenu->Append(Exec_Save2File, wxT("Save \tCtrl-S"), wxT("Save focused output window to file"));
    editMenu->AppendSeparator();
    editMenu->Append(Exec_ClearLog, wxT("&Clear output"),
        wxT("Clear output of the focused window"));


    editMenu->Enable(Exec_SelectAll, false);
    editMenu->Enable(Exec_Copy2ClipBoard, false);
    editMenu->Enable(Exec_Save2File, false);
    editMenu->Enable(Exec_ClearLog, false);

		wxMenu *execMenu = new wxMenu;
		execMenu->Append(Exec_Redirect, wxT("&Run command...\tCtrl-R"),
				wxT("Launch a program and capture its output"));
		execMenu->AppendSeparator();
		execMenu->Append(Exec_Kill, wxT("&Kill process...\tCtrl-K"),
				wxT("Kill a process by PID"));

		wxMenu *helpMenu = new wxMenu(wxEmptyString, wxMENU_TEAROFF);
		helpMenu->Append(wxID_ABOUT, wxT("&About\tF1"),
				wxT("Show about dialog"));

		m_PanelMenu = new wxMenu(wxEmptyString, wxMENU_TEAROFF);
		m_PanelMenu->AppendCheckItem( Exec_ToggleFileBrowserPanel, wxT("File Browser"));
		m_PanelMenu->AppendCheckItem( Exec_ToggleExecutedCommandsPanel, wxT("Executed Commands"));

	  m_PanelMenu->Check(Exec_ToggleFileBrowserPanel, true);
		m_PanelMenu->Check(Exec_ToggleExecutedCommandsPanel, true);

		wxMenu *windowMenu = new wxMenu(wxEmptyString, wxMENU_TEAROFF);
		windowMenu->AppendSubMenu(m_PanelMenu, wxT("&Dockable panels"),
            wxT("Toggle panel visibility"));
		windowMenu->AppendSeparator();
		windowMenu->Append(Exec_PerspectiveReset, wxT("&Reset Perspective"),
				wxT("Reset Perspective"));
    windowMenu->AppendSeparator();
    windowMenu->Append(Exec_Preferences, wxT("&Preferences \tCtrl-P"),
        wxT("mCRL2-gui Preferences"));

		// add menus to the menu bar
		wxMenuBar *menuBar = new wxMenuBar();
		menuBar->Append(menuFile, wxT("&File"));
		menuBar->Append(editMenu, wxT("&Edit"));
#ifdef DEBUG
		menuBar->Append(execMenu, wxT("&Process"));
#endif
		menuBar->Append(windowMenu, wxT("&Window"));
		menuBar->Append(helpMenu, wxT("&Help"));


		// Attach menu bar
		SetMenuBar(menuBar);

		m_mgr.SetManagedWindow(this);

		// m_ExecutedCommandsPanel needs to be declared before declaring left_panel for output
		m_ExecutedCommandsPanel = new OutPutListBox(this, wxID_ANY, wxPoint(-1, -1), wxSize(-1, -1));
		m_notebookpanel = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_DEFAULT_STYLE );

		m_FileBrowserPanel = new GenericDirCtrl(this, m_tool_catalog,
				m_extention_tool_mapping, m_ExecutedCommandsPanel, this->GetNoteBookToolPanel());

		m_FileBrowserPanel->SetSize(250,-1);
		m_FileBrowserPanel->Refresh();

		m_mgr.AddPane(m_FileBrowserPanel, wxLEFT, wxT("File Browser"));

		m_ExecutedCommandsPanel->SetSize(400,250);

		m_mgr.AddPane(m_ExecutedCommandsPanel, wxBOTTOM, wxT("Executed Commands"));
		m_mgr.AddPane(m_notebookpanel, wxCENTER);
		m_notebookpanel->Layout();

		// tell the manager to "commit" all the changes just made
		m_mgr.Update();
		m_default_perspective = m_mgr.SavePerspective();

		//cout << m_mgr.SavePerspective().mb_str(wxConvUTF8) << endl ;

#if wxUSE_STATUSBAR
		// create a status
		CreateStatusBar();
		SetStatusText(wxT("Welcome to mCRL2-gui"));
#endif // wxUSE_STATUSBAR
		Centre();
		SetMinSize(wxSize(800, 600));

	}

	// event handlers (these functions should _not_ be virtual)
	void OnQuit(wxCommandEvent& /*event*/) {
		/* TODO: Kill all running processes in "running_processes" */

		for(size_t i = 0  ; i < running_processes.size(); ++i)
		{
			std::cout << "blaat";
		}

		Close(true);
	}
	;

	void OnKill(wxCommandEvent& /*event*/) {
		long pid = wxGetNumberFromUser(
				wxT("Please specify the process to kill"),
				wxT("Enter PID:"), wxT("Exec question"), m_pidLast,
				// we need the full unsigned int range
				-INT_MAX, INT_MAX, this);
		if (pid == -1) {
			return;
		}

		static const wxString signalNames[] = { wxT("Just test (SIGNONE)"),
				wxT("Hangup (SIGHUP)"), wxT("Interrupt (SIGINT)"),
				wxT("Quit (SIGQUIT)"), wxT("Illegal instruction (SIGILL)"),
				wxT("Trap (SIGTRAP)"), wxT("Abort (SIGABRT)"),
				wxT("Emulated trap (SIGEMT)"),
				wxT("FP exception (SIGFPE)"), wxT("Kill (SIGKILL)"),
				wxT("Bus (SIGBUS)"), wxT("Segment violation (SIGSEGV)"),
				wxT("System (SIGSYS)"), wxT("Broken pipe (SIGPIPE)"),
				wxT("Alarm (SIGALRM)"), wxT("Terminate (SIGTERM)"), };

		int sig = wxGetSingleChoiceIndex(wxT("How to kill the process?"),
				wxT("Exec question"), WXSIZEOF(signalNames), signalNames,
				this);
		switch (sig) {
		default:
			wxFAIL_MSG( wxT("unexpected return value") );
			// fall through

		case -1:
			// cancelled
			return;

		case wxSIGNONE:
		case wxSIGHUP:
		case wxSIGINT:
		case wxSIGQUIT:
		case wxSIGILL:
		case wxSIGTRAP:
		case wxSIGABRT:
		case wxSIGEMT:
		case wxSIGFPE:
		case wxSIGKILL:
		case wxSIGBUS:
		case wxSIGSEGV:
		case wxSIGSYS:
		case wxSIGPIPE:
		case wxSIGALRM:
		case wxSIGTERM:
			break;
		}

		if (sig == 0) {
			if (wxProcess::Exists(pid))
				wxLogStatus(wxT("Process %ld is running."), pid);
			else
				wxLogStatus(wxT("No process with pid = %ld."), pid);
		} else // not SIGNONE
		{
			wxKillError rc = wxProcess::Kill(pid, (wxSignal) sig);
			if (rc == wxKILL_OK) {
				wxLogStatus(wxT("Process %ld killed with signal %d."), pid, sig);
			} else {
				static const wxChar *errorText[] = {
						wxT(""), // no error
						wxT("signal not supported"),
						wxT("permission denied"), wxT("no such process"),
						wxT("unspecified error"), };

				wxLogStatus(
						wxT("Failed to kill process %ld with signal %d: %s"),
						pid, sig, errorText[rc]);
			}
		}
	}
	;

	~MainFrame() {
		// deinitialize the frame manager		
		m_mgr.UnInit();
	}
	;

	void OnToggleFileBrowserPanel(wxCommandEvent& /*event*/) {
     if (m_mgr.GetPane(m_FileBrowserPanel).IsShown()){
     //if( m_show_file_browser_pane_info ){
       m_mgr.GetPane(m_FileBrowserPanel).Hide();
     } else{
       m_mgr.GetPane(m_FileBrowserPanel).Show();
     }
     m_mgr.Update();
     m_PanelMenu->Check(Exec_ToggleFileBrowserPanel, m_mgr.GetPane(m_FileBrowserPanel).IsShown() );
 	}

	void OnToggleExecutedCommandsPanel(wxCommandEvent& /*event*/) {
    if (m_mgr.GetPane(m_ExecutedCommandsPanel).IsShown()){
    //if( m_show_file_browser_pane_info ){
      m_mgr.GetPane(m_ExecutedCommandsPanel).Hide();
    } else{
      m_mgr.GetPane(m_ExecutedCommandsPanel).Show();
    }
    m_mgr.Update();
    m_PanelMenu->Check(Exec_ToggleExecutedCommandsPanel, m_mgr.GetPane(m_ExecutedCommandsPanel).IsShown() );

	}

	void OnClosePane(wxAuiManagerEvent& event ){
	  //Closing File Browser Pane
	  if ( event.pane == &m_mgr.GetPane(m_FileBrowserPanel) )
	  {
	    m_PanelMenu->Check(Exec_ToggleFileBrowserPanel, false );
	  }

	  //Closing Executed Commands Pane
    if ( event.pane == &m_mgr.GetPane(m_ExecutedCommandsPanel) )
    {
      m_PanelMenu->Check(Exec_ToggleExecutedCommandsPanel, false );
    }
	}

	void UpdateFocus(wxCommandEvent& event){
	  if( event.GetClientData() == NULL){
	    FocusedOutPutListBox = NULL;
	    editMenu->Enable(Exec_SelectAll, false);
	    editMenu->Enable(Exec_Copy2ClipBoard, false);
	    editMenu->Enable(Exec_Save2File, false);
	    editMenu->Enable(Exec_ClearLog, false);
	  } else {
	    FocusedOutPutListBox = (OutPutListBox*) event.GetClientData();
	    editMenu->Enable(Exec_SelectAll, true);
      editMenu->Enable(Exec_Copy2ClipBoard, true);
      editMenu->Enable(Exec_Save2File, true);
      editMenu->Enable(Exec_ClearLog, true);
	  }
	}

	void OnClear(wxCommandEvent& /*event*/) {
	  if( FocusedOutPutListBox ){
	    FocusedOutPutListBox->Clear();
	  }
	}
	;

	 void OnCopy2Clipboard(wxCommandEvent& /*event*/) {
	    if( FocusedOutPutListBox ){
	      FocusedOutPutListBox->CopyLine();
	    }
	  }
	  ;

	  void OnSave(wxCommandEvent& /*event*/) {
	    if( FocusedOutPutListBox ){
	      FocusedOutPutListBox->Save();
	    }
	  }
	  ;

	  void OnSelectAll(wxCommandEvent& /*event*/) {
	    if( FocusedOutPutListBox ){
	      FocusedOutPutListBox->SelectAll();
	    }
	  }
	  ;

	void OnNewFile(wxCommandEvent& /*event*/) {
		m_FileBrowserPanel->CreateNewFile();
	}
	;

	void OnRenameFile(wxCommandEvent& /*event*/) {
		m_FileBrowserPanel->Rename();
	}
	;

	void OnRefresh(wxCommandEvent& /*event*/) {
		m_FileBrowserPanel->Refresh();
	}
	;

 void OnCopyFile(wxCommandEvent& /*event*/) {
	    m_FileBrowserPanel->CopyFile();
	  }
	  ;

	void OnDeleteFile(wxCommandEvent& /*event*/) {
		m_FileBrowserPanel->Delete();
	}
	;

	void OnEditFile(wxCommandEvent& /*event*/) {
		m_FileBrowserPanel->Edit();
	}
	;

	void OnExecWithRedirect(wxCommandEvent& /*event*/) {
		wxString cmd = wxGetTextFromUser(wxT("Enter the command: "),
				DIALOG_TITLE, m_cmdLast);

		if (!cmd)
			return;

		{
			MyPipedProcess *process = new MyPipedProcess(NULL);
			if (!wxExecute(cmd, wxEXEC_ASYNC, process)) {
				wxLogError(wxT("Execution of '%s' failed."), cmd.c_str());

				delete process;
			} else {
				process->AddAsyncProcess(this->GetLogListBox());
			}
		}

		m_cmdLast = cmd;
	}
	;

	// polling output of async processes
	void OnTimer(wxTimerEvent& /*event*/) {
		/* Send a system wide idle event */
		wxWakeUpIdle();
	}
	;

	void OnExecPreferences(wxCommandEvent& /*event*/){
	  Preferences *p = new Preferences();
	  p->Show(true);
	};

	void OnResetLayout(wxCommandEvent& /*event*/) {
		 m_mgr.LoadPerspective(m_default_perspective);
     m_PanelMenu->Check(Exec_ToggleFileBrowserPanel, m_mgr.GetPane(m_FileBrowserPanel).IsShown() );

	};

	void OnIdle(wxIdleEvent& event) {
		size_t count = running_processes.GetCount();

		for (size_t n = 0; n < count; n++) {
			if (running_processes[n]->HasInput()) {
				event.RequestMore();
			}
		}

	}
	;

	wxTextCtrl *GetLogListBox() const {
		return m_ExecutedCommandsPanel;
	}

	wxAuiNotebook *GetNoteBookToolPanel() const {
		return m_notebookpanel;
	}

	void
	OnUpdateProjectTree(wxCommandEvent& evt)
    {

      if (evt.GetClientData())
      {
        wxStringClientData *p;
        p = (wxStringClientData*) evt.GetClientData();
        if (p)
        {
          m_FileBrowserPanel->Refresh();
          m_FileBrowserPanel->ExpandPath(p->GetData());
        }
      }

    }

private:

	wxMenu *m_PanelMenu;
  wxMenu *editMenu;

	bool m_show_executed_commands;

	bool m_show_file_browser_pane_info;
	wxString m_file_browser_pane_info;

  std::vector<Tool> m_tool_catalog;
  multimap<string, string> m_extention_tool_mapping;

	// the PID of the last process we launched asynchronously
	long m_pidLast;

	// last command we executed
	wxString m_cmdLast;

	OutPutListBox *m_ExecutedCommandsPanel;

	OutPutListBox *FocusedOutPutListBox;

	wxAuiManager m_mgr;

	wxString m_default_perspective;

	wxAuiNotebook *m_notebookpanel;

	GenericDirCtrl *m_FileBrowserPanel;

DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_MENU(Exec_Quit, MainFrame::OnQuit)
EVT_MENU(Exec_Kill, MainFrame::OnKill)
EVT_MENU(Exec_NewFile, MainFrame::OnNewFile)
EVT_MENU(Exec_OpenFile, MainFrame::OnEditFile)
EVT_MENU(Exec_RenameFile, MainFrame::OnRenameFile)
EVT_MENU(Exec_DeleteFile, MainFrame::OnDeleteFile)
EVT_MENU(Exec_Refresh, MainFrame::OnRefresh)
EVT_MENU(Exec_CopyFile, MainFrame::OnCopyFile)
EVT_MENU(Exec_Preferences, MainFrame::OnExecPreferences)
EVT_MENU(Exec_ToggleFileBrowserPanel, MainFrame::OnToggleFileBrowserPanel)
EVT_MENU(Exec_ToggleExecutedCommandsPanel, MainFrame::OnToggleExecutedCommandsPanel)
EVT_MENU(Exec_ClearLog, MainFrame::OnClear)
EVT_MENU(Exec_Redirect, MainFrame::OnExecWithRedirect)
EVT_MENU(Exec_PerspectiveReset, MainFrame::OnResetLayout)
EVT_MENU(Exec_Copy2ClipBoard, MainFrame::OnCopy2Clipboard)
EVT_MENU(Exec_Save2File, MainFrame::OnSave)
EVT_MENU(Exec_SelectAll, MainFrame::OnSelectAll)

EVT_IDLE(MainFrame::OnIdle)
EVT_TIMER(wxID_ANY, MainFrame::OnTimer)
EVT_AUI_PANE_CLOSE(MainFrame::OnClosePane)
EVT_UPDATE_PROJECT_TREE(wxID_ANY, MainFrame::OnUpdateProjectTree)
EVT_UPDATE_FOCUS(wxID_ANY, MainFrame::UpdateFocus)
END_EVENT_TABLE()

#endif /* MAINFRAME_H_ */
