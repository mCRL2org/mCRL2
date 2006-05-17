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
    Edge(unsigned int _numN1, unsigned int _numN2, double _pos1X, double _pos1Y, double _pos2X, double _pos2Y, wxString _Slbl);
    void OnPaint(wxPaintDC * ptrDC);
	unsigned int Get_numN1();
	unsigned int Get_numN2();
	void Set_pos1(double _x, double _y);
	void Set_pos2(double _x, double _y);
	double GetXpos1();
	double GetYpos1();
	double GetXpos2();
	double GetYpos2();
	void Lock1();
	void Unlock1();
	void Lock2();
	void Unlock2();

private:
	bool locked1;
	bool locked2;
	unsigned int numN1;
	unsigned int numN2;
    double pos1X;
	double pos1Y;
    double pos2X;
	double pos2Y;
    Label * lbl;
    

};

#endif //edge_h
