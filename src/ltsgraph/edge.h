#ifndef edge_h
#define edge_h

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/wxprec.h"
#include <wx/dcps.h>
#include "node.h"
#include <string>

#define FONT_SIZE 9
#define POS_EDGE_LBL_X -10.0
#define POS_EDGE_LBL_Y -10.0

using namespace std;

class edge
{
public:
  edge();
  edge(Node* _N1, Node* _N2, wxString _Slbl);

  void on_paint(wxDC * ptrDC);
  Node* get_n1();
  Node* get_n2();
  string get_lbl();

  double get_x_pos1();
  double get_y_pos1();
  double get_x_pos2();
  double get_y_pos2();

	bool LabelVisible();
	void ShowLabels();
	void HideLabels();

private:

  Node* N1; 
  Node* N2; // Start and end nodes.

  wxString lbl;

	bool labelsVisible;
    

};

#endif //edge_h
