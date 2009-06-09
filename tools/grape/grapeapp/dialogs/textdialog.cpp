// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file textdialog.h
//
// Defines a rename dialog.

#include "wx.hpp" // precompiled headers

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

#include "textdialog.h"

using namespace grape::grapeapp;

grape_text_dlg::grape_text_dlg( const wxString &p_title, const wxString &p_message, const wxString &p_initial_text, bool p_multiline )
: wxDialog( 0, wxID_ANY, p_title )
{
  wxBoxSizer *wnd_sizer = new wxBoxSizer(wxVERTICAL);

  wxStaticText *text = new wxStaticText( this, wxID_ANY, p_message );
  wnd_sizer->Add(text, 0, wxALIGN_TOP, 0 );

  wnd_sizer->AddSpacer( 5 );

  // create text control
  if ( p_multiline )
  {
    m_input = new wxTextCtrl(this, wxID_ANY, p_initial_text, wxDefaultPosition, wxSize(400, 300), wxTE_MULTILINE );
  }
  else
  {
    m_input = new wxTextCtrl(this, wxID_ANY, p_initial_text );
  }
  wnd_sizer->Add(m_input, 0, wxEXPAND, 0);

  wnd_sizer->AddSpacer( 5 );

  // create buttons
  wxSizer *sizer = CreateButtonSizer(wxOK | wxCANCEL);
  sizer->Layout();
  wnd_sizer->Add(sizer, 0, wxALIGN_BOTTOM, 0);

  // realize sizers
  SetSizer(wnd_sizer);
  wnd_sizer->Fit(this);
  wnd_sizer->SetSizeHints(this);

  m_input->SetFocus();
  CentreOnParent();
}

grape_text_dlg::grape_text_dlg()
{
  // shouldn't be called
}

grape_text_dlg::~grape_text_dlg()
{
  delete m_input;
}

bool grape_text_dlg::show_modal( wxString &p_text )
{
  int result = ShowModal();
  if ( result == wxID_CANCEL )
  {
    return false;
  }

  p_text = m_input->GetValue();
  return true;
}
