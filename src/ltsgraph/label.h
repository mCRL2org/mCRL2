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
    Label(wxString _name, wxPoint _pos);
    void OnPaint(wxPaintDC * ptrDC); 
	void SetXY(wxCoord _x, wxCoord _y);

private:
    wxString name;
    wxPoint pos;

};

#endif //label_h
