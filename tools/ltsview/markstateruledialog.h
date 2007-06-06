#ifndef MARKSTATERULEDIALOG_H
#define MARKSTATERULEDIALOG_H
#include <map>
#include <wx/wx.h>
#include <wx/checklst.h>
#include "mediator.h"
#include "lts.h"
#include "utils.h"

class MarkStateRuleDialog : public wxDialog { 
  public:
    MarkStateRuleDialog(wxWindow* parent,Mediator* owner,LTS* alts);
    ~MarkStateRuleDialog();
    Utils::MarkRule*	getMarkRule();
    wxString	getMarkRuleString();
    void	onParameterChoice(wxCommandEvent& event);
    void	setMarkRule(Utils::MarkRule* mr);
  private:
    Mediator*	mediator;
    std::map< wxString, int >	parameterIndices;
    wxListBox*	parameterListBox;
    wxListBox*	relationListBox;
    std::map< wxString, int >	valueIndices;
    wxCheckListBox*	valuesListBox;
		LTS*	lts;

    void loadValues(wxString paramName);

    DECLARE_EVENT_TABLE();
};

#endif
