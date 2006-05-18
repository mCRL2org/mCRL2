#ifndef node_h
#define node_h

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/wxprec.h"
#include "wx/dataobj.h"
#include <string>

#define POS_NODE_LBL_X 3.0
#define POS_NODE_LBL_Y 0.0
#define FONT_SIZE 9
#define CIRCLE_RADIUS 10


using namespace std;

class Node
{
public:
  Node(unsigned int _num, double _posX, double _posY, wxString _lbl);
  void OnPaint(wxPaintDC * ptrDC);
  unsigned int Get_num();
  double GetX(); 
  double GetY();
  void SetXY(double, double);
  bool IsLocked();
  void Lock();
  void Unlock();

private:
  bool locked;
  double posX;
  double posY;
  wxString lbl;
  unsigned int num;
    

};



#endif //node_h
