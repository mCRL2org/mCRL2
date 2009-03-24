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
: wxDialog( 0, wxID_ANY, wxT("Edit channel"), wxDefaultPosition, wxDefaultSize )
{  
  wxBoxSizer *wnd_sizer = new wxBoxSizer(wxVERTICAL);

  wxStaticText *text_name = new wxStaticText( this, wxID_ANY, _T("channel name") );
  wnd_sizer->Add(text_name, 0, wxALIGN_TOP, 0 );
  wnd_sizer->AddSpacer( 5 );

  m_name_input = new wxTextCtrl(this, wxID_ANY, p_channel.get_name() );            
  wnd_sizer->Add(m_name_input, 0, wxEXPAND, 0);
  wnd_sizer->AddSpacer( 5 );
  
  wxStaticText *text_rename = new wxStaticText( this, wxID_ANY, _T("channel rename") );
  wnd_sizer->Add(text_rename, 0, wxALIGN_TOP, 0 );
  wnd_sizer->AddSpacer( 5 );

  m_rename_input = new wxTextCtrl(this, wxID_ANY, p_channel.get_rename_to() );            
  wnd_sizer->Add(m_rename_input, 0, wxEXPAND, 0);
  wnd_sizer->AddSpacer( 5 );

  if (p_channel.get_channel_communications()->GetCount() == 0)
  {
    wxStaticText *text_property = new wxStaticText( this, wxID_ANY, _T("channel property") );
    wnd_sizer->Add(text_property, 0, wxALIGN_TOP, 0 );
    wnd_sizer->AddSpacer( 5 );
    
    // select the correct property of the channel
    int index = p_channel.get_channel_type();
    wxString combobox_list[3] = {_T("visible"), _T("hidden"), _T("blocked")};
    m_combobox = new wxComboBox(this, wxID_ANY, combobox_list[index], wxDefaultPosition, wxDefaultSize, 3, combobox_list, wxCB_READONLY);
    wnd_sizer->Add(m_combobox, 0, wxEXPAND, 0);
    wnd_sizer->AddSpacer( 5 );
  }

  // create buttons
  wxSizer *sizer = CreateButtonSizer(wxOK | wxCANCEL);
  sizer->Layout();
  wnd_sizer->Add(sizer, 0, wxALIGN_BOTTOM, 0);

  // realize sizers
  SetSizer(wnd_sizer);
  wnd_sizer->Fit(this);
  wnd_sizer->SetSizeHints(this);

  m_name_input->SetFocus();
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
    p_channel.set_name(m_name_input->GetValue());     
    p_channel.set_rename_to(m_rename_input->GetValue());
    if (p_channel.get_channel_communications()->GetCount() == 0)
    {
      if (m_combobox->GetValue() == _T("visible")) p_channel.set_channel_type(VISIBLE_CHANNEL);
      if (m_combobox->GetValue() == _T("hidden")) p_channel.set_channel_type(HIDDEN_CHANNEL);
      if (m_combobox->GetValue() == _T("blocked")) p_channel.set_channel_type(BLOCKED_CHANNEL);
    }
    return true;
  }
  
  return false;
}
