// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file labeldialog.cpp
//
// Defines the label dialog.

#include "wx.hpp" // precompiled headers

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/statusbr.h>

#include "grape_ids.h"
#include "labeldialog.h"
#include "../../mcrl2gen/mcrl2gen_validate.h"


using namespace grape::grapeapp;
using namespace grape::mcrl2gen;

grape_label_dialog::grape_label_dialog( const label &p_label )
: wxDialog( 0, wxID_ANY, _T( "Edit label" ), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE )
{
  m_label = new label( p_label );
  wxBoxSizer *vsizer = new wxBoxSizer( wxVERTICAL );

  // create variable declarations text
  wxStaticText *text = new wxStaticText( this, wxID_ANY, _T("Variable declarations:" ), wxDefaultPosition, wxSize(200, 25) );
  
  // create variable declarations text
  m_var_decls_input = new wxTextCtrl(this, GRAPE_VAR_DECLS_INPUT_TEXT, wxEmptyString, wxDefaultPosition, wxSize(300, 25) );
  m_var_decls_input->ChangeValue( m_label->get_declarations_text() );
 
  // create sizer
  wxSizer *var_decl_sizer = new wxBoxSizer(wxHORIZONTAL);
  var_decl_sizer->Add( text);
  var_decl_sizer->Add( m_var_decls_input );
  vsizer->Add( var_decl_sizer );
  
  vsizer->AddSpacer( 5 );
    
  // create condition text
  text = new wxStaticText( this, wxID_ANY, _T("Condition:"), wxDefaultPosition, wxSize(200, 25) );
  
  // create condition input
  m_condition_input = new wxTextCtrl(this, GRAPE_CONDITION_INPUT_TEXT, wxEmptyString, wxDefaultPosition, wxSize(300, 25) );
  m_condition_input->ChangeValue( m_label->get_condition() );
 
  // create sizer
  wxSizer *condition_sizer = new wxBoxSizer(wxHORIZONTAL);
  condition_sizer->Add( text );
  condition_sizer->Add( m_condition_input );
  vsizer->Add( condition_sizer );
  
  vsizer->AddSpacer( 5 );
  
  // create multiaction text
  text = new wxStaticText( this, wxID_ANY, _T("Multiaction:"), wxDefaultPosition, wxSize(200, 25) );
 
  // create multiaction grid
  m_multiaction_input = new wxTextCtrl(this, GRAPE_MULTIACTION_INPUT_TEXT, wxEmptyString, wxDefaultPosition, wxSize(300, 25) );
  m_multiaction_input->ChangeValue( m_label->get_actions_text() );

  // create sizer
  wxSizer *multiaction_sizer = new wxBoxSizer(wxHORIZONTAL);
  multiaction_sizer->Add( text);
  multiaction_sizer->Add( m_multiaction_input );
  vsizer->Add( multiaction_sizer );
  
  vsizer->AddSpacer( 5 );
  
  // create timestamp text
  text = new wxStaticText( this, wxID_ANY, _T("Timestamp:"), wxDefaultPosition, wxSize(200, 25) );

  // create timestamp grid
  m_timestamp_input = new wxTextCtrl(this, GRAPE_TIMESTAMP_INPUT_TEXT, wxEmptyString, wxDefaultPosition, wxSize(300, 25) );
  m_timestamp_input->ChangeValue( m_label->get_timestamp() );
   
  // create sizer
  wxSizer *timestamp_sizer = new wxBoxSizer(wxHORIZONTAL);
  timestamp_sizer->Add( text);
  timestamp_sizer->Add( m_timestamp_input );
  vsizer->Add( timestamp_sizer );
  
  vsizer->AddSpacer( 5 );
  
  // create variable updates text
  text = new wxStaticText( this, wxID_ANY, _T("Variable updates:"), wxDefaultPosition, wxSize(200, 25) );

  // create variable updates input
  m_var_updates_input = new wxTextCtrl(this, GRAPE_VAR_UPDATES_INPUT_TEXT, wxEmptyString, wxDefaultPosition, wxSize(300, 25) );
  m_var_updates_input->ChangeValue( m_label->get_variable_updates_text() );

  // create sizer
  wxSizer *var_updates_sizer = new wxBoxSizer(wxHORIZONTAL);
  var_updates_sizer->Add( text);
  var_updates_sizer->Add( m_var_updates_input );
  vsizer->Add( var_updates_sizer );
  
  vsizer->AddSpacer( 10 );
    
  // create preview
  m_preview_text = new wxStaticText( this, wxID_ANY, m_label->get_text() );
  vsizer->Add(m_preview_text );
  
  // create sizer
  wxSizer *preview_sizer = new wxBoxSizer(wxHORIZONTAL);
  preview_sizer->Add( new wxStaticText( this, wxID_ANY, _T("Preview: ") ) );
  preview_sizer->Add( m_preview_text );
  vsizer->Add( preview_sizer );
  
  wxSizer *sizer = CreateButtonSizer(wxOK | wxCANCEL);
  sizer->Layout();
  vsizer->Add( sizer, 0, wxALIGN_RIGHT );
  
  // add statusbar
  m_statusbar = new wxStatusBar(this, wxID_ANY);
  m_statusbar->SetFieldsCount(1);
  m_statusbar->SetStatusText(_T(""), 0);
  vsizer->Add( m_statusbar, 0, wxALIGN_BOTTOM );

  m_statusbar->GetFieldsCount();
  // realize sizers
  SetSizer(vsizer);
  vsizer->SetSizeHints(this);
  vsizer->Fit(this);

  m_var_decls_input->SetFocus();  
  
  CentreOnParent();
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

  m_statusbar->SetStatusText(_T("Syntax: x:S,...,x:S for identifier x and sort expression S"), 0);
  
  update_preview();
}

void grape_label_dialog::event_change_var_updates_text( wxCommandEvent &p_event )
{
  // fill label with variable updates
  m_label->set_variable_updates_text( m_var_updates_input->GetValue() );
  
  m_statusbar->SetStatusText(_T("Syntax: x:=d,...,x:=d for identifier x and data expression d"), 0);

  update_preview();
}

void grape_label_dialog::event_change_multiaction_text( wxCommandEvent &p_event )
{
  // fill label with actions  
  m_label->set_actions_text( m_multiaction_input->GetValue() );
  
  m_statusbar->SetStatusText(_T("Syntax: a(d,...,d)|...|a(d,...,d) for identifier data expression d and action a"), 0);
  
  update_preview();
}

void grape_label_dialog::event_change_condition_text( wxCommandEvent &p_event )
{
  // fill label with condition
  m_label->set_condition( m_condition_input->GetValue() );

  m_statusbar->SetStatusText(_T("Enter an expression of sort bool"), 0);
  
  update_preview();
}

void grape_label_dialog::event_change_timestamp_text( wxCommandEvent &p_event )
{
  // fill label with timestamp
  m_label->set_timestamp( m_timestamp_input->GetValue() );

  m_statusbar->SetStatusText(_T("Enter an expression of sort real"), 0);
  
  update_preview();
}

void grape_label_dialog::update_preview()
{
  //display updated text
  m_preview_text->SetLabel( m_label->get_text() );
}

bool grape_label_dialog::show_modal( label &p_label )
{
  bool is_valid = false;
  while (!is_valid)
  {
    int result = ShowModal();
    if ( result == wxID_CANCEL )
    {
      return false;
    }

    is_valid = true;
    update_preview();
    p_label = *m_label;
    
    //check label variable declarations
    if (p_label.get_declarations_text() != m_var_decls_input->GetValue())
    {
      is_valid = false;      
      wxMessageBox( _T("The variable declaration invalid."), _T("error"), wxOK | wxICON_ERROR );
    }
    
    //check label condition
    if ((!m_condition_input->GetValue().IsEmpty()) && ( parse_data_expr(p_label.get_condition()) == 0 ))
    {
      is_valid = false;      
      wxMessageBox( _T("The condition is invalid."), _T("error"), wxOK | wxICON_ERROR );
    }
        
    //check label multiactions
    if (p_label.get_actions_text() != m_multiaction_input->GetValue())
    {
      is_valid = false;      
      wxMessageBox( _T("The multiaction is invalid."), _T("error"), wxOK | wxICON_ERROR );
    }
    
    //check label timestamp
    if ((!m_timestamp_input->GetValue().IsEmpty()) && ( parse_sort_expr(p_label.get_timestamp()) == 0 ))
    {      
      is_valid = false;      
      wxMessageBox( _T("The timestamp is invalid."), _T("error"), wxOK | wxICON_ERROR );
    }
    
    //check label variable updates
    if (p_label.get_variable_updates_text() != m_var_updates_input->GetValue())
    {
      is_valid = false;      
      wxMessageBox( _T("The variable update is invalid."), _T("error"), wxOK | wxICON_ERROR );
    }   

  }

  return true;
}

BEGIN_EVENT_TABLE(grape_label_dialog, wxDialog)
  EVT_TEXT(GRAPE_VAR_DECLS_INPUT_TEXT, grape_label_dialog::event_change_var_decls_text) 
  EVT_TEXT(GRAPE_MULTIACTION_INPUT_TEXT, grape_label_dialog::event_change_multiaction_text)
  EVT_TEXT(GRAPE_CONDITION_INPUT_TEXT, grape_label_dialog::event_change_condition_text)
  EVT_TEXT(GRAPE_TIMESTAMP_INPUT_TEXT, grape_label_dialog::event_change_timestamp_text)
  EVT_TEXT(GRAPE_VAR_UPDATES_INPUT_TEXT, grape_label_dialog::event_change_var_updates_text)  
END_EVENT_TABLE()
