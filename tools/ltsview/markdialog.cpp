// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file markdialog.cpp
/// \brief Source file for mark dialog class

#include "wx.hpp" // precompiled headers

#include "markdialog.h"
#include "mediator.h"
#include "ids.h"
#include "utils.h"
#include <string>
#include <vector>

using namespace IDs;
using namespace Utils;
using namespace std;

BEGIN_EVENT_TABLE(MarkDialog, wxDialog)
  EVT_RADIOBUTTON(myID_MARK_RADIOBUTTON, MarkDialog::onMarkRadio)
  EVT_CHOICE(myID_MARK_ANYALL, MarkDialog::onMarkAnyAll)
  EVT_CHOICE(myID_MARK_CLUSTER, MarkDialog::onMarkCluster)
  EVT_LISTBOX_DCLICK(myID_MARK_RULES, MarkDialog::onMarkRuleEdit)
  EVT_CHECKLISTBOX(myID_MARK_RULES, MarkDialog::onMarkRuleActivate)
  EVT_CHECKLISTBOX(myID_MARK_TRANSITIONS, MarkDialog::onMarkTransition)
  EVT_BUTTON(myID_ADD_RULE, MarkDialog::onAddMarkRuleButton)
  EVT_BUTTON(myID_REMOVE_RULE, MarkDialog::onRemoveMarkRuleButton)
END_EVENT_TABLE()


MarkDialog::MarkDialog(wxWindow* parent, Mediator* owner)
  : wxDialog(parent, wxID_ANY, wxT("Mark"), wxDefaultPosition,
      wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
{
  mediator = owner;

  wxFlexGridSizer* markSizer = new wxFlexGridSizer(7,1,0,0);
  markSizer->AddGrowableCol(0);
  markSizer->AddGrowableRow(5);
  markSizer->AddGrowableRow(6);

  int flags = wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL;
  int border = 3;

  nomarksRadio = new wxRadioButton(this,myID_MARK_RADIOBUTTON,
      wxT("No marks"),wxDefaultPosition,wxDefaultSize,wxRB_GROUP);
  nomarksRadio->SetValue(true);
  markDeadlocksRadio = new wxRadioButton(this,myID_MARK_RADIOBUTTON,
      wxT("Mark deadlocks"));
  markStatesRadio = new wxRadioButton(this,myID_MARK_RADIOBUTTON,
      wxT("Mark states"));
  markTransitionsRadio = new wxRadioButton(this,myID_MARK_RADIOBUTTON,
      wxT("Mark transitions"));

  markSizer->Add(nomarksRadio,0,flags,border);
  markSizer->Add(markDeadlocksRadio,0,flags,border);
  markSizer->Add(markStatesRadio,0,flags,border);
  markSizer->Add(markTransitionsRadio,0,flags,border);

  wxString choices1[2] = {
    wxT("Mark cluster if any state is marked"),
    wxT("Mark cluster if all states are marked") };
  markClusterChoice = new wxChoice(this,myID_MARK_CLUSTER,
      wxDefaultPosition,wxDefaultSize,2,choices1);
  markClusterChoice->SetSelection(0);
  markSizer->Add(markClusterChoice,0,flags,border);

  wxStaticBoxSizer* markStatesSizer = new wxStaticBoxSizer(wxVERTICAL,
      this,wxT("Mark states"));

  wxString choices2[3] = {
    wxT("Match any of the following"),
    wxT("Match all of the following"),
    wxT("Match the following separately") };
  markAnyAllChoice = new wxChoice(this,myID_MARK_ANYALL,
      wxDefaultPosition,wxDefaultSize,3,choices2);
  markAnyAllChoice->SetSelection(0);
  markStatesSizer->Add(markAnyAllChoice,0,flags,border);

  markStatesListBox = new wxCheckListBox(this,myID_MARK_RULES,
      wxDefaultPosition,wxSize(200,100),0,NULL,
      wxLB_SINGLE|wxLB_NEEDED_SB|wxLB_HSCROLL);
  //markStatesListBox->SetMinSize(wxSize(200,-1));
  markStatesSizer->Add(markStatesListBox,1,flags|wxEXPAND,border);
  wxBoxSizer* addremoveSizer = new wxBoxSizer(wxHORIZONTAL);
  addremoveSizer->Add(new wxButton(this,myID_ADD_RULE,wxT("Add")),0,
      flags,border);
  addremoveSizer->Add(new wxButton(this,myID_REMOVE_RULE,wxT("Remove")),
      0,flags,border);

  markStatesSizer->Add(addremoveSizer,0,flags,border);

  wxStaticBoxSizer* markTransitionsSizer = new wxStaticBoxSizer(
      wxVERTICAL,this,wxT("Mark transitions"));
  markTransitionsListBox = new wxCheckListBox(this,
      myID_MARK_TRANSITIONS,wxDefaultPosition,wxSize(200,-1),0,NULL,
      wxLB_SINGLE|wxLB_SORT|wxLB_NEEDED_SB|wxLB_HSCROLL);
  markTransitionsListBox->SetMinSize(wxSize(200,-1));
  markTransitionsSizer->Add(markTransitionsListBox,1,flags|wxEXPAND,
      border);

  markSizer->Add(markStatesSizer,0,wxEXPAND|wxALL,border);
  markSizer->Add(markTransitionsSizer,0,wxEXPAND|wxALL,border);
  markSizer->Fit(this);
  SetSizer(markSizer);
  Fit();
  Layout();

}

void MarkDialog::onMarkRadio(wxCommandEvent& event)
{
  wxRadioButton* buttonClicked = (wxRadioButton*)event.GetEventObject();

  if (buttonClicked == nomarksRadio)
    mediator->setMarkStyle(NO_MARKS);
  else if (buttonClicked == markDeadlocksRadio)
    mediator->setMarkStyle(MARK_DEADLOCKS);
  else if (buttonClicked == markStatesRadio)
    mediator->setMarkStyle(MARK_STATES);
  else if (buttonClicked == markTransitionsRadio)
    mediator->setMarkStyle(MARK_TRANSITIONS);
}

void MarkDialog::onMarkRuleActivate(wxCommandEvent& event)
{
  int i = event.GetInt();
  mediator->activateMarkRule(
      *(static_cast<int*>(markStatesListBox->GetClientData(i))),
      markStatesListBox->IsChecked(i));
  markStatesRadio->SetValue(true);
}

void MarkDialog::onMarkRuleEdit(wxCommandEvent& event)
{
  mediator->editMarkRule(*(static_cast<int*>
        (markStatesListBox->GetClientData(event.GetInt()))));
}

void MarkDialog::onMarkAnyAll(wxCommandEvent& event)
{
  if (event.GetSelection() == 0) {
    mediator->setMatchStyle(MATCH_ANY);
  } else if (event.GetSelection() == 1) {
    mediator->setMatchStyle(MATCH_ALL);
  } else if (event.GetSelection() == 2) {
    mediator->setMatchStyle(MATCH_MULTI);
  }
  markStatesRadio->SetValue(true);
}

void MarkDialog::onMarkCluster(wxCommandEvent& event)
{
  if (event.GetSelection() == 0)
  {
    mediator->setMatchStyleClusters(MATCH_ANY);
  }
  else if (event.GetSelection() == 1)
  {
    mediator->setMatchStyleClusters(MATCH_ALL);
  }
}

void MarkDialog::onAddMarkRuleButton(wxCommandEvent& /*event*/)
{
  mediator->addMarkRule();
}

void MarkDialog::addMarkRule(wxString str,int mr)
{
  markStatesListBox->Append(str,new int(mr));
  markStatesListBox->Check(markStatesListBox->GetCount()-1,true);
  markStatesRadio->SetValue(true);
  markStatesListBox->GetParent()->Fit();
  Layout();
}

void MarkDialog::onRemoveMarkRuleButton(wxCommandEvent& /*event*/)
{
  int i = markStatesListBox->GetSelection();
  if (i != wxNOT_FOUND) {
    int *p = static_cast<int*>(markStatesListBox->GetClientData(i));
    markStatesListBox->Delete(i);
    mediator->removeMarkRule(*p);
    markStatesRadio->SetValue(true);
    markStatesListBox->GetParent()->Fit();
    Layout();
    delete p;
  }
}

void MarkDialog::onMarkTransition(wxCommandEvent& event)
{
  int i = event.GetInt();
  mediator->setActionMark(
      label_index[markTransitionsListBox->GetString(i)],
      markTransitionsListBox->IsChecked(i));
  markTransitionsRadio->SetValue(true);
}

void MarkDialog::replaceMarkRule(wxString str,int mr)
{
  unsigned int i = 0;
  while (i < markStatesListBox->GetCount() &&
      mr != *(static_cast<int*>(markStatesListBox->GetClientData(i))))
  {
    ++i;
  }
  if (i == markStatesListBox->GetCount())
  {
    return;
  }
  bool isChecked = markStatesListBox->IsChecked(i);
  markStatesListBox->SetString(i,str);
  markStatesListBox->Check(i,isChecked);
  markStatesRadio->SetValue(true);
}

void MarkDialog::resetMarkRules()
{
  for (unsigned int i = 0; i < markStatesListBox->GetCount(); ++i)
  {
    delete static_cast<int*>(markStatesListBox->GetClientData(i));
  }
  markStatesListBox->Clear();
  markAnyAllChoice->SetSelection(0);
  nomarksRadio->SetValue(true);
  markStatesListBox->GetParent()->Fit();
  Layout();
}

void MarkDialog::setActionLabels(vector<string> &labels)
{
  wxArrayString strLabels;
  strLabels.Alloc(labels.size());
  label_index.clear();
  wxString wxlabel;
  for (unsigned int i = 0; i < labels.size(); ++i)
  {
    wxlabel = wxString(labels[i].c_str(),wxConvLocal);
    strLabels.Add(wxlabel);
    label_index[wxlabel] = i;
  }
  strLabels.Sort();
  markTransitionsListBox->Set(strLabels);
  markTransitionsListBox->GetParent()->Fit();
  Layout();
}

