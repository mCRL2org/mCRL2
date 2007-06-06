// --- bitmappanel.h ------------------------------------------------
// (c) 2007  -  A.J. Pretorius  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------

#ifndef BITMAPPANEL_H
#define BITMAPPANEL_H

#include <vector>
using namespace std;

#include <wx/wx.h>
#include <wx/image.h>

class BitmapPanel : public wxPanel
{
public:
    // -- constructors and desctructor ------------------------------
    BitmapPanel( 
        wxWindow* parent,
        wxSize size,
        const char* const* image );
    virtual ~BitmapPanel();
    
protected:
    // -- event handlers --------------------------------------------
    void onEvtPaint( wxPaintEvent& event );

private:
    // -- data members ----------------------------------------------
    wxBitmap* bitmap;
    
    // -- declare event table ---------------------------------------
    DECLARE_EVENT_TABLE()
};

#endif

// -- end -----------------------------------------------------------
