#ifndef edge_h
#define edge_h

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/wxprec.h"
#include "label.h"

using namespace std;

class Edge
{
public:
    Edge();
    Edge(unsigned int _numN1, unsigned int _numN2, wxPoint _pos1, wxPoint _pos2, wxString _Slbl);
    void OnPaint(wxPaintDC * ptrDC);
	unsigned int Get_numN1();
	unsigned int Get_numN2();
	void Set_pos1(wxCoord _x, wxCoord _y);
	void Set_pos2(wxCoord _x, wxCoord _y);
	int GetXpos1();
	int GetYpos1();
	int GetXpos2();
	int GetYpos2();
	void Lock1();
	void Unlock1();
	void Lock2();
	void Unlock2();

private:
	bool locked1;
	bool locked2;
	unsigned int numN1;
	unsigned int numN2;
    wxPoint pos1;
    wxPoint pos2;
    Label * lbl;
    

};

#endif //edge_h
