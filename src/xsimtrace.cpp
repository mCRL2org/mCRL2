#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "xsimtrace.h"
#endif

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <sstream>
#include <aterm2.h>
#include "xsimtrace.h"
#include "gslowlevel.h"
#include "gsfunc.h"
#include "libprint_types.h"
#include "libprint_cxx.h"
#include "libgsnextstate.h"

using namespace std;

//------------------------------------------------------------------------------
// XSimMain
//------------------------------------------------------------------------------

const int ID_LISTVIEW = 10101;

BEGIN_EVENT_TABLE(XSimTrace,wxFrame)
    EVT_CLOSE(XSimTrace::OnCloseWindow)
    EVT_LIST_ITEM_ACTIVATED(ID_LISTVIEW,XSimTrace::OnListItemActivated)
END_EVENT_TABLE()

static void PrintState(stringstream &ss ,ATerm state)
{
        for (int i=0; i<gsGetStateLength(); i++)
        {
                if ( i > 0 )
                {
			ss << ", ";
                }

                ATermAppl a = gsGetStateArgument(state,i);
                if ( gsIsDataVarId(a) )
                {
			ss << "_";
                } else {
                        PrintPart_CXX(ss, (ATerm) a, ppAdvanced);
                }
        }
}

XSimTrace::XSimTrace( wxWindow *parent ) :
    wxFrame( parent, -1, wxT("XSim Trace"), wxDefaultPosition, wxSize(300,400), wxDEFAULT_FRAME_STYLE )
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


void XSimTrace::Registered(SimulatorInterface *Simulator)
{
	simulator = Simulator;
}

void XSimTrace::Unregistered()
{
	simulator = NULL;
	current_pos = -1;
	traceview->DeleteAllItems();
}

void XSimTrace::Initialise(ATermList Pars)
{
}

void XSimTrace::AddState(ATermAppl Transition, ATerm State, bool enabled)
{
	if ( Transition != NULL )
	{
		stringstream ss;
		int l = traceview->GetItemCount();

		traceview->InsertItem(l,wxString::Format(wxT("%i"),current_pos));
		traceview->SetItem(l,1,wxConvLocal.cMB2WX(PrintPart_CXX((ATerm) Transition, ppAdvanced).c_str()));
		traceview->SetItem(l,2,wxConvLocal.cMB2WX(ss.str().c_str()));
		traceview->SetColumnWidth(2,wxLIST_AUTOSIZE);
		if ( enabled )
		{
			wxColor col(255,255,255);
			PrintState(ss,State);
			traceview->SetItemBackgroundColour(l,col);
		} else {
			wxColor col(245,245,245);
			PrintState(ss,State);
			traceview->SetItemBackgroundColour(l,col);
		}
	}
}

void XSimTrace::StateChanged(ATermAppl Transition, ATerm State, ATermList NextStates)
{
	if ( Transition != NULL )
	{
		int l = traceview->GetItemCount()-1;

		while ( l > current_pos )
		{
			traceview->DeleteItem(l);
			l--;
		}
		AddState(Transition,State,true);
		current_pos++;
	}
}

void XSimTrace::Reset(ATerm State)
{
	stringstream ss;

	traceview->DeleteAllItems();
	traceview->InsertItem(0,wxT("0"));
	traceview->SetItem(0,1,wxT(""));
	PrintState(ss,State);
	traceview->SetItem(0,2,wxConvLocal.cMB2WX(ss.str().c_str()));
	traceview->SetColumnWidth(2,wxLIST_AUTOSIZE);
	current_pos = 0;
}

void XSimTrace::Undo(int Count)
{
	while ( Count > 0 )
	{
		wxColor col(245,245,245);
		traceview->SetItemBackgroundColour(current_pos,col);
		current_pos--;
		Count--;
	}
}

void XSimTrace::Redo(int Count)
{
	while ( Count > 0 )
	{
		wxColor col(255,255,255);
		current_pos++;
		traceview->SetItemBackgroundColour(current_pos,col);
		Count--;
	}
}

void XSimTrace::TraceChanged(ATermList Trace, int From)
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
			Reset(ATgetFirst(ATgetNext(ATLgetFirst(Trace))));
		} else {
			AddState(ATAgetFirst(ATLgetFirst(Trace)),ATgetFirst(ATgetNext(ATLgetFirst(Trace))),current_pos >= From);
		}
		From++;
	}
}

void XSimTrace::TracePosChanged(ATermAppl Transition, ATerm State, int Index)
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


void XSimTrace::OnCloseWindow( wxCloseEvent &event )
{
    if ( event.CanVeto() )
    {
	    Show(FALSE);
	    event.Veto();
    } else {
	    Destroy();
    }
}

void XSimTrace::OnListItemActivated( wxListEvent &event )
{
	if ( simulator != NULL )
	{
		simulator->SetTracePos(event.GetIndex());
	}
}
