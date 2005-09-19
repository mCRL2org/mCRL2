#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "tagdialog.h"
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "tagdialog.h"

BEGIN_EVENT_TABLE(TagDialog,wxDialog)
	EVT_BUTTON(wxID_OK, TagDialog::OnOK)
END_EVENT_TABLE()

TagDialog::TagDialog(wxWindow *parent, const wxString &info, const wxString &title) : wxDialog(parent,-1,title)
{
	CreateContent(info);
}

TagDialog::TagDialog(wxWindow *parent, const wxString &info, const wxString &title, const wxString &tag, const wxString &description) : wxDialog(parent,-1,title)
{
	CreateContent(info);
	tagedit->WriteText(tag);
	descedit->WriteText(description);
}

void TagDialog::OnOK(wxCommandEvent& event)
{
	if ( tagedit->GetValue().IsEmpty() )
	{
		wxMessageDialog dialog(this,wxT("You must supply a tag."),wxT("No tag"),wxOK|wxICON_ERROR);
		dialog.ShowModal();
	} else {
		wxDialog::OnOK(event);
	}
}

void TagDialog::CreateContent(const wxString &info)
{
	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	wxFlexGridSizer *grid = new wxFlexGridSizer(2,2,5,5);
	wxSizer *buttonsizer = CreateButtonSizer(wxOK|wxCANCEL);

	wxStaticText *infolabel = new wxStaticText(this,-1,info);
	wxStaticText *taglabel = new wxStaticText(this,-1,wxT("Tag:"));
	tagedit = new wxTextCtrl(this,-1,wxT(""),wxDefaultPosition,wxSize(200,-1));
	wxStaticText *desclabel = new wxStaticText(this,-1,wxT("Description:"));
	descedit = new wxTextCtrl(this,-1,wxT(""),wxDefaultPosition,wxSize(200,-1));

	grid->Add(taglabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	grid->Add(tagedit, 0, wxALIGN_CENTER|wxALL, 5);
	grid->Add(desclabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	grid->Add(descedit, 0, wxALIGN_CENTER|wxALL, 5);

	sizer->Add(infolabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizer->Add(grid, 0, wxALIGN_CENTER|wxALL, 5);
	sizer->Add(buttonsizer, 0, wxALIGN_CENTER|wxALL, 5);

	SetSizer(sizer);
	sizer->Fit(this);

	tagedit->SetFocus();
}

wxString TagDialog::GetTag()
{
	return tagedit->GetValue();
}

wxString TagDialog::GetDescription()
{
	return descedit->GetValue();
}
