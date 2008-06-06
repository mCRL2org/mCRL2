// Author(s): VitaminB100
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file preambledialog.cpp
//
// Defines the preamble dialog.

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

#include "preambledialog.h"

using namespace grape::grapeapp;

grape_preamble_dialog::grape_preamble_dialog( preamble *p_preamble )
: wxDialog( 0, wxID_ANY, _T( "Edit preamble" ), wxDefaultPosition, wxDefaultSize, wxRESIZE_BORDER | wxDEFAULT_DIALOG_STYLE )
{
  wxBoxSizer *vsizer = new wxBoxSizer( wxVERTICAL );

  wxStaticText *text = new wxStaticText( this, wxID_ANY, _T("Parameter declarations:") );
  vsizer->Add( text, 0, wxEXPAND );

  m_parameters = new wxTextCtrl( this, wxID_ANY, p_preamble->get_parameter_declarations(), wxDefaultPosition, wxSize( 400, 300 ), wxTE_MULTILINE );
  vsizer->Add( m_parameters, 1, wxEXPAND );

  text = new wxStaticText( this, wxID_ANY, _T("Local variable declarations:") );
  vsizer->Add( text, 0, wxEXPAND );

  m_localvars = new wxTextCtrl( this, wxID_ANY, p_preamble->get_local_variable_declarations(), wxDefaultPosition, wxSize( 400, 300 ), wxTE_MULTILINE );
  vsizer->Add( m_localvars, 1, wxEXPAND );

  wxSizer *sizer = CreateButtonSizer(wxOK | wxCANCEL);
  sizer->Layout();
  vsizer->Add( sizer, 0, wxALIGN_RIGHT );

  // realize sizers
  SetSizer(vsizer);
  vsizer->Fit(this);
  vsizer->SetSizeHints(this);

  m_parameters->SetFocus();
}

grape_preamble_dialog::grape_preamble_dialog()
: wxDialog()
{
  // shouldn't be called
}

grape_preamble_dialog::~grape_preamble_dialog()
{
}

wxString grape_preamble_dialog::get_parameter_declarations() const
{
  return m_parameters->GetValue();
}

wxString grape_preamble_dialog::get_local_variable_declarations() const
{
  return m_localvars->GetValue();
}
