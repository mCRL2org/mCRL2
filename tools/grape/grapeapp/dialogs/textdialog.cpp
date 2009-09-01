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

#include "mcrl2gen/mcrl2gen_validate.h"
#include "textdialog.h"
#include "grape_ids.h"

using namespace grape::grapeapp;
using namespace grape::mcrl2gen;

grape_text_dlg::grape_text_dlg( const wxString &p_title, const wxString &p_message, const wxString &p_initial_text, bool p_multiline )
: wxDialog( 0, wxID_ANY, p_title )
{
  wxBoxSizer *wnd_sizer = new wxBoxSizer(wxVERTICAL);

  wxStaticText *text = new wxStaticText( this, wxID_ANY, p_message );
  wnd_sizer->Add(text, 0, wxALIGN_TOP | wxLEFT | wxTOP | wxRIGHT, 5 );

  wnd_sizer->AddSpacer( 5 );

  // create text control
  if ( p_multiline )
  {
    m_input = new wxTextCtrl(this, GRAPE_INPUT_TEXT, p_initial_text, wxDefaultPosition, wxSize(400, 300), wxTE_MULTILINE );
  }
  else
  {
    m_input = new wxTextCtrl(this, GRAPE_INPUT_TEXT, wxEmptyString );
    m_input->ChangeValue(p_initial_text);
  }
  wnd_sizer->Add(m_input, 0, wxEXPAND | wxLEFT | wxRIGHT, 5);

  wnd_sizer->AddSpacer( 5 );

  // create buttons
  wxSizer *sizer = CreateButtonSizer(wxOK | wxCANCEL);
  FindWindow(GetAffirmativeId())->Enable( update_validation() );
  sizer->Layout();
  wnd_sizer->Add(sizer, 0, wxALIGN_BOTTOM | wxALIGN_RIGHT | wxLEFT | wxRIGHT | wxBOTTOM, 1);

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

bool grape_text_dlg::update_validation()
{
  wxString m_input_altered = m_input->GetValue();
  m_input_altered.Replace(_T(" "), _T("_"));
// TODO: use other line
  return is_identifier(m_input_altered);
//  return mcrl2::core::detail::gsIsUserIdentifier(m_input_altered.fn_str());
}

void grape_text_dlg::event_update_validation( wxCommandEvent &p_event )
{
  if (m_input->IsSingleLine())
  {
    FindWindow(GetAffirmativeId())->Enable( update_validation() );
  }
  else
  {
    FindWindow(GetAffirmativeId())->Enable( true );
  }
}

BEGIN_EVENT_TABLE(grape_text_dlg, wxDialog)
  EVT_TEXT(GRAPE_INPUT_TEXT, grape_text_dlg::event_update_validation)   
END_EVENT_TABLE()
