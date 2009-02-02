// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file parameterdialog.cpp
//
// Defines a parameter initialisation dialog.

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

#include "parameterdialog.h"

using namespace grape::grapeapp;

grape_parameter_dialog::grape_parameter_dialog(list_of_decl &p_parameter_declarations)
: wxDialog( 0, wxID_ANY, _T("Set parameter initialisation"), wxDefaultPosition, wxDefaultSize, wxRESIZE_BORDER | wxDEFAULT_DIALOG_STYLE )
{
  wxPanel *panel = new wxPanel(this);

  wxGridSizer *grid = new wxFlexGridSizer(2, 3, 0);

  // refers to
  wxBoxSizer *vsizer = new wxBoxSizer( wxVERTICAL );
  wxStaticText *text = new wxStaticText( panel, wxID_ANY, _T( "Parameter:" ) );
  grid->Add( text, 0 );

  // choices
  wxArrayString choices;
  m_init.Empty();
  for(unsigned int i=0; i<p_parameter_declarations.GetCount(); ++i)
  {
    decl_init t_param;
    wxString param_name = p_parameter_declarations[i].get_name();
    t_param.set_name(param_name);
    m_init.Add(t_param);
    choices.Add(param_name);
  }

  m_combo = new wxComboBox(panel, GRAPE_PARAMETER_DIALOG_COMBO, wxEmptyString, wxDefaultPosition, wxDefaultSize, choices, wxCB_SORT | wxCB_READONLY);
  grid->Add(m_combo, 1, wxEXPAND, 0);
  m_combo->SetSelection(0);
  m_combo_current = 0;

  vsizer->Add(grid);

  text = new wxStaticText( panel, wxID_ANY, _T("Parameter initialisation:") );
  vsizer->Add( text, 0 );

  // create text control
  m_input = new wxTextCtrl( panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(400, 300), wxTE_MULTILINE );
  vsizer->Add(m_input, 1, wxEXPAND );

  panel->SetSizer( vsizer );

  wxBoxSizer *wnd_sizer = new wxBoxSizer(wxVERTICAL);

  wnd_sizer->AddSpacer( 3 );
  wnd_sizer->Add(panel, 1, wxEXPAND, 0 );
  wnd_sizer->AddSpacer( 3 );

  // create buttons
  wxSizer *sizer = CreateButtonSizer(wxOK | wxCANCEL);
  sizer->Layout();
  wnd_sizer->Add(sizer, 0, wxALIGN_RIGHT, 0);

  // realize sizers
  SetSizer(wnd_sizer);
  wnd_sizer->Fit(this);
  wnd_sizer->SetSizeHints(this);

  m_input->SetFocus();

}

grape_parameter_dialog::grape_parameter_dialog()
: wxDialog()
{
  // shouldn't be called
}

grape_parameter_dialog::~grape_parameter_dialog()
{
}

bool grape_parameter_dialog::show_modal()
{
  return ShowModal() != wxID_CANCEL;
}

list_of_decl_init grape_parameter_dialog::get_initialisations() const
{
  return m_init;
}

void grape_parameter_dialog::event_combo(wxCommandEvent &WXUNUSED(p_event))
{
  // save initialisation
  wxString param_init = m_input->GetValue();
  m_init[m_combo_current].set_value(param_init);
  m_combo_current = m_combo->GetCurrentSelection();
  m_input->SetValue(m_init[m_combo_current].get_value());
}

void grape_parameter_dialog::event_ok(wxCommandEvent &WXUNUSED(p_event))
{
  // save initialisation
  if(m_init.GetCount() > 0)
  {
    m_init[m_combo_current].set_value(m_input->GetValue());
  }
  EndModal(wxID_OK);
}

BEGIN_EVENT_TABLE(grape_parameter_dialog, wxDialog)
  EVT_COMBOBOX(GRAPE_PARAMETER_DIALOG_COMBO, grape_parameter_dialog::event_combo)
  EVT_BUTTON(wxID_OK, grape_parameter_dialog::event_ok)
END_EVENT_TABLE()
