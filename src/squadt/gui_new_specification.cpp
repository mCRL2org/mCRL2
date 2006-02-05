#include "gui_new_specification.h"

#include <wx/statline.h>

IMPLEMENT_CLASS(NewSpecificationDialog, wxDialog)

BEGIN_EVENT_TABLE(NewSpecificationDialog, wxDialog)
END_EVENT_TABLE()

NewSpecificationDialog::NewSpecificationDialog(wxWindow* parent, wxWindowID id) :
  wxDialog(parent, id, wxT("Add a specification to the project"), wxDefaultPosition, wxSize(400,400), wxCAPTION) {

  /* Create controls */
  wxBoxSizer*   sizer       = new wxBoxSizer(wxVERTICAL);
  wxStaticText* file_text   = new wxStaticText(this, wxID_ANY, wxT("Select a file to add to the project:"));
  wxStaticText* name_text   = new wxStaticText(this, wxID_ANY, wxT("The name for the specification in the project:"));

  /* The text control that holds the new name for the specification */
  name_field = new wxTextCtrl(this, wxID_ANY);

  /* TODO, start from the project directory */
  file_field   = new wxGenericDirCtrl(this, wxID_ANY, wxT("~/"), wxDefaultPosition, wxDefaultSize, wxDIRCTRL_3D_INTERNAL|wxSUNKEN_BORDER|wxDIRCTRL_SHOW_FILTERS, wxT("All files (*.*)|*.*|mCRL2 files (*.mcrl2)|*.mcrl2|mCRL2 files (*.lpe)|*.lpe"));
  select_field = new wxTextCtrl(this, wxID_ANY);

  /* Tie event to update the select field to the tree control of file_field */
  wxTreeCtrl* selectiontree = file_field->GetTreeCtrl();

  selectiontree->Connect(selectiontree->GetId(), wxEVT_COMMAND_TREE_SEL_CHANGED, wxCommandEventHandler(NewSpecificationDialog::UpdateSelectField), NULL, this);

  wxStaticBoxSizer* boxtsizer = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Select specification file"));
  wxStaticBoxSizer* boxmsizer = new wxStaticBoxSizer(wxVERTICAL, this, wxT("A name for the specification"));

  boxtsizer->Add(file_text, 0, wxALL|wxEXPAND, 2);
  boxtsizer->Add(file_field, 1, wxALL|wxEXPAND, 2);
  boxtsizer->Add(select_field, 0, wxALL|wxEXPAND, 2);
  boxmsizer->Add(name_text, 0, wxALL|wxEXPAND, 5);
  boxmsizer->Add(name_field, 0, wxALL|wxEXPAND, 5);

  sizer->Add(boxtsizer, 2, wxALL|wxEXPAND, 2);
  sizer->Add(boxmsizer, 0, wxALL|wxEXPAND, 2);
  sizer->Add(new wxStaticLine(this, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxALL|wxEXPAND, 2);

  wxBoxSizer* asizer = new wxBoxSizer(wxHORIZONTAL);

  /* Add control buttons */
  asizer->Add(new wxButton(this, wxID_OK), 0, wxALL|wxALIGN_RIGHT, 3);
  asizer->Add(new wxButton(this, wxID_CANCEL), 0, wxALL|wxALIGN_RIGHT, 3);
  sizer->Add(asizer, 0, wxALL|wxALIGN_RIGHT, 3);

  SetSizer(sizer);

  Centre();
}

/* Precondition the user has pushed the OK button */
wxString NewSpecificationDialog::GetName() {
  wxString name = name_field->GetValue();

  if (name == wxT("")) {
    wxString selected = select_field->GetValue();

    if (selected == wxT("")) {
      wxMessageDialog* dialog;
     
      dialog = new wxMessageDialog(this, wxT("No name, no file name so nothing to do!"), wxT("Warning"), wxOK);

      dialog->ShowModal();

      dialog->~wxMessageDialog();
    }
    else {
      name = selected;
    }
  }

  return(name);
}

/* Precondition the user has pushed the OK button */
wxString NewSpecificationDialog::GetFileName() {
  return(select_field->GetValue());
}

/* Precondition the user has pushed the OK button */
wxString NewSpecificationDialog::GetPath() {
  return(file_field->GetFilePath().BeforeLast('/'));
}

/* Precondition the user has pushed the OK button */
wxString NewSpecificationDialog::GetFilePath() {
  return(file_field->GetFilePath());
}

void NewSpecificationDialog::UpdateSelectField(wxCommandEvent&) {
  wxString filepath = file_field->GetFilePath();

  if (filepath == wxT("")) {
    select_field->SetValue(wxT(""));
  }
  else {
    select_field->SetValue(filepath.AfterLast('/'));
  }
}

