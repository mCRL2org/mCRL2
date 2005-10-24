#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/listctrl.h>

#include "project_manager.h"
#include "specification_properties.h"
#include "resources.h"

IMPLEMENT_CLASS(SpecificationPropertiesDialog, wxDialog)

BEGIN_EVENT_TABLE(SpecificationPropertiesDialog, wxDialog)
END_EVENT_TABLE()

SpecificationPropertiesDialog::SpecificationPropertiesDialog(wxWindow* parent, wxWindowID id, wxString title, Specification*& aspecification) :
  wxDialog(parent, id, title, wxDefaultPosition, wxSize(400,400), wxCAPTION|wxSTAY_ON_TOP) {

  /* Create controls */
  wxBoxSizer*       sizer      = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer*       controlbox = new wxBoxSizer(wxHORIZONTAL);

  specification = aspecification;

  if (specification->description != "") {
    wxStaticBoxSizer* descriptionbox = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Description"));

    descriptionbox->Add(new wxStaticText(this, wxID_ANY, wxString(specification->description.c_str(), wxConvLocal)), 1, wxALL|wxALIGN_LEFT, 5);

    sizer->Add(descriptionbox, 1, wxALL|wxEXPAND, 8);
  }

  if (0 < specification->input_objects.size()) {
    /* Specification has output objects (of course) */
    wxStaticBoxSizer* inputbox = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Input objects"));

    /* Add content */
//    inputbox->Add(new wxStaticText(this, wxID_ANY, wxString(specification->name.c_str(), wxConvLocal).Prepend(wxT("Name : "))), wxALL, 10);

    inputs = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_VRULES|wxLC_HRULES);

    /* Assign iconlist (TODO type icon list) */
    inputs->SetImageList(main_icon_list, wxIMAGE_LIST_SMALL);

    inputs->InsertColumn(0, wxT("Name"));
    inputs->InsertColumn(1, wxT("Location"));

    inputbox->Add(inputs, 1, wxEXPAND, 0);

    sizer->Add(inputbox, 2, wxALL|wxEXPAND, 5);

    /* Specification type is generated */
    wxStaticBoxSizer* toolbox  = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Tool information"));
    wxFlexGridSizer*  textgrid = new wxFlexGridSizer(2, 2, 5, 5);

    textgrid->Add(new wxStaticText(this, wxID_ANY, wxT("Name : ")), 1);
    textgrid->Add(new wxStaticText(this, wxID_ANY, wxString(specification->tool_identifier.c_str(), wxConvLocal)), 2);
    textgrid->Add(new wxStaticText(this, wxID_ANY, wxT("Arguments : ")), 1);
    textgrid->Add(new wxStaticText(this, wxID_ANY, wxString(specification->tool_configuration.c_str(), wxConvLocal)), 2);
    toolbox->Add(textgrid, 1, wxALL|wxEXPAND, 8);

    std::list < InputPair >::iterator b = specification->input_objects.end();
    std::list < InputPair >::iterator i = specification->input_objects.begin();
    unsigned int c = 0;

    while (i != b) {
      inputs->InsertItem(c, wxString((*i).first->name.c_str(), wxConvLocal), 0);
      inputs->SetItem(c, 1, wxString((*i).second.c_str(), wxConvLocal));

      ++c;
      ++i;
    }

    /* Add content */
//    toolbox->Add(new wxStaticText(this, wxID_ANY, wxT("")));

    sizer->Add(toolbox, 1, wxALL|wxEXPAND, 5);
  }
  else {
    wxStaticBoxSizer* textbox = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Specification objects"));

    textbox->Add(new wxStaticText(this, wxID_ANY, wxT("Specification is not generated from other specifications.")), wxALL|wxEXPAND, 10);

    sizer->Add(textbox, 1, wxALL|wxEXPAND, 10);
  }

  sizer->Add(new wxStaticLine(this, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxALL|wxEXPAND, 2);
  sizer->Add(controlbox, 0, wxALL|wxALIGN_RIGHT, 5);

  /* Add ok and cancel button */
  controlbox->Add(new wxButton(this, wxID_OK), 0, wxALL|wxALIGN_RIGHT, 3);
  controlbox->Add(new wxButton(this, wxID_CANCEL), 0, wxALL|wxALIGN_RIGHT, 3);

  SetSizer(sizer);

  Centre();
}

void SpecificationPropertiesDialog::UpdateSizes() {
  /* Compute proper size for columns */
  if (0 < specification->input_objects.size()) {
    inputs->SetColumnWidth(0, wxLIST_AUTOSIZE);
    inputs->SetColumnWidth(1, (inputs->GetClientSize().GetWidth() - inputs->GetColumnWidth(0)));
  }
}

