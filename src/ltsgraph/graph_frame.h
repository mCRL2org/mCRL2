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

// Colour dialog
#include <wx/colordlg.h>

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
#include "export_latex.h"
#include "ltsgraph_backup.h"

#define INITIAL_WIN_HEIGHT 780
#define INITIAL_WIN_WIDTH  1024


using namespace std;
using namespace mcrl2::lts;

const int ID_OPTIMIZE       = wxID_HIGHEST +  0;
const int ID_STOP_OPTIMIZE  = wxID_HIGHEST +  1;
const int ID_CHECK_NODE     = wxID_HIGHEST +  2;
const int ID_CHECK_EDGE     = wxID_HIGHEST +  3;
const int ID_BUTTON_OPTI    = wxID_HIGHEST +  4;
const int ID_EXPORT_PS      = wxID_HIGHEST +  5;
const int ID_EXPORT_LATEX   = wxID_HIGHEST +  6;
const int ID_BACKUP_CREATE  = wxID_HIGHEST +  7;
const int ID_BACKUP_RESTORE = wxID_HIGHEST +  8;
const int ID_BUTTON_COLOUR  = wxID_HIGHEST +  9;
const int ID_CHECK_CURVES   = wxID_HIGHEST + 10;

/* To show what is the selected item */
enum selected_type {
  none_t,
  node_t,
  edge_t,
  edge_label_t,
};
  
class ViewPort;
class ExportToLatex;

class GraphFrame : public wxFrame
{
public:
  GraphFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style = wxDEFAULT_FRAME_STYLE);
  void BuildLayout();
  void Init(wxString LTSfile);
  void Draw(wxPaintDC * myDC);
  void ExportPostScript(wxCommandEvent& event);
  void ExportLatex(wxCommandEvent& event);
  void CreateBackup(wxCommandEvent& event);
  void Resize(wxSize);
  void ReplaceAfterDrag(wxPoint);
  void FixNode();
  void OnOpen(wxCommandEvent& event);
  void OnQuit(wxCommandEvent& event);//When the user clicks on the Quit menu
  void OnClose(wxCloseEvent& event);//When the user clicks on the cross of the window
  void OnOptimize( wxCommandEvent &event );
  void OnStopOptimize( wxCommandEvent &event );
  void OnCheckNode( wxCommandEvent &event ); //when the user clicks on the node checkbox
  void OnCheckEdge( wxCommandEvent &event ); //when the user clicks on the edge checkbox
  void on_check_curves (wxCommandEvent &event); // when the user clicks on the curves checkbox.
  void OnBtnOpti( wxCommandEvent &event );
  void on_btn_pick_colour( wxCommandEvent &event ); // when the user wants to pick a colour for a node.
  bool OptimizeDrawing(double precision);
  void RestoreBackup();
  void FindNode(wxPoint);
  void CreateMenu();
  void CreateStatusBar();
  void FillStatusBar(const wxString text, unsigned int no);
  wxString GetInfoCurrentNode(Node* info_node) const;
  wxString GetInfoWinSize(wxSize) const;
  void enable_btn_colour_picker();
  void disable_btn_colour_picker();

private:
  bool StopOpti;
  bool StoppedOpti;
  double EdgeStiffness;   //stiffness of the edge
  double NodeStrength;   //Strength of the electrical repulsion
  double NaturalLength;

  string inputFileName;

  bool curve_edges;

  ViewPort * leftPanel;
  wxPanel * rightPanel;

  wxMenuBar * menu;
  wxMenu * file;
  wxMenu * draw;
  wxMenu * exports;
  wxMenuItem * openItem;
  wxMenuItem * exportPsItem;
  wxMenuItem * exportLatexItem;
  wxMenuItem * backupCreate;
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
  wxCheckBox * ck_curve_edges;

  wxButton * btnOptiStop;	
  wxButton * btn_pick_colour;

DECLARE_EVENT_TABLE()
};

class ViewPort : public wxPanel {
	friend class GraphFrame;
	public:
	  Node * get_selected_node();
	private :
          ViewPort(wxWindow* parent, const wxPoint& pos, const wxSize& size, long style);
	  void OnPaint(wxPaintEvent& evt);
	  void OnResize(wxSizeEvent& event);
	  void PressLeft(wxMouseEvent& event);
	  void Drag(wxMouseEvent& event);
	  void ReleaseLeft(wxMouseEvent& event);
	  void PressRight(wxMouseEvent& event);//to fix a node
	  void FillStatusBar();
         
          Node * selected_node; //Pointer to the node that was last clicked. NULL if none.
	  edge * selected_edge;

          selected_type selection;
	
          wxSize sz;

	  GraphFrame * GF;

	  int Get_Width();
	  int Get_Height();

	DECLARE_EVENT_TABLE();

};

