#ifndef node_h
#define node_h

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/wxprec.h"
#include "wx/dataobj.h"
#include <wx/dcps.h>
#include <string>

#define POS_NODE_LBL_X 3.0
#define POS_NODE_LBL_Y 5.0
#define FONT_SIZE 9
#define CIRCLE_RADIUS 10


using namespace std;

class Node
{
public:
  Node(unsigned int _num, double _posX, double _posY, wxString _lbl, bool _initState);
  void OnPaint(wxDC * ptrDC);
  unsigned int Get_num();
  double GetX(); 
  double GetY();
  int get_radius();
  string Get_lbl();
  void SetXY(double, double);
  void ForceSetXY(double, double);//set the x and y even if the node is locked
  bool IsLocked();
  void Lock();
  void Unlock();
  bool LabelVisible();
  void ShowLabels();
  void HideLabels();
  void SetRadius(int);
  bool IsInitState();
  wxColour get_node_colour();
  wxColour get_border_colour();
  void set_node_colour(wxColour colour);
  void set_border_colour(wxColour colour);
  void reset_border_colour();
  //TODO (CT): Maybe make a reset for node colour?

private:
  int radius;
  bool locked;
  bool labelsVisible;
  double posX;
  double posY;
  wxString lbl;
  unsigned int num;
  bool initState;
  wxColour node_colour;
  wxColour border_colour;

};



#endif //node_h
