#ifndef MARKSTATERULEDIALOG_H
#define MARKSTATERULEDIALOG_H
#include <map>
#include <vector>
#include <wx/wx.h>
#include <wx/checklst.h>
#include <wx/statline.h>
#include "mediator.h"
#include "ids.h"
#include "aterm/aterm2.h"
#include "utils.h"

using namespace IDs;
using namespace Utils;

class MarkStateRuleDialog : public wxDialog
{
  public:
    MarkStateRuleDialog( wxWindow* parent, Mediator* owner, ATermList svspec );
    ~MarkStateRuleDialog();
    void      onParameterChoice( wxCommandEvent& event );
    MarkRule* getMarkRule();
    wxString  getMarkRuleString();
    void      setMarkRule( MarkRule* mr, ATermList svspec );
  private:
    Mediator*			mediator;
    map< wxString, int >	parameterIndices;
    wxListBox*			parameterListBox;
    map< wxString, ATermAppl >	parameterTypes;
    wxListBox*			relationListBox;
    map< wxString, int >	valueIndices;
    wxCheckListBox*		valuesListBox;

    void loadValues( wxString paramName );

    DECLARE_EVENT_TABLE();
};

#endif
