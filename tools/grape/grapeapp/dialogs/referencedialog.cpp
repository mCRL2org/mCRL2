// Author(s): VitaminB100
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file referencedialog.cpp
//
// Defines a reference selection dialog.

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

#include "referencedialog.h"

using namespace grape::grapeapp;

grape_reference_dialog::grape_reference_dialog( process_reference *p_ref, grape_specification *p_spec )
: wxDialog( 0, wxID_ANY, _T( "Edit process reference" ), wxDefaultPosition, wxDefaultSize, wxRESIZE_BORDER | wxDEFAULT_DIALOG_STYLE )
{
  init_for_processes( p_ref->get_relationship_refers_to(), p_ref->get_text(), p_spec );
}

grape_reference_dialog::grape_reference_dialog( reference_state *p_ref, grape_specification *p_spec )
: wxDialog( 0, wxID_ANY, _T( "Edit process reference" ), wxDefaultPosition, wxDefaultSize, wxRESIZE_BORDER | wxDEFAULT_DIALOG_STYLE )
// description != bug
{
  init_for_processes( p_ref->get_relationship_refers_to(), p_ref->get_text(), p_spec );
}

grape_reference_dialog::grape_reference_dialog( architecture_reference *p_ref, grape_specification *p_spec )
: wxDialog( 0, wxID_ANY, _T("Edit architecture reference") )
{
  wxPanel *panel = new wxPanel( this );

  wxGridSizer *grid = new wxFlexGridSizer( 2, 3, 0 );

  wxStaticText *text = new wxStaticText( panel, wxID_ANY, _T( "Refers to:" ) );
  grid->Add( text, 0 );

  // choices
  int selected = wxNOT_FOUND;
  uint count = p_spec->count_architecture_diagram();
  wxArrayString choices;
  for ( uint i = 0; i < count; ++i )
  {
    architecture_diagram *diagram = p_spec->get_architecture_diagram( i );
    int pos = choices.Add( diagram->get_name() );
    m_pos2diagramid[pos] = diagram->get_id();
    if ( p_ref->get_relationship_refers_to() &&
         p_ref->get_relationship_refers_to()->get_id() == diagram->get_id() )
    {
      selected = pos;
    }
  }
  m_combo = new wxComboBox( panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, choices, wxCB_SORT );
  grid->Add( m_combo, 1, wxEXPAND, 0 );
  m_combo->SetSelection( selected );

  panel->SetSizer( grid );

  init( panel );

  m_combo->SetFocus();
}

void grape_reference_dialog::init_for_processes( diagram *p_diagram, const wxString &p_text, grape_specification *p_spec )
{
  wxPanel *panel = new wxPanel( this );

  wxGridSizer *grid = new wxFlexGridSizer( 2, 3, 0 );

  // refers to
  wxBoxSizer *vsizer = new wxBoxSizer( wxVERTICAL );
  wxStaticText *text = new wxStaticText( panel, wxID_ANY, _T( "Refers to:" ) );
  grid->Add( text, 0 );

  // choices
  int selected = wxNOT_FOUND;
  uint count = p_spec->count_process_diagram();
  wxArrayString choices;
  for ( uint i = 0; i < count; ++i )
  {
    process_diagram *diagram = p_spec->get_process_diagram( i );
    int pos = choices.Add( diagram->get_name() );
    m_pos2diagramid[pos] = diagram->get_id();
    if ( p_diagram && p_diagram->get_id() == diagram->get_id() )
    {
      selected = pos;
    }
  }
  m_combo = new wxComboBox( panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, choices, wxCB_SORT );
  grid->Add( m_combo, 1, wxEXPAND );
  m_combo->SetSelection( selected );

  vsizer->Add( grid );

  text = new wxStaticText( panel, wxID_ANY, _T("Parameter initializations:") );
  vsizer->Add( text, 0 );

  // create text control
  m_input = new wxTextCtrl( panel, wxID_ANY, p_text, wxDefaultPosition, wxSize(400, 300), wxTE_MULTILINE );
  vsizer->Add(m_input, 1, wxEXPAND );

  panel->SetSizer( vsizer );

  init( panel );

  m_combo->SetFocus();
}

grape_reference_dialog::grape_reference_dialog()
: wxDialog()
{
  // shouldn't be called
}

void grape_reference_dialog::init( wxPanel *p_panel )
{
  wxBoxSizer *wnd_sizer = new wxBoxSizer(wxVERTICAL);

  wnd_sizer->AddSpacer( 3 );
  wnd_sizer->Add(p_panel, 1, wxEXPAND, 0 );
  wnd_sizer->AddSpacer( 3 );

  // create buttons
  wxSizer *sizer = CreateButtonSizer(wxOK | wxCANCEL);
  sizer->Layout();
  wnd_sizer->Add(sizer, 0, wxALIGN_RIGHT, 0);

  // realize sizers
  SetSizer(wnd_sizer);
  wnd_sizer->Fit(this);
  wnd_sizer->SetSizeHints(this);
}

grape_reference_dialog::~grape_reference_dialog()
{
}

bool grape_reference_dialog::show_modal()
{
  return ShowModal() != wxID_CANCEL;
}

int grape_reference_dialog::get_diagram_id()
{
  int selected = m_combo->GetSelection();
  if ( selected == wxNOT_FOUND )
  {
    return wxNOT_FOUND;
  }
  else
  {
    return m_pos2diagramid[ m_combo->GetSelection() ];
  }
}

wxString grape_reference_dialog::get_diagram_name() const
{
  return m_combo->GetValue();
}

wxString grape_reference_dialog::get_initializations() const
{
  return m_input ? m_input->GetValue() : _T("");
}
