// Author(s): Bas Ploeger and Carst Tankink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file markstateruledialog.cpp
/// \brief Add your file description here.

#include "markstateruledialog.h"
#include <wx/statline.h>
#include "ids.h"
#include "utils.h"

using namespace std;
using namespace IDs;
using namespace Utils;

BEGIN_EVENT_TABLE(MarkStateRuleDialog,wxDialog)
  EVT_LISTBOX(myID_PARAMETER_CHOICE,MarkStateRuleDialog::onParameterChoice)
END_EVENT_TABLE()

MarkStateRuleDialog::MarkStateRuleDialog(wxWindow* parent,Mediator* owner,
    LTS *alts)
 : wxDialog(parent,wxID_ANY,wxT("Add mark state rule"),wxDefaultPosition) {

  mediator = owner;
  lts = alts;

  wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
  wxFlexGridSizer* controlSizer = new wxFlexGridSizer(3,3,0,0);
  
  int numParams = lts->getNumParameters();
  wxArrayString paramChoices;
  wxString str;
  paramChoices.Alloc(numParams);
  for (int i = 0; i < numParams; ++i) {
    str = wxString(lts->getParameterName(i).c_str(),wxConvLocal);
    parameterIndices[str] = i;
    paramChoices.Add(str);
  }
  paramChoices.Sort();
  wxString relChoices[2] = { wxT("is an element of"),
    wxT("is not an element of") };

  wxSize lbSize(200,200);

  parameterListBox = new wxListBox(this,myID_PARAMETER_CHOICE,
      wxDefaultPosition,lbSize,paramChoices,wxLB_SINGLE|wxLB_HSCROLL|
      wxLB_NEEDED_SB);
  parameterListBox->SetSelection(0);
  relationListBox = new wxListBox(this,wxID_ANY,wxDefaultPosition,lbSize,2,
      relChoices);
  relationListBox->SetSelection(0);
  valuesListBox = new wxCheckListBox(this,wxID_ANY,wxDefaultPosition,lbSize,0,
      NULL,wxLB_SINGLE|wxLB_HSCROLL|wxLB_NEEDED_SB|wxLB_SORT);
  
  int f = wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL;
  int b = 5;
  
  controlSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Rule Colour: ")), 
                                     0, f, b);

  ruleClrButton = new mcrl2::utilities::wxColorButton(
                                    this, this, wxID_ANY, wxDefaultPosition, 
                                    wxSize(25,25));
  ruleClrButton->SetBackgroundColour(RGB_to_wxC(
    mediator->getNewRuleColour()));

  controlSizer->Add(ruleClrButton, 0, f,b);
  controlSizer->AddStretchSpacer();
  controlSizer->Add(new wxStaticText(this,wxID_ANY,wxT("Parameter:")),0,f,b);
  controlSizer->Add(new wxStaticText(this,wxID_ANY,wxT("Relation:")),0,f,b);
  controlSizer->Add(new wxStaticText(this,wxID_ANY,wxT("Values:")),0,f,b);
  controlSizer->Add(parameterListBox,0,f,b);
  controlSizer->Add(relationListBox,0,f,b);
  controlSizer->Add(valuesListBox,0,f,b);

  
  
  mainSizer->Add(controlSizer,0,wxEXPAND|wxALL,b);
  mainSizer->Add(new wxStaticLine(this,wxID_ANY),0,wxEXPAND|wxALL,b);
  mainSizer->Add(CreateButtonSizer(wxOK|wxCANCEL),0,wxEXPAND|wxALL,b);
  
  mainSizer->Fit(this);
  SetSizer(mainSizer);
  Layout();
  
  if (paramChoices.Count() > 0) {
    loadValues(paramChoices[0]);
  }
}

MarkStateRuleDialog::~MarkStateRuleDialog() {
}

void MarkStateRuleDialog::loadValues(wxString paramName) {
  int p = parameterIndices[paramName];
  wxArrayString values;
  wxString str;
  valueIndices.clear();
  for (int i = 0; i < lts->getNumParameterValues(p); ++i) {
    wxString str = wxString(lts->getParameterValue(p,i).c_str(),wxConvLocal);
    values.Add(str);
    valueIndices[str] = i;
  }
  values.Sort();
  valuesListBox->Set(values);
}

void MarkStateRuleDialog::onParameterChoice(wxCommandEvent& event) {
  loadValues(event.GetString());
}

void MarkStateRuleDialog::setData(int p,RGB_Color col,bool neg,vector<bool> &vals) {
  wxString paramName = wxString(lts->getParameterName(p).c_str(),wxConvLocal);
  parameterListBox->SetStringSelection(paramName);
  loadValues(paramName);

  ruleClrButton->SetBackgroundColour(RGB_to_wxC(col));

  relationListBox->SetSelection(neg ? 1 : 0);

  for (int i = 0; i < lts->getNumParameterValues(p); ++i) {
    if (vals[i]) {
      valuesListBox->Check(valuesListBox->FindString(wxString(
        lts->getParameterValue(p,i).c_str(),wxConvLocal)),true);
    }
  }
}

int MarkStateRuleDialog::getParamIndex() {
  int parIndex = parameterListBox->GetSelection();
  if (parIndex == wxNOT_FOUND) {
    return -1;
  }
  return parameterIndices[parameterListBox->GetString(parIndex)];
}

bool MarkStateRuleDialog::getNegated() {
  return (relationListBox->GetSelection() == 1);
}

void MarkStateRuleDialog::getValues(std::vector<bool> &vals) {
  vals.assign(valuesListBox->GetCount(),false);
  for (unsigned int i = 0; i < valuesListBox->GetCount(); ++i) {
    vals[valueIndices[valuesListBox->GetString(i)]] =
      valuesListBox->IsChecked(i);
  }
}

Utils::RGB_Color MarkStateRuleDialog::getColor() {
  return wxC_to_RGB(ruleClrButton->GetBackgroundColour());
}

wxString MarkStateRuleDialog::getMarkRuleString() {
  int parIndex = parameterListBox->GetSelection();
  if (parIndex == wxNOT_FOUND) {
    return wxEmptyString;
  }
  
  wxString result = parameterListBox->GetString(parIndex);
  if (relationListBox->GetSelection() == 0) {
    result += wxT(" in { ");
  } else {
    result += wxT(" not in { ");
  }
  bool isfirst = true;
  for (unsigned int i = 0 ; i < valuesListBox->GetCount(); ++i) {
    if (valuesListBox->IsChecked(i)) {
      if (!isfirst) {
        result += wxT(", ");
      }
      result += valuesListBox->GetString( i );
      isfirst = false;
    }
  }
  result += wxT(" }");
  return result;
}
