#ifndef __xsim_H__
#define __xsim_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "xsim.h"
#endif

// Include wxWindows' headers

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/splitter.h>
#include <wx/listctrl.h>
#include "aterm2.h"

#include "xsimtrace.h"

//----------------------------------------------------------------------------
// XSimMain
//----------------------------------------------------------------------------

const int ID_SPLITTER = 10000;
const int ID_LISTCTRL1 = 10001;
const int ID_LISTCTRL2 = 10002;
const int ID_MENU = 10003;
const int ID_UNDO = 10004;
const int ID_REDO = 10005;
const int ID_RESET = 10006;
const int ID_FITCS = 10007;
const int ID_TRACE = 10008;

class XSimMain: public wxFrame, public SimulatorInterface
{
public:
    // constructors and destructors
    XSimMain( wxWindow *parent, wxWindowID id, const wxString &title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_FRAME_STYLE );
    void SetViews(wxListCtrl *state, wxListCtrl *trans);
    void LoadFile(wxString filename);
    void GotoTracePos(int pos);
    
private:
    // WDR: method declarations for XSimMain
    void CreateMenu();
    void CreateStatus();
    void CreateContent();
    void traceReset(ATermList state);
    void traceSetNext(ATermList transition);
    ATermList traceUndo();
    ATermList traceRedo();
    
private:
    // WDR: member variable declarations for XSimMain
    ATermList state_vars;
    ATermList initial_state;
    ATermList current_state;
    ATermList next_states;
    ATermList trace;
    ATermList ecart;
    wxMenuBar *menu;
    wxMenuItem *undo;
    wxMenuItem *redo;
    wxBoxSizer *mainsizer;
    wxSplitterWindow *split;
    wxPanel *toppanel;
    wxBoxSizer *topsizer;
    wxStaticBox *topbox;
    wxBoxSizer *topboxsizer;
    wxListView *stateview;
    wxPanel *bottompanel;
    wxBoxSizer *bottomsizer;
    wxStaticBox *bottombox;
    wxBoxSizer *bottomboxsizer;
    wxListView *transview;
    XSimTrace *tracewin;
    
private:
    // WDR: handler declarations for XSimMain
    void OnOpen( wxCommandEvent &event );
    void OnQuit( wxCommandEvent &event );
    void OnUndo( wxCommandEvent &event );
    void OnRedo( wxCommandEvent &event );
    void OnReset( wxCommandEvent &event );
    void OnFitCurrentState( wxCommandEvent &event );
    void OnTrace( wxCommandEvent &event );
    void OnAbout( wxCommandEvent &event );
    void OnTraceClose( wxCloseEvent &event );
    void OnCloseWindow( wxCloseEvent &event );
    void stateOnListItemSelected( wxListEvent &event );
    void transOnListItemActivated( wxListEvent &event );
    
private:
    void SetCurrentState(ATermList state, bool showchange = false);
    void UpdateTransitions();

private:
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// XSim
//----------------------------------------------------------------------------

class XSim: public wxApp
{
private:
    wxListCtrl *stateview;
    wxListCtrl *transview;

public:
    XSim();
    
    virtual bool OnInit();
    virtual int OnExit();
};

#endif
