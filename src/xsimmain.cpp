#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "xsimmain.h"
#endif

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <wx/dynlib.h>
#include <wx/config.h>
#include <wx/textfile.h>
#include <wx/dynarray.h>
#include <fstream>
#include <sstream>
#include <aterm2.h>
#include "xsimbase.h"
#include "xsimmain.h"
#include "gslowlevel.h"
#include "gsfunc.h"
#include "libgsnextstate.h"
#include "libgsrewrite.h"
#include "libprint_types.h"
#include "libprint_cxx.h"

using namespace std;

//------------------------------------------------------------------------------
// XSimMain
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(XSimMain,wxFrame)
    EVT_MENU(wxID_OPEN, XSimMain::OnOpen)
    EVT_MENU(wxID_EXIT, XSimMain::OnQuit)
    EVT_MENU(ID_UNDO, XSimMain::OnUndo)
    EVT_MENU(ID_REDO, XSimMain::OnRedo)
    EVT_MENU(ID_RESET, XSimMain::OnReset)
    EVT_MENU(ID_LOADTRACE, XSimMain::OnLoadTrace)
    EVT_MENU(ID_SAVETRACE, XSimMain::OnSaveTrace)
    EVT_MENU(ID_FITCS, XSimMain::OnFitCurrentState)
    EVT_MENU(ID_TRACE, XSimMain::OnTrace)
    EVT_MENU(ID_LOADVIEW, XSimMain::OnLoadView)
    EVT_MENU(wxID_ABOUT, XSimMain::OnAbout)
    EVT_CLOSE(XSimMain::OnCloseWindow)
    EVT_LIST_ITEM_SELECTED(ID_LISTCTRL1,XSimMain::stateOnListItemSelected)
    EVT_LIST_ITEM_ACTIVATED(ID_LISTCTRL2,XSimMain::transOnListItemActivated)
END_EVENT_TABLE()

XSimMain::XSimMain( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxFrame( parent, id, title, position, size, style )
{
    use_dummies = false;
    rewr_strat = GS_REWR_INNER3; // XXX add to constructor?

    CreateMenu();
    CreateStatus();
    CreateContent();

    state_vars = ATmakeList0();
    ATprotectList(&state_vars);
    state_varnames = ATmakeList0();
    ATprotectList(&state_varnames);
    initial_state = NULL;;
    ATprotect(&initial_state);
    current_state = NULL;
    ATprotect(&current_state);
    next_states = ATmakeList0();
    ATprotectList(&next_states);
    trace = ATmakeList0();
    ATprotectList(&trace);
    ecart = ATmakeList0();
    ATprotectList(&ecart);
    
    tracewin = new XSimTrace(this);
    Register(tracewin);
    //tracewin->Show(FALSE); // default, so not needed
    
    wxConfig config(wxT("xsimrc"));
    if ( config.HasGroup(wxT("LoadLibrary")) )
    {
	    config.SetPath(wxT("/LoadLibrary"));

	    wxString s;
	    long i;
	    bool b = config.GetFirstEntry(s,i);
	    while ( b )
	    {
		    LoadDLL(config.Read(s,wxT("")));
		    b = config.GetNextEntry(s,i);
	    }
    }
}

XSimMain::~XSimMain()
{
	for (viewlist::iterator i = views.begin(); i != views.end(); i++)
	{
		(*i)->Unregistered();
	}
	
	delete tracewin;

	ATunprotectList(&state_vars);
	ATunprotectList(&state_varnames);
	ATunprotect(&initial_state);
	ATunprotect(&current_state);
	ATunprotectList(&next_states);
	ATunprotectList(&trace);
	ATunprotectList(&ecart);
}

void XSimMain::CreateMenu()
{
    menu = new wxMenuBar;

    wxMenu *file = new wxMenu;
    file->Append( wxID_OPEN, wxT("&Open...	CTRL-o"), wxT("") );
    file->AppendSeparator();
    file->Append( wxID_EXIT, wxT("&Quit	CTRL-q"), wxT("") );
    menu->Append( file, wxT("&File") );

    wxMenu *edit = new wxMenu;
    undo = edit->Append( ID_UNDO, wxT("&Undo	CTRL-LEFT"), wxT("") );
    undo->Enable(false);
    redo = edit->Append( ID_REDO, wxT("Re&do	CTRL-RIGHT"), wxT("") );
    redo->Enable(false);
    edit->Append( ID_RESET, wxT("&Reset	CTRL-r"), wxT("") );
    edit->AppendSeparator();
    edit->Append( ID_LOADTRACE, wxT("&Load trace...	CTRL-l"), wxT("") );
    edit->Append( ID_SAVETRACE, wxT("&Save trace...	CTRL-s"), wxT("") );
    edit->AppendSeparator();
#ifdef __WINDOWS__
    edit->Append( ID_FITCS, wxT("F&it to Current State	CTRL-f"), wxT("") )->Enable(false);
#else
    edit->Append( ID_FITCS, wxT("F&it to Current State	CTRL-f"), wxT("") );
#endif
    menu->Append( edit, wxT("&Edit") );
    
    wxMenu *views = new wxMenu;
    views->Append( ID_TRACE, wxT("&Trace	CTRL-t"), wxT("") );
    views->Append( ID_MENU, wxT("&Graph"), wxT("") )->Enable(false);
    views->Append( ID_LOADVIEW, wxT("&Load Dynamic..."), wxT("") );
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
    split     = new wxSplitterWindow(this,ID_SPLITTER,wxDefaultPosition,wxDefaultSize,wxCLIP_CHILDREN);

    toppanel    = new wxPanel(split,-1);
    topsizer    = new wxBoxSizer(wxVERTICAL);
    topboxsizer = new wxStaticBoxSizer(wxVERTICAL,toppanel,wxT("Current State"));
    stateview   = new wxListView(toppanel,ID_LISTCTRL1,wxDefaultPosition,wxDefaultSize,wxLC_REPORT|wxSUNKEN_BORDER|wxLC_HRULES|wxLC_VRULES|wxLC_SINGLE_SEL);

    topboxsizer->Add(stateview,1,wxEXPAND|wxALIGN_CENTER|wxALL,5);
    topsizer->Add(topboxsizer,1,wxEXPAND|wxALIGN_CENTER|wxALL,5);
    toppanel->SetSizer(topsizer);

    bottompanel    = new wxPanel(split,-1);
    bottomsizer    = new wxBoxSizer(wxVERTICAL);
    bottomboxsizer = new wxStaticBoxSizer(wxVERTICAL,bottompanel,wxT("Transitions"));
    transview      = new wxListView(bottompanel,ID_LISTCTRL2,wxDefaultPosition,wxDefaultSize,wxLC_REPORT|wxSUNKEN_BORDER|wxLC_HRULES|wxLC_VRULES|wxLC_SINGLE_SEL);

    bottomboxsizer->Add(transview,1,wxEXPAND|wxALIGN_CENTER|wxALL,5);
    bottomsizer->Add(bottomboxsizer,1,wxEXPAND|wxALIGN_CENTER|wxALL,5);
    bottompanel->SetSizer(bottomsizer);

    split->SplitHorizontally(bottompanel,toppanel);
    split->SetMinimumPaneSize(27);
    mainsizer->Add(split,1,wxEXPAND|wxALIGN_CENTER|wxALL,5);
    
    SetMinSize(wxSize(240,160));

    stateview->InsertColumn(0,wxT("Parameter"), wxLIST_FORMAT_CENTRE, 120);
    stateview->InsertColumn(1,wxT("Value"), wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE_USEHEADER|wxLIST_AUTOSIZE);

    transview->InsertColumn(0,wxT("Action"), wxLIST_FORMAT_CENTRE, 120);
    transview->InsertColumn(1,wxT("State Change"), wxLIST_AUTOSIZE_USEHEADER|wxLIST_AUTOSIZE);
    transview->SetFocus();

    /* Show in order to be able to query client width */
    Show();

    stateview->SetColumnWidth(1,stateview->GetClientSize().GetWidth() - stateview->GetColumnWidth(0));
    transview->SetColumnWidth(1,transview->GetClientSize().GetWidth() - transview->GetColumnWidth(0));
}

void XSimMain::LoadFile(const wxString &filename)
{
    FILE *f;
    
    if ( (f = fopen(filename.fn_str(),"rb")) == NULL )
    {
	    wxMessageDialog msg(this, wxT("Failed to open file."),
		wxT("Error"), wxOK|wxICON_ERROR);
	    msg.ShowModal();
	    return;
    }

    ATermAppl Spec = (ATermAppl) ATreadFromFile(f);
    fclose(f);

    if ( Spec == NULL )
    {
	    wxMessageDialog msg(this, wxT("Invalid file."),
		wxT("Error"), wxOK|wxICON_ERROR);
	    msg.ShowModal();
	    return;
    }

    ATermList l = ATLgetArgument(ATAgetArgument(Spec,5),1);
    ATermList m = ATmakeList0();
    ATermList n = ATmakeList0();
    stateview->DeleteAllItems();
    for (int i=0; !ATisEmpty(l); l=ATgetNext(l), i++)
    {
	    wxString s(ATgetName(ATgetAFun(ATAgetArgument(ATAgetFirst(l),0)))
#ifdef wxUSE_UNICODE
			    ,wxConvLocal
#endif
			    );
	    stateview->InsertItem(i,s);
	    m = ATinsert(m,ATgetArgument(ATAgetFirst(l),0));
	    n = ATinsert(n,ATgetFirst(l));
    }
    state_varnames = ATreverse(m);
    state_vars = ATreverse(n);
    if ( initial_state != NULL )
    {
	    gsNextStateFinalise();
    }
    initial_state = gsNextStateInit(Spec,!use_dummies,GS_STATE_VECTOR,rewr_strat);

    InitialiseViews();
    Reset(initial_state);
}

void XSimMain::LoadDLL(const wxString &filename)
{
	wxDynamicLibrary lib(filename);

	if ( lib.IsLoaded() )
	{
		void (*f)(SimulatorInterface *);

		f = (void (*)(SimulatorInterface *)) lib.GetSymbol(wxT("SimulatorViewDLLAddView"));
		if ( f != NULL )
		{
			f(this);
			lib.Detach(); //XXX
		} else {
			wxMessageDialog msg(this, wxT("DLL does not appear to contain a View."), wxT("Error"), wxOK|wxICON_ERROR);
			msg.ShowModal();
		}
	} else {
		/*wxMessageDialog msg(this, wxT("Failed to open DLL."), wxT("Error"), wxOK|wxICON_ERROR);
		msg.ShowModal();*/
       }
 }
 

void XSimMain::Register(SimulatorViewInterface *View)
{
	views.push_back(View);
	View->Registered(this);
	if ( !ATisEmpty(trace) )
	{
	        View->Initialise(state_vars);
                View->StateChanged(NULL, current_state, next_states);
		View->TraceChanged(GetTrace(),0);
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

ATermList XSimMain::GetParameters()
{
	return state_vars;
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

void XSimMain::Reset(ATerm State)
{
	initial_state = State;
	Reset();
}

bool XSimMain::Undo()
{
	if ( ATgetLength(trace) > 1 )
	{
		ATermList l = traceUndo();
		ATerm state = ATgetFirst(ATgetNext(l));

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
		ATerm state = ATgetFirst(ATgetNext(trans));

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

ATerm XSimMain::GetState()
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
		ATerm state = ATgetFirst(ATgetNext(l));

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
	ATerm state;

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
		state = ATgetFirst(ATgetNext(ATLgetFirst(trace)));

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
	return false;
}


void XSimMain::InitialiseViews()
{
	for (viewlist::iterator i = views.begin(); i != views.end(); i++)
	{
		(*i)->Initialise(state_vars);
	}
}


void XSimMain::traceReset(ATerm state)
{
	trace = ATmakeList1((ATerm) ATmakeList2((ATerm) gsMakeNil(), state));
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
    wxFileDialog dialog( this, wxT("Select a LPE file..."), wxT(""), wxT(""), wxT("LPEs (*.lpe)|*.lpe|All files|*"));
    if ( dialog.ShowModal() == wxID_OK )
    {
	    LoadFile(dialog.GetPath());
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

void XSimMain::OnLoadTrace( wxCommandEvent &event )
{
    wxFileDialog dialog( this, wxT("Load trace..."), wxT(""), wxT(""), wxT("Traces (*.trc)|*.trc|All Files|*.*"));
    if ( dialog.ShowModal() == wxID_OK )
    {
	    wxTextFile f;

	    f.Open(dialog.GetPath());

	    Reset();

	    if ( f.GetLineCount() > 0 )
	    {
		    wxString s;
		    
		    f.AddLine(wxT(""));
		    for (s=f.GetFirstLine(); !f.Eof(); s=f.GetNextLine())
		    {
			    if ( s.Length() > 0 )
			    {
				    if ( s[0u] == wxT('"') )
				    {
					    s = s.Mid(1,s.Length()-2);
				    }
				    
				    long l = transview->FindItem(-1,s);
				    if ( l >= 0 )
				    {
					    ChooseTransition(transview->GetItemData(l));
					    Update();
				    } else {
					    wxMessageDialog dialog(this,wxString::Format(wxT("Cannot execute transition '%s'.\n"),s.c_str()),wxT("Error in trace"),wxOK|wxICON_ERROR);
					    dialog.ShowModal();
					    break;
				    }
			    }
		    }
	    }

	    f.Close();
    }
}

void XSimMain::OnSaveTrace( wxCommandEvent &event )
{
    wxFileDialog dialog( this, wxT("Save trace..."), wxT(""), wxT(""), wxT("Traces (*.trc)|*.trc|All Files|*.*"),wxSAVE);
    if ( dialog.ShowModal() == wxID_OK )
    {
	    ofstream f(dialog.GetPath().mb_str());

	    if ( !f.is_open() )
            {
              wxMessageDialog dialog(this,wxT("Cannot open file for writing"),wxT("File error"),wxOK|wxICON_ERROR);
              dialog.ShowModal();
              return;
            }

	    if ( !ATisEmpty(trace) )
	    {
		    for (ATermList l=ATconcat(ATgetNext(ATreverse(trace)),ecart); !ATisEmpty(l); l=ATgetNext(l))
		    {
			PrintPart_CXX(f, ATgetFirst(ATLgetFirst(l)), ppAdvanced);
                        f << endl;
		    }
	    }
	    
            f.close();
    }
}

void XSimMain::OnFitCurrentState( wxCommandEvent &event )
{
#ifndef __WINDOWS__
    int w,h,n;

    n = stateview->GetViewRect().GetHeight()+stateview->m_headerHeight;
    stateview->GetClientSize(&w,&h);
    split->SetSashPosition(split->GetSashPosition()+n-h);
#endif
}

void XSimMain::OnTrace( wxCommandEvent &event )
{
    tracewin->Show(!tracewin->IsShown());
}

void XSimMain::OnLoadView( wxCommandEvent &event )
{
    wxFileDialog dialog( this, wxT("Select a View DLL..."), wxT(""), wxT(""), wxT("Dynamic Libraries (*.so,*.dll)|*.so;*.dll|All Files|*.*"));
    if ( dialog.ShowModal() == wxID_OK )
    {
	    LoadDLL(dialog.GetPath());
    }
}

void XSimMain::OnAbout( wxCommandEvent &event )
{
    wxMessageDialog dialog( this, wxT("mCRL2 Simulator GUI"),
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
	ChooseTransition(event.GetData());
}

void XSimMain::SetCurrentState(ATerm state, bool showchange)
{
	ATerm old;

	if ( (current_state == NULL) || (ATgetLength(current_state) != ATgetLength(state)) )
	{
		old = state;
	} else {
		old = current_state;
	}
	current_state = state;

	for (int i=0; i<ATgetLength(state_vars); i++)
	{
		ATermAppl oldval = gsGetStateArgument(old,i);
		ATermAppl newval = gsGetStateArgument(state,i);

		if ( gsIsDataVarId(newval) )
		{
			stateview->SetItem(i,1,wxT("_"));
		} else {
			stateview->SetItem(i,1,wxConvLocal.cMB2WX(PrintPart_CXX((ATerm) newval, ppAdvanced).c_str()));
		}
		if ( showchange && !(ATisEqual(oldval,newval) || (gsIsDataVarId(oldval) && gsIsDataVarId(newval)) ) )
		{
		        wxColour col(255,255,210);
		        stateview->SetItemBackgroundColour(i,col);
		} else {
		        wxColour col(255,255,255); // XXX is this the correct colour?
		        stateview->SetItemBackgroundColour(i,col);
		}
	}

        stateview->SetColumnWidth(1,stateview->GetClientSize().GetWidth() - stateview->GetColumnWidth(0));
}

static void sort_transitions(wxArrayString &actions, wxArrayString &statechanges, wxArrayInt &indices)
{
	int len = indices.GetCount();

	for (int i=1; i<len; i++)
	{
		int j = i;
		while ( (j > 0) && ( (actions[j] < actions[j-1]) || ((actions[j] == actions[j-1]) && (statechanges[j] < statechanges[j-1])) ) )
		{
			wxString s;
			int h;
			
			s = actions[j];
			actions[j] = actions[j-1];
			actions[j-1] = s;

			s = statechanges[j];
			statechanges[j] = statechanges[j-1];
			statechanges[j-1] = s;

			h = indices[j];
			indices[j] = indices[j-1];
			indices[j-1] = h;

			j--;
		}
	}
}

void XSimMain::UpdateTransitions()
{
	wxArrayString actions;
	wxArrayString statechanges;
	wxArrayInt indices;

	next_states = gsNextState(current_state,NULL);

	transview->DeleteAllItems();
	int i = 0;
	for (ATermList l=next_states; !ATisEmpty(l); l=ATgetNext(l), i++)
	{
		actions.Add(wxConvLocal.cMB2WX(PrintPart_CXX(ATgetFirst(ATLgetFirst(l)), ppAdvanced).c_str()));
		indices.Add(i);
//		transview->SetItemData(i,i);
		stringstream ss;
		ATerm m = current_state;
		ATerm n = ATgetFirst(ATgetNext(ATLgetFirst(l)));
		ATermList o = state_varnames;
		bool comma = false;
		for (int i=0; i<ATgetLength(state_vars); i++)
		{
			ATermAppl oldval = gsGetStateArgument(m,i);
			ATermAppl newval = gsGetStateArgument(n,i);

			if ( !ATisEqual(oldval,newval) )
			{
				if ( comma )
				{
					ss << ", ";
				} else {
					comma = true;
				}
				PrintPart_CXX(ss, ATgetFirst(o), ppAdvanced);
				ss << " := ";
				if ( gsIsDataVarId(newval) )
				{
					ss << "_";
				} else {
					PrintPart_CXX(ss, (ATerm) newval, ppAdvanced);
				}
			}

			o = ATgetNext(o);
		}
//		transview->SetItem(i,1,s);
		statechanges.Add(wxConvLocal.cMB2WX(ss.str().c_str()));
	}

	sort_transitions(actions,statechanges,indices);
	for (unsigned int i=0; i<indices.GetCount(); i++)
	{
		transview->InsertItem(i,actions[i]);
		transview->SetItem(i,1,statechanges[i]);
		transview->SetItemData(i,indices[i]);
	}

	if ( !ATisEmpty(next_states) )
	{
		transview->Select(0);
	}

        /* Adapt column width */
	transview->SetColumnWidth(1,wxLIST_AUTOSIZE);
        transview->SetColumnWidth(1,transview->GetClientSize().GetWidth() - transview->GetColumnWidth(0));
}


