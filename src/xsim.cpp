#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "xsim.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "xsim.h"
#include "aterm2.h"
#include "gslowlevel.h"
#include "gsfunc.h"
#include "libgsnextstate.h"

extern "C" void gsPrintPart(FILE *f, ATerm a, int b, int c);
extern "C" void gsPrintParts(FILE *f, ATerm a, int b, int c, char *s, char *t);

//------------------------------------------------------------------------------
// XSimMain
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(XSimMain,wxFrame)
    EVT_MENU(wxID_OPEN, XSimMain::OnOpen)
    EVT_MENU(wxID_EXIT, XSimMain::OnQuit)
    EVT_MENU(ID_UNDO, XSimMain::OnUndo)
    EVT_MENU(ID_REDO, XSimMain::OnRedo)
    EVT_MENU(ID_RESET, XSimMain::OnReset)
    EVT_MENU(ID_FITCS, XSimMain::OnFitCurrentState)
    EVT_MENU(ID_TRACE, XSimMain::OnTrace)
    EVT_MENU(wxID_ABOUT, XSimMain::OnAbout)
    EVT_CLOSE(XSimMain::OnCloseWindow)
    EVT_LIST_ITEM_SELECTED(ID_LISTCTRL1,XSimMain::stateOnListItemSelected)
    EVT_LIST_ITEM_ACTIVATED(ID_LISTCTRL2,XSimMain::transOnListItemActivated)
END_EVENT_TABLE()

XSimMain::XSimMain( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxFrame( parent, id, title, position, size, style )
{
    // XXX Easy hack... should be separated from XSimMain
    tracewin = new XSimTrace(this,-1,wxT("XSim Trace"),wxDefaultPosition,wxSize(300,400));
    tracewin->SetSimulator(this);
    //tracewin->Show(FALSE); // default, so not needed

    CreateMenu();
    CreateStatus();
    CreateContent();

    ATprotectList(&state_vars);
    ATprotectList(&initial_state);
    ATprotectList(&current_state);
    ATprotectList(&next_states);
    ATprotectList(&trace);
    ATprotectList(&ecart);

    state_vars = ATmakeList0();
    initial_state = ATmakeList0();
    current_state = ATmakeList0();
    next_states = ATmakeList0();
    trace = ATmakeList0();
    ecart = ATmakeList0();
}

void XSimMain::CreateMenu()
{
    menu = new wxMenuBar;

    wxMenu *file = new wxMenu;
    file->Append( wxID_OPEN, wxT("&Open...	ALT-o"), wxT("") );
    file->AppendSeparator();
    file->Append( wxID_EXIT, wxT("&Quit	ALT-q"), wxT("") );
    menu->Append( file, wxT("&File") );

    wxMenu *edit = new wxMenu;
    undo = edit->Append( ID_UNDO, wxT("&Undo	ALT-LEFT"), wxT("") );
    undo->Enable(false);
    redo = edit->Append( ID_REDO, wxT("Re&do	ALT-RIGHT"), wxT("") );
    redo->Enable(false);
    edit->Append( ID_RESET, wxT("&Reset	ALT-r"), wxT("") );
    edit->AppendSeparator();
    edit->Append( ID_MENU, wxT("&Load trace...	ALT-l"), wxT("") )->Enable(false);
    edit->Append( ID_MENU, wxT("&Save trace...	ALT-s"), wxT("") )->Enable(false);
    edit->AppendSeparator();
    edit->Append( ID_FITCS, wxT("F&it to Current State	F3"), wxT("") );
    menu->Append( edit, wxT("&Edit") );
    
    wxMenu *views = new wxMenu;
    views->Append( ID_TRACE, wxT("&Trace"), wxT("") );
    views->Append( ID_MENU, wxT("&Graph"), wxT("") )->Enable(false);
    menu->Append( views, wxT("&Views") );
    
    wxMenu *help = new wxMenu;
    help->Append( wxID_ABOUT, wxT("&About"), wxT("") );
    menu->Append( help, wxT("&Help") );

    SetMenuBar( menu );
}

void XSimMain::CreateStatus()
{
    CreateStatusBar(1);
    SetStatusText(wxT(""));
}

void XSimMain::CreateContent()
{
    mainsizer = new wxBoxSizer(wxVERTICAL);
    split = new wxSplitterWindow(this,ID_SPLITTER,wxDefaultPosition,wxSize(0,0),wxCLIP_CHILDREN);

    toppanel = new wxPanel(split,-1);
    topsizer = new wxBoxSizer(wxVERTICAL);
    topbox = new wxStaticBox(toppanel,-1,wxT("Current State"));
    topboxsizer = new wxStaticBoxSizer(topbox,wxVERTICAL);
    stateview = new wxListView(toppanel,ID_LISTCTRL1,wxDefaultPosition,wxSize(0,0),wxLC_REPORT|wxSUNKEN_BORDER|wxLC_HRULES|wxLC_VRULES|wxLC_SINGLE_SEL);
    topboxsizer->Add(stateview,1,wxGROW|wxALIGN_CENTER|wxALL,5);
    topsizer->Add(topboxsizer,1,wxGROW|wxALIGN_CENTER|wxALL,5);
    toppanel->SetSizer(topsizer);

    bottompanel = new wxPanel(split,-1);
    bottomsizer = new wxBoxSizer(wxVERTICAL);
    bottombox = new wxStaticBox(bottompanel,-1,wxT("Transitions"));
    bottomboxsizer = new wxStaticBoxSizer(bottombox,wxVERTICAL);
    transview = new wxListView(bottompanel,ID_LISTCTRL2,wxDefaultPosition,wxSize(0,0),wxLC_REPORT|wxSUNKEN_BORDER|wxLC_HRULES|wxLC_VRULES|wxLC_SINGLE_SEL);
    bottomboxsizer->Add(transview,1,wxGROW|wxALIGN_CENTER|wxALL,5);
    bottomsizer->Add(bottomboxsizer,1,wxGROW|wxALIGN_CENTER|wxALL,5);
    bottompanel->SetSizer(bottomsizer);

    split->SplitHorizontally(toppanel,bottompanel);
    split->SetMinimumPaneSize(27);
    mainsizer->Add(split,1,wxALIGN_CENTER|wxALL,5);
    
    wxSize s(240,160);
    SetMinSize(s);

    stateview->InsertColumn(0,wxT("Parameter"));
    stateview->SetColumnWidth(0,120);
    stateview->InsertColumn(1,wxT("Value"));
    stateview->SetColumnWidth(1,120);

    transview->InsertColumn(0,wxT("Action"));
    transview->SetColumnWidth(0,120);
    transview->InsertColumn(1,wxT("State Change"));
    transview->SetColumnWidth(1,120);
    transview->SetFocus();
}

void XSimMain::traceReset(ATermList state)
{
	trace = ATmakeList1((ATerm) ATmakeList2((ATerm) gsMakeNil(),(ATerm) state));
	ecart = ATmakeList0();
	undo->Enable(false);
	redo->Enable(false);
	tracewin->Reset(state);
}

void XSimMain::traceSetNext(ATermList transition)
{
	trace = ATinsert(trace,(ATerm) transition);
	ecart = ATmakeList0();
	undo->Enable();
	redo->Enable(false);
	tracewin->SetNext(transition);
}

ATermList XSimMain::traceUndo()
{
	ecart = ATinsert(ecart,ATgetFirst(trace));
	trace = ATgetNext(trace);
	if ( ATisEmpty(ATgetNext(trace)) )
	{
		undo->Enable(false);
	}
	redo->Enable();

	tracewin->Undo();

	return ATLgetFirst(trace);
}

ATermList XSimMain::traceRedo()
{
	trace = ATinsert(trace,ATgetFirst(ecart));
	ecart = ATgetNext(ecart);
	if ( ATisEmpty(ecart) )
	{
		redo->Enable(false);
	}
	undo->Enable();

	tracewin->Redo();


	return ATLgetFirst(trace);
}

void XSimMain::OnOpen( wxCommandEvent &event )
{
    wxFileDialog dialog( this, wxT("Select a LPE file..."));
    if ( dialog.ShowModal() == wxID_OK )
    {
	    LoadFile(dialog.GetFilename());
    }
}

void XSimMain::OnQuit( wxCommandEvent &event )
{
     Close( TRUE );
}

void XSimMain::OnUndo( wxCommandEvent &event )
{
	ATermList trans = traceUndo();
	SetCurrentState(ATLgetFirst(ATgetNext(trans)));
	UpdateTransitions();
}

void XSimMain::OnRedo( wxCommandEvent &event )
{
	ATermList trans = traceRedo();
	SetCurrentState(ATLgetFirst(ATgetNext(trans)));
	UpdateTransitions();
}

void XSimMain::OnReset( wxCommandEvent &event )
{
	if ( !ATisEmpty(trace) )
	{
		traceReset(initial_state);
		SetCurrentState(initial_state);
		UpdateTransitions();
	}
}

void XSimMain::OnFitCurrentState( wxCommandEvent &event )
{
    int w,h,n;

    n = stateview->GetViewRect().GetHeight()+stateview->m_headerHeight;
    stateview->GetClientSize(&w,&h);
    split->SetSashPosition(split->GetSashPosition()+n-h);
}

void XSimMain::OnTrace( wxCommandEvent &event )
{
    tracewin->Show(!tracewin->IsShown());
}

void XSimMain::OnAbout( wxCommandEvent &event )
{
    wxMessageDialog dialog( this, wxT("GenSpect Simulator GUI"),
        wxT("About XSim"), wxOK|wxICON_INFORMATION );
    dialog.ShowModal();
}

void XSimMain::OnTraceClose( wxCloseEvent &event )
{
    if ( event.CanVeto() )
    {
	    ((wxFrame *) event.GetEventObject())->Show(FALSE);
	    event.Veto();
    } else {
	    Destroy();
    }
}

void XSimMain::OnCloseWindow( wxCloseEvent &event )
{
    Destroy();
}

void XSimMain::stateOnListItemSelected( wxListEvent &event )
{
	stateview->Select(event.GetIndex(),FALSE);
}

void XSimMain::transOnListItemActivated( wxListEvent &event )
{
	ATermList l = ATLelementAt(next_states,event.GetIndex());

	traceSetNext(l);
	SetCurrentState(ATLgetFirst(ATgetNext(l)),true);
	UpdateTransitions();
}

void XSimMain::LoadFile(wxString filename)
{
    FILE *f;
    
    if ( (f = fopen(filename.c_str(),"r")) == NULL )
    {
	    wxMessageDialog msg(this, wxT("Failed to open file."),
		wxT("Error"), wxOK);
	    msg.ShowModal();
	    return;
    }

    ATermAppl Spec = (ATermAppl) ATreadFromFile(f);
    fclose(f);

    ATermList l = ATLgetArgument(ATAgetArgument(Spec,5),1);
    ATermList m = ATmakeList0();
    stateview->DeleteAllItems();
    for (int i=0; !ATisEmpty(l); l=ATgetNext(l), i++)
    {
	    stateview->InsertItem(i,wxT(ATgetName(ATgetAFun(ATAgetArgument(ATAgetFirst(l),0)))));
	    m = ATinsert(m,ATgetArgument(ATAgetFirst(l),0));
    }
    state_vars = ATreverse(m);
    initial_state = gsNextStateInit(Spec);
    traceReset(initial_state);
    SetCurrentState(initial_state);
    UpdateTransitions();
}

void XSimMain::SetCurrentState(ATermList state, bool showchange)
{
	char s[1000];
	FILE *f;
	ATermList old;

	if ( (current_state == NULL) || (ATgetLength(current_state) != ATgetLength(state)) )
	{
		old = state;
	} else {
		old = current_state;
	}
	current_state = state;

	for (int i=0; !ATisEmpty(state); state=ATgetNext(state), old=ATgetNext(old), i++)
	{
		f = fopen("xsim.tmp","w+");
		gsPrintPart(f,ATgetFirst(state),0,0);
		rewind(f);
		fgets(s,1000,f);
		fclose(f);
		stateview->SetItem(i,1,wxT(s));
		if ( showchange && !ATisEqual(ATgetFirst(state),ATgetFirst(old)) )
		{
		        wxColour col(255,255,210);
		        stateview->SetItemBackgroundColour(i,col);
		} else {
		        wxColour col(255,255,255); // XXX is this the correct colour?
		        stateview->SetItemBackgroundColour(i,col);
		}
	}
	stateview->SetColumnWidth(1,wxLIST_AUTOSIZE);
}

void XSimMain::UpdateTransitions()
{
	char s[1000];
	FILE *f;

	next_states = gsNextState(current_state);

	transview->DeleteAllItems();
	int i = 0;
	for (ATermList l=next_states; !ATisEmpty(l); l=ATgetNext(l), i++)
	{
		f = fopen("xsim.tmp","w+");
		gsPrintPart(f,ATgetFirst(ATLgetFirst(l)),0,0);
		rewind(f);
		fgets(s,1000,f);
		fclose(f);
		transview->InsertItem(i,wxT(s));
		f = fopen("xsim.tmp","w+");
		ATermList m = current_state;
		ATermList n = ATLgetFirst(ATgetNext(ATLgetFirst(l)));
		ATermList o = state_vars;
		bool comma = false;
		for (; !ATisEmpty(n); n=ATgetNext(n),m=ATgetNext(m),o=ATgetNext(o))
		{
			if ( !ATisEqual(ATgetFirst(n),ATgetFirst(m)) )
			{
				if ( comma )
				{
					fprintf(f,", ");
				} else {
					comma = true;
				}
				gsPrintPart(f,ATgetFirst(o),0,0);
				fprintf(f," := ");
				gsPrintPart(f,ATgetFirst(n),0,0);
			}
		}
		rewind(f);
		fgets(s,1000,f);
		fclose(f);
		transview->SetItem(i,1,wxT(s));
	}

	if ( !ATisEmpty(next_states) )
	{
		transview->Select(0);
	}
	transview->SetColumnWidth(1,wxLIST_AUTOSIZE);
}

void XSimMain::GotoTracePos(int pos)
{
	int l = ATgetLength(trace)-1;

	while ( l < pos )
	{
		trace = ATinsert(trace,ATgetFirst(ecart));
		ecart = ATgetNext(ecart);
		l++;
	}
	while ( l > pos )
	{
		ecart = ATinsert(ecart,ATgetFirst(trace));
		trace = ATgetNext(trace);
		l--;
	}
	SetCurrentState(ATLgetFirst(ATgetNext(ATLgetFirst(trace))));
	UpdateTransitions();
	tracewin->Goto(pos);
}

//------------------------------------------------------------------------------
// XSim
//------------------------------------------------------------------------------

IMPLEMENT_APP_NO_MAIN(XSim)
IMPLEMENT_WX_THEME_SUPPORT

XSim::XSim()
{
}

bool XSim::OnInit()
{
//    ATinit(0,0,(ATerm *)0xbffffffc); // XXX Awful hack

    XSimMain *frame = new XSimMain( NULL, -1, wxT("XSim"), wxPoint(-1,-1), wxSize(500,400) );
    frame->Show( TRUE );
    
    gsEnableConstructorFunctions();

    if ( argc > 1 )
    {
	frame->LoadFile(wxT(argv[1]));
    }

    return TRUE;
}

int XSim::OnExit()
{
    return 0;
}

int main(int argc, char **argv)
{
	ATerm bot;

	ATinit(argc,argv,&bot);

	return wxEntry(argc, argv);
}
