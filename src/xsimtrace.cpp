#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "xsim.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "xsimtrace.h"
#include "aterm2.h"
#include "gslowlevel.h"
#include "gsfunc.h"

extern "C" void gsPrintPart(FILE *f, ATerm a, bool b, int c);
extern "C" void gsPrintParts(FILE *f, ATerm a, bool b, int c, char *s, char *t);

//------------------------------------------------------------------------------
// XSimMain
//------------------------------------------------------------------------------

const int ID_LISTVIEW = 10101;

BEGIN_EVENT_TABLE(XSimTrace,wxFrame)
    EVT_CLOSE(XSimTrace::OnCloseWindow)
    EVT_LIST_ITEM_ACTIVATED(ID_LISTVIEW,XSimTrace::OnListItemActivated)
END_EVENT_TABLE()

XSimTrace::XSimTrace( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxFrame( parent, id, title, position, size, style )
{
    // XXX Easy hack... should be separated from XSimMain
    wxPanel *panel = new wxPanel(this,-1);
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    wxStaticBox *box = new wxStaticBox(panel,-1,wxT("Transitions"));
    wxStaticBoxSizer *boxsizer = new wxStaticBoxSizer(box,wxVERTICAL);
    traceview = new wxListView(panel,ID_LISTVIEW,wxDefaultPosition,wxSize(0,0),wxLC_REPORT|wxSUNKEN_BORDER|wxLC_HRULES|wxLC_VRULES|wxLC_SINGLE_SEL);
    traceview->InsertColumn(0,wxT(""));
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

void XSimTrace::SetSimulator(SimulatorInterface *sim)
{
	simulator = sim;
}

void XSimTrace::Reset(ATermList state)
{
	char s[1000];
	FILE *f;

	traceview->DeleteAllItems();
	traceview->InsertItem(0,wxT("0"));
	traceview->SetItem(0,1,wxT(""));
	f = fopen("xsim.tmp","w+");
	gsPrintParts(f,(ATerm) state,false,0,NULL,",");
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

void XSimTrace::SetNext(ATermList transition)
{
	char s[1000];
	FILE *f;
	int l = traceview->GetItemCount();

	while ( l > current_pos )
	{
		traceview->DeleteItem(l);
		l--;
	}
	current_pos++;
	traceview->InsertItem(current_pos,wxString::Format("%i",current_pos));
	f = fopen("xsim.tmp","w+");
	gsPrintPart(f,ATgetFirst(transition),false,0);
	rewind(f);
	if ( fgets(s,1000,f) == NULL )
	{
		s[0] = 0;
	}
	fclose(f);
	traceview->SetItem(current_pos,1,wxT(s));
	f = fopen("xsim.tmp","w+");
	gsPrintParts(f,ATgetFirst(ATgetNext(transition)),false,0,NULL,",");
	rewind(f);
	if ( fgets(s,1000,f) == NULL )
	{
		s[0] = 0;
	}
	fclose(f);
	traceview->SetItem(current_pos,2,wxT(s));
	traceview->SetColumnWidth(2,wxLIST_AUTOSIZE);
}

void XSimTrace::Undo()
{
	wxColor col(245,245,245);
	traceview->SetItemBackgroundColour(current_pos,col);
	current_pos--;
}

void XSimTrace::Redo()
{
	wxColor col(255,255,255);
	current_pos++;
	traceview->SetItemBackgroundColour(current_pos,col);
}

void XSimTrace::Goto(int pos)
{
	while ( current_pos > pos )
	{
		Undo();
	}
	while ( current_pos < pos )
	{
		Redo();
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
		simulator->GotoTracePos(event.GetIndex());
	}
}
