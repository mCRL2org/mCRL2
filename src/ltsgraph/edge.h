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
  // void Set_pos1(double _x, double _y);
  // void Set_pos2(double _x, double _y);
  double GetXpos1();
  double GetYpos1();
  double GetXpos2();
  double GetYpos2();
// Locking is done on a node, not on the edges.
//  void Lock1();
//  void Unlock1();
//  void Lock2();
//  void Unlock2();

private:
//  bool locked1;
//  bool locked2;

    Node* N1; 
    Node* N2; // Start and end nodes.
//  unsigned int numN1; // These numbers are now an
//  unsigned int numN2; // index in the state vector
                      // where other data can be retrieved.
// The information below is superfluous, 
// as it also occurs in nodes. Storing 
// it more than once is actually dangerous,
// due to possible data inconsistencies.
// double pos1X;
// double pos1Y;
// double pos2X;
// double pos2Y;
   wxString lbl;
    

};

#endif //edge_h
