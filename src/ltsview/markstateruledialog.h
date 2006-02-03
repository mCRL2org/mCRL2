#ifndef MARKSTATERULEDIALOG_H
#define MARKSTATERULEDIALOG_H
#include <map>
#include <wx/wx.h>
#include <wx/statline.h>
#include "mediator.h"
#include "ids.h"
#include "aterm/aterm2.h"

using namespace IDs;

class MarkStateRuleDialog : public wxDialog
{
  public:
    MarkStateRuleDialog( wxWindow* parent, Mediator* owner, ATermList svspec );
    ~MarkStateRuleDialog();
    void onParameterChoice( wxCommandEvent& event );
  private:
    Mediator*			mediator;
    wxChoice*			parameterChoice;
    map< wxString, ATermAppl >	parameterTypes;
    wxChoice*			relationChoice;
    ATermList			stateVectorSpec;
    wxCheckListBox*		valuesListBox;

    void loadValues( wxString paramName );

    DECLARE_EVENT_TABLE();
};

#endif
