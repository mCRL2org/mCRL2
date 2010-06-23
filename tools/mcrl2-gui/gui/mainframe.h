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
	Exec_OpenFile,
	Exec_OpenURL,
	Exec_DDEExec,
	Exec_DDERequest,
	Exec_Redirect,
	Exec_Pipe,
	Exec_About = 300,
	Exec_PerspectiveReset,

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
		menuFile->Append(Exec_Kill, wxT("&Kill process...\tCtrl-K"),
				wxT("Kill a process by PID"));
		menuFile->AppendSeparator();
		menuFile->Append(Exec_Quit, wxT("E&xit\tAlt-X"),
				wxT("Quit the program"));

		wxMenu *execMenu = new wxMenu;
		execMenu->Append(Exec_Redirect, wxT("&Execute command...\tCtrl-E"),
				wxT("Launch a program and capture its output"));

		wxMenu *helpMenu = new wxMenu(wxEmptyString, wxMENU_TEAROFF);
		helpMenu->Append(Exec_About, wxT("&About\tF1"),
				wxT("Show about dialog"));

		wxMenu *windowMenu = new wxMenu(wxEmptyString, wxMENU_TEAROFF);
		windowMenu->Append(Exec_ClearLog, wxT("&Clear Output"),
				wxT("Clear the log with performed commands"));
		windowMenu->AppendSeparator();
		windowMenu->Append(Exec_PerspectiveReset, wxT("&Reset Perspective"),
				wxT("Reset Perspective"));

		// add menus to the menu bar
		wxMenuBar *menuBar = new wxMenuBar();
		menuBar->Append(menuFile, wxT("&File"));
		menuBar->Append(execMenu, wxT("&Exec"));
		menuBar->Append(windowMenu, wxT("&Window"));
		menuBar->Append(helpMenu, wxT("&Help"));


		// Attach menu bar
		SetMenuBar(menuBar);

		m_mgr.SetManagedWindow(this);

		// m_lbox needs to be declared before declaring left_panel for output
		m_lbox = new OutputListBox(this, wxID_ANY, wxPoint(-1, -1), wxSize(-1, -1));
		m_notebookpanel = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP |
				 wxAUI_NB_TAB_SPLIT |
				 wxAUI_NB_TAB_MOVE |
				 wxAUI_NB_CLOSE_ON_ACTIVE_TAB |
				 wxAUI_NB_MIDDLE_CLICK_CLOSE  );

		GenericDirCtrl *left_panel = new GenericDirCtrl(this, m_tool_catalog,
				m_extention_tool_mapping, m_lbox, this->GetNoteBookToolPanel());

		left_panel->SetSize(250,-1);
		m_mgr.AddPane(left_panel, wxLEFT, wxT("File Selector"));

		m_lbox->SetSize(400,250);

		m_mgr.AddPane(m_lbox, wxBOTTOM, wxT("Output"));
		m_mgr.AddPane(m_notebookpanel, wxCENTER);
		m_notebookpanel->Layout();

		// tell the manager to "commit" all the changes just made
		m_mgr.Update();
		m_default_perspective = m_mgr.SavePerspective();

#if wxUSE_STATUSBAR
		// create a status
		CreateStatusBar();
		SetStatusText(wxT("Welcome to mCRL2-gui"));
#endif // wxUSE_STATUSBAR
		Centre();
		SetMinSize(wxSize(800, 600));

	}

	std::vector<Tool> m_tool_catalog;
	multimap<string, string> m_extention_tool_mapping;

	// event handlers (these functions should _not_ be virtual)
	void OnQuit(wxCommandEvent& event) {
		/* TODO: Kill all running processes in "running_processes" */

		for(size_t i = 0  ; i < running_processes.size(); ++i)
		{
			std::cout << "blaat";
		}

		Close(true);
	}
	;

	void OnKill(wxCommandEvent& event) {
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

	void OnClear(wxCommandEvent& event) {
		m_lbox->Clear();
	}
	;

	void OnExecWithRedirect(wxCommandEvent& event) {
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

	void OnAbout(wxCommandEvent& event) {
		wxMessageBox(wxT("mCRL2-gui"), wxT("mCRL2-gui"), wxOK
				| wxICON_INFORMATION, this);
	}
	;

	// polling output of async processes
	void OnTimer(wxTimerEvent& event) {
		/* Send a system wide idle event */
		wxWakeUpIdle();
	}
	;

	void OnResetLayout(wxCommandEvent& event) {
		 m_mgr.LoadPerspective(m_default_perspective);
	};

	void OnIdle(wxIdleEvent& event) {
		size_t count = running_processes.GetCount();

		for (size_t n = 0; n < count; n++) {
			if (running_processes[n]->HasInput()) {
				event.RequestMore();
			}
			// AutoScroll
			// m_lbox->Select( m_lbox->GetCount() -1);
			// m_lbox->SetSelection( wxNOT_FOUND );
		}

	}
	;

	wxListBox *GetLogListBox() const {
		return m_lbox;
	}

	wxAuiNotebook *GetNoteBookToolPanel() const {
		return m_notebookpanel;
	}


private:

	// the PID of the last process we launched asynchronously
	long m_pidLast;

	// last command we executed
	wxString m_cmdLast;

	OutputListBox *m_lbox;

	wxAuiManager m_mgr;

	wxString m_default_perspective;

	wxAuiNotebook *m_notebookpanel;

DECLARE_EVENT_TABLE()
};
BEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_MENU(Exec_Quit, MainFrame::OnQuit) EVT_MENU(Exec_Kill, MainFrame::OnKill)
EVT_MENU(Exec_ClearLog, MainFrame::OnClear)

EVT_MENU(Exec_Redirect, MainFrame::OnExecWithRedirect)

EVT_MENU(Exec_About, MainFrame::OnAbout)
EVT_MENU(Exec_PerspectiveReset, MainFrame::OnResetLayout)

EVT_IDLE(MainFrame::OnIdle)

EVT_TIMER(wxID_ANY, MainFrame::OnTimer)

END_EVENT_TABLE()

#endif /* MAINFRAME_H_ */
