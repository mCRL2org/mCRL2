// ---------------------------------
// garagecanvas.cpp
// (c) 2005
// A.J. Pretorius
//
// Dept of Math and Computer Science
// Technische Universitiet Eindhoven
// a.j.pretorius@tue.nl
// ---------------------------------
//
// This class extends wxGLCanvas and
//    handles mouse, paint and
//    resize events.
//
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#include "garagecanvas.h"




// ------------------------------------------
GarageCanvas::GarageCanvas( wxWindow* parent,
                    int xPos,
                    int yPos,
                    int width,
                    int height       )
        : wxGLCanvas( parent,
                      -1,
                      wxPoint(xPos, yPos),
                      wxSize(width, height) )
// ------------------------------------------
{
}



// --------------------------
GarageCanvas::~GarageCanvas()
// --------------------------
{
}


// ------------------------------------------
void GarageCanvas::InitialiseCanvas()
// ------------------------------------------
{
  //initialise floor state
  for ( int i = 0; i < 3; ++i )
  {
    for ( int j = 0; j < 20; ++j )
      floorState[i][j] = -1;
  }
  //initialise shuttle state
  for ( int i = 0; i < 3; ++i )
  {
    for ( int j = 0; j < 2; ++j )
      shuttleState[i][j] = -1;
  }
  //initialise lift state
  liftHeight = 0;
  liftOccupied = -1;
}



// -----------------------------------------------
void GarageCanvas::SetDataStructs( int floor[3][20],
                               int shuttle[3][2],
                               int liftH,
                               int liftO)
// -----------------------------------------------
{
  // Update state of floor
  for ( int i = 0; i < 3; ++i )
  {
    for ( int j = 0; j < 20; ++j )
      floorState[i][j] = floor[i][j];
  }

  // Update state of shuttles
  {
  for ( int i = 0; i < 3; ++i )
  {
    for ( int j = 0; j < 2; ++j )
    shuttleState[i][j] = shuttle[i][j];
  }
  }

  // Update lift state 
  liftHeight = liftH;
  liftOccupied = liftO;

}




// ------------------
void GarageCanvas::Draw()
// ------------------
//  This function sets
//    up and draws
//    to the OpenGL
//    canvas.
//
// ^^^^^^^^^^^^^^^^^^
{
  // 1. Get current size of the GarageCanvas
  int width, height;
  this->GetSize( &width, &height );

  // 2. Set this as current GL context
  this->SetCurrent();

  // 3. Set up viewing volume
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  // Casting to GLdouble is important to ensure smooth transitions
  GLdouble aspect = (GLdouble)width / (GLdouble)height;
  // Specify clipping rectangle in the order ( left, right, bottom, top )
  if ( aspect >= 1 )
    // width > height
    gluOrtho2D( -1*aspect, 1*aspect, -1, 1 );
  else
    // height >= width
    gluOrtho2D( -1, 1, -1/aspect, 1/aspect );

  // 4. Switch back to the default matrix mode
  glMatrixMode( GL_MODELVIEW );

  // 5. Visualize transition system:
  VisUtils::clear();

  // 6. Set up modelview matrix (scale, translate)
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

// ###
  double scaleFactor;
  double garageRatio = ( 11*VisUtils::getColInterval() ) / ( 4*VisUtils::getRowInterval() );
  if ( aspect >= garageRatio )
    scaleFactor = 4*VisUtils::getRowInterval();
  else
    scaleFactor = 11*VisUtils::getColInterval();

  glScalef( 
        2.0/scaleFactor,
        2.0/scaleFactor,
        2.0/scaleFactor );
  glTranslatef( -10.0*VisUtils::getColInterval()/2,
            3.0*VisUtils::getRowInterval()/2,
            0.0 );

  // 7. Draw garage
  DrawGarage();

  // 8. Force all issued OpenGL commands to finish
  glFlush();

  // 9. Swap buffers and display results of OpenGL commands
  this->SwapBuffers();
}




// -------------------------------------------
// This is the event table macro for handling
//    events with wxWidgets.
// -------------------------------------------

BEGIN_EVENT_TABLE( GarageCanvas, wxGLCanvas )
  EVT_PAINT( GarageCanvas::OnEvtPaint )
  EVT_SIZE( GarageCanvas::OnEvtSize )
END_EVENT_TABLE()

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^




// ---------------------------------------------
void GarageCanvas::OnEvtPaint( wxPaintEvent& )
// ---------------------------------------------
{
  // Get device context for only that
  //    part of the screen that needs
  //    to be repainted
  wxPaintDC dc( this );
  Draw();
}




// -------------------------------------------
void GarageCanvas::OnEvtSize( wxSizeEvent& event )
// -------------------------------------------
// This function ensures that the viewport is
//    adjusted to match the GarageCanvas when it
//    is resized.
//
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
{
  wxGLCanvas::OnSize(event);
  // Gets size of GarageCanvas
  int width;
  int height;
  this->GetSize( &width, &height );

  // Set this tocurrent GL context
  this->SetCurrent();

  // Set up viewport to match canvas size
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  glViewport( 0, 0, width, height );
  //Force redraw because the paint event is not triggered in Windows
  Draw();
}



// ------------------------
void GarageCanvas::DrawGarage()
// ------------------------
{
  // Draw walls
  VisUtils::drawWalls( 3, 10 );

  // Draw parking bays
  int row;
  int col;
  int part;
  {
  for ( int i = 0; i < 3; ++i )
  {
    // Current row
    row = i;
    for ( int j = 0; j < 20; ++j )
    {
      // Current col
      col = j/2;

      // Current part of cell
      if ( j%2 == 0 )
        part = 0;
      else
        part = 1;

      // Color cell
      if ( floorState[i][j] == -1 )
        VisUtils::drawHalfBayNA( row, col, part);
      else if ( floorState[i][j] == 0 )
        VisUtils::drawHalfBayFree( row, col, part );
      else if ( floorState[i][j] == 1 )
        VisUtils::drawHalfBayOccupied( row, col, part );
    }
  }
  }

  // Draw shuttles
  {
  for ( int i = 0; i < 3; ++i )
  {
	  for ( int j = 0; j < 2; ++j )
	  {
		if ( shuttleState[i][j] == 1 )
		  VisUtils::drawTiltedShuttle( i, j*9, j );
	//    else
	//      VisUtils::drawShuttle( i, j*9 );
	  }
  }
  }

  // Draw lift
  if ( liftOccupied == -1 || liftHeight == -2 )
    VisUtils::drawLiftNA( 0, 5, 1, 3, liftHeight );
  else if ( liftOccupied == 0 )
    VisUtils::drawLiftFree( 0, 5, 1, 3, liftHeight );
  else if ( liftOccupied == 1 )
    VisUtils::drawLiftOccupied( 0, 5, 1, 3, liftHeight );

}
