#ifndef node_h
#define node_h

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/wxprec.h"
#include "wx/dataobj.h"
#include "label.h"


#define CIRCLE_RADIUS 10


using namespace std;

class Node
{
public:
    Node(unsigned int _num, double _posX, double _posY, wxString _Slbl);
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
    Label * lbl;
    unsigned int num;
    

};

#endif //node_h
