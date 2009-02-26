// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file channeldialog.h
//
// Defines a channel dialog.

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

#include "channeldialog.h"

using namespace grape::grapeapp;

grape_channel_dlg::grape_channel_dlg( channel &p_channel )
: wxDialog( 0, wxID_ANY, _T("Edit channel") )
{  
  wxBoxSizer *wnd_sizer = new wxBoxSizer(wxVERTICAL);

  wxStaticText *text_rename = new wxStaticText( this, wxID_ANY, _T("channel rename") );
  wnd_sizer->Add(text_rename, 0, wxALIGN_TOP, 0 );
  wnd_sizer->AddSpacer( 5 );

  m_input = new wxTextCtrl(this, wxID_ANY, p_channel.get_name() );            
  wnd_sizer->Add(m_input, 0, wxEXPAND, 0);
  wnd_sizer->AddSpacer( 5 );

  wxStaticText *text_property = new wxStaticText( this, wxID_ANY, _T("channel property") );
  wnd_sizer->Add(text_property, 0, wxALIGN_TOP, 0 );
  wnd_sizer->AddSpacer( 5 );
  
  // select the correct property of the channel
  int index=0;  
  if (p_channel.get_channel_communication() == 0)
  {  
    if (p_channel.get_channeltype() == channel_hidden) index = 0;
    if (p_channel.get_channeltype() == channel_blocked) index = 1;
    if (p_channel.get_channeltype() == channel_visible) index = 2;
    wxString combobox_list[3] = {_T("hidden"), _T("blocked"), _T("visible")};
    m_combobox = new wxComboBox(this, wxID_ANY, combobox_list[index], wxDefaultPosition, wxDefaultSize, 3, combobox_list, wxCB_READONLY);
  } else {
    wxString combobox_list[1] = {_T("hidden")};
    m_combobox = new wxComboBox(this, wxID_ANY, combobox_list[index], wxDefaultPosition, wxDefaultSize, 0, combobox_list, wxCB_READONLY);
  }
  wnd_sizer->Add(m_combobox, 0, wxEXPAND, 0);
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
}

grape_channel_dlg::grape_channel_dlg()
{
  // shouldn't be called
}

grape_channel_dlg::~grape_channel_dlg()
{
}

bool grape_channel_dlg::show_modal( channel &p_channel )
{
  if (ShowModal() != wxID_CANCEL)
  {
    p_channel.set_name(m_input->GetValue());     
    if (m_combobox->GetValue() == _T("hidden")) p_channel.set_channeltype(channel_hidden);
    if (m_combobox->GetValue() == _T("blocked")) p_channel.set_channeltype(channel_blocked);
    if (m_combobox->GetValue() == _T("visible")) p_channel.set_channeltype(channel_visible);
  
    return true;
  } else {
    return false;
  }
}
