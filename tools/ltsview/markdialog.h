// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file markdialog.h
/// \brief Header file for mark dialog class

#ifndef MARKDIALOG_H
#define MARKDIALOG_H

#include <wx/wx.h>
#include "mediator.h"

class MarkDialog: public wxDialog {
  public:
    MarkDialog(wxWindow* parent, Mediator* owner);

    void onAddMarkRuleButton(wxCommandEvent& event);
    void onRemoveMarkRuleButton(wxCommandEvent& event);

    void onMarkAnyAll(wxCommandEvent& event);
    void onMarkRadio(wxCommandEvent& event);
    void onMarkRuleActivate(wxCommandEvent& event);
    void onMarkRuleEdit(wxCommandEvent& event);
    void onMarkTransition(wxCommandEvent& event);
    
    void addMarkRule(wxString str,int mr); 
    void replaceMarkRule(wxString str, int mr);
    void resetMarkRules();    
    void setActionLabels(std::vector<std::string> &labels);
    
  private:
    Mediator* mediator;

    wxRadioButton* nomarksRadio;
    wxRadioButton* markDeadlocksRadio;
    wxRadioButton* markStatesRadio;
    wxRadioButton* markTransitionsRadio;

    wxChoice* markAnyAllChoice;
    wxCheckListBox* markStatesListBox;
    wxCheckListBox* markTransitionsListBox;

  DECLARE_EVENT_TABLE()
};

#endif //MARKDIALOG_H
