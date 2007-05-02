// --- bitmappanel.cpp ----------------------------------------------
// (c) 2006  -  A.J. Pretorius  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------


#include "bitmappanel.h"


// -- constructors and desctructor ----------------------------------


// ---------------------------------------
BitmapPanel::BitmapPanel( 
    wxWindow* parent,
    wxSize size,
    wxString path )
    : wxPanel(
        parent,
        wxID_ANY )
// --------------------------------------
{
    // no resize
    this->SetClientSize( size );
    this->SetMinSize( this->GetSize() );
    this->SetMaxSize( this->GetSize() );

    // load image
    bitmap = new wxBitmap( 
        path, 
        wxBITMAP_TYPE_BMP );
    if ( bitmap->Ok() != true )
    {
        wxString msg;
        msg.Append( wxT("Error opening file: " ));
        msg.Append( path );
        wxLogError( msg );
    }
}


// ------------------------
BitmapPanel::~BitmapPanel()
// ------------------------
{
    delete bitmap;
    bitmap = NULL;
}


// -- event handlers ------------------------------------------------


// ------------------------------------------------
void BitmapPanel::onEvtPaint( wxPaintEvent& event )
// ------------------------------------------------
{
    if ( bitmap->Ok() == true )
    {
        wxClientDC dc( this );

        // aparently BeginDrawing() and EndDrawing() are depricated and do nothing
        //dc.BeginDrawing();
        dc.DrawBitmap( *bitmap, 0, 0, false );
        //dc.EndDrawing();
    }
}


// -- event table ---------------------------------------------------


BEGIN_EVENT_TABLE( BitmapPanel, wxPanel )
    EVT_PAINT( BitmapPanel::onEvtPaint )
END_EVENT_TABLE()


// -- end -----------------------------------------------------------
