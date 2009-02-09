// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file labeldialog.cpp
//
// Defines the label dialog.

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

#include "grape_ids.h"
#include "labeldialog.h"

using namespace grape::grapeapp;

grape_label_dialog::grape_label_dialog( const label &p_label )
: wxDialog( 0, wxID_ANY, _T( "Edit label" ), wxDefaultPosition, wxDefaultSize, wxRESIZE_BORDER | wxDEFAULT_DIALOG_STYLE )
{
  m_label = new label( p_label );
  wxBoxSizer *vsizer = new wxBoxSizer( wxVERTICAL );

  // define notebook
  m_notebook = new wxNotebook( this, 0, wxDefaultPosition, wxSize(570, 300) );

  // define notebook pages
  m_var_decls_page = new wxNotebookPage( m_notebook, -1 );
  m_condition_page = new wxNotebookPage( m_notebook, -1 );
  m_multiaction_page = new wxNotebookPage( m_notebook, -1 );
  m_timestamp_page = new wxNotebookPage( m_notebook, -1 );
  m_var_updates_page = new wxNotebookPage( m_notebook, -1 );

  // add notebook pages
  m_notebook->AddPage( m_var_decls_page, _T("variable declarations") );

  m_notebook->AddPage( m_condition_page, _T("condition") );
  m_notebook->AddPage( m_multiaction_page, _T("multiaction") );
  m_notebook->AddPage( m_timestamp_page, _T("timestamp") );
  m_notebook->AddPage( m_var_updates_page, _T("variable updates") );
  m_notebook->Layout();

  vsizer->Add(m_notebook, 1, wxEXPAND );

  // create variable declarations grid
  m_var_decls_grid = new wxGrid( m_var_decls_page, GRAPE_VAR_DECLS_GRID_TEXT, wxDefaultPosition, wxSize(570, 300));
  m_var_decls_grid->CreateGrid( m_label->get_declarations().GetCount()+1, 2 );
  m_var_decls_grid->SetColSize( 0, 250 );
  m_var_decls_grid->SetColSize( 1, 250 );
  m_var_decls_grid->SetColLabelValue(0, _T("identifier"));
  m_var_decls_grid->SetColLabelValue(1, _T("sort expression"));
  m_var_decls_grid->SetRowLabelSize(30);

  // fill grid with variable declarations
  for ( unsigned int i = 0; i < m_label->get_declarations().GetCount(); ++i )
  {
    //fill cells
    decl var_decl = m_label->get_declarations().Item(i);
    m_var_decls_grid->SetCellValue(i, 0, var_decl.get_name());
    m_var_decls_grid->SetCellValue(i, 1, var_decl.get_type());
  }

  // create condition input
  m_condition_input = new wxTextCtrl(m_condition_page, GRAPE_CONDITION_INPUT_TEXT, m_label->get_condition(), wxDefaultPosition, wxSize(570, 20) );

  // create multiaction grid
  m_multiaction_grid = new wxGrid( m_multiaction_page, GRAPE_MULTIACTION_GRID_TEXT, wxDefaultPosition, wxSize(570, 300));
  m_multiaction_grid->CreateGrid( m_label->get_actions().GetCount()+1, 2 );
  m_multiaction_grid->SetColSize( 0, 250 );
  m_multiaction_grid->SetColSize( 1, 250 );
  m_multiaction_grid->SetColLabelValue(0, _T("action"));
  m_multiaction_grid->SetColLabelValue(1, _T("parameter 1"));
  m_multiaction_grid->SetRowLabelSize(30);

  // fill grid with mulit actions
  for ( unsigned int i = 0; i < m_label->get_actions().GetCount(); ++i )
  {
    //fill cells
    action multi_action = m_label->get_actions().Item(i);
    m_multiaction_grid->SetCellValue(i, 0, multi_action.get_name());
    list_of_dataexpression multi_action_params = multi_action.get_parameters();
    int multiaction_count = multi_action_params.GetCount();
    for ( int j = 0; j < multiaction_count; ++j )
    {
      if ( m_multiaction_grid->GetNumberCols() <= j+2 )
      {
        m_multiaction_grid->AppendCols();
        wxString label_text = _T("parameter ");
        label_text = label_text << j+2;
        m_multiaction_grid->SetColLabelValue( j + 2, label_text );
        m_multiaction_grid->SetColSize( j + 2, 250 );
        m_multiaction_grid->ForceRefresh();
      }
      m_multiaction_grid->SetCellValue(i, j+1, multi_action_params[j].get_expression());
    }
  }

  // create timestamp grid
  m_timestamp_input = new wxTextCtrl(m_timestamp_page, GRAPE_TIMESTAMP_INPUT_TEXT, m_label->get_timestamp(), wxDefaultPosition, wxSize(570, 20) );

  // create variable updates grid
  m_var_updates_grid = new wxGrid( m_var_updates_page, GRAPE_VAR_UPDATES_GRID_TEXT, wxDefaultPosition, wxSize(570, 300));
  m_var_updates_grid->CreateGrid( m_label->get_variable_updates().GetCount()+1, 2 );
  m_var_updates_grid->SetColSize( 0, 250 );
  m_var_updates_grid->SetColSize( 1, 250 );
  m_var_updates_grid->SetColLabelValue(0, _T("variable"));
  m_var_updates_grid->SetColLabelValue(1, _T("data expression"));
  m_var_updates_grid->SetRowLabelSize(30);

  // fill grid with variable updates
  for ( unsigned int i = 0; i < m_label->get_variable_updates().GetCount(); ++i )
  {
    //fill cells
    varupdate var_update = m_label->get_variable_updates().Item(i);
    m_var_updates_grid->SetCellValue(i, 0, var_update.get_lhs());
    m_var_updates_grid->SetCellValue(i, 1, var_update.get_rhs());
  }

  // add preview
  m_preview_text = new wxStaticText( this, wxID_ANY, m_label->get_text() );
  vsizer->Add(m_preview_text );

  wxSizer *sizer = CreateButtonSizer(wxOK | wxCANCEL);
  sizer->Layout();
  vsizer->Add( sizer, 0, wxALIGN_RIGHT );

  // realize sizers
  SetSizer(vsizer);
  vsizer->SetSizeHints(this);

  m_var_decls_grid->SetFocus();
  Centre();

  SetSize(400, 400);
}

grape_label_dialog::grape_label_dialog()
: wxDialog()
{
  // shouldn't be called
}

grape_label_dialog::~grape_label_dialog()
{
}

void grape_label_dialog::event_change_var_decls_text( wxGridEvent &p_event )
{
  int rows_count = m_var_decls_grid->GetNumberRows();
  // add new rows
  while ( (m_var_decls_grid->GetCellValue(rows_count-1, 0) != _T("")) || (m_var_decls_grid->GetCellValue(rows_count-1, 1) != _T("")) ) {
    m_var_decls_grid->AppendRows();
    rows_count = m_var_decls_grid->GetNumberRows();
  }
  update_preview();
}

void grape_label_dialog::event_change_var_updates_text( wxGridEvent &p_event )
{
  int rows_count = m_var_updates_grid->GetNumberRows();
  // add new rows
  while ( (m_var_updates_grid->GetCellValue(rows_count-1, 0) != _T("")) || (m_var_updates_grid->GetCellValue(rows_count-1, 1) != _T("")) ) {
    m_var_updates_grid->AppendRows();
    rows_count = m_var_updates_grid->GetNumberRows();
  }
  update_preview();
}

void grape_label_dialog::event_change_multiaction_text( wxGridEvent &p_event )
{
  int rows_count = m_multiaction_grid->GetNumberRows();
  // add new cols
  for ( int i = 0; i < rows_count; ++i )
  {
    int cols_count = m_multiaction_grid->GetNumberCols();
    if ( !m_multiaction_grid->GetCellValue(i, cols_count-1).IsEmpty() )
    {
      m_multiaction_grid->AppendCols();
      wxString label_text = _T("parameter ");
      label_text = label_text << cols_count;
      m_multiaction_grid->SetColLabelValue(cols_count, label_text);
      m_multiaction_grid->SetColSize( cols_count, 250 );
      m_multiaction_grid->ForceRefresh();
      m_multiaction_grid->MoveCursorRight(false);
    }
  }
  // add new rows
  if ( !m_multiaction_grid->GetCellValue(rows_count-1, 0).IsEmpty() ) {
    m_multiaction_grid->AppendRows();
    m_multiaction_grid->ForceRefresh();
  }
  update_preview();
}

void grape_label_dialog::event_change_condition_text( wxCommandEvent &p_event )
{
  update_preview();
}

void grape_label_dialog::event_change_timestamp_text( wxCommandEvent &p_event )
{
  update_preview();
}

void grape_label_dialog::update_preview()
{
  // fill label with variable declarations
  m_label->get_declarations().Clear();
  for ( int i = 0; i < m_var_decls_grid->GetNumberRows(); ++i )
  {
    if ( m_var_decls_grid->GetCellValue(i, 0) != _T("") )
    {
      decl var_decl;
      var_decl.set_name(m_var_decls_grid->GetCellValue(i, 0));
      var_decl.set_type(m_var_decls_grid->GetCellValue(i, 1));
      m_label->get_declarations().Add(var_decl);
    }
  }

  // fill label with variable updates
  m_label->get_variable_updates().Clear();
  for ( int i = 0; i < m_var_updates_grid->GetNumberRows(); ++i )
  {
    if ( m_var_updates_grid->GetCellValue(i, 0) != _T("") )
    {
      varupdate var_update;
      var_update.set_lhs(m_var_updates_grid->GetCellValue(i, 0));
      var_update.set_rhs(m_var_updates_grid->GetCellValue(i, 1));
      m_label->get_variable_updates().Add(var_update);
    }
  }

  // fill label with condition
  m_label->set_condition( m_condition_input->GetValue() );

  // fill label with timestamp
  m_label->set_timestamp( m_timestamp_input->GetValue() );

  // fill label with actions
  m_label->get_actions().Clear();
  for ( int i = 0; i < m_multiaction_grid->GetNumberRows(); ++i )
  {
    if ( m_multiaction_grid->GetCellValue(i, 0) != _T("") )
    {
      action multi_action;
      multi_action.set_name(m_multiaction_grid->GetCellValue(i, 0));
      list_of_dataexpression multi_action_parameters;
      for( int j = 1; j < m_multiaction_grid->GetNumberCols(); ++j )
      {
        if ( m_multiaction_grid->GetCellValue(i, j) != _T("") )
        {
          dataexpression multi_action_parameter;
          multi_action_parameter.set_expression(m_multiaction_grid->GetCellValue(i, j));
          multi_action_parameters.Add( multi_action_parameter );
        }
      }
      multi_action.set_parameters(multi_action_parameters);
      m_label->get_actions().Add(multi_action);
    }
  }

  //display updated text
  m_preview_text->SetLabel( m_label->get_text() );
}

bool grape_label_dialog::show_modal( label &p_label )
{
  int result = ShowModal();
  if ( result == wxID_CANCEL )
  {
    return false;
  }

  update_preview();

  p_label = *m_label;

  return true;
}

BEGIN_EVENT_TABLE(grape_label_dialog, wxDialog)
  EVT_GRID_CMD_CELL_CHANGE(GRAPE_VAR_DECLS_GRID_TEXT, grape_label_dialog::event_change_var_decls_text)
  EVT_GRID_CMD_CELL_CHANGE(GRAPE_VAR_UPDATES_GRID_TEXT, grape_label_dialog::event_change_var_updates_text)
  EVT_GRID_CMD_CELL_CHANGE(GRAPE_MULTIACTION_GRID_TEXT, grape_label_dialog::event_change_multiaction_text)
  EVT_TEXT_ENTER(GRAPE_CONDITION_INPUT_TEXT, grape_label_dialog::event_change_condition_text)
  EVT_TEXT_ENTER(GRAPE_TIMESTAMP_INPUT_TEXT, grape_label_dialog::event_change_timestamp_text)
END_EVENT_TABLE()
