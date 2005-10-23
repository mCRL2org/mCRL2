#include "specification_properties.h"

IMPLEMENT_CLASS(SpecificationPropertiesDialog, wxDialog)

BEGIN_EVENT_TABLE(SpecificationPropertiesDialog, wxDialog)
END_EVENT_TABLE()

SpecificationPropertiesDialog::SpecificationPropertiesDialog(wxWindow* parent, wxWindowID id, wxString title) :
  wxDialog(parent, id, title, wxDefaultPosition, wxSize(400,400), wxCAPTION) {

  /* Create controls */
  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

  SetSizer(sizer);

  Centre();
}

