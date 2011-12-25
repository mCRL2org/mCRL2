// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file markstateruledialog.h
/// \brief Header file for MarkStateRuleDialog class

#ifndef MARKSTATERULEDIALOG_H
#define MARKSTATERULEDIALOG_H

#include <map>
#include <vector>
#include <wx/wx.h>

#include "mcrl2/atermpp/map.h"
#include "mcrl2/atermpp/set.h"
#include "mcrl2/utilities/colorbutton.h"

class LTS;
class Mediator;
class RGB_Color;
class wxCheckListBox;

class MarkStateRuleDialog : public wxDialog
{
  public:
    MarkStateRuleDialog(wxWindow* parent,Mediator* owner,LTS* alts);
    ~MarkStateRuleDialog();
    int getParamIndex();
    bool getNegated();
    std::set<std::string> getValues();
    RGB_Color getColor();
    wxString getMarkRuleString();
    void setData(int p,RGB_Color col,bool neg, const std::set<std::string> &vals);
    void onParameterChoice(wxCommandEvent& event);
  private:
    Mediator* mediator;
    std::map< wxString, int > parameterIndices;
    wxListBox* parameterListBox;
    wxListBox* relationListBox;
    mcrl2::utilities::wx::wxColorButton* ruleClrButton;
    std::map< wxString, std::string > values;
    wxCheckListBox* valuesListBox;
    LTS*  lts;
    void loadValues(wxString paramName);
    DECLARE_EVENT_TABLE()
};

#endif
