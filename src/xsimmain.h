#ifndef __xsimmain_H__
#define __xsimmain_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "xsimmain.h"
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/splitter.h>
#include <wx/listctrl.h>
#include <aterm2.h>

#include "libgsrewrite.h"
#include "xsimbase.h"
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
const int ID_LOADVIEW = 10009;
const int ID_LOADTRACE = 10010;
const int ID_SAVETRACE = 10011;

class XSimMain: public wxFrame, public SimulatorInterface
{
public:
    // constructors and destructors
    XSimMain( wxWindow *parent, wxWindowID id, const wxString &title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_FRAME_STYLE );
    ~XSimMain();
    void SetViews(wxListCtrl *state, wxListCtrl *trans);
    void LoadFile(const wxString &filename);
    void LoadDLL(const wxString &filename);

    // XXX make private and use functions?
    bool use_dummies;
    RewriteStrategy rewr_strat;

    // SimulatorInterface methods
    virtual void Register(SimulatorViewInterface *View);
    virtual void Unregister(SimulatorViewInterface *View);
    virtual wxWindow *MainWindow();
    virtual ATermList GetParameters();
    virtual void Reset();
    virtual void Reset(ATerm State);
    virtual bool Undo();
    virtual bool Redo();
    virtual ATerm GetState();
    virtual ATermList GetNextStates();
    virtual bool ChooseTransition(int index);
    virtual int GetTraceLength();
    virtual int GetTracePos();
    virtual bool SetTracePos(int pos);
    virtual ATermList GetTrace();
    virtual bool SetTrace(ATermList Trace, int From);
    virtual void InitialiseViews();
    
private:
    // WDR: method declarations for XSimMain
    void CreateMenu();
    void CreateStatus();
    void CreateContent();
    void traceReset(ATerm state);
    void traceSetNext(ATermList transition);
    ATermList traceUndo();
    ATermList traceRedo();
    
private:
    // WDR: member variable declarations for XSimMain
    ATermList state_vars;
    ATermList state_varnames;
    ATerm initial_state;
    ATerm current_state;
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
    viewlist views;
    
private:
    // WDR: handler declarations for XSimMain
    void OnOpen( wxCommandEvent &event );
    void OnQuit( wxCommandEvent &event );
    void OnUndo( wxCommandEvent &event );
    void OnRedo( wxCommandEvent &event );
    void OnReset( wxCommandEvent &event );
    void OnLoadTrace( wxCommandEvent &event );
    void OnSaveTrace( wxCommandEvent &event );
    void OnFitCurrentState( wxCommandEvent &event );
    void OnTrace( wxCommandEvent &event );
    void OnLoadView( wxCommandEvent &event );
    void OnAbout( wxCommandEvent &event );
    void OnTraceClose( wxCloseEvent &event );
    void OnCloseWindow( wxCloseEvent &event );
    void stateOnListItemSelected( wxListEvent &event );
    void transOnListItemActivated( wxListEvent &event );
    
private:
    void SetCurrentState(ATerm state, bool showchange = false);
    void UpdateTransitions();

private:
    DECLARE_EVENT_TABLE()
};

#endif
