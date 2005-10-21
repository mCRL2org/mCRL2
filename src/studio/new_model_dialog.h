#ifndef NEW_MODEL_DIALOG_H_
#define NEW_MODEL_DIALOG_H_

#include <wx/wx.h>
#include <wx/dialog.h>

class NewModelDialog : public wxDialog {
  DECLARE_CLASS(NewModelDialog)
  DECLARE_EVENT_TABLE()

  public:
    NewModelDialog();
    ~NewModelDialog();
    NewModelDialog(wxWindow*, wxWindowID);

    wxString GetModelName();
    wxString GetModelFileName();
    wxString GetModelPath();

  private:
    wxTextCtrl*       name_field;
    wxTextCtrl*       select_field;
    wxGenericDirCtrl* file_field;

    void UpdateSelectField(wxCommandEvent& event);
};

#endif
