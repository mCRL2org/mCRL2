#ifndef edge_h
#define edge_h

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/wxprec.h"
#include "node.h"
#include <string>

#define FONT_SIZE 9
#define POS_EDGE_LBL_X -10.0
#define POS_EDGE_LBL_Y -10.0

using namespace std;

class Edge
{
public:
  Edge();
  Edge(Node* _N1, Node* _N2, wxString _Slbl);

  void OnPaint(wxPaintDC * ptrDC);
  Node* Get_N1();
  Node* Get_N2();

  double GetXpos1();
  double GetYpos1();
  double GetXpos2();
  double GetYpos2();

	void ShowLabels();
	void HideLabels();

private:

  Node* N1; 
  Node* N2; // Start and end nodes.

  wxString lbl;

	bool labelsVisible;
    

};

#endif //edge_h
