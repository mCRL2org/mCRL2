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

		m_configpanel = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
				wxAUI_NB_BOTTOM
			);

		m_tool_output = new OutputListBox(m_configpanel, wxID_ANY, wxDefaultPosition, wxDefaultSize);
		m_wsw = new wxScrolledWindow(m_configpanel, wxID_ANY, wxDefaultPosition, wxDefaultSize);

		/* Creates a 1000px & 1000px virtual panel */
		m_wsw->SetScrollbars( 20, 20, 50, 50 );

		/* Define size large enough for top*/
		wxPanel *top = new wxPanel(m_wsw , wxID_ANY, wxDefaultPosition, wxSize(4096,4096));


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
			height += h + 2 * border;

		    new wxStaticLine(top, wxID_ANY, wxPoint(border, height),
		        wxSize(4000,1));

		    height += border;

		}

		/* Parse and display options */
		vector<Tool_option> vto = tool.m_tool_options;

		wxRadioBox *rb;
		wxStaticText *ws, *ws1, *ws2;
		wxArrayString as;
		wxCheckBox *cb;
		wxTextCtrl *tc;
		wxFilePickerCtrl *fp;

		int textctrl_width = 0;

		for (vector<Tool_option>::iterator i = vto.begin(); i != vto.end(); ++i) {
			if (((*i).m_widget == textctrl) || ((*i).m_widget == filepicker )){
				int w, u;
				wxStaticText *tmp = new wxStaticText(this, wxID_ANY, wxString(
						(*i).m_flag.c_str(), wxConvUTF8), wxPoint(border, height));
				tmp->GetSize(&w, &u);

				textctrl_width = max(textctrl_width, w);

				delete tmp;
			}
		}


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
				as.Clear();
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
			case textctrl:
				/* display label */
				ws1 = new wxStaticText(top, wxID_ANY, wxString(
						(*i).m_flag.c_str(), wxConvUTF8), wxPoint(border, height));

				ws1->GetSize(&v, &f);

				w = textctrl_width + 2*border;

				// TODO: Set Default values

				/* create text input box */
				tc = new wxTextCtrl(top, wxID_ANY, wxT(""),
						wxPoint(w,height));

				tc->SetLabel(wxString(
						(*i).m_flag.c_str(), wxConvUTF8));

				m_textctrl_ptrs.push_back(tc);

				tc->GetSize(&v, &h);

				w = v+ border + w;

				ws2 = new wxStaticText(top, wxID_ANY, wxString(
						(*i).m_help.c_str(), wxConvUTF8), wxPoint(w , height));

				v =0 ;
				ws1->GetSize(&w, &h);
				v = max(h,v);
				ws2->GetSize(&w, &h);
				v = max(h,v);
				tc->GetSize(&w, &h);
				h = max(h,v);

				height = height + h;
				break;
			case filepicker:
				/* display label */
				ws1 = new wxStaticText(top, wxID_ANY, wxString(
						(*i).m_flag.c_str(), wxConvUTF8), wxPoint(border, height));

				ws1->GetSize(&v, &f);

				w = v+ 2*border;

				// TODO: Set Default values

				/* create text input box */
				fp = new wxFilePickerCtrl(top, wxID_ANY,  wxT(""),
						wxT("Select a file"), wxT("*.*"), wxPoint(w , height), wxSize(350, 25),
						wxFLP_USE_TEXTCTRL | wxFLP_OPEN );

				fp->SetLabel(wxString(
						(*i).m_flag.c_str(), wxConvUTF8));

				m_filepicker_ptrs.push_back(fp);

				fp->GetSize(&v, &h);

				w = v+ border + w;

				ws2 = new wxStaticText(top, wxID_ANY, wxString(
						(*i).m_help.c_str(), wxConvUTF8), wxPoint(w , height));

				v =0 ;
				ws1->GetSize(&w, &h);
				v = max(h,v);
				ws2->GetSize(&w, &h);
				v = max(h,v);
				fp->GetSize(&w, &h);
				h = max(h,v);

				height = height + h;
				break;
			}

		};

	    height += border;

	    new wxStaticLine(top, wxID_ANY, wxPoint(border, height),
	        wxSize(4000,1));

	    height += border;

		m_runbutton = new wxButton(top, ID_RUN_TOOL, wxT("Run"), wxPoint(
				border, height));

		m_abortbutton = new wxButton(top, ID_ABORT_TOOL, wxT("Abort"),
				wxPoint(100, height));
		m_abortbutton->Show(false);

		top->Layout();

		m_configpanel->AddPage( m_wsw , wxT("Configuration"), true);
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
		run = run + wxT(' ') + input_file + wxT(' ') + output_file;

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
