// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file channeldialog.h
//
// Defines a channel dialog.

#include "wx.hpp" // precompiled headers

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

#include "inputvalidation.h"
#include "channeldialog.h"
#include "grape_ids.h"

using namespace grape::grapeapp;

grape_channel_dlg::grape_channel_dlg( channel &p_channel )
: wxDialog( 0, wxID_ANY, wxT("Edit channel"), wxDefaultPosition, wxDefaultSize )
{  
  wxBoxSizer *wnd_sizer = new wxBoxSizer(wxVERTICAL);
  
  // create name text
  wxStaticText *text_name = new wxStaticText( this, wxID_ANY, _T("Name:"), wxDefaultPosition, wxSize(100, 25) );

  // create name input
  m_name_input = new wxTextCtrl(this, GRAPE_CHANNEL_NAME_INPUT_TEXT, wxEmptyString, wxDefaultPosition, wxSize(200, 25) );
  m_name_input->ChangeValue(p_channel.get_name());

  // create sizer
  wxSizer *name_sizer = new wxBoxSizer(wxHORIZONTAL);
  name_sizer->Add( text_name );
  name_sizer->Add( m_name_input );
  wnd_sizer->Add( name_sizer );
  
  wnd_sizer->AddSpacer( 5 );
  
  // create rename text
  wxStaticText *text_rename = new wxStaticText( this, wxID_ANY, _T("Rename:"), wxDefaultPosition, wxSize(100, 25) );

  // create rename input
  m_rename_input = new wxTextCtrl(this, GRAPE_CHANNEL_RENAME_INPUT_TEXT, wxEmptyString, wxDefaultPosition, wxSize(200, 25) );
  m_rename_input->ChangeValue(p_channel.get_rename_to());

  // create sizer
  wxSizer *rename_sizer = new wxBoxSizer(wxHORIZONTAL);
  rename_sizer->Add( text_rename );
  rename_sizer->Add( m_rename_input );
  wnd_sizer->Add( rename_sizer );

  wnd_sizer->AddSpacer( 5 );
        
  // select the correct property of the channel
  int index = p_channel.get_channel_type();
  if (p_channel.get_channel_communications()->GetCount() == 0)
  { 
    wxString radiobox_list[3] = {_T("visible"), _T("hidden"), _T("blocked")};
    m_radiobox = new wxRadioBox( this, wxID_ANY, _T("Property:"), wxDefaultPosition, wxSize(300, 95), 3, radiobox_list );
    m_radiobox->SetSelection(index);
  } 
  else 
  {
    wxString radiobox_list[1] = {_T("hidden")};
    m_radiobox = new wxRadioBox( this, wxID_ANY, _T("Property:"), wxDefaultPosition, wxSize(300, 45), 1, radiobox_list );
    m_radiobox->SetSelection(0);
  }
        
  // create sizer
  wxSizer *property_sizer = new wxBoxSizer(wxVERTICAL);
  property_sizer->Add( m_radiobox );
  wnd_sizer->Add( property_sizer );

  wnd_sizer->AddSpacer( 5 );
  
  // create buttons
  wxSizer *sizer = CreateButtonSizer(wxOK | wxCANCEL);
  FindWindow(GetAffirmativeId())->Enable( update_validation() );
  sizer->Layout();
  wnd_sizer->Add(sizer, 0, wxALIGN_RIGHT, 0);

  // realize sizers
  SetSizer(wnd_sizer);
  wnd_sizer->Fit(this);
  wnd_sizer->SetSizeHints(this);

  m_name_input->SetFocus();
  CentreOnParent();
}

grape_channel_dlg::grape_channel_dlg()
{
  // shouldn't be called
}

grape_channel_dlg::~grape_channel_dlg()
{
}

bool grape_channel_dlg::update_validation()
{
  bool is_valid = !m_name_input->GetValue().IsEmpty() && identifier_valid(m_name_input->GetValue());
  is_valid &= identifier_valid(m_rename_input->GetValue());
  return is_valid;
}
void grape_channel_dlg::event_update_validation( wxCommandEvent &p_event )
{
  FindWindow(GetAffirmativeId())->Enable( update_validation() );
}
        
bool grape_channel_dlg::show_modal( channel &p_channel )
{
  if (ShowModal() != wxID_CANCEL)
  {
    p_channel.set_name(m_name_input->GetValue());     
    p_channel.set_rename_to(m_rename_input->GetValue());
  
    if (m_radiobox->GetStringSelection() == _T("visible")) p_channel.set_channel_type(VISIBLE_CHANNEL);
    if (m_radiobox->GetStringSelection() == _T("hidden")) p_channel.set_channel_type(HIDDEN_CHANNEL);
    if (m_radiobox->GetStringSelection() == _T("blocked")) p_channel.set_channel_type(BLOCKED_CHANNEL);
  
    return true;
  }
  
  return false;
}

BEGIN_EVENT_TABLE(grape_channel_dlg, wxDialog)
  EVT_TEXT(GRAPE_CHANNEL_NAME_INPUT_TEXT, grape_channel_dlg::event_update_validation)   
  EVT_TEXT(GRAPE_CHANNEL_RENAME_INPUT_TEXT, grape_channel_dlg::event_update_validation)   
END_EVENT_TABLE()
