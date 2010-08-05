/*
 * configpanel.h
 *
 *  Created on: Jun 9, 2010
 *      Author: fstapper
 */

#ifndef MCRL2_GUI_CONFIGPANEL_H_
#define MCRL2_GUI_CONFIGPANEL_H_

#include "initialization.h"
#include <wx/utils.h>
#include <mcrl2-process.h>
#include <wx/radiobox.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/filepicker.h>
#include <wx/statline.h>
#include <gui/outputlistbox.h>
#include <wx/scrolwin.h>
#include <wx/gbsizer.h>
#include <wx/event.h>

enum ToolStatus { STATUS_NONE, STATUS_RUNNING, STATUS_COMPLETE, STATUS_FAILED};


#define ID_RUN_TOOL 1000
#define ID_OUTPUT_FILE 1001
#define ID_ABORT_TOOL 1002
#define ID_TIMER 1003

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_UPDATE_PROJECT_TREE, 7777)
END_DECLARE_EVENT_TYPES()

DEFINE_EVENT_TYPE(wxEVT_UPDATE_PROJECT_TREE)

// it may also be convenient to define an event table macro for this event type
#define EVT_UPDATE_PROJECT_TREE(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_UPDATE_PROJECT_TREE, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
        (wxObject *) NULL \
    ),


class ConfigPanel: public wxNotebookPage {
public:

	ConfigPanel(wxAuiNotebook *parent, wxWindowID id, Tool& tool,
			OutputListBox *listbox_output, FileIO& fileIO) :
				wxNotebookPage(parent, id) {
		m_tool = tool;
		m_parent = parent;
		m_listbox_output = listbox_output;
		m_fileIO = fileIO;
		m_pid = 0;



		int row = 0;

		// Top Panel
		m_configpanel = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
				wxAUI_NB_BOTTOM
			);

		m_tool_output = new OutputListBox(m_configpanel, wxID_ANY, wxDefaultPosition, wxDefaultSize);
		m_wsw = new wxScrolledWindow(m_configpanel, wxID_ANY, wxDefaultPosition, wxDefaultSize);

		/* Define size large enough for top*/
		wxPanel *top = new wxPanel(m_wsw , wxID_ANY, wxDefaultPosition, wxSize(4096,4096));

    wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);

    wxGridBagSizer *fgs = new wxGridBagSizer(5, 5);

    fgs->Add( new wxStaticText(top, wxID_ANY, wxT("input file:")) , wxGBPosition(row,0));
    fgs->Add( new wxStaticText(top, wxID_ANY, wxString( m_fileIO.input_file.c_str(), wxConvUTF8 )), wxGBPosition(row,1),
        wxGBSpan(1,2));

    if (!m_tool.m_output_type.empty())
      {
        ++row;

        fgs->Add(new wxStaticText(top, wxID_ANY, wxT("output file:")),
            wxGBPosition(row, 0));

        wxString filesuggestion = GenerateOutputFileSuggestion();

        wxFilePickerCtrl *fpc = new wxFilePickerCtrl(top, ID_OUTPUT_FILE,
            wxT(""), wxT("Select a file"), wxT("*.*"), wxDefaultPosition, wxDefaultSize,
            wxFLP_USE_TEXTCTRL | wxFLP_SAVE | wxFLP_OVERWRITE_PROMPT);

        fpc->SetPath(filesuggestion);
        fpc->SetSize(wxSize(350, 25));
        fgs->Add(fpc, wxGBPosition(row, 1), wxGBSpan(1,2));

        m_fileIO.output_file = filesuggestion.mb_str(wxConvUTF8);

      }

    ++row;
    fgs->Add(new wxStaticLine(top,wxID_ANY, wxDefaultPosition, wxSize(800,1)), wxGBPosition(row,0), wxGBSpan(1,3));

		/* Parse and display options */
		vector<Tool_option> vto = tool.m_tool_options;

		wxRadioBox *rb;
		wxStaticText *ws, *ws1, *ws2;
		wxArrayString as;
		wxCheckBox *cb;
		wxTextCtrl *tc;
		wxFilePickerCtrl *fp;


		for (vector<Tool_option>::iterator i = vto.begin(); i != vto.end(); ++i) {
			//TODO: extend with Optional/mandatory checkbox

			switch ((*i).m_widget) {
			case none:
				break;
			case checkbox:

				cb = new wxCheckBox(top, wxID_ANY, wxString(
						(*i).m_flag.c_str(), wxConvUTF8));

				if ((*i).m_values[(*i).m_default_value].compare("true") == 0) {
					cb->SetValue(true);
				}

				m_checkbox_ptrs.push_back(cb);

				/* Display help text */
				ws = new wxStaticText(top, wxID_ANY, wxString(
						(*i).m_help.c_str(), wxConvUTF8));

				ws->Wrap(800);

        row++;
        fgs->Add(cb, wxGBPosition(row,0));
        fgs->Add(ws, wxGBPosition(row,1), wxGBSpan(1,2));

				break;
			case radiobox:

				/* Prepare radio values */
				as.Clear();
				for (vector<string>::iterator j = (*i).m_values.begin(); j
						!= (*i).m_values.end(); ++j) {
					as.Add(wxString((*j).c_str(), wxConvUTF8));
				}

				/* create radio box */
				rb = new wxRadioBox(top, wxID_ANY, wxString(
						(*i).m_flag.c_str(), wxConvUTF8), wxDefaultPosition, wxDefaultSize, as
						,0, wxRA_SPECIFY_ROWS
						);

				m_radiobox_ptrs.push_back(rb);

				/* Set default value */
				rb->SetStringSelection(wxString((*i).m_values[(*i).m_default_value].c_str(),wxConvUTF8));

				/* Display help text */
				ws = new wxStaticText(top, wxID_ANY, wxString((*i).m_help.c_str(), wxConvUTF8));

				ws->Wrap(800);

        row++;
        fgs->Add(rb, wxGBPosition(row,0));
        fgs->Add(ws, wxGBPosition(row,1), wxGBSpan(1,2));

				break;
			case textctrl:
				/* display label */

				// TODO: Set Default values
				/* create text input box */
				tc = new wxTextCtrl(top, wxID_ANY, wxT(""));

				tc->SetLabel(wxString(
						(*i).m_flag.c_str(), wxConvUTF8));

				tc->SetValue(wxT(""));

				m_textctrl_ptrs.push_back(tc);

				row++;
        fgs->Add( new wxStaticText(top, wxID_ANY, wxString(
            (*i).m_flag.c_str(), wxConvUTF8)),
            wxGBPosition(row,0));
        fgs->Add(tc, wxGBPosition(row,1));
        fgs->Add(new wxStaticText(top, wxID_ANY, wxString(
            (*i).m_help.c_str(), wxConvUTF8)),
            wxGBPosition(row,2));

				break;

			case filepicker:
				/* display label */
				ws1 = new wxStaticText(top, wxID_ANY, wxString(
						(*i).m_flag.c_str(), wxConvUTF8));

				// TODO: Set Default values

				/* create text input box */
				fp = new wxFilePickerCtrl(top, wxID_ANY,  wxT(""),
						wxT("Select a file"), wxT("*.*"), wxDefaultPosition, wxDefaultSize,
						wxFLP_USE_TEXTCTRL | wxFLP_OPEN );

				fp->SetLabel(wxString(
						(*i).m_flag.c_str(), wxConvUTF8));

				m_filepicker_ptrs.push_back(fp);

				ws2 = new wxStaticText(top, wxID_ANY, wxString(
						(*i).m_help.c_str(), wxConvUTF8));

				row++;
        fgs->Add(ws1, wxGBPosition(row,0));
        fgs->Add(fp,  wxGBPosition(row,1), wxGBSpan(1,2));
        fp->SetSize(wxSize(350,25));
        fgs->Add(ws2,  wxGBPosition(row,3));

				break;
			}
		};

		m_runbutton = new wxButton(top, ID_RUN_TOOL, wxT("Run"));

		m_abortbutton = new wxButton(top, ID_ABORT_TOOL, wxT("Abort"));
		m_abortbutton->Show(false);

	  ++row;
	  fgs->Add(new wxStaticLine(top,wxID_ANY, wxDefaultPosition, wxSize(800,1)), wxGBPosition(row,0), wxGBSpan(1,3));
		++row;
    fgs->Add(m_runbutton, wxGBPosition(row,0));
    fgs->Add(m_abortbutton, wxGBPosition(row,1));

    hbox->Add(fgs, 1, wxALL, 15);
    top->SetSizer(hbox);
		top->Layout();

		int w, h;
		top->GetBestSize(&w,&h);
    m_wsw->SetScrollbars( 20, 20, w/20 , h/20 );

		m_configpanel->AddPage( m_wsw , wxT("Configuration"), true);
		m_configpanel->AddPage( m_tool_output , wxT("Output"), false);

	}
	;

	void UpdateToolTipStatus(ToolStatus s){

	  //enum ToolStatus { STATUS_NONE, STATUS_RUNNING, STATUS_COMPLETE, STATUS_FAILED};

    /* Remove text after last last '[' */
    wxString toolTipText = m_parent->GetPageText( m_parent->GetPageIndex( this ) );

    if( !toolTipText.BeforeFirst(_T(' ')).empty() ){
      toolTipText = toolTipText.BeforeFirst(_T(' '));
    }

	  switch (s){
	    case STATUS_NONE:
	      break;
	    case STATUS_RUNNING:
	      toolTipText = toolTipText.Append(wxT(" [Running]"));
	      break;
      case STATUS_COMPLETE:
        toolTipText = toolTipText.Append(wxT(" [Done]"));
        break;
      case STATUS_FAILED:
        toolTipText = toolTipText.Append(wxT(" [Failed]"));
        break;
	  }

    m_parent->SetPageText(m_parent->GetPageIndex( this ), toolTipText );

	}

	void OnRunClick(wxCommandEvent& /*event*/) {

	  UpdateToolTipStatus(STATUS_RUNNING);

		wxString cmd = wxString(m_tool.m_location.c_str(), wxConvUTF8);

		wxString run = cmd;
		for (vector<wxRadioBox*>::iterator i = m_radiobox_ptrs.begin(); i
				!= m_radiobox_ptrs.end(); ++i) {
			run = run + wxT(" --") + (*i)->GetLabel() + wxT("=")
					+ (*i)->GetStringSelection();
		}

		for (vector<wxCheckBox*>::iterator i = m_checkbox_ptrs.begin(); i
				!= m_checkbox_ptrs.end(); ++i) {
			if ((*i)->GetValue())
				run = run + wxT(" --") + (*i)->GetLabel();
		}

		for (vector<wxTextCtrl*>::iterator i = m_textctrl_ptrs.begin(); i
				!= m_textctrl_ptrs.end(); ++i) {
			if ((*i)->GetValue() && !(*i)->GetValue().empty() )
				run = run + wxT(" --") + (*i)->GetLabel() + wxT("=") +(*i)->GetValue();
		}

		for (vector<wxFilePickerCtrl*>::iterator i = m_filepicker_ptrs.begin(); i
				!= m_filepicker_ptrs.end(); ++i) {
			if ((*i)->GetPath() && !(*i)->GetPath().empty() )
				run = run + wxT(" --") + (*i)->GetLabel() + wxT("=") +(*i)->GetPath();
		}


		wxString input_file = wxString(m_fileIO.input_file.c_str(), wxConvUTF8);

		wxString output_file = wxString(m_fileIO.output_file.c_str(),
				wxConvUTF8);

		run = run + wxT(" ") + input_file + wxT(" ") + output_file;

		m_listbox_output->Append(run);

		m_process = new MyPipedProcess(this);

		// Gui tools should be visible:
		// Tools redirect the standard input and/or output of the process being launched by calling Redirect. 
		// For these child processes IO is redirected. For Windows these process windows are not shown by default.
		// To avoid that GUI tools are not shown a wxEXEC_NOHIDE flag is used to flag that the child process window
		// are shown normally.

		switch (m_tool.m_tool_type){
		  case shell:
	      m_pid = wxExecute(run, wxEXEC_ASYNC, m_process);
	      break;
		  case gui:
			m_pid = wxExecute(run, wxEXEC_ASYNC | wxEXEC_NOHIDE , m_process);
			break;
		case ishell:
		  m_pid = wxShell(run);
		  break;
		}

		if ((m_tool.m_tool_type == shell) || (m_tool.m_tool_type == gui))
      {
        if (!m_pid)
        {
          wxLogError(wxT("Execution of '%s' failed."), run.c_str());
          m_pid = 0;
          delete m_process;
          UpdateToolTipStatus(STATUS_FAILED);
        }
        else
        {
          m_process->AddAsyncProcess(m_tool_output);
          m_runbutton->Disable();
          m_abortbutton->Show(true);

          m_process->m_ext_pid = m_pid;

          m_configpanel->SetSelection(m_configpanel->GetSelection() + 1);
        };
      }
	}
	;

	void OnAbortClick(wxCommandEvent& /*event*/) {
		if(wxProcess::Exists(m_pid) && (m_pid != 0) ){
		  wxProcess::Kill(m_pid);
		}
		m_abortbutton->Show(false);
		m_runbutton->Enable();
	};

	void OnOutputFileChange(wxFileDirPickerEvent& event) {
		m_fileIO.output_file = event.GetPath().mb_str(wxConvUTF8);
	};

	virtual bool Destroy() {

	  /* Implement Veto */
		//Killing m_pid == 0 can terminate application
		if (wxProcess::Exists(m_pid) && (m_pid != 0)) {
			wxProcess::Kill(m_pid);
		}
		return true;
	}
	;

	void OnResize(wxSizeEvent& /*event*/){
		m_configpanel->SetSize( this->GetSize() );
	}

	wxAuiNotebook *m_parent;
	wxAuiNotebook *m_configpanel;
	OutputListBox *m_listbox_output;
	OutputListBox *m_tool_output;
	wxScrolledWindow *m_wsw;
	wxString m_input_file;
	Tool m_tool;

	FileIO m_fileIO;

	vector<wxRadioBox*> m_radiobox_ptrs;
	vector<wxCheckBox*> m_checkbox_ptrs;
	vector<wxTextCtrl*> m_textctrl_ptrs;
	vector<wxFilePickerCtrl*> m_filepicker_ptrs;

	wxButton *m_runbutton;
	wxButton *m_abortbutton;

	MyPipedProcess *m_process;

	long m_pid;

private:

	wxString
    GenerateOutputFileSuggestion()
    {
      /* Suggest/Generate output file */

      size_t found;
      string file_suggestion = m_fileIO.input_file;
      found = file_suggestion.find_last_of(".");
      file_suggestion = file_suggestion.substr(0, found) + "." + m_tool.m_name;

      struct stat stFileInfo;
      int intStat = 0;
      int cntr = 0;

      char c[5];
      while (intStat == 0)
      {
        sprintf(c, "%02d", cntr);
        intStat = stat(
            (file_suggestion + c + "." + m_tool.m_output_type).c_str(),
            &stFileInfo);
        ++cntr;
      }

      file_suggestion = file_suggestion + c + "." + m_tool.m_output_type;
      return wxString(file_suggestion.c_str(), wxConvUTF8);
    }

	  void
    OnProcessEnd(wxCommandEvent& evt)
    {
      m_abortbutton->Show(false);
      m_runbutton->Enable();

      wxCommandEvent eventCustom(wxEVT_UPDATE_PROJECT_TREE);
      /* Notify parents to expand to the created file*/

      if (!m_fileIO.output_file.empty())
      {
        wxStringClientData *scd = new wxStringClientData(wxString(
            m_fileIO.output_file.c_str(), wxConvUTF8));
        eventCustom.SetClientData(scd);
      }
      else
      {
        eventCustom.SetClientData(NULL);
      }

      wxPostEvent(m_parent, eventCustom);

      UpdateToolTipStatus(STATUS_COMPLETE);
    }

DECLARE_EVENT_TABLE()
};
BEGIN_EVENT_TABLE(ConfigPanel, wxNotebookPage)
		EVT_BUTTON(ID_RUN_TOOL, ConfigPanel::OnRunClick) EVT_BUTTON(ID_ABORT_TOOL, ConfigPanel::OnAbortClick)

		EVT_FILEPICKER_CHANGED(ID_OUTPUT_FILE, ConfigPanel::OnOutputFileChange)
		EVT_SIZE(ConfigPanel::OnResize)
		EVT_MY_PROCESS_END( wxID_ANY, ConfigPanel::OnProcessEnd )
		END_EVENT_TABLE ()

#endif /* MCRL2_GUI_CONFIGPANEL_H_ */
