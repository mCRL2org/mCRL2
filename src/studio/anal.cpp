#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "anal.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <dlfcn.h>
#include "anal.h"
#include "aterm2.h"
#include "gslowlevel.h"
#include "gsfunc.h"
#include "lintool.h"
#include "simtool.h"

extern "C" void gsPrintPart(FILE *f, ATerm a, bool b, int c);
extern "C" void gsPrintParts(FILE *f, ATerm a, bool b, int c, char *s, char *t);

//------------------------------------------------------------------------------
// StudioAnal
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(StudioAnal,wxFrame)
    EVT_MENU(wxID_OPEN, StudioAnal::OnOpen)
    EVT_MENU(wxID_EXIT, StudioAnal::OnQuit)
    EVT_MENU(ID_SWAP, StudioAnal::OnSwap)
    EVT_MENU(ID_ADDROOT, StudioAnal::OnAddRoot)
    EVT_MENU(ID_TMPLIN, StudioAnal::OnTmpLin)
    EVT_MENU(ID_TMPSIM, StudioAnal::OnTmpSim)
    EVT_MENU(ID_EXEC, StudioAnal::OnTmpExec)
    EVT_MENU(wxID_ABOUT, StudioAnal::OnAbout)
    EVT_CLOSE(StudioAnal::OnCloseWindow)
    EVT_LIST_ITEM_SELECTED(aID_LISTCTRL1,StudioAnal::stateOnListItemSelected)
    EVT_LIST_ITEM_ACTIVATED(aID_LISTCTRL2,StudioAnal::transOnListItemActivated)
END_EVENT_TABLE()

StudioAnal::StudioAnal( wxWindow *parent, gsAnalysis *analysis, gsObjectCollection *oc ) :
    wxFrame( parent, -1,wxT("mCRL2 Analysis"), wxDefaultPosition, wxDefaultSize,
	     wxDEFAULT_FRAME_STYLE )
{
    CreateMenu();
    CreateStatus();
    CreateContent();

    this->analysis = analysis;
    objcol = oc;

    analysis->LinkTreeView(tree);
}

StudioAnal::~StudioAnal()
{
	tree->DeleteAllItems();
}

void StudioAnal::CreateMenu()
{
    menu = new wxMenuBar;

    wxMenu *file = new wxMenu;
    file->Append( wxID_OPEN, wxT("&Open...	CTRL-o"), wxT("") );
    file->AppendSeparator();
    file->Append( wxID_EXIT, wxT("&Quit	CTRL-q"), wxT("") );
    menu->Append( file, wxT("&File") );

    wxMenu *edit = new wxMenu;
    edit->Append( ID_SWAP, wxT("&Swap Splitter	CTRL-s"), wxT("") );
    menu->Append( edit, wxT("&Edit") );
    
    wxMenu *anal = new wxMenu;
    anal->Append( ID_ADDROOT, wxT("&Add initial object..."), wxT("") );
    anal->AppendSeparator();
    anal->Append( ID_TMPLIN, wxT("&Linearise..."), wxT("") );
    anal->Append( ID_TMPSIM, wxT("&Simulate..."), wxT("") );
    anal->AppendSeparator();
    anal->Append( ID_EXEC, wxT("&Execute..."), wxT("") );
    menu->Append( anal, wxT("&Analysis") );


    wxMenu *help = new wxMenu;
    help->Append( wxID_ABOUT, wxT("&About"), wxT("") );
    menu->Append( help, wxT("&Help") );

    SetMenuBar( menu );
}

void StudioAnal::CreateStatus()
{
    CreateStatusBar(1);
    SetStatusText(wxT(""));
}

void StudioAnal::CreateContent()
{
	mainsplit = new wxSplitterWindow(this,-1,wxDefaultPosition,wxDefaultSize,wxSP_NOBORDER);
	tree = new wxTreeCtrl(mainsplit);
	wxSplitterWindow *split1 = new wxSplitterWindow(mainsplit,-1,wxDefaultPosition,wxDefaultSize,wxSP_NOBORDER);

	mainsplit->SplitVertically(tree,split1);
//	mainsplit->SplitHorizontally(tree,split1);

	wxPanel *panel1 = new wxPanel(split1);
	wxStaticBox *box1 = new wxStaticBox(panel1,-1,wxT("gsinstantiate spec1.lpe"),wxDefaultPosition,wxDefaultSize,wxTHICK_FRAME);
	wxSplitterWindow *split2 = new wxSplitterWindow(split1,-1,wxDefaultPosition,wxDefaultSize,wxSP_NOBORDER);
	wxBoxSizer *sizer1 = new wxBoxSizer(wxVERTICAL);
	wxStaticBoxSizer *sizer2 = new wxStaticBoxSizer(box1,wxVERTICAL);
	sizer1->Add(sizer2,1,wxGROW|wxALIGN_CENTER|wxALL,5);
	panel1->SetSizer(sizer1);

	split1->SplitHorizontally(panel1,split2);

	wxPanel *panel2 = new wxPanel(split2);
	wxStaticBox *box2 = new wxStaticBox(panel2,-1,wxT("gscheck spec1.lpe deadlockfree"),wxDefaultPosition,wxDefaultSize,wxTHICK_FRAME);
	wxSplitterWindow *split3 = new wxSplitterWindow(split2,-1,wxDefaultPosition,wxDefaultSize,wxSP_NOBORDER);
	wxBoxSizer *sizer3 = new wxBoxSizer(wxVERTICAL);
	wxStaticBoxSizer *sizer4 = new wxStaticBoxSizer(box2,wxVERTICAL);
	sizer3->Add(sizer4,1,wxGROW|wxALIGN_CENTER|wxALL,5);
	panel2->SetSizer(sizer3);

	split2->SplitHorizontally(panel2,split3);

/*	wxTreeItemId item1 = tree->InsertItem((wxTreeItemId *) NULL,0,wxT("spec1"));
	wxTreeItemId item2 = tree->InsertItem(item1,0,wxT("gslinearise -regular"));
	wxTreeItemId item3 = tree->InsertItem(item2,0,wxT("gsinstantiate -svc-term"));
	wxTreeItemId item4 = tree->InsertItem(item2,1,wxT("check deadlockfree"));
	tree->Expand(item1);
	tree->Expand(item2);
	tree->Expand(item3);
	tree->Expand(item4);*/

	mainsplit->SetMinimumPaneSize(50);
	mainsplit->SetSashGravity(0.5);
	split1->SetMinimumPaneSize(50);
	split1->SetSashGravity(1.0/3.0);
	split2->SetMinimumPaneSize(50);
	split2->SetSashGravity(0.5);
	split3->SetMinimumPaneSize(50);
	split3->SetSashGravity(0.5);
	
	wxSize s(240,200);
	SetMinSize(s);
}

void StudioAnal::OnOpen( wxCommandEvent &event )
{
    wxFileDialog dialog( this, wxT("Select a LPE file..."));
    if ( dialog.ShowModal() == wxID_OK )
    {
	    LoadFile(dialog.GetFilename());
    }
}

void StudioAnal::OnQuit( wxCommandEvent &event )
{
     Close( TRUE );
}

void StudioAnal::OnSwap( wxCommandEvent &event )
{
	if ( mainsplit->GetSplitMode() == wxSPLIT_VERTICAL )
	{
		mainsplit->SetSplitMode(wxSPLIT_HORIZONTAL);
	} else {
		mainsplit->SetSplitMode(wxSPLIT_VERTICAL);
	}
	mainsplit->SetSashPosition(mainsplit->GetSashPosition());
}

void StudioAnal::OnAddRoot( wxCommandEvent &event )
{
	gsObject *obj;

	if ( (obj = objcol->GetObject(this)) != NULL )
	{
		analysis->AddRoot(obj);
	}
}

void StudioAnal::OnTmpLin( wxCommandEvent &event )
{
	if ( tree->GetSelection().IsOk() )
	{
		analysis->AddTool(new LineariseTool);
	}
}

void StudioAnal::OnTmpSim( wxCommandEvent &event )
{
	if ( tree->GetSelection().IsOk() )
	{
		analysis->AddTool(new SimulatorTool);
	}
}

void StudioAnal::OnTmpExec( wxCommandEvent &event )
{
	if ( tree->GetSelection().IsOk() )
	{
		analysis->RunTool();
	}
}

void StudioAnal::OnAbout( wxCommandEvent &event )
{
    wxMessageDialog dialog( this, wxT("mCRL2 Studio"),
        wxT("About XSim"), wxOK|wxICON_INFORMATION );
    dialog.ShowModal();
}

void StudioAnal::OnCloseWindow( wxCloseEvent &event )
{
    Destroy();
}

void StudioAnal::stateOnListItemSelected( wxListEvent &event )
{
	stateview->Select(event.GetIndex(),FALSE);
}

void StudioAnal::transOnListItemActivated( wxListEvent &event )
{
}

void StudioAnal::LoadFile(wxString filename)
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
