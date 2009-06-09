// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file referencedialog.cpp
//
// Defines a reference selection dialog.

#include "wx.hpp" // precompiled headers

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

#include "grape_ids.h"
#include "referencedialog.h"
#include "../events/event_base.h"
#include "../grape_frame.h"

using namespace grape::grapeapp;

grape_reference_dialog::grape_reference_dialog( grape_frame *p_main_frame, process_reference *p_ref, grape_specification *p_spec )
: wxDialog( 0, wxID_ANY, _T( "Edit process reference" ), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE )
// process reference
{
  m_main_frame = p_main_frame;
  list_of_varupdate varupdate_list = p_ref->get_parameter_updates();
  init_for_processes( p_ref->get_relationship_refers_to(), varupdate_list, p_spec );
}

grape_reference_dialog::grape_reference_dialog( grape_frame *p_main_frame, reference_state *p_ref, grape_specification *p_spec )
: wxDialog( 0, wxID_ANY, _T( "Edit process reference" ), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE )
// reference state
{
  m_main_frame = p_main_frame;
  init_for_processes( p_ref->get_relationship_refers_to(), p_ref->get_parameter_updates(), p_spec );
}

grape_reference_dialog::grape_reference_dialog( grape_frame *p_main_frame, architecture_reference *p_ref, grape_specification *p_spec )
: wxDialog( 0, wxID_ANY, _T("Edit architecture reference") , wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE )
// architecture reference
{
  m_main_frame = p_main_frame;
  wxPanel *panel = new wxPanel( this );

  wxGridSizer *grid = new wxFlexGridSizer( 2, 3, 0 );

  wxStaticText *text = new wxStaticText( panel, wxID_ANY, _T( "Refers to:" ) );
  grid->Add( text, 0 );

  // choices
  int selected = wxNOT_FOUND;
  wxString selected_name = wxEmptyString;
  unsigned int count = p_spec->count_architecture_diagram();
  wxArrayString choices;
  for ( unsigned int i = 0; i < count; ++i )
  {
    architecture_diagram *diagram = p_spec->get_architecture_diagram( i );
    int pos = choices.Add( diagram->get_name() );
    m_name2diagramid[ diagram->get_name() ] = diagram->get_id();
    if ( p_ref->get_relationship_refers_to() &&
         p_ref->get_relationship_refers_to()->get_id() == diagram->get_id() )
    {
      selected = pos;
      selected_name = p_ref->get_relationship_refers_to()->get_name();
    }
  }
  m_combo = new wxComboBox( panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, choices, wxCB_SORT );
  grid->Add( m_combo, 1, wxEXPAND, 0 );
  if (selected == wxNOT_FOUND)
  {
    m_combo->SetSelection( selected );
  }
  else
  {
    m_combo->SetStringSelection( selected_name );
  }

  panel->SetSizer( grid );

  init( panel );

  m_combo->SetFocus();
  CentreOnParent();
}

void grape_reference_dialog::init_for_processes( diagram *p_diagram, list_of_varupdate p_list_of_varupdate, grape_specification *p_spec )
{
  wxPanel *panel = new wxPanel( this );

  wxGridSizer *grid = new wxFlexGridSizer( 2, 3, 0 );

  // refers to
  wxBoxSizer *vsizer = new wxBoxSizer( wxVERTICAL );
  wxStaticText *text = new wxStaticText( panel, wxID_ANY, _T( "Refers to:" ) );
  grid->Add( text, 0 );

  // choices
  int selected = wxNOT_FOUND;
  wxString selected_name = wxEmptyString;
  unsigned int count = p_spec->count_process_diagram();
  wxArrayString choices;
  for ( unsigned int i = 0; i < count; ++i )
  {
    process_diagram *diagram = p_spec->get_process_diagram( i );
    int pos = choices.Add( diagram->get_name() );
    m_name2diagramid[ diagram->get_name() ] = diagram->get_id();
    if ( p_diagram && p_diagram->get_id() == diagram->get_id() )
    {
      selected = pos;
      selected_name = p_diagram->get_name();
    }
  }
  m_combo = new wxComboBox( panel, GRAPE_COMBO_TEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, choices, wxCB_SORT );
  grid->Add( m_combo, 1, wxEXPAND );
  if (selected == wxNOT_FOUND)
  {
    m_combo->SetSelection( selected );
  }
  else
  {
    m_combo->SetStringSelection( selected_name );
  }

  vsizer->Add( grid );

  text = new wxStaticText( panel, wxID_ANY, _T("Parameter initializations:") );
  vsizer->Add( text, 0 );

  // create grid
  m_grid = new wxGrid( panel, GRAPE_GRID_TEXT, wxDefaultPosition, wxSize(400, 300));
  m_grid->CreateGrid( p_list_of_varupdate.GetCount(), 2 );
  for ( unsigned int i = 0; i < p_list_of_varupdate.GetCount(); ++i )
  {
    // fill cells
    varupdate parameter_assignment = p_list_of_varupdate.Item( i );
    m_grid->SetCellValue(i, 0, parameter_assignment.get_lhs());
    m_grid->SetReadOnly(i, 0);
    m_grid->SetCellValue(i, 1, parameter_assignment.get_rhs());
  }

  m_grid->SetColSize( 0, 170 );
  m_grid->SetColSize( 1, 100 );
  m_grid->SetColLabelValue(0, _T("Name"));
  m_grid->SetColLabelValue(1, _T("Value"));
  m_grid->SetRowLabelSize(30);

  vsizer->Add(m_grid, 1, wxEXPAND );

  panel->SetSizer( vsizer );

  init( panel );

  m_combo->SetFocus();
  CentreOnParent();
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
  delete m_combo;
  delete m_grid;
}

bool grape_reference_dialog::show_modal()
{
  return ShowModal() != wxID_CANCEL;
}

int grape_reference_dialog::get_diagram_id()
{
  if (m_name2diagramid.find( m_combo->GetValue() ) == m_name2diagramid.end())
  {
    return wxNOT_FOUND;
  }
  else
  {
    return m_name2diagramid[ m_combo->GetValue() ];
  }
}

wxString grape_reference_dialog::get_diagram_name() const
{
  return m_combo->GetValue();
}


wxString grape_reference_dialog::get_initializations() const
{
  wxString result;
  for ( int i = 0; i < m_grid->GetNumberRows(); ++i )
  {
    if (!m_grid->GetCellValue(i, 0).IsEmpty() || !m_grid->GetCellValue(i, 1).IsEmpty())
    {
	  result += m_grid->GetCellValue(i, 0) + _T( ":=" ) + m_grid->GetCellValue(i, 1) + _T( ";" );
    }
  }
  return result;
}

void grape_reference_dialog::check_text()
{
  bool valid = true;
  static grape::libgrape::process_reference tmp_reference;
  valid = tmp_reference.set_text( get_initializations() );

  FindWindow(GetAffirmativeId())->Enable(valid);
}


void grape_reference_dialog::event_change_text( wxGridEvent &p_event )
{
  check_text();
}

void grape_reference_dialog::change_combobox()
{
  int start_index = 0;
  process_diagram *diagram_ptr = static_cast<process_diagram*>(find_a_diagram( m_main_frame, get_diagram_id() ) );
  if (diagram_ptr != 0)
  {
    list_of_decl parameter_declarations = diagram_ptr->get_preamble()->get_parameter_declarations_list();
    
    // fill grid with parameters
    int param_count = parameter_declarations.GetCount();
    while ( m_grid->GetNumberRows() < param_count) m_grid->AppendRows();
    if (diagram_ptr != 0)
    {  
      for ( int i = 0; i < param_count; ++i )
      {
        m_grid->SetCellValue(i, 0, parameter_declarations.Item( i ).get_name());
        m_grid->SetCellValue(i, 1, _T(""));
      }
      start_index = parameter_declarations.GetCount();
    }
   
    // fill grid with empty values
    for ( int i = start_index; i < m_grid->GetNumberRows(); ++i )
    {
      m_grid->SetCellValue(i, 0, _T(""));
      m_grid->SetCellValue(i, 1, _T(""));
    }
    
    // remove empty rows
    while ( (m_grid->GetNumberRows() > 0) && (m_grid->GetCellValue(m_grid->GetNumberRows()-1, 0).IsEmpty()) &&  (m_grid->GetCellValue(m_grid->GetNumberRows()-1, 1).IsEmpty()) ) m_grid->DeleteRows(m_grid->GetNumberRows()-1);
    
    check_text();
  }
}

void grape_reference_dialog::event_change_combobox( wxCommandEvent &p_event )
{
  change_combobox();
}

BEGIN_EVENT_TABLE(grape_reference_dialog, wxDialog)
  EVT_COMBOBOX(GRAPE_COMBO_TEXT, grape_reference_dialog::event_change_combobox)
  EVT_GRID_CMD_CELL_CHANGE(GRAPE_GRID_TEXT, grape_reference_dialog::event_change_text)
END_EVENT_TABLE()

