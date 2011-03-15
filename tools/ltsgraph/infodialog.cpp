// Author(s): Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file infodialog.cpp
/// \brief LTS information dialog

#include "wx.hpp" // precompiled headers

#include "infodialog.h"
#include <wx/notebook.h>

InfoDialog::InfoDialog(wxWindow* parent) :
  wxDialog(parent, wxID_ANY, wxT("Info"), wxDefaultPosition, wxDefaultSize,
           wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
{
  int lf = wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxEXPAND | wxALL;
  int lcf = wxLC_REPORT|wxSUNKEN_BORDER|wxLC_HRULES|wxLC_VRULES;


  lts_info = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                            lcf|wxLC_NO_HEADER);

  lts_info->InsertColumn(0, wxT(""));
  lts_info->InsertColumn(1, wxT(""), wxLIST_FORMAT_RIGHT);
  lts_info->InsertItem(0, wxT("Initial state:"));
  lts_info->InsertItem(1, wxT("States:"));
  lts_info->InsertItem(2, wxT("Transitions:"));
  lts_info->InsertItem(3, wxT("Labels:"));
  lts_info->SetColumnWidth(0, wxLIST_AUTOSIZE);

  wxFlexGridSizer* infoSizer = new wxFlexGridSizer(1, 1, 0, 0);
  infoSizer->AddGrowableRow(0);
  infoSizer->AddGrowableCol(0);
  infoSizer->Add(lts_info, 0, lf, 3);
  infoSizer->Fit(this);
  SetSizer(infoSizer);
  Fit();
  Layout();
}

void InfoDialog::setLTSInfo(size_t is, size_t ns, size_t nt, size_t nl)
{

  lts_info->SetItem(0, 1, wxString::Format(wxT("%lu"), is));
  lts_info->SetItem(1, 1, wxString::Format(wxT("%lu"), ns));
  lts_info->SetItem(2, 1, wxString::Format(wxT("%lu"), nt));
  lts_info->SetItem(3, 1, wxString::Format(wxT("%lu"), nl));
  lts_info->SetColumnWidth(1, wxLIST_AUTOSIZE);

  Fit();
  Layout();
}
