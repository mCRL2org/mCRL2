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

  // create variable declarations text
  wxStaticText *text = new wxStaticText( this, wxID_ANY, _T("Variable declarations:   id:N,...") );
  vsizer->Add( text );
  // create variable declarations text
  m_var_decls_input = new wxTextCtrl(this, GRAPE_VAR_DECLS_INPUT_TEXT, wxEmptyString, wxDefaultPosition, wxSize(570, 20) );
  m_var_decls_input->ChangeValue( m_label->get_declarations_text() );
  vsizer->Add( m_var_decls_input );
 
  // create condition text
  text = new wxStaticText( this, wxID_ANY, _T("Condition:") );
  vsizer->Add( text );
  // create condition input
  m_condition_input = new wxTextCtrl(this, GRAPE_CONDITION_INPUT_TEXT, wxEmptyString, wxDefaultPosition, wxSize(570, 20) );
  m_condition_input->ChangeValue( m_label->get_condition() );
  vsizer->Add( m_condition_input );
  
  // create multiaction text
  text = new wxStaticText( this, wxID_ANY, _T("Multiaction:   act(par1,...)|...") );
  vsizer->Add( text );
  // create multiaction grid
  m_multiaction_input = new wxTextCtrl(this, GRAPE_MULTIACTION_INPUT_TEXT, wxEmptyString, wxDefaultPosition, wxSize(570, 20) );
  m_multiaction_input->ChangeValue( m_label->get_actions_text() );
  vsizer->Add( m_multiaction_input );
 
  // create timestamp text
  text = new wxStaticText( this, wxID_ANY, _T("Timestamp:") );
  vsizer->Add( text );
  // create timestamp grid
  m_timestamp_input = new wxTextCtrl(this, GRAPE_TIMESTAMP_INPUT_TEXT, wxEmptyString, wxDefaultPosition, wxSize(570, 20) );
  m_timestamp_input->ChangeValue( m_label->get_timestamp() );
  vsizer->Add( m_timestamp_input );
    
  // create variable updates text
  text = new wxStaticText( this, wxID_ANY, _T("Variable updates:   var:=expr,...") );
  vsizer->Add( text );  
  // create variable updates input
  m_var_updates_input = new wxTextCtrl(this, GRAPE_VAR_UPDATES_INPUT_TEXT, wxEmptyString, wxDefaultPosition, wxSize(570, 20) );
  m_var_updates_input->ChangeValue( m_label->get_variable_updates_text() );
  vsizer->Add( m_var_updates_input );

  // add preview
  m_preview_text = new wxStaticText( this, wxID_ANY, m_label->get_text() );
  vsizer->Add(m_preview_text );

  wxSizer *sizer = CreateButtonSizer(wxOK | wxCANCEL);
  sizer->Layout();
  vsizer->Add( sizer, 0, wxALIGN_RIGHT );

  // realize sizers
  SetSizer(vsizer);
  vsizer->SetSizeHints(this);

  m_var_decls_input->SetFocus();
  Centre();
}

grape_label_dialog::grape_label_dialog()
: wxDialog()
{
  // shouldn't be called
}

grape_label_dialog::~grape_label_dialog()
{
}

void grape_label_dialog::event_change_var_decls_text( wxCommandEvent &p_event )
{
  // fill label with variable declarations
  m_label->set_declarations_text( m_var_decls_input->GetValue() );

  update_preview();
}

void grape_label_dialog::event_change_var_updates_text( wxCommandEvent &p_event )
{
  // fill label with variable updates
  m_label->set_variable_updates_text( m_var_updates_input->GetValue() );

  update_preview();
}

void grape_label_dialog::event_change_multiaction_text( wxCommandEvent &p_event )
{
  // fill label with actions  
  m_label->set_actions_text( m_multiaction_input->GetValue() );
  
  update_preview();
}

void grape_label_dialog::event_change_condition_text( wxCommandEvent &p_event )
{
  // fill label with condition
  m_label->set_condition( m_condition_input->GetValue() );

  update_preview();
}

void grape_label_dialog::event_change_timestamp_text( wxCommandEvent &p_event )
{
  // fill label with timestamp
  m_label->set_timestamp( m_timestamp_input->GetValue() );
 
  update_preview();
}

void grape_label_dialog::update_preview()
{
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
  EVT_TEXT(GRAPE_VAR_DECLS_INPUT_TEXT, grape_label_dialog::event_change_var_decls_text)
  EVT_TEXT(GRAPE_MULTIACTION_INPUT_TEXT, grape_label_dialog::event_change_multiaction_text)
  EVT_TEXT(GRAPE_CONDITION_INPUT_TEXT, grape_label_dialog::event_change_condition_text)
  EVT_TEXT(GRAPE_TIMESTAMP_INPUT_TEXT, grape_label_dialog::event_change_timestamp_text)
  EVT_TEXT(GRAPE_VAR_UPDATES_INPUT_TEXT, grape_label_dialog::event_change_var_updates_text)
END_EVENT_TABLE()
