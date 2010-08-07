/*
 * output_listbox.h
 *
 *  Created on: Jun 18, 2010
 *      Author: fstapper
 */

#ifndef MCRL2_OUTPUTLISTBOX_H_
#define MCRL2_OUTPUTLISTBOX_H_

#include <wx/filedlg.h>
#include <wx/listbox.h>

#define ID_CLEAR_LISTBOX	1500
#define ID_SAVE_LISTBOX		1501

class OutputListBoxMenu: public wxMenu{
public:
	wxListBox *p;

	OutputListBoxMenu(wxListBox *parent): wxMenu(){
		this->Append(ID_CLEAR_LISTBOX, wxT("Clear"));
		this->AppendSeparator();
		this->Append(ID_SAVE_LISTBOX, wxT("Save to file"));

		p = parent;
	}

		void OnClear(wxCommandEvent &/*event*/){
			p->Clear();
		}

		void OnSave(wxCommandEvent &/*event*/){

			wxFileDialog *fd = new wxFileDialog(p, wxT("Choose a file"), wxT("") , wxT(""),  wxT("*.*"), wxSAVE | wxOVERWRITE_PROMPT,  wxDefaultPosition);
			if (fd->ShowModal() == wxID_OK ){
				wxString sfile = fd->GetPath();

				if(wxFile::Exists(sfile)){
					wxRemoveFile(sfile);
				}

				wxFile *f = new wxFile(sfile, wxFile::write);

				for( unsigned int i = 0; i < p->GetCount(); ++i ){
					f->Write( p->GetString(i) );
					f->Write( wxT("\n"));
				}

			}
		}
	
	DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(OutputListBoxMenu, wxMenu)
EVT_MENU(ID_CLEAR_LISTBOX, OutputListBoxMenu::OnClear )
EVT_MENU(ID_SAVE_LISTBOX, OutputListBoxMenu::OnSave )
END_EVENT_TABLE ()

class OutputListBox: public wxListBox {
public:

	OutputListBox(wxWindow *parent, wxWindowID id, const wxPoint& pos =
			wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0,
			const wxString choices[] = (const wxString *) NULL, long style = 0,
			const wxValidator& validator = wxDefaultValidator,
			const wxString& name = wxListBoxNameStr) :
		wxListBox(parent, id, pos, size, n, choices, style, validator, name) {

    wxFont font(wxNORMAL_FONT->GetPointSize(),
    		wxMODERN, wxFONTSTYLE_NORMAL,
    		wxLIGHT, false);
    this->SetFont(font);
	}



	void OnRightClick(wxMouseEvent& /*event*/){

		OutputListBoxMenu *m = new OutputListBoxMenu(this);
		PopupMenu(m);
	}

DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(OutputListBox, wxListBox)
EVT_RIGHT_UP( OutputListBox::OnRightClick )
END_EVENT_TABLE ()


#endif /* OUTPUTLISTBOX_H_ */
