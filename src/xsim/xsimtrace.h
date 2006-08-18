#ifndef __xsimtrace_H__
#define __xsimtrace_H__

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <aterm2.h>
#include "xsimbase.h"

//----------------------------------------------------------------------------
// XSimTrace
//----------------------------------------------------------------------------

class XSimTrace: public wxFrame, public SimulatorViewInterface
{
public:
    // constructors and destructors
    XSimTrace( wxWindow *parent );
/*    XSimTrace( wxWindow *parent, wxWindowID id, const wxString &title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_FRAME_STYLE ) : XSimTrace(parent);*/

    // SimulatorViewInterface
    virtual void Registered(SimulatorInterface *Simulator);
    virtual void Unregistered();
    virtual void Initialise(ATermList Pars);
    virtual void StateChanged(ATermAppl Transition, ATerm State, ATermList NextStates);
    virtual void Reset(ATerm State);
    virtual void Undo(int Count);
    virtual void Redo(int Count);
    virtual void TraceChanged(ATermList Trace, int From);
    virtual void TracePosChanged(ATermAppl Transition, ATerm State, int Index);

private:
    // WDR: method declarations for XSimMain
    void AddState(ATermAppl Transition, ATerm State, bool enabled);
    
private:
    // WDR: member variable declarations for XSimMain
    wxListView *traceview;
    SimulatorInterface *simulator;
    int current_pos;
    
private:
    // WDR: handler declarations for XSimMain
    void OnCloseWindow( wxCloseEvent &event );
    void OnListItemActivated( wxListEvent &event );
    
private:

private:
    DECLARE_EVENT_TABLE()
};

#endif
