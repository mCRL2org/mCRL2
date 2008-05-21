// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file markstateruledialog.h
/// \brief Add your file description here.

#ifndef MARKSTATERULEDIALOG_H
#define MARKSTATERULEDIALOG_H
#include <map>
#include <wx/wx.h>
#include <wx/checklst.h>
#include "mediator.h"
#include "lts.h"
#include "utils.h"
#include "mcrl2/utilities/colorbutton.h"

class MarkStateRuleDialog : public wxDialog { 
  public:
    MarkStateRuleDialog(wxWindow* parent,Mediator* owner,LTS* alts);
    ~MarkStateRuleDialog();
    int getParamIndex();
    bool getNegated();
    void getValues(std::vector<bool> &vals);
    Utils::RGB_Color getColor();
    wxString getMarkRuleString();
    void setData(int p,Utils::RGB_Color col,bool neg,std::vector<bool> &vals);
    void onParameterChoice(wxCommandEvent& event);
  private:
    Mediator* mediator;
    std::map< wxString, int > parameterIndices;
    wxListBox* parameterListBox;
    wxListBox* relationListBox;
    mcrl2::utilities::wxColorButton* ruleClrButton;
    std::map< wxString, int > valueIndices;
    wxCheckListBox* valuesListBox;
    LTS*  lts;
    void loadValues(wxString paramName);
    DECLARE_EVENT_TABLE();
};

#endif
