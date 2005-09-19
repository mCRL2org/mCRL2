#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "proj.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <wx/filefn.h>
#include <wx/treectrl.h>
#include <dlfcn.h>
#include "proj.h"
#include "anal.h"
#include "tagdialog.h"
#include "aterm2.h"
#include "gslowlevel.h"
#include "gsfunc.h"
#include "common.h"

//------------------------------------------------------------------------------
// StudioProj
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(StudioProj,wxFrame)
    EVT_MENU(ID_NEW, StudioProj::OnNew)
    EVT_MENU(ID_OPEN, StudioProj::OnOpen)
    EVT_MENU(ID_QUIT, StudioProj::OnQuit)
    EVT_MENU(ID_ADD, StudioProj::OnAdd)
    EVT_MENU(ID_ADDANAL, StudioProj::OnAddAnal)
    EVT_MENU(ID_ABOUT, StudioProj::OnAbout)
    EVT_CLOSE(StudioProj::OnCloseWindow)
    EVT_LIST_ITEM_SELECTED(ID_FILELIST,StudioProj::fileOnListItemSelected)
    EVT_LIST_ITEM_ACTIVATED(ID_ANALLIST,StudioProj::analOnListItemActivated)
END_EVENT_TABLE()

StudioProj::StudioProj( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxFrame( parent, id, title, position, size, style )
{
    CreateMenu();
    CreateStatus();
    CreateContent();

    current_project = new gsProject();
}

StudioProj::~StudioProj()
{
}

gsObject *StudioProj::GetObject(wxWindow *parent)
{
	if ( fileview->GetItemCount() == 0 )
	{
		wxMessageDialog dialog(parent,wxT("There are no files in this project."),wxT("No files"),wxOK|wxICON_EXCLAMATION);
		dialog.ShowModal();
	} else {
		wxArrayString a;
		for (int i=0; i<fileview->GetItemCount(); i++)
		{
			a.Add(fileview->GetItemText(i));
		}
		wxSingleChoiceDialog dialog(parent,wxT("Please select an object:"),wxT("Object selection"),a);
		if ( (dialog.ShowModal() == wxID_OK) && (dialog.GetSelection() >= 0) )
		{
			return current_project->GetObject(toSTL(dialog.GetStringSelection()));
		}
	}

	return NULL;
}

bool StudioProj::AddObject(wxWindow *parent, gsObject *obj)
{
	return false;
}

void StudioProj::CreateMenu()
{
    menu = new wxMenuBar;

    wxMenu *file = new wxMenu;
    file->Append( ID_NEW, wxT("&New Project	CTRL-n"), wxT("") );
    file->Append( ID_OPEN, wxT("&Open Project...	CTRL-o"), wxT("") );
    file->Append( ID_SAVEAS, wxT("Save Project &As..."), wxT("") );
    file->Append( ID_SAVE, wxT("&Save Project	CTRL-s"), wxT("") );
    file->Append( ID_CLOSE, wxT("&Close Project"), wxT("") );
    file->AppendSeparator();
    file->Append( ID_QUIT, wxT("&Quit	CTRL-q"), wxT("") );
    menu->Append( file, wxT("&File") );

    wxMenu *edit = new wxMenu;
    edit->Append( ID_CUT, wxT("Cu&t	CTRL-x"), wxT("") );
    edit->Append( ID_COPY, wxT("&Copy	CTRL-c"), wxT("") );
    edit->Append( ID_PASTE, wxT("&Paste	CTRL-v"), wxT("") );
    menu->Append( edit, wxT("&Edit") );
    
    wxMenu *proj = new wxMenu;
    proj->Append( ID_ADD, wxT("&Add to Project..."), wxT("") );
    //proj->Append( wxID_ABOUT, wxT("&Remove from Project..."), wxT("") );
    proj->AppendSeparator();
    proj->Append( ID_ADDANAL, wxT("Add &New Analysis..."), wxT("") );
    proj->AppendSeparator();
    proj->Append( ID_OPTIONS, wxT("&Options..."), wxT("") );
    menu->Append( proj, wxT("&Project") );

    wxMenu *help = new wxMenu;
    help->Append( ID_ABOUT, wxT("&About"), wxT("") );
    menu->Append( help, wxT("&Help") );

    SetMenuBar( menu );
}

void StudioProj::CreateStatus()
{
    CreateStatusBar(1);
    SetStatusText(wxT(""));
}

void StudioProj::CreateContent()
{
	wxSplitterWindow *mainsplit = new wxSplitterWindow(this);
	wxPanel *filepanel = new wxPanel(mainsplit);
	wxPanel *analpanel = new wxPanel(mainsplit);
	wxBoxSizer *filesizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *analsizer = new wxBoxSizer(wxVERTICAL);
	wxStaticBox *filebox = new wxStaticBox(filepanel,-1,wxT("Files"));
	wxStaticBox *analbox = new wxStaticBox(analpanel,-1,wxT("Analyses"));
	wxStaticBoxSizer *fileboxsizer = new wxStaticBoxSizer(filebox,wxVERTICAL);
	wxStaticBoxSizer *analboxsizer = new wxStaticBoxSizer(analbox,wxVERTICAL);
	fileview = new wxListView(filepanel,ID_FILELIST,wxDefaultPosition,wxDefaultSize,wxLC_REPORT|wxSUNKEN_BORDER|wxLC_HRULES|wxLC_VRULES);
	analview = new wxListView(analpanel,ID_ANALLIST,wxDefaultPosition,wxDefaultSize,wxLC_REPORT|wxSUNKEN_BORDER|wxLC_HRULES|wxLC_VRULES);
	
	fileboxsizer->Add(fileview,1,wxGROW|wxALIGN_CENTER|wxALL,5);
	analboxsizer->Add(analview,1,wxGROW|wxALIGN_CENTER|wxALL,5);
	filesizer->Add(fileboxsizer,1,wxGROW|wxALIGN_CENTER|wxALL,5);
	analsizer->Add(analboxsizer,1,wxGROW|wxALIGN_CENTER|wxALL,5);
	filepanel->SetSizer(filesizer);
	analpanel->SetSizer(analsizer);

	mainsplit->SplitHorizontally(filepanel,analpanel);

	fileview->InsertColumn(0,wxT("Tag"));
	fileview->SetColumnWidth(0,70);
	fileview->InsertColumn(1,wxT("File"));
	fileview->SetColumnWidth(1,150);
	fileview->InsertColumn(2,wxT("Type"));
	fileview->SetColumnWidth(1,150);
	fileview->InsertColumn(3,wxT("Description"));
	fileview->SetColumnWidth(2,230);
	
/*	fileview->InsertItem(0,wxT("spec1"));
	fileview->SetItem(0,1,wxT("/home/karel/spec1.gs"));
	fileview->SetItem(0,2,wxT("mCRL2 Specification"));
	fileview->SetItem(0,3,wxT("Initial specification of system"));
	fileview->InsertItem(1,wxT("deadlockfree"));
	fileview->SetItem(1,1,wxT("/smb/forms/nodelta.gsf"));
	fileview->SetItem(1,2,wxT("Modal mu-calculus formula"));
	fileview->SetItem(1,3,wxT("Deadlock free check"));
	fileview->InsertItem(2,wxT("safe"));
	fileview->SetItem(2,1,wxT("/home/karel/safe.gsf"));
	fileview->SetItem(2,2,wxT("Modal mu-calculus formula"));
	fileview->SetItem(2,3,wxT("Check that b may never occur after a"));*/

	analview->InsertColumn(0,wxT("Tag"));
	analview->SetColumnWidth(0,70);
	analview->InsertColumn(1,wxT("Description"));
	analview->SetColumnWidth(1,380);

/*	analview->InsertItem(0,wxT("main"));
	analview->SetItem(0,1,wxT("Check for deadlock and instantiate"));*/
}

void StudioProj::NewProject()
{
	current_project = new gsProject();
}

void StudioProj::CloseProject()
{
	delete current_project;
	current_project = NULL;

	fileview->DeleteAllItems();
	analview->DeleteAllItems();
}

void StudioProj::AddObject(const wxString &tag, const wxString &filename, const wxString &description)
{
	current_project->AddObject(new gsObject(toSTL(tag),toSTL(filename),toSTL(description)));
	int i = fileview->InsertItem(fileview->GetItemCount(),tag);
	fileview->SetItem(i,1,filename);
	fileview->SetItem(i,3,description);
}

void StudioProj::AddAnalysis(const wxString &tag, const wxString &description)
{
	current_project->AddAnalysis(new gsAnalysis(toSTL(tag),toSTL(description)));
	int i = analview->InsertItem(analview->GetItemCount(),tag);
	analview->SetItem(i,1,description);
}

void StudioProj::OnNew( wxCommandEvent &event )
{
	if ( current_project != NULL )
	{
		wxMessageDialog dialog(this, wxT("Current project has not been saved. Continue?"), wxT("Project not saved"), wxYES_NO|wxNO_DEFAULT|wxICON_EXCLAMATION);
		if ( dialog.ShowModal() == wxID_NO )
		{
			return;
		}
		CloseProject();
	}

	NewProject();
}

void StudioProj::OnOpen( wxCommandEvent &event )
{
}

void StudioProj::OnQuit( wxCommandEvent &event )
{
     Close( TRUE );
}

void StudioProj::OnAdd( wxCommandEvent &event )
{
	wxFileDialog dialog( this, wxT("Select a file to add..."),wxT(""),wxT(""),wxT("Supported files|*.mcrl2;*.mcrl;*.lpe;*.tbf;*.aut;*.svc;*.bcg;*.fsm;*.mcf|Specifications (*.mcrl2,*.mcrl)|*.mcrl2;*.mcrl|Linear Processes (*.lpe,*.tbf)|*.lpe;*.tbf|State Spaces (*.aut,*.svc,*.bcg,*.fsm)|*.aut;*.svc;*.bcg;*.fsm|Modal Formulas (*.mcf)|*.mcf|All files|*.*"));
	if ( dialog.ShowModal() == wxID_OK )
	{
		wxString filename = dialog.GetPath();

		if ( !wxFileExists(filename) )
		{
			wxMessageDialog dialog2( this, wxT("File does not exist."), wxT("No such file"), wxOK|wxICON_EXCLAMATION );
			dialog2.ShowModal();
			return;
		}

		TagDialog dialog3(this,wxT("Please supply a tag and description for the selected file."),wxT("Set tag"));
		if ( dialog3.ShowModal() == wxID_OK )
		{
			AddObject(dialog3.GetTag(),filename,dialog3.GetDescription());
		}
	}
}

void StudioProj::OnAddAnal( wxCommandEvent &event )
{
	TagDialog dialog(this,wxT("Please supply a tag and description for the new analysis."),wxT("Set tag"));
	if ( dialog.ShowModal() == wxID_OK )
	{
		AddAnalysis(dialog.GetTag(),dialog.GetDescription());
	}
}

void StudioProj::OnAbout( wxCommandEvent &event )
{
    wxMessageDialog dialog( this, wxT("GenSpect GUI"),
        wxT("About XSim"), wxOK|wxICON_INFORMATION );
    dialog.ShowModal();
}

void StudioProj::OnCloseWindow( wxCloseEvent &event )
{
    Destroy();
}

void StudioProj::fileOnListItemSelected( wxListEvent &event )
{
}

void StudioProj::analOnListItemActivated( wxListEvent &event )
{
	wxString s(analview->GetItemText(event.GetIndex()));
	gsAnalysis *anal = current_project->GetAnalysis(toSTL(s));
	StudioAnal *ganal = new StudioAnal(this,anal,this);
	ganal->Show();
}

void StudioProj::LoadFile(wxString filename)
{
    FILE *f;
    
    if ( (f = fopen(filename.c_str(),"r")) == NULL )
    {
	    wxMessageDialog msg(this, wxT("Failed to open file."),
		wxT("Error"), wxOK|wxICON_ERROR);
	    msg.ShowModal();
	    return;
    }

}
