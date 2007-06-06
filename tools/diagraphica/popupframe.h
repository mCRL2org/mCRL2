// --- popupframe.h -------------------------------------------------
// (c) 2007  -  A.J. Pretorius  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------

#ifndef POPUPFRAME_H
#define POPUPFRAME_H

#include <vector>
using namespace std;

#include <wx/wx.h>
#include "colleague.h"
#include "mediator.h"

class PopupFrame : public Colleague, public wxFrame
{
public:
    // -- constructors and desctructor ------------------------------
    PopupFrame(
        Mediator* m,
        wxWindow* parent,
        wxWindowID id,
        wxString title,
        wxPoint position,
        wxSize size );
    virtual ~PopupFrame();

protected:
    // -- event handlers --------------------------------------------
    virtual void OnClose( wxCloseEvent& event );
    
private:
    // -- declare event table ---------------------------------------
    DECLARE_EVENT_TABLE()
};

#endif

// -- end -----------------------------------------------------------
