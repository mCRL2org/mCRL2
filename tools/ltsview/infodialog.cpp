#include "infodialog.h"
#include <wx/notebook.h>
#include <algorithm>

InfoDialog::InfoDialog(wxWindow* parent) :
  wxDialog(parent,wxID_ANY,wxT("Info"),wxDefaultPosition) {
  
  int lf = wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxEXPAND | wxALL;
  
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
  topNotebook->AddPage(panel, wxT("LTS info"), false);

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
  topNotebook->AddPage(panel1, wxT("State info"), false);

  //panel->Layout();
  //panel->UpdateSize();
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
}

void InfoDialog::setParameterValue(int i,std::string v) {
  state_info->SetItem(i,1,wxString(v.c_str(),wxConvLocal));
  setStateInfoColWidth(1);
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
}

void InfoDialog::setStateInfoColWidth(int col) {
  state_info->SetColumnWidth(col,wxLIST_AUTOSIZE);
  int w = state_info->GetColumnWidth(col);
  state_info->SetColumnWidth(col,wxLIST_AUTOSIZE_USEHEADER);
  w = std::max(w,state_info->GetColumnWidth(col));
  state_info->SetColumnWidth(col,w);
}
