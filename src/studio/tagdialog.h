#ifndef __TAGDIALOG_H
#define __TAGDIALOG_H

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "tagdialog.h"
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/dialog.h>
#include <wx/textctrl.h>

class TagDialog: public wxDialog
{
	public:
		TagDialog(wxWindow *parent, const wxString &info, const wxString &title);
		TagDialog(wxWindow *parent, const wxString &info, const wxString &title, const wxString &tag, const wxString &description);

	public:
		void OnOK(wxCommandEvent& event);

	public:
		void CreateContent(const wxString &info);
		wxString GetTag();
		wxString GetDescription();

	private:
		wxTextCtrl *tagedit;
		wxTextCtrl *descedit;

	DECLARE_EVENT_TABLE()
};

#endif
