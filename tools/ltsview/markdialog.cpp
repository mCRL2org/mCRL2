#include "markdialog.h"
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
  EVT_LISTBOX_DCLICK(myID_MARK_RULES, MarkDialog::onMarkRuleEdit)
  EVT_CHECKLISTBOX(myID_MARK_RULES, MarkDialog::onMarkRuleActivate)
  EVT_CHECKLISTBOX(myID_MARK_TRANSITIONS, MarkDialog::onMarkTransition)
  EVT_BUTTON(myID_ADD_RULE, MarkDialog::onAddMarkRuleButton)
  EVT_BUTTON(myID_REMOVE_RULE, MarkDialog::onRemoveMarkRuleButton)
END_EVENT_TABLE()


MarkDialog::MarkDialog(wxWindow* parent, Mediator* owner)
  : wxDialog(parent, wxID_ANY, wxT("Mark"), wxDefaultPosition, wxDefaultSize,
             wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
{
  mediator = owner;
  

  wxFlexGridSizer* markSizer = new wxFlexGridSizer(6,1,0,0);
  markSizer->AddGrowableCol(0);
  markSizer->AddGrowableRow(4);
  markSizer->AddGrowableRow(5);

  int flags = wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL;
  int border = 3;
  
  nomarksRadio = new wxRadioButton(this,myID_MARK_RADIOBUTTON,wxT("No marks"),
      wxDefaultPosition,wxDefaultSize,wxRB_GROUP);
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
  
  wxStaticBoxSizer* markStatesSizer = new wxStaticBoxSizer(wxVERTICAL,this,
      wxT("Mark states"));

  //TODO: Name correct?
  wxString choices[3] = { wxT("Match any of the following"),
      wxT("Match all of the following"), wxT("Match the following separately") };

  markAnyAllChoice = new wxChoice(this,myID_MARK_ANYALL,wxDefaultPosition,
      wxDefaultSize,3,choices);
  markAnyAllChoice->SetSelection(0);
  markStatesSizer->Add(markAnyAllChoice,0,flags,border);
  
  markStatesListBox = new wxCheckListBox(this,myID_MARK_RULES,
      wxDefaultPosition,wxSize(200,100),0,NULL,
      wxLB_SINGLE|wxLB_NEEDED_SB|wxLB_HSCROLL);
  //markStatesListBox->SetMinSize(wxSize(200,-1));
  markStatesSizer->Add(markStatesListBox,1,flags|wxEXPAND,border);
  wxBoxSizer* addremoveSizer = new wxBoxSizer(wxHORIZONTAL);
  addremoveSizer->Add(new wxButton(this,myID_ADD_RULE,wxT("Add")),0,flags,
      border);
  addremoveSizer->Add(new wxButton(this,myID_REMOVE_RULE,wxT("Remove")),0,
      flags,border);

  markStatesSizer->Add(addremoveSizer,0,flags,border);
  
  wxStaticBoxSizer* markTransitionsSizer = new wxStaticBoxSizer(wxVERTICAL,
      this,wxT("Mark transitions"));
  markTransitionsListBox = new wxCheckListBox(this,myID_MARK_TRANSITIONS,
      wxDefaultPosition,wxSize(200,-1),0,NULL,wxLB_SINGLE|wxLB_SORT|
      wxLB_NEEDED_SB|wxLB_HSCROLL);
  markTransitionsListBox->SetMinSize(wxSize(200,-1));
  markTransitionsSizer->Add(markTransitionsListBox,1,flags|wxEXPAND,border);
  
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
    mediator->applyMarkStyle(NO_MARKS);
  else if (buttonClicked == markDeadlocksRadio)
    mediator->applyMarkStyle(MARK_DEADLOCKS);
  else if (buttonClicked == markStatesRadio)
    mediator->applyMarkStyle(MARK_STATES);
  else if (buttonClicked == markTransitionsRadio)
    mediator->applyMarkStyle(MARK_TRANSITIONS);
}

void MarkDialog::onMarkRuleActivate(wxCommandEvent& event) 
{
  int i = event.GetInt();
  mediator->activateMarkRule(i, markStatesListBox->IsChecked(i));
  markStatesRadio->SetValue(true);
}

void MarkDialog::onMarkRuleEdit(wxCommandEvent& event)
{
  mediator->editMarkRule(event.GetSelection());
}

void MarkDialog::onMarkAnyAll(wxCommandEvent& event)
{
  mediator->setMatchAnyMarkRule(event.GetSelection());
  markStatesRadio->SetValue(true);
}

void MarkDialog::onAddMarkRuleButton(wxCommandEvent& /*event*/)
{
  mediator->addMarkRule();
}

void MarkDialog::addMarkRule(wxString str)
{
  markStatesListBox->Append(str);
  markStatesListBox->Check(markStatesListBox->GetCount()-1,true);
  markStatesRadio->SetValue(true);
  markStatesListBox->GetParent()->Fit();
  Layout();
}

void MarkDialog::onRemoveMarkRuleButton(wxCommandEvent& /*event*/)
{
  int sel_index = markStatesListBox->GetSelection();
  if (sel_index != wxNOT_FOUND) {
    markStatesListBox->Delete(sel_index);
    mediator->removeMarkRule(sel_index);
    markStatesRadio->SetValue(true);
    markStatesListBox->GetParent()->Fit();
    Layout();
  }
}

void MarkDialog::onMarkTransition(wxCommandEvent& event)
{
  int i = event.GetInt();
  if (markTransitionsListBox->IsChecked(i)) {
    mediator->markAction(string(markTransitionsListBox->GetString(i).fn_str()));
  } else {
    mediator->unmarkAction(string(
          markTransitionsListBox->GetString(i).fn_str()));
  }
  markTransitionsRadio->SetValue(true);
}

void MarkDialog::replaceMarkRule(int index, wxString str)
{
  bool isChecked = markStatesListBox->IsChecked(index);
  markStatesListBox->SetString(index,str);
  markStatesListBox->Check(index,isChecked);
  markStatesRadio->SetValue(true);
}

void MarkDialog::resetMarkRules()
{
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
  for (vector<string>::iterator it = labels.begin(); it != labels.end(); ++it) {
    strLabels.Add(wxString(it->c_str(),wxConvLocal));
  }
  strLabels.Sort();
  markTransitionsListBox->Set(strLabels);
  markTransitionsListBox->GetParent()->Fit();
  Layout();
}

