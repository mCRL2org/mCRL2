#ifndef label_h
#define label_h


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/wxprec.h"


using namespace std;

class Label
{
public:
    Label();
    Label(wxString _name, double _x, double _y);
    void OnPaint(wxPaintDC * ptrDC); 
	void SetXY(double _x, double _y);

private:
    wxString name;
    double posX;
	double posY;

};

#endif //label_h
