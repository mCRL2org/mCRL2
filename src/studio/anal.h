#ifndef __anal_H__
#define __anal_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "anal.h"
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

using namespace std;

#include <wx/splitter.h>
#include <wx/listctrl.h>
#include "wx/treectrl.h"
#include "aterm2.h"
#include "object.h"
#include "analysis.h"

//----------------------------------------------------------------------------
// StudioAnal
//----------------------------------------------------------------------------

const int aID_SPLITTER = 10000;
const int aID_LISTCTRL1 = 10001;
const int aID_LISTCTRL2 = 10002;
const int aID_MENU = 10003;
const int aID_UNDO = 10004;
const int aID_REDO = 10005;
const int aID_RESET = 10006;
const int aID_FITCS = 10007;
const int aID_TRACE = 10008;
const int aID_TRACELOAD = 10009;
const int ID_SWAP = 10040;
const int ID_ADDROOT = 10041;
const int ID_EXEC = 10042;

const int ID_TMPLIN = 10050;
const int ID_TMPSIM = 10051;

class StudioAnal: public wxFrame
{
public:
    // constructors and destructors
    StudioAnal( wxWindow *parent, gsAnalysis *analysis, gsObjectCollection *oc );
    ~StudioAnal();
    void LoadFile(wxString filename);
    
private:
    // WDR: method declarations for StudioAnal
    void CreateMenu();
    void CreateStatus();
    void CreateContent();
    
private:
    // WDR: member variable declarations for StudioAnal
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
    wxSplitterWindow *mainsplit;
    gsAnalysis *analysis;
    gsObjectCollection *objcol;
    wxTreeCtrl *tree;
    
private:
    // WDR: handler declarations for StudioAnal
    void OnOpen( wxCommandEvent &event );
    void OnQuit( wxCommandEvent &event );
    void OnSwap( wxCommandEvent &event );
    void OnAddRoot( wxCommandEvent &event );
    void OnTmpLin( wxCommandEvent &event );
    void OnTmpSim( wxCommandEvent &event );
    void OnTmpExec( wxCommandEvent &event );
    void OnAbout( wxCommandEvent &event );
    void OnTraceClose( wxCloseEvent &event );
    void OnCloseWindow( wxCloseEvent &event );
    void stateOnListItemSelected( wxListEvent &event );
    void transOnListItemActivated( wxListEvent &event );
    
private:
    DECLARE_EVENT_TABLE()
};

#endif
