#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <wx/wxprec.h>
#include <wx/cmdline.h>
#include <wx/utils.h>
#include <wx/msgdlg.h>
#include <wx/gdicmn.h>
#include <wx/dcps.h>
//#include <wx/cmndata.h>
//layout
#include <wx/splitter.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>

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

#include "spinctrlfloat.h"

#include "node.h"
#include "edge.h"

#define INITIAL_WIN_HEIGHT 780
#define INITIAL_WIN_WIDTH  1024


using namespace std;
using namespace mcrl2::lts;

const int ID_OPTIMIZE      = wxID_HIGHEST + 0;
const int ID_STOP_OPTIMIZE = wxID_HIGHEST + 1;
const int ID_CHECK_NODE    = wxID_HIGHEST + 2;
const int ID_CHECK_EDGE    = wxID_HIGHEST + 3;
const int ID_BUTTON_OPTI   = wxID_HIGHEST + 4;
const int ID_EXPORT_PS     = wxID_HIGHEST + 5;



class ViewPort;

class GraphFrame : public wxFrame
{
public:
  GraphFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style = wxDEFAULT_FRAME_STYLE);
	void BuildLayout();
  void Init(wxString LTSfile);
	void Draw(wxPaintDC * myDC);
	void ExportPostScript(wxCommandEvent& event);
	void Resize(wxSize);
	void ReplaceAfterDrag(wxPoint);
	void FixNode(int num);
  void OnOpen(wxCommandEvent& event);
  void OnQuit(wxCommandEvent& event);//When the user clicks on the Quit menu
  void OnClose(wxCloseEvent& event);//When the user clicks on the cross of the window
  void OnOptimize( wxCommandEvent &event );
  void OnStopOptimize( wxCommandEvent &event );
	void OnCheckNode( wxCommandEvent &event ); //when the user clicks on the node checkbox
	void OnCheckEdge( wxCommandEvent &event ); //when the user clicks on the edge checkbox
	void OnBtnOpti( wxCommandEvent &event );
  bool OptimizeDrawing(double precision);
  int  FindNode(wxPoint);
  void CreateMenu();
	void CreateStatusBar();
	void FillStatusBar(const wxString text, unsigned int no);
	wxString GetInfoCurrentNode(int num) const;
	wxString GetInfoWinSize(wxSize) const;


private:
  bool StopOpti;
  
  double EdgeStiffness;   //stiffness of the edge
  double NodeStrength;   //Strength of the electrical repulsion
  double NaturalLength;

  ViewPort * leftPanel;
	wxPanel * rightPanel;

  wxMenuBar * menu;
  wxMenu * file;
  wxMenu * draw;
  wxMenuItem * openItem;
  wxMenuItem * exportPsItem;
  wxMenuItem * quitItem;
  wxMenuItem * optimizeGraph;
  wxMenuItem * stopOptimize;

	wxStatusBar * statusBar;

	wxStaticText * numberOfStatesLabel;
	wxStaticText * numberOfTransitionsLabel;
	wxStaticText * initialStateLabel;
	wxStaticText * numberOfLabelsLabel;

	wxSpinCtrlFloat * spinNodeStrength;
	wxSpinCtrlFloat * spinEdgeStiffness;
	wxSpinCtrlFloat * spinNaturalLength;
	wxSpinCtrl * spinNodeRadius;

	wxCheckBox * ckNodeLabels;
	wxCheckBox * ckEdgeLabels;

	wxButton * btnOptiStop;

DECLARE_EVENT_TABLE()
};

class ViewPort : public wxPanel {
	friend class GraphFrame;
		private :
			ViewPort(wxWindow* parent, const wxPoint& pos, const wxSize& size, long style);
		  void OnPaint(wxPaintEvent& evt);
			void OnResize(wxSizeEvent& event);
			void PressLeft(wxMouseEvent& event);
			void Drag(wxMouseEvent& event);
			void ReleaseLeft(wxMouseEvent& event);
			void PressRight(wxMouseEvent& event);//to fix a node
			void FillStatusBar();
	
			wxSize sz;
			GraphFrame * GF;

			int Get_Width();
			int Get_Height();

	DECLARE_EVENT_TABLE();

};

