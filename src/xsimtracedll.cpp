#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "xsimtracedll.h"
#endif

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <aterm2.h>
#include "xsimtracedll.h"
#include "gslowlevel.h"
#include "gsfunc.h"
#include "libgsparse.h"

//------------------------------------------------------------------------------
// XSimMain
//------------------------------------------------------------------------------

const int ID_LISTVIEW = 10101;

BEGIN_EVENT_TABLE(XSimTraceDLL,wxFrame)
    EVT_CLOSE(XSimTraceDLL::OnCloseWindow)
    EVT_LIST_ITEM_ACTIVATED(ID_LISTVIEW,XSimTraceDLL::OnListItemActivated)
END_EVENT_TABLE()

static void PrintState(FILE *f ,ATermList state)
{
	for (; !ATisEmpty(state); state=ATgetNext(state))
	{
		if ( gsIsDataVarId(ATAgetFirst(state)) )
		{
			fprintf(f,"_");
		} else {
			gsPrintPart(f,ATAgetFirst(state),false,0);
		}
		if ( !ATisEmpty(ATgetNext(state)) )
		{
			fprintf(f,", ");
		}
	}
}

XSimTraceDLL::XSimTraceDLL( wxWindow *parent ) :
    wxFrame( parent, -1, wxT("XSim Trace (DLL)"), wxDefaultPosition, wxSize(300,400), wxDEFAULT_FRAME_STYLE )
{
    wxPanel *panel = new wxPanel(this,-1);
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    wxStaticBox *box = new wxStaticBox(panel,-1,wxT("Transitions"));
    wxStaticBoxSizer *boxsizer = new wxStaticBoxSizer(box,wxVERTICAL);
    traceview = new wxListView(panel,ID_LISTVIEW,wxDefaultPosition,wxSize(0,0),wxLC_REPORT|wxSUNKEN_BORDER|wxLC_HRULES|wxLC_VRULES|wxLC_SINGLE_SEL);
    traceview->InsertColumn(0,wxT("#"));
    traceview->InsertColumn(1,wxT("Action"));
    traceview->InsertColumn(2,wxT("State"));
    traceview->SetColumnWidth(0,30);
    traceview->SetColumnWidth(1,120);
    boxsizer->Add(traceview,1,wxGROW|wxALIGN_CENTER|wxALL,5);
    sizer->Add(boxsizer,1,wxGROW|wxALIGN_CENTER|wxALL,5);
    panel->SetSizer(sizer);

    simulator = NULL;
    current_pos = -1;
}


void XSimTraceDLL::Registered(SimulatorInterface *Simulator)
{
	simulator = Simulator;
	if ( xsimdll != NULL )
	{
		xsimdll->SetSimulator(this,Simulator);
	}
}

void XSimTraceDLL::Unregistered()
{
	simulator = NULL;
	current_pos = -1;
	traceview->DeleteAllItems();
	if ( xsimdll != NULL )
	{
		xsimdll->ClearSimulator(this);
	}
}

void XSimTraceDLL::Initialise(ATermList Pars)
{
}

void XSimTraceDLL::StateChanged(ATermAppl Transition, ATermList State, ATermList NextStates)
{
	if ( Transition != NULL )
	{
		char s[1000];
		FILE *f;
		int l = traceview->GetItemCount()-1;

		while ( l > current_pos )
		{
			traceview->DeleteItem(l);
			l--;
		}
		current_pos++;
		traceview->InsertItem(current_pos,wxString::Format("%i",current_pos));
		f = fopen("xsim.tmp","w+");
		gsPrintPart(f,Transition,false,0);
		rewind(f);
		if ( fgets(s,1000,f) == NULL )
		{
			s[0] = 0;
		}
		fclose(f);
		traceview->SetItem(current_pos,1,wxT(s));
		f = fopen("xsim.tmp","w+");
		PrintState(f,State);
//		gsPrintParts(f,(ATerm) State,false,0,NULL,",");
		rewind(f);
		if ( fgets(s,1000,f) == NULL )
		{
			s[0] = 0;
		}
		fclose(f);
		traceview->SetItem(current_pos,2,wxT(s));
		traceview->SetColumnWidth(2,wxLIST_AUTOSIZE);
	}
}

void XSimTraceDLL::Reset(ATermList State)
{
	char s[1000];
	FILE *f;

	traceview->DeleteAllItems();
	traceview->InsertItem(0,wxT("0"));
	traceview->SetItem(0,1,wxT(""));
	f = fopen("xsim.tmp","w+");
	PrintState(f,State);
//	gsPrintParts(f,(ATerm) State,false,0,NULL,",");
	rewind(f);
	if ( fgets(s,1000,f) == NULL )
	{
		s[0] = 0;
	}
	fclose(f);
	traceview->SetItem(0,2,wxT(s));
	traceview->SetColumnWidth(2,wxLIST_AUTOSIZE);
	current_pos = 0;
}

void XSimTraceDLL::Undo(int Count)
{
	while ( Count > 0 )
	{
		wxColor col(245,245,245);
		traceview->SetItemBackgroundColour(current_pos,col);
		current_pos--;
		Count--;
	}
}

void XSimTraceDLL::Redo(int Count)
{
	while ( Count > 0 )
	{
		wxColor col(255,255,255);
		current_pos++;
		traceview->SetItemBackgroundColour(current_pos,col);
		Count--;
	}
}

void XSimTraceDLL::TraceChanged(ATermList Trace, int From)
{
	int l = traceview->GetItemCount()-1;
	
	while ( l >= From )
	{
		traceview->DeleteItem(l);
		l--;
	}

	for (; !ATisEmpty(Trace); Trace=ATgetNext(Trace))
	{
		if ( From == 0 )
		{
			Reset(ATLgetFirst(ATgetNext(ATLgetFirst(Trace))));
		} else {
			StateChanged(ATAgetFirst(ATLgetFirst(Trace)),ATLgetFirst(ATgetNext(ATLgetFirst(Trace))),NULL);
		}
		From++;
	}
}

void XSimTraceDLL::TracePosChanged(ATermAppl Transition, ATermList State, int Index)
{
	while ( current_pos > Index )
	{
		Undo(1);
	}
	while ( current_pos < Index )
	{
		Redo(1);
	}
}


void XSimTraceDLL::OnCloseWindow( wxCloseEvent &event )
{
	if ( xsimdll != NULL )
	{
		xsimdll->Remove(this,true);
	}
	Destroy();
}

void XSimTraceDLL::OnListItemActivated( wxListEvent &event )
{
	if ( simulator != NULL )
	{
		simulator->SetTracePos(event.GetIndex());
	}
}


static XSimViewsDLL *xsimdll;

extern "C" void SimulatorViewDLLAddView(SimulatorInterface *Simulator)
{
	XSimTraceDLL *v;
	v = new XSimTraceDLL(Simulator->MainWindow());
	v->Show();
	v->SetXSimViewsDLL(xsimdll);
	xsimdll->Add(v,Simulator);
}

extern "C" __attribute__((constructor)) void SimulatorViewDLLInit()
{
	gsEnableConstructorFunctions();
	xsimdll = new XSimViewsDLL;
}

extern "C" __attribute__((destructor)) void SimulatorViewDLLCleanUp()
{
	delete xsimdll;
}
