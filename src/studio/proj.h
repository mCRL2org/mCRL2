#ifndef __proj_H__
#define __proj_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "proj.h"
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

using namespace std;

#include <wx/splitter.h>
#include <wx/listctrl.h>
#include "aterm2.h"
#include "project.h"
#include "object.h"

//----------------------------------------------------------------------------
// StudioProj
//----------------------------------------------------------------------------

const int ID_FASPLITTER = 10000;
const int ID_FILELIST = 10001;
const int ID_ANALLIST = 10002;
const int ID_NEW = 10003;
const int ID_OPEN = 10004;
const int ID_SAVEAS = 10005;
const int ID_SAVE = 10006;
const int ID_CLOSE = 10007;
const int ID_QUIT = 10008;
const int ID_CUT = 10009;
const int ID_COPY = 10010;
const int ID_PASTE = 10011;
const int ID_ADD = 10012;
const int ID_ADDANAL = 10013;
const int ID_OPTIONS = 10014;
const int ID_ABOUT = 10015;

class StudioProj: public wxFrame, public gsObjectCollection
{
public:
    // constructors and destructors
    StudioProj( wxWindow *parent, wxWindowID id, const wxString &title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_FRAME_STYLE );
    ~StudioProj();
    void LoadFile(wxString filename);
    gsObject *GetObject(wxWindow *parent);
    bool AddObject(wxWindow *parent, gsObject *obj);
    
private:
    // WDR: method declarations for StudioProj
    void CreateMenu();
    void CreateStatus();
    void CreateContent();
    
private:
    // WDR: member variable declarations for StudioProj
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
    wxListView *fileview;
    wxListView *analview;
    gsProject *current_project;
    
private:
    void NewProject();
    void CloseProject();

    void AddObject(const wxString &tag, const wxString &filename, const wxString &description);
    void AddAnalysis(const wxString &tag, const wxString &description);

private:
    void OnNew( wxCommandEvent &event );
    void OnOpen( wxCommandEvent &event );
    void OnQuit( wxCommandEvent &event );
    void OnAdd( wxCommandEvent &event );
    void OnAddAnal( wxCommandEvent &event );
    void OnAbout( wxCommandEvent &event );
    void OnTraceClose( wxCloseEvent &event );
    void OnCloseWindow( wxCloseEvent &event );
    void fileOnListItemSelected( wxListEvent &event );
    void analOnListItemActivated( wxListEvent &event );
    
private:
    DECLARE_EVENT_TABLE()
};

#endif
