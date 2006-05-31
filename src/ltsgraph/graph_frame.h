#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <wx/wxprec.h>
#include <wx/cmdline.h>
#include <wx/utils.h>
#include <wx/msgdlg.h>
#include <wx/gdicmn.h>
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

#include "./../../../tmp/MCRL2/trunk/src/ltsview/spinctrlfloat.h"
//#include "./../ltsview/spinctrlfloat.h"

#include "node.h"
#include "edge.h"

#define INITIAL_WIN_HEIGHT 780
#define INITIAL_WIN_WIDTH  1024


using namespace std;
using namespace mcrl2::lts;

const int ID_OPTIMIZE      = 10000;
const int ID_STOP_OPTIMIZE = 10001;
const int ID_CHECK_NODE    = 10002;
const int ID_CHECK_EDGE    = 10003;
const int ID_BUTTON_OPTI   = 10004;



class ViewPort;

class GraphFrame : public wxFrame
{
public:
  GraphFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style = wxDEFAULT_FRAME_STYLE);
	void BuildLayout();
  void Init(wxString LTSfile);
	void Draw(wxPaintDC * myDC);
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

