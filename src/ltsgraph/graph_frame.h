#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <wx/wxprec.h>
#include <wx/cmdline.h>

#include <vector>
#include <iostream>
#include <string>
#include "stdlib.h"
//random
#include <cstdlib>
#include <ctime> 
//LTS
#include "liblts.h"
#include "libstruct.h"

#include "node.h"
#include "edge.h"

#define INITIAL_WIN_HEIGHT 600
#define INITIAL_WIN_WIDTH  800


using namespace std;
using namespace mcrl2::lts;

class GraphFrame : public wxFrame
{
public:
	GraphFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style = wxDEFAULT_FRAME_STYLE);
	void Init(wxString LTSfile);
	void OnPaint(wxPaintEvent& evt);
	void OnOpen(wxCommandEvent& event);
	void OptimizeDrawing();
	void OnResize(wxSizeEvent& event);
	int  FindNode(wxPoint);
	void PressLeft(wxMouseEvent& event);
	void Drag(wxMouseEvent& event);
	void ReleaseLeft(wxMouseEvent& event);
	void PressRight(wxMouseEvent& event);//to fix a node
	void CreateMenu();


private:
	wxSize sz;
	vector<Node*> vectNode;
	vector<Edge*> vectEdge;
	wxMenu *file;
    wxMenuBar *menu;
    wxMenuItem *openitem;

DECLARE_EVENT_TABLE()
};
