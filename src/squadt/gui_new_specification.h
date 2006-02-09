#ifndef NEW_SPECIFICATION_DIALOG_H
#define NEW_SPECIFICATION_DIALOG_H

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/dirctrl.h>

namespace squadt {
  namespace GUI {

    class NewSpecificationDialog : public wxDialog {
      DECLARE_CLASS(NewSpecificationDialog)
      DECLARE_EVENT_TABLE()
 
      public:
        NewSpecificationDialog();
        NewSpecificationDialog(wxWindow*, wxWindowID);
  
        wxString GetName();
        wxString GetFileName();
        wxString GetFilePath();
        wxString GetPath();
  
      private:
        wxTextCtrl*       name_field;
        wxTextCtrl*       select_field;
        wxGenericDirCtrl* file_field;
  
        void UpdateSelectField(wxCommandEvent& event);
    };
  }
}

#endif
