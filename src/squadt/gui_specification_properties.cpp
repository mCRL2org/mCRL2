#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/listctrl.h>
#include <wx/textctrl.h>
#include <wx/filename.h>

#include <boost/filesystem/operations.hpp>

#include "gui_project_overview.h"
#include "gui_specification_properties.h"
#include "gui_resources.h"

#include "project_manager.h"
#include "tool_manager.h"

#include "ui_core.h"

IMPLEMENT_CLASS(SpecificationPropertiesDialog, wxDialog)

BEGIN_EVENT_TABLE(SpecificationPropertiesDialog, wxDialog)
END_EVENT_TABLE()

SpecificationPropertiesDialog::SpecificationPropertiesDialog(wxWindow* parent, wxWindowID id, wxString title, Specification& aspecification, std::string aproject_root) :
  wxDialog(parent, id, title, wxDefaultPosition, wxSize(450,450), wxCAPTION|wxSTAY_ON_TOP), specification(aspecification) {

  /* Create controls */
  wxBoxSizer* sizer      = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* controlbox = new wxBoxSizer(wxHORIZONTAL);

  description   = new wxTextCtrl(this, wxID_ANY, wxString(specification.GetDescription().c_str(), wxConvLocal), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER|wxTE_MULTILINE|wxTE_BESTWRAP);

  project_root = aproject_root;

  /* Control to add or edit a description */
  wxStaticBoxSizer* descriptionbox   = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Description"));

  descriptionbox->Add(description, 1, wxEXPAND|wxALIGN_LEFT, 0);

  sizer->Add(descriptionbox, 1, wxALL|wxEXPAND, 8);

  if (0 < specification.GetNumberOfInputObjects()) {
    /* Specification has output objects (of course) */
    wxStaticBoxSizer* inputbox = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Input details"));

    inputs = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_VRULES|wxLC_HRULES);

    /* Assign iconlist (TODO type icon list) */
    inputs->SetImageList(format_small_icon_list, wxIMAGE_LIST_SMALL);

    inputs->InsertColumn(0, wxT("Name"));
    inputs->InsertColumn(1, wxT("Location"));

    inputbox->Add(inputs, 1, wxEXPAND, 0);

    sizer->Add(inputbox, 2, wxALL|wxEXPAND, 5);

    /* Specification type is generated */
    wxStaticBoxSizer* toolbox        = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Tool information"));
    wxFlexGridSizer*  textgrid       = new wxFlexGridSizer(2, 2, 5, 5);

    textgrid->AddGrowableCol(1, 2);

    tool_arguments = new wxTextCtrl(this, wxID_ANY, wxString(specification.GetToolConfiguration().c_str(), wxConvLocal), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);

    textgrid->Add(new wxStaticText(this, wxID_ANY, wxT("Name : ")), 1);
    textgrid->Add(new wxStaticText(this, wxID_ANY, wxString(tool_manager.GetTool(specification.GetToolIdentifier())->GetIdentifier().c_str(), wxConvLocal)), 2);
    textgrid->Add(new wxStaticText(this, wxID_ANY, wxT("Arguments : ")), 1);
    textgrid->Add(tool_arguments, 1, wxALL|wxEXPAND, 0);
    toolbox->Add(textgrid, 1, wxLEFT|wxTOP|wxEXPAND, 2);

    std::vector < SpecificationInputType >::const_iterator b = specification.GetInputObjects().end();
    std::vector < SpecificationInputType >::const_iterator i = specification.GetInputObjects().begin();
    unsigned int c = 0;

    while (i != b) {
      Specification* parent = (*i).derived_from.pointer;

      inputs->InsertItem(c, wxString(parent->GetName().c_str(), wxConvLocal), 0);
      inputs->SetItem(c, 1, wxString(parent->GetOutputObjects()[(*i).output_number].file_name.c_str(), wxConvLocal));

      ++c;
      ++i;
    }

    /* Add content */
    sizer->Add(toolbox, 1, wxALL|wxEXPAND, 5);
  }
  else {
    wxStaticBoxSizer* outputbox = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Output details"));

    outputs = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_VRULES|wxLC_HRULES);

    /* Assign iconlist (TODO type icon list) */
    outputs->SetImageList(format_small_icon_list, wxIMAGE_LIST_SMALL);

    outputs->InsertColumn(0, wxT("Name"));
    outputs->InsertColumn(1, wxT("Format"));
    outputs->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER);
    outputs->InsertColumn(2, wxT("Last modified"));
    outputs->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
    outputs->InsertColumn(3, wxT("Size"));

    outputbox->Add(outputs, 1, wxEXPAND, 0);

    sizer->Add(outputbox, 2, wxALL|wxEXPAND, 5);

    const std::vector < SpecificationOutputType >::const_iterator b = specification.GetOutputObjects().end();
          std::vector < SpecificationOutputType >::const_iterator i = specification.GetOutputObjects().begin();
    unsigned int c = 0;

    while (i != b) {
      wxString                file_name((*i).file_name.c_str(), wxConvLocal);
      wxString                file_format((*i).format.c_str(), wxConvLocal);
      boost::filesystem::path full_path(project_root);

      full_path /= boost::filesystem::path((*i).file_name);

      outputs->InsertItem(c, file_name, 0);
      outputs->SetItem(c, 1, file_format);

      if (boost::filesystem::exists(full_path)) {
        /* Outputs exist on storage */
        wxFileName name(wxString(full_path.string().c_str(), wxConvLocal));

        outputs->SetItem(c, 2, name.GetModificationTime().Format(wxT("%D")));

        try {
          size_t size = boost::filesystem::file_size(full_path);

          outputs->SetItem(c, 3, wxString::Format(wxT("%u"), size));
        }
        catch (...) {
          outputs->SetItem(c, 3, wxT("0"));
        }
      }
      else {
        outputs->SetItem(c, 2, wxT("N.A."));
        outputs->SetItem(c, 3, wxT("N.A."));
      }

      ++c;
      ++i;
    }
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
  if (0 < specification.GetNumberOfInputObjects()) {
    inputs->SetColumnWidth(0, wxLIST_AUTOSIZE);
    inputs->SetColumnWidth(1, (inputs->GetClientSize().GetWidth() - inputs->GetColumnWidth(0)));
  }
  else {
    outputs->SetColumnWidth(0, wxLIST_AUTOSIZE);
    outputs->SetColumnWidth(1, wxLIST_AUTOSIZE);
    outputs->SetColumnWidth(3, (outputs->GetClientSize().GetWidth() - outputs->GetColumnWidth(0) - outputs->GetColumnWidth(1) - outputs->GetColumnWidth(2)));
  }
}

void SpecificationPropertiesDialog::StoreChanges() {
  if (description->IsModified()) {
    specification.SetDescription(std::string(description->GetValue().fn_str()));
  }

  if (0 < specification.GetNumberOfInputObjects()) {
    if (tool_arguments->IsModified()) {
      wxString value = tool_arguments->GetValue();
 
      /* Strip line feeds */
      value.Replace(wxT("\n"), wxT(""), true);
 
      specification.SetToolConfiguration(std::string(value.fn_str()));

      specification.ForceRegeneration();
    }
  }
}

