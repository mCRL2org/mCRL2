// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file infodialog.cpp
/// \brief Source file for information dialog class

#include "wx.hpp" // precompiled headers

#include "infodialog.h"
#include <wx/notebook.h>
#include <wx/listctrl.h>

InfoDialog::InfoDialog(wxWindow* parent) :
  wxDialog(parent,wxID_ANY,wxT("Info"),wxDefaultPosition,wxDefaultSize,
             wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER) {

  int lf = wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxEXPAND | wxALL;

  wxFlexGridSizer* sizer = new wxFlexGridSizer(1,1,0,0);
  sizer->AddGrowableRow(0);
  sizer->AddGrowableCol(0);
  wxNotebook* topNotebook = new wxNotebook(this, wxID_ANY);

  wxPanel* panel = new wxPanel(topNotebook, wxID_ANY);
  lts_info = new wxListCtrl(panel,wxID_ANY,wxDefaultPosition,wxDefaultSize,
      wxLC_REPORT|wxSUNKEN_BORDER|wxLC_HRULES|wxLC_VRULES|wxLC_NO_HEADER);
  lts_info->InsertColumn(0, wxT(""));
  lts_info->InsertColumn(1, wxT(""),wxLIST_FORMAT_RIGHT);
  lts_info->InsertItem(0,wxT("States:"));
  lts_info->InsertItem(1,wxT("Transitions:"));
  lts_info->InsertItem(2,wxT("Clusters:"));
  lts_info->InsertItem(3,wxT("Ranks:"));
  lts_info->InsertItem(4,wxT("Marked states:"));
  lts_info->InsertItem(5,wxT("Marked transitions:"));
  lts_info->SetColumnWidth(0,wxLIST_AUTOSIZE);
  wxFlexGridSizer* topSizer = new wxFlexGridSizer(1,1,0,0);
  topSizer->AddGrowableRow(0);
  topSizer->AddGrowableCol(0);
  topSizer->Add(lts_info,0,lf,3);
  topSizer->Fit(panel);
  panel->SetSizer(topSizer);
  panel->Fit();
  panel->Layout();

  wxPanel* clus_panel = new wxPanel(topNotebook, wxID_ANY);
  cluster_info = new wxListCtrl(clus_panel, wxID_ANY, wxDefaultPosition,
      wxDefaultSize,
      wxLC_REPORT|wxSUNKEN_BORDER|wxLC_HRULES|wxLC_VRULES | wxLC_NO_HEADER);
  cluster_info->InsertColumn(0, wxT(""));
  cluster_info->InsertColumn(1, wxT(""), wxLIST_FORMAT_RIGHT);
  cluster_info->InsertItem(0, wxT("Nr of states:"));
  cluster_info->InsertItem(1, wxT(""));
  cluster_info->InsertItem(2, wxT("Parameter"));
  cluster_info->SetItem(2,1, wxT("Value"));
  wxFlexGridSizer* clusSizer = new wxFlexGridSizer(1,1,0,0);
  clusSizer->AddGrowableRow(0);
  clusSizer->AddGrowableCol(0);
  clusSizer->Add(cluster_info, 0, lf, 3);
  clusSizer->Fit(clus_panel);
  clus_panel->SetSizer(clusSizer);
  clus_panel->Fit();
  clus_panel->Layout();


  wxPanel* panel1 = new wxPanel(topNotebook, wxID_ANY);
  state_info = new wxListCtrl(panel1,wxID_ANY,wxDefaultPosition,wxDefaultSize,
      wxLC_REPORT|wxSUNKEN_BORDER|wxLC_HRULES|wxLC_VRULES);
  state_info->InsertColumn(0, wxT("Parameter"));
  state_info->InsertColumn(1, wxT("Value"));
  wxFlexGridSizer* selSizer = new wxFlexGridSizer(1,1,0,0);
  selSizer->AddGrowableRow(0);
  selSizer->AddGrowableCol(0);
  selSizer->Add(state_info,0,lf,3);
  selSizer->Fit(panel1);
  panel1->SetSizer(selSizer);
  panel1->Fit();
  panel1->Layout();

  topNotebook->AddPage(panel, wxT("LTS info"), false);
  topNotebook->AddPage(clus_panel, wxT("Cluster info"), false);
  topNotebook->AddPage(panel1, wxT("State info"), false);

  sizer->Add(topNotebook,0,wxEXPAND|wxALL,5);
  SetSizer(sizer);
  Layout();
}

void InfoDialog::setLTSInfo(int ns,int nt,int nc,int nr) {
  lts_info->SetItem(0,1,wxString::Format(wxT("%d"),ns));
  lts_info->SetItem(1,1,wxString::Format(wxT("%d"),nt));
  lts_info->SetItem(2,1,wxString::Format(wxT("%d"),nc));
  lts_info->SetItem(3,1,wxString::Format(wxT("%d"),nr));
  lts_info->SetColumnWidth(1,wxLIST_AUTOSIZE);
}

void InfoDialog::setNumMarkedStates(int n) {
  lts_info->SetItem(4,1,wxString::Format(wxT("%d"),n));
  lts_info->SetColumnWidth(1,wxLIST_AUTOSIZE);
}

void InfoDialog::setNumMarkedTransitions(int n) {
  lts_info->SetItem(5,1,wxString::Format(wxT("%d"),n));
  lts_info->SetColumnWidth(1,wxLIST_AUTOSIZE);
}

void InfoDialog::setParameterName(int i,std::string p) {
  state_info->InsertItem(i,wxString(p.c_str(),wxConvLocal));
  setStateInfoColWidth(0);

  cluster_info->InsertItem(i+3, wxString(p.c_str(), wxConvLocal));

  cluster_info->SetColumnWidth(0, wxLIST_AUTOSIZE);
}

void InfoDialog::setParameterValue(int i,std::string v) {
  state_info->SetItem(i,1,wxString(v.c_str(),wxConvLocal));
  setStateInfoColWidth(1);
}

void InfoDialog::setParameterValues(int i, std::vector<std::string> vs)
{
  wxString values = wxT("{");

  for (size_t j = 0; j < vs.size(); ++j)
  {
    values += wxString(vs[j].c_str(), wxConvLocal);

    if (j != vs.size() - 1)
    {
      values += wxT(", ");
    }
  }

  values += wxT("}");
  cluster_info->SetItem(i+3, 1, values);
  cluster_info->SetColumnWidth(1, wxLIST_AUTOSIZE);
}

void InfoDialog::setClusterStateNr(int i)
{
  cluster_info->SetItem(0,1, wxString::Format(wxT("%d"), i));
  cluster_info->SetColumnWidth(1, wxLIST_AUTOSIZE);
}

void InfoDialog::resetParameterNames() {
  state_info->DeleteAllItems();
  setStateInfoColWidth(0);
  setStateInfoColWidth(1);
}

void InfoDialog::resetParameterValues() {
  for (int j = 0; j < state_info->GetItemCount(); ++j) {
    state_info->SetItem(j,1,wxEmptyString);
  }
  setStateInfoColWidth(1);

  for(int j = 0; j < cluster_info->GetItemCount(); ++j)
  {
    cluster_info->SetItem(j, 1, wxEmptyString);
  }

  cluster_info->SetColumnWidth(1, wxLIST_AUTOSIZE);
}

void InfoDialog::setStateInfoColWidth(int col) {
  state_info->SetColumnWidth(col,wxLIST_AUTOSIZE);
  int w = state_info->GetColumnWidth(col);
  state_info->SetColumnWidth(col,wxLIST_AUTOSIZE_USEHEADER);
  w = std::max(w,state_info->GetColumnWidth(col));
  state_info->SetColumnWidth(col,w);
}

