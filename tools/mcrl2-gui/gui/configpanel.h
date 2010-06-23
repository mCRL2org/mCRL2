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
#include <stdlib.h>

#define ID_RUN_TOOL 1000
#define ID_OUTPUT_FILE 1001
#define ID_ABORT_TOOL 1002
#define ID_TIMER 1003

class ConfigPanel: public wxNotebookPage {
public:

	ConfigPanel(wxAuiNotebook *parent, wxWindowID id, Tool& tool,
			OutputListBox *listbox_output, FileIO& fileIO) :
				wxNotebookPage(parent, id) {
		m_tool = tool;
		m_parent = parent;
		m_listbox_output = listbox_output;
		m_fileIO = fileIO;
		m_timer = new wxTimer(this, ID_TIMER);
		m_pid = 0;

		int height = 0;
		int border = 3;
		int w, h, v, f;

		// Top Panel
		wxPanel *top = new wxPanel(this , wxID_ANY);


		if (!m_tool.m_output_type.empty()) {
			/* Suggest/Generate output file */

			size_t found;
			string file_suggestion = m_fileIO.input_file;
			found = file_suggestion.find_last_of(".");
			file_suggestion = file_suggestion.substr(0, found) + "."
					+ tool.m_name;

			struct stat stFileInfo;
			int intStat = 0;
			int cntr = 0;

			char c[5];
			while (intStat == 0) {
				sprintf(c, "%02d", cntr);
				intStat
						= stat(
								(file_suggestion + c + "." + tool.m_output_type).c_str(),
								&stFileInfo);
				++cntr;
			}

			file_suggestion = file_suggestion + c + "." + tool.m_output_type;

			/* Create PathFilePicker */
			wxStaticText *st = new wxStaticText(top, wxID_ANY,
					wxT("output file: "), wxPoint(border, height + border));
			st->GetSize(&w, &h);

			wxFilePickerCtrl *fpc = new wxFilePickerCtrl(top, ID_OUTPUT_FILE, wxT(""),
					wxT("Select a file"), wxT("*.*"), wxPoint(w + (2
							* border), height + border), wxSize(350, 25),
					wxFLP_USE_TEXTCTRL | wxFLP_SAVE | wxFLP_OVERWRITE_PROMPT);
			//FPC->Fit();
			fpc->SetPath(wxString(file_suggestion.c_str(), wxConvUTF8));
			m_fileIO.output_file = file_suggestion;
			fpc->GetSize(&w, &h);
			height = height + h + 2 * border;

		}

		/* Parse and display options */
		vector<Tool_option> vto = tool.m_tool_options;

		wxRadioBox *rb;
		wxStaticText *ws;
		wxArrayString as;
		wxCheckBox *cb;

		for (vector<Tool_option>::iterator i = vto.begin(); i != vto.end(); ++i) {
			//TODO: extend with Optional/mandatory checkbox

			switch ((*i).m_widget) {
			case none:
				break;
			case checkbox:

				cb = new wxCheckBox(top, wxID_ANY, wxString(
						(*i).m_flag.c_str(), wxConvUTF8), wxPoint(border,
						height), wxDefaultSize); //m_tool_panel, wxID_ANY,
				cb->GetSize(&w, &h);

				if ((*i).m_values[(*i).m_default_value].compare("true") == 0) {
					cb->SetValue(true);
				}

				m_checkbox_ptrs.push_back(cb);

				/* Display help text */
				ws = new wxStaticText(top, wxID_ANY, wxString(
						(*i).m_help.c_str(), wxConvUTF8), wxPoint(max(w + 2
						* border, 130), height + 4));

				ws->Wrap(800);

				ws->GetSize(&v, &f);

				h = max(f, h);

				height = height + h;

				break;
			case radiobox:

				/* Prepare radio values */
				for (vector<string>::iterator j = (*i).m_values.begin(); j
						!= (*i).m_values.end(); ++j) {
					as.Add(wxString((*j).c_str(), wxConvUTF8));
				}

				/* create radio box */
				rb = new wxRadioBox(top, wxID_ANY, wxString(
						(*i).m_flag.c_str(), wxConvUTF8), wxPoint(border,
						height), wxDefaultSize, as
				//,1, wxRA_SPECIFY_ROWS
						);
				rb->GetSize(&w, &h);

				m_radiobox_ptrs.push_back(rb);

				/* Set default value */
				rb->SetStringSelection(
						wxString((*i).m_values[(*i).m_default_value].c_str(),
								wxConvUTF8));

				/* Display help text */
				ws = new wxStaticText(top, wxID_ANY, wxString(
						(*i).m_help.c_str(), wxConvUTF8), wxPoint(w + 2
						* border, height));

				ws->Wrap(800);

				ws->GetSize(&v, &f);

				h = max(f, h);

				height = height + h;

				break;
			case textbox:
				break;
			case filepicker:
				break;
			}

		};

		height = height + border;

		m_runbutton = new wxButton(top, ID_RUN_TOOL, wxT("Run"), wxPoint(
				border, height));

		m_abortbutton = new wxButton(top, ID_ABORT_TOOL, wxT("Abort"),
				wxPoint(100, height));
		m_abortbutton->Show(false);

		top->Layout();

		m_configpanel = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
				wxAUI_NB_BOTTOM
			);

		m_tool_output = new OutputListBox(m_configpanel, wxID_ANY, wxDefaultPosition, wxDefaultSize);

		m_configpanel->AddPage( top , wxT("Configuration"), true);
		m_configpanel->AddPage( m_tool_output , wxT("Output"), false);
	}
	;

	void OnRunClick(wxCommandEvent& evt) {

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

		wxString input_file = wxString(m_fileIO.input_file.c_str(), wxConvUTF8);
		wxString output_file = wxString(m_fileIO.output_file.c_str(),
				wxConvUTF8);
		run = run + ' ' + input_file + ' ' + output_file;

		m_listbox_output->Append(run);

		m_process = new MyPipedProcess(NULL);
		m_pid = wxExecute(run, wxEXEC_ASYNC, m_process);
		if (!m_pid) {
			wxLogError(wxT("Execution of '%s' failed."), run.c_str());
			m_pid=0;

			delete m_process;
		} else {
			m_process->AddAsyncProcess(m_tool_output);
			m_runbutton->Disable();
			m_abortbutton->Show(true);

			m_process->m_ext_pid = m_pid;

			m_configpanel->SetSelection(m_configpanel->GetSelection()+1);
		};
		m_timer -> Start(150);
	}
	;

	void OnAbortClick(wxCommandEvent& evt) {
		if(wxProcess::Exists(m_pid) && (m_pid != 0) ){
		  wxProcess::Kill(m_pid);
		}
		m_abortbutton->Show(false);
		m_runbutton->Enable();
		m_timer->Stop();
	};

	void OnOutputFileChange(wxFileDirPickerEvent& evt) {
		m_fileIO.output_file = evt.GetPath().mb_str(wxConvUTF8);
	};

	void OnTimer(wxTimerEvent& event) {
		if (!wxProcess::Exists(m_pid)) {
			m_abortbutton->Show(false);
			m_runbutton->Enable();
			m_timer->Stop();
		}
	};

	virtual bool Destroy() {
		//TODO: If running, show modal dialog to ensure exit

		//Killing m_pid == 0 can terminate application
		if (wxProcess::Exists(m_pid) && (m_pid != 0)) {
			wxProcess::Kill(m_pid);
		}
		//Stop and clean up timer
		m_timer->Stop();
		return true;
	}
	;

	void OnResize(wxSizeEvent& evt){
		m_configpanel->SetSize( this->GetSize() );
	}

	wxAuiNotebook *m_parent;
	wxAuiNotebook *m_configpanel;
	OutputListBox *m_listbox_output;
	OutputListBox *m_tool_output;
	wxString m_input_file;
	Tool m_tool;

	FileIO m_fileIO;

	vector<wxRadioBox*> m_radiobox_ptrs;
	vector<wxCheckBox*> m_checkbox_ptrs;

	wxButton *m_runbutton;
	wxButton *m_abortbutton;

	MyPipedProcess *m_process;

	long m_pid;

	wxTimer *m_timer;

DECLARE_EVENT_TABLE()
};
BEGIN_EVENT_TABLE(ConfigPanel, wxNotebookPage)
		EVT_BUTTON(ID_RUN_TOOL, ConfigPanel::OnRunClick) EVT_BUTTON(ID_ABORT_TOOL, ConfigPanel::OnAbortClick)

		EVT_FILEPICKER_CHANGED(ID_OUTPUT_FILE, ConfigPanel::OnOutputFileChange)
		EVT_TIMER(ID_TIMER, ConfigPanel::OnTimer)
		EVT_SIZE(ConfigPanel::OnResize)
		END_EVENT_TABLE ()

#endif /* MCRL2_GUI_CONFIGPANEL_H_ */
