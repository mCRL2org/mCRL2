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
#include "channelcommunicationdialog.h"
#include "grape_ids.h"

using namespace grape::grapeapp;

grape_channel_communication_dlg::grape_channel_communication_dlg( channel_communication &p_channel_communication )
: wxDialog( 0, wxID_ANY, _T("Edit channel communication"), wxDefaultPosition, wxDefaultSize )
{  
  wxBoxSizer *wnd_sizer = new wxBoxSizer(wxVERTICAL);

  // create name text
  wxStaticText *text_name = new wxStaticText( this, wxID_ANY, _T("Name:"), wxDefaultPosition, wxSize(100, 25) );
  
  // create name input
  m_name_input = new wxTextCtrl(this, GRAPE_CHANNEL_COMMUNICATION_NAME_INPUT_TEXT, wxEmptyString, wxDefaultPosition, wxSize(200, 25) );            
  m_name_input->ChangeValue(p_channel_communication.get_name_to());
  // create sizer
  wxSizer *name_sizer = new wxBoxSizer(wxHORIZONTAL);
  name_sizer->Add( text_name );
  name_sizer->Add( m_name_input );
  wnd_sizer->Add( name_sizer, 0, wxLEFT | wxTOP | wxRIGHT, 5 );
  
  wnd_sizer->AddSpacer( 5 );

  // select the correct property of the channel communication
  int index = p_channel_communication.get_channel_communication_type();
  wxString radiobox_list[3] = {_T("visible"), _T("hidden"), _T("blocked")};
  m_radiobox = new wxRadioBox( this, wxID_ANY, _T("Property:"), wxDefaultPosition, wxSize(300, 45), 3, radiobox_list );
  m_radiobox->SetSelection(index);
  // create sizer
  wxSizer *property_sizer = new wxBoxSizer(wxVERTICAL);
  property_sizer->Add( m_radiobox );
  wnd_sizer->Add( property_sizer, 0, wxLEFT | wxRIGHT, 5 );
  
  wnd_sizer->AddSpacer( 5 );

  // create buttons
  wxSizer *sizer = CreateButtonSizer(wxOK | wxCANCEL);
  FindWindow(GetAffirmativeId())->Enable( update_validation() );
  sizer->Layout();
  wnd_sizer->Add(sizer, 0, wxALIGN_RIGHT | wxLEFT | wxRIGHT | wxBOTTOM, 1);

  // realize sizers
  SetSizer(wnd_sizer);
  wnd_sizer->Fit(this);
  wnd_sizer->SetSizeHints(this);

  m_name_input->SetFocus();
  CentreOnParent();
}

grape_channel_communication_dlg::grape_channel_communication_dlg()
{
  // shouldn't be called
}

grape_channel_communication_dlg::~grape_channel_communication_dlg()
{
}

bool grape_channel_communication_dlg::show_modal( channel_communication &p_channel_communication )
{
  if (ShowModal() != wxID_CANCEL)
  {      
    p_channel_communication.set_name_to(m_name_input->GetValue());     
    if (m_radiobox->GetStringSelection() == _T("visible")) p_channel_communication.set_channel_communication_type(VISIBLE_CHANNEL_COMMUNICATION);
    if (m_radiobox->GetStringSelection() == _T("hidden")) p_channel_communication.set_channel_communication_type(HIDDEN_CHANNEL_COMMUNICATION);
    if (m_radiobox->GetStringSelection() == _T("blocked")) p_channel_communication.set_channel_communication_type(BLOCKED_CHANNEL_COMMUNICATION);
  
    return true;
  }

  return false;
}

bool grape_channel_communication_dlg::update_validation()
{
  return identifier_valid(m_name_input->GetValue());
}
void grape_channel_communication_dlg::event_update_validation( wxCommandEvent &p_event )
{
  FindWindow(GetAffirmativeId())->Enable( update_validation() );
}

BEGIN_EVENT_TABLE(grape_channel_communication_dlg, wxDialog)
  EVT_TEXT(GRAPE_CHANNEL_COMMUNICATION_NAME_INPUT_TEXT, grape_channel_communication_dlg::event_update_validation)   
END_EVENT_TABLE()

