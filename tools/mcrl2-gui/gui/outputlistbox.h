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

	void OnPopupClick(wxCommandEvent &evt){
		switch (evt.GetId()) {
		case ID_CLEAR_LISTBOX:
			this->Clear();
			break;
		case ID_SAVE_LISTBOX:
			wxFileDialog *fd = new wxFileDialog(this, wxT("Choose a file"), wxT("") , wxT(""),  wxT("*.*"), wxSAVE | wxOVERWRITE_PROMPT,  wxDefaultPosition);
			if (fd->ShowModal() == wxID_OK ){
				wxString sfile = fd->GetPath();

				if(wxFile::Exists(sfile)){
					wxRemoveFile(sfile);
				}

				wxFile *f = new wxFile(sfile, wxFile::write);

				for( unsigned int i = 0; i < this->GetCount(); ++i ){
					f->Write( this->GetString(i) );
					f->Write( wxT("\n"));
				}

			}
			break;
		}
	}

	void OnRightClick(wxMouseEvent& evt){

#ifndef __WINDOWS__
		/* Disabled for WIN32 due to invalid cast */
		wxMenu mnu;
		mnu.Append(ID_CLEAR_LISTBOX, wxT("Clear"));
		mnu.AppendSeparator();
		mnu.Append(ID_SAVE_LISTBOX, wxT("Save to file"));
		mnu.Connect(wxEVT_COMMAND_MENU_SELECTED,
				(wxObjectEventFunction) &OutputListBox::OnPopupClick, NULL,
				this);
		PopupMenu(&mnu);
#endif
	}

DECLARE_EVENT_TABLE()
};
BEGIN_EVENT_TABLE(OutputListBox, wxListBox)
EVT_RIGHT_UP( OutputListBox::OnRightClick )
END_EVENT_TABLE ()
#endif /* OUTPUTLISTBOX_H_ */
