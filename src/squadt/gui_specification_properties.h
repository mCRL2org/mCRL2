#ifndef SPECIFICATION_PROPERTIES_DIALOG_H
#define SPECIFICATION_PROPERTIES_DIALOG_H

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/listctrl.h>
#include <wx/treectrl.h>
    
namespace squadt {
  namespace GUI {
    
    class SpecificationPropertiesDialog : public wxDialog {
      DECLARE_CLASS(SpecificationPropertiesDialog)
      DECLARE_EVENT_TABLE()
    
      private:
        Specification& specification;
        std::string    project_root;
        wxListCtrl*    inputs;
        wxListCtrl*    outputs;
        wxTextCtrl*    description;
        wxTextCtrl*    tool_arguments;
    
      public:
    
        SpecificationPropertiesDialog(wxWindow*, wxWindowID, wxString, Specification&, std::string);
    
        void SpecificationPropertiesDialog::UpdateSizes();
    
        void StoreChanges();
    };
  }
}
  
#endif
