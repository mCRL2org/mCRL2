#ifndef __xsimtrace_H__
#define __xsimtrace_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "xsim.h"
#endif

// Include wxWindows' headers

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/listctrl.h>
#include "aterm2.h"

class SimulatorInterface
{
public:
	virtual void GotoTracePos(int pos) = 0;
};


//----------------------------------------------------------------------------
// XSimTrace
//----------------------------------------------------------------------------

class XSimTrace: public wxFrame
{
public:
    // constructors and destructors
    XSimTrace( wxWindow *parent, wxWindowID id, const wxString &title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_FRAME_STYLE );
    void SetSimulator(SimulatorInterface *Sim);
    void Reset(ATermList state);
    void SetNext(ATermList trans);
    void Undo();
    void Redo();
    void Goto(int pos);
    
private:
    // WDR: method declarations for XSimMain
    
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
