// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file preambledialog.cpp
//
// Defines the preamble dialog.

#include "wx.hpp" // precompiled headers

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

#include "grape_ids.h"
#include "preambledialog.h"
#include "inputvalidation.h"

using namespace grape::grapeapp;

grape_preamble_dialog::grape_preamble_dialog( preamble *p_preamble, bool p_edit_parameter )
: wxDialog( 0, wxID_ANY, _T( "Edit preamble" ), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE )
{
  wxBoxSizer *vsizer = new wxBoxSizer( wxVERTICAL );

  wxSize parameter_grid_size = wxSize( 400, 300 );
  wxSize localvar_grid_size = wxSize( 400, 300 );  
  if (!p_edit_parameter)
  {
    // give the parameter grid zero size (do not hide it due to wxEvent issues)
    parameter_grid_size = wxSize( 0, 0 );
    //m_parameter_grid->Hide();
  }
  else
  {
    // give the variable grid zero size (do not hide it due to wxEvent issues)
    localvar_grid_size = wxSize( 0, 0 );
    //m_localvar_grid->Hide();
  }
  
  if (p_edit_parameter)
  { 
    wxStaticText *text = new wxStaticText( this, wxID_ANY, _T("Parameter declarations:") );  
    vsizer->Add( text, 0, wxEXPAND );
  }
  
  // create grid
  m_parameter_grid = new wxGrid( this, GRAPE_PARAMETER_GRID_TEXT, wxDefaultPosition, parameter_grid_size );
  m_parameter_grid->CreateGrid( p_preamble->get_parameter_declarations_list().GetCount()+1, 2 );
  for ( unsigned int i = 0; i < p_preamble->get_parameter_declarations_list().GetCount(); ++i )
  {
    //fill cells
    decl parameter_assignment = p_preamble->get_parameter_declarations_list().Item( i );
    m_parameter_grid->SetCellValue(i, 0, parameter_assignment.get_name());
    m_parameter_grid->SetCellValue(i, 1, parameter_assignment.get_type());
  }

  m_parameter_grid->SetColSize( 0, 200 );
  m_parameter_grid->SetColSize( 1, 170 );
  m_parameter_grid->SetColLabelValue(0, _T("Name"));
  m_parameter_grid->SetColLabelValue(1, _T("Type"));
  m_parameter_grid->SetRowLabelSize(30);
  vsizer->Add(m_parameter_grid );

  
  if (!p_edit_parameter)
  { 
    wxStaticText *text = new wxStaticText( this, wxID_ANY, _T("Local variable declarations:") );
    vsizer->Add( text, 0, wxEXPAND );
  }
  
  // create grid
  m_localvar_grid = new wxGrid( this, GRAPE_LOCALVAR_GRID_TEXT, wxDefaultPosition, localvar_grid_size );
  m_localvar_grid->CreateGrid( p_preamble->get_local_variable_declarations_list().GetCount()+1, 3 );
  for ( unsigned int i = 0; i < p_preamble->get_local_variable_declarations_list().GetCount(); ++i )
  {
    //fill cells
    decl_init localvar_assignment = p_preamble->get_local_variable_declarations_list().Item( i );
    m_localvar_grid->SetCellValue(i, 0, localvar_assignment.get_name());
    m_localvar_grid->SetCellValue(i, 1, localvar_assignment.get_type());
    m_localvar_grid->SetCellValue(i, 2, localvar_assignment.get_value());
  }

  m_localvar_grid->SetColSize( 0, 170 );
  m_localvar_grid->SetColSize( 1, 100 );
  m_localvar_grid->SetColSize( 2, 100 );
  m_localvar_grid->SetColLabelValue(0, _T("Name"));
  m_localvar_grid->SetColLabelValue(1, _T("Type"));
  m_localvar_grid->SetColLabelValue(2, _T("Value"));
  m_localvar_grid->SetRowLabelSize(30);
  vsizer->Add( m_localvar_grid );
  
  wxSizer *sizer = CreateButtonSizer(wxOK | wxCANCEL);
  sizer->Layout();
  vsizer->Add( sizer, 0, wxALIGN_RIGHT );
  
  // realize sizers
  SetSizer(vsizer);
  vsizer->Fit(this);
  vsizer->SetSizeHints(this);

  m_parameter_grid->SetFocus();
  CentreOnParent();
  check_text();
}

grape_preamble_dialog::grape_preamble_dialog()
: wxDialog()
{
  // shouldn't be called
}

grape_preamble_dialog::~grape_preamble_dialog()
{
  delete m_parameter_grid;
  delete m_localvar_grid;
}

wxString grape_preamble_dialog::get_parameter_declarations() const
{
  wxString result;
  for ( int i = 0; i < m_parameter_grid->GetNumberRows(); ++i )
  {
	if (!m_parameter_grid->GetCellValue(i,0).IsEmpty() || !m_parameter_grid->GetCellValue(i,1).IsEmpty())
	{
      result += m_parameter_grid->GetCellValue(i, 0) + _T( ":" ) + m_parameter_grid->GetCellValue(i, 1) + _T( ";" );
    }
  }
  return result;

}

wxString grape_preamble_dialog::get_local_variable_declarations() const
{
  wxString result;
  for ( int i = 0; i < m_localvar_grid->GetNumberRows(); ++i )
  {
	if (!m_localvar_grid->GetCellValue(i,0).IsEmpty() || !m_localvar_grid->GetCellValue(i,1).IsEmpty() || !m_localvar_grid->GetCellValue(i,2).IsEmpty())
	{
	  result += m_localvar_grid->GetCellValue(i, 0) + _T( ":" ) + m_localvar_grid->GetCellValue(i, 1) + _T( "=" ) + m_localvar_grid->GetCellValue(i, 2) + _T( ";" );
    }
  }
  return result;

}

void grape_preamble_dialog::check_text()
{
  bool valid = true;
  static grape::libgrape::preamble tmp_preamble;
  valid = tmp_preamble.set_parameter_declarations( get_parameter_declarations() );
  valid &= tmp_preamble.set_local_variable_declarations( get_local_variable_declarations() );
  
  for ( int i = 0; i < m_parameter_grid->GetNumberRows(); ++i )
  {
    //check all the names in the grid
    valid &= identifier_valid( m_parameter_grid->GetCellValue(i, 0) );
  }
  for ( int i = 0; i < m_localvar_grid->GetNumberRows(); ++i )
  {
    //check all the names in the grid
    valid &= identifier_valid( m_localvar_grid->GetCellValue(i, 0) );
  }
   
  FindWindow(GetAffirmativeId())->Enable(valid);
}

void grape_preamble_dialog::event_change_parameter_text( wxGridEvent &p_event )
{
  check_text();
  int rows_count = m_parameter_grid->GetNumberRows();

  while ( (m_parameter_grid->GetCellValue(rows_count-1, 0) != _T("")) || (m_parameter_grid->GetCellValue(rows_count-1, 1) != _T(""))) {
    m_parameter_grid->AppendRows();
    rows_count = m_parameter_grid->GetNumberRows();
  }
}

void grape_preamble_dialog::event_change_localvar_text( wxGridEvent &p_event )
{
  check_text();
  int rows_count = m_localvar_grid->GetNumberRows();

  while ( (m_localvar_grid->GetCellValue(rows_count-1, 0) != _T("")) || (m_localvar_grid->GetCellValue(rows_count-1, 1) != _T("")) || (m_localvar_grid->GetCellValue(rows_count-1, 2) != _T(""))) {
    m_localvar_grid->AppendRows();
    rows_count = m_localvar_grid->GetNumberRows();
  }
}

BEGIN_EVENT_TABLE(grape_preamble_dialog, wxDialog)
  EVT_GRID_CMD_CELL_CHANGE(GRAPE_PARAMETER_GRID_TEXT, grape_preamble_dialog::event_change_parameter_text)
  EVT_GRID_CMD_CELL_CHANGE(GRAPE_LOCALVAR_GRID_TEXT, grape_preamble_dialog::event_change_localvar_text)
END_EVENT_TABLE()
