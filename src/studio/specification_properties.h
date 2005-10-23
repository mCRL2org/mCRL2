#ifndef SPECIFICATION_PROPERTIES_DIALOG_H
#define SPECIFICATION_PROPERTIES_DIALOG_H

#include <wx/wx.h>
#include <wx/dialog.h>

class SpecificationPropertiesDialog : public wxDialog {
  DECLARE_CLASS(SpecificationPropertiesDialog)
  DECLARE_EVENT_TABLE()

  public:
    SpecificationPropertiesDialog();
    SpecificationPropertiesDialog(wxWindow*, wxWindowID, wxString);

  private:
};

#endif
