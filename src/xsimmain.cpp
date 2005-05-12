#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "xsimmain.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <dlfcn.h>
#include "xsimbase.h"
#include "xsimmain.h"
#include "aterm2.h"
#include "gslowlevel.h"
#include "gsfunc.h"
#include "libgsnextstate.h"

extern "C" void gsPrintPart(FILE *f, ATerm a, bool b, int c);
extern "C" void gsPrintParts(FILE *f, ATerm a, bool b, int c, char *s, char *t);

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
    EVT_MENU(ID_TRACELOAD, XSimMain::OnTraceLoad)
    EVT_MENU(wxID_ABOUT, XSimMain::OnAbout)
    EVT_CLOSE(XSimMain::OnCloseWindow)
    EVT_LIST_ITEM_SELECTED(ID_LISTCTRL1,XSimMain::stateOnListItemSelected)
    EVT_LIST_ITEM_ACTIVATED(ID_LISTCTRL2,XSimMain::transOnListItemActivated)
END_EVENT_TABLE()

XSimMain::XSimMain( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxFrame( parent, id, title, position, size, style )
{
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
    
    tracewin = new XSimTrace(this);
    Register(tracewin);
    //tracewin->Show(FALSE); // default, so not needed
}

XSimMain::~XSimMain()
{
	for (viewlist::iterator i = views.begin(); i != views.end(); i++)
	{
		(*i)->Unregistered();
	}
	
	delete tracewin;

	ATunprotectList(&state_vars);
	ATunprotectList(&initial_state);
	ATunprotectList(&current_state);
	ATunprotectList(&next_states);
	ATunprotectList(&trace);
	ATunprotectList(&ecart);
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
    views->Append( ID_TRACELOAD, wxT("&Load Dynamic..."), wxT("") );
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


void XSimMain::Register(SimulatorViewInterface *View)
{
	views.push_back(View);
	View->Registered(this);
	View->TraceChanged(GetTrace(),0);
	if ( !ATisEmpty(trace) )
	{
		View->TracePosChanged(ATAgetFirst(ATLgetFirst(trace)),current_state,ATgetLength(trace)-1);
	}
}

void XSimMain::Unregister(SimulatorViewInterface *View)
{
	views.remove(View);
	View->Unregistered();
}

wxWindow *XSimMain::MainWindow()
{
	return this;
}

void XSimMain::Reset()
{
	if ( initial_state != NULL )
	{
		traceReset(initial_state);
		SetCurrentState(initial_state);
		UpdateTransitions();

		for (viewlist::iterator i = views.begin(); i != views.end(); i++)
		{
			(*i)->Reset(initial_state);
			(*i)->StateChanged(NULL,initial_state,next_states);
		}

		undo->Enable(false);
		redo->Enable(false);
	}
}

void XSimMain::Reset(ATermList State)
{
	initial_state = State;
	Reset();
}

bool XSimMain::Undo()
{
	if ( ATgetLength(trace) > 1 )
	{
		ATermList l = traceUndo();
		ATermList state = ATLgetFirst(ATgetNext(l));

		SetCurrentState(state);
		UpdateTransitions();
		
		for (viewlist::iterator i = views.begin(); i != views.end(); i++)
		{
			(*i)->Undo(1);
			(*i)->StateChanged(NULL,state,next_states);
		}

		if ( ATisEmpty(ATgetNext(trace)) )
		{
			undo->Enable(false);
		}
		redo->Enable();

		return true;
	} else {
		return false;
	}
}

bool XSimMain::Redo()
{
	if ( !ATisEmpty(ecart) )
	{
		ATermList trans = traceRedo();
		ATermList state = ATLgetFirst(ATgetNext(trans));

		SetCurrentState(state);
		UpdateTransitions();

		for (viewlist::iterator i = views.begin(); i != views.end(); i++)
		{
			(*i)->Redo(1);
			(*i)->StateChanged(NULL,state,next_states);
		}

		if ( ATisEmpty(ecart) )
		{
			redo->Enable(false);
		}
		undo->Enable();

		return true;
	} else {
		return false;
	}
}

ATermList XSimMain::GetState()
{
	return current_state;
}

ATermList XSimMain::GetNextStates()
{
	return next_states;
}

bool XSimMain::ChooseTransition(int index)
{
	if ( !ATisEmpty(next_states) && (index < ATgetLength(next_states)) )
	{
		ATermList l = ATLelementAt(next_states,index);
		ATermAppl trans = ATAgetFirst(l);
		ATermList state = ATLgetFirst(ATgetNext(l));

		SetCurrentState(state,true);
		UpdateTransitions();

		traceSetNext(l);

		for (viewlist::iterator i = views.begin(); i != views.end(); i++)
		{
			(*i)->StateChanged(trans,state,next_states);
		}

		undo->Enable();
		redo->Enable(false);

		return true;
	} else {
		return false;
	}
}

int XSimMain::GetTraceLength()
{
	return ATgetLength(trace)+ATgetLength(ecart);
}

int XSimMain::GetTracePos()
{
	return ATgetLength(trace)-1;
}

bool XSimMain::SetTracePos(int pos)
{
	int l = ATgetLength(trace)-1;
	ATermAppl trans;
	ATermList state;

	if ( (l >= 0) && (pos <= l+ATgetLength(ecart)) )
	{
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

		trans = ATAgetFirst(ATLgetFirst(trace));
		state = ATLgetFirst(ATgetNext(ATLgetFirst(trace)));

		SetCurrentState(state);
		UpdateTransitions();

		for (viewlist::iterator i = views.begin(); i != views.end(); i++)
		{
			(*i)->TracePosChanged(trans,state,pos);
			(*i)->StateChanged(NULL,state,next_states);
		}

		undo->Enable(!ATisEmpty(ATgetNext(trace)));
		redo->Enable(!ATisEmpty(ecart));

		return true;
	} else {
		return false;
	}
}

ATermList XSimMain::GetTrace()
{
	ATermList l = ecart;
	ATermList m = trace;

	for (; !ATisEmpty(m); m=ATgetNext(m))
	{
		l = ATinsert(l,ATgetFirst(m));
	}

	return l;
}

bool XSimMain::SetTrace(ATermList Trace, int From)
{
	// XXX
}



void XSimMain::traceReset(ATermList state)
{
	trace = ATmakeList1((ATerm) ATmakeList2((ATerm) gsMakeNil(),(ATerm) state));
	ecart = ATmakeList0();
}

void XSimMain::traceSetNext(ATermList transition)
{
	trace = ATinsert(trace,(ATerm) transition);
	ecart = ATmakeList0();
}

ATermList XSimMain::traceUndo()
{
	ecart = ATinsert(ecart,ATgetFirst(trace));
	trace = ATgetNext(trace);

	return ATLgetFirst(trace);
}

ATermList XSimMain::traceRedo()
{
	trace = ATinsert(trace,ATgetFirst(ecart));
	ecart = ATgetNext(ecart);

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
	Undo();
}

void XSimMain::OnRedo( wxCommandEvent &event )
{
	Redo();
}

void XSimMain::OnReset( wxCommandEvent &event )
{
	Reset();
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

void XSimMain::OnTraceLoad( wxCommandEvent &event )
{
    wxFileDialog dialog( this, wxT("Select a View DLL..."), wxT(""), wxT(""), wxT("Shared Object Files (*.so)|*.so|All Files|*.*"));
    if ( dialog.ShowModal() == wxID_OK )
    {
	    void *h;

	    h = dlopen(dialog.GetFilename().c_str(),RTLD_LAZY);
	    if ( h != NULL )
	    {
		    void (*f)(SimulatorInterface *);

		    f = (void (*)(SimulatorInterface *))dlsym(h,"SimulatorViewDLLAddView");
		    if ( f != NULL )
		    {
		    	f(this);
		    } else {
			    wxMessageDialog msg(this, wxT("DLL does not appear to contain a View."), wxT("Error"), wxOK);
			    msg.ShowModal();
		    }
	    } else {
		    wxMessageDialog msg(this, wxT("Failed to open DLL."), wxT("Error"), wxOK);
		    msg.ShowModal();
	    }
    }
}

void XSimMain::OnAbout( wxCommandEvent &event )
{
    wxMessageDialog dialog( this, wxT("GenSpect Simulator GUI"),
        wxT("About XSim"), wxOK|wxICON_INFORMATION );
    dialog.ShowModal();
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
	ChooseTransition(event.GetIndex());
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
    initial_state = gsNextStateInit(Spec,true);

    Reset(initial_state);
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
		if ( gsIsDataVarId(ATAgetFirst(state)) )
		{
			fprintf(f,"_");
		} else {
			gsPrintPart(f,ATgetFirst(state),false,0);
		}
		rewind(f);
		if ( fgets(s,1000,f) == NULL )
		{
			s[0] = 0;
		}
		fclose(f);
		stateview->SetItem(i,1,wxT(s));
		if ( showchange && !(ATisEqual(ATgetFirst(state),ATgetFirst(old)) || (gsIsDataVarId(ATAgetFirst(state)) && gsIsDataVarId(ATAgetFirst(old))) ) )
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
		gsPrintPart(f,ATgetFirst(ATLgetFirst(l)),false,0);
		rewind(f);
		if ( fgets(s,1000,f) == NULL )
		{
			s[0] = 0;
		}
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
				gsPrintPart(f,ATgetFirst(o),false,0);
				fprintf(f," := ");
				if ( gsIsDataVarId(ATAgetFirst(n)) )
				{
					fprintf(f,"_");
				} else {
					gsPrintPart(f,ATgetFirst(n),false,0);
				}
			}
		}
		rewind(f);
		if ( fgets(s,1000,f) == NULL )
		{
			s[0] = 0;
		}
		fclose(f);
		transview->SetItem(i,1,wxT(s));
	}

	if ( !ATisEmpty(next_states) )
	{
		transview->Select(0);
	}
	transview->SetColumnWidth(1,wxLIST_AUTOSIZE);
}


