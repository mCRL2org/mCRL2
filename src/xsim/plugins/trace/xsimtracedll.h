#ifndef __xsimtracedll_H__
#define __xsimtracedll_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "xsimtracedll.h"
#endif

// Include wxWindows' headers

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/listctrl.h>
#include <aterm2.h>
#include "xsimbase.h"

//----------------------------------------------------------------------------
// XSimTraceDLL
//----------------------------------------------------------------------------

class XSimTraceDLL: public wxFrame, public SimulatorViewDLLInterface
{
public:
    // constructors and destructors
    XSimTraceDLL( wxWindow *parent );
/*    XSimTraceDLL( wxWindow *parent, wxWindowID id, const wxString &title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_FRAME_STYLE ) : XSimTraceDLL(parent);*/

    // SimulatorViewInterface
    virtual void Registered(SimulatorInterface *Simulator);
    virtual void Unregistered();
    virtual void Initialise(ATermList Pars);
    virtual void StateChanged(ATermAppl Transition, ATerm State, ATermList NextStates);
    virtual void Reset(ATerm State);
    virtual void Undo(unsigned int Count);
    virtual void Redo(unsigned int Count);
    virtual void TraceChanged(ATermList Trace, unsigned int From);
    virtual void TracePosChanged(ATermAppl Transition, ATerm State, unsigned int Index);

private:
    // WDR: method declarations for XSimMain
    void AddState(ATermAppl Transition, ATerm State, bool enabled);
    
private:
    // WDR: member variable declarations for XSimMain
    wxListView *traceview;
    SimulatorInterface *simulator;
    unsigned int current_pos;
    
private:
    // WDR: handler declarations for XSimMain
    void OnCloseWindow( wxCloseEvent &event );
    void OnListItemActivated( wxListEvent &event );
    
private:

private:
    DECLARE_EVENT_TABLE()
};

#endif
