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
    Node(unsigned int _num, wxPoint _pos, wxString _Slbl);
    void OnPaint(wxPaintDC * ptrDC);
    unsigned int Get_num();
    wxCoord GetX(); 
    wxCoord GetY();
	void SetXY(wxCoord, wxCoord);
	bool IsLocked();
	void Lock();
	void Unlock();

private:
	bool locked;
    wxPoint pos;
    Label * lbl;
    unsigned int num;
    

};

#endif //node_h
