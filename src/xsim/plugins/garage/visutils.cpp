// ---------------------------------
// visutils.cpp
// (c) 2005
// A.J. Pretorius
//
// Dept of Math and Computer Science
// Technische Universitiet Eindhoven
// a.j.pretorius@tue.nl
// ---------------------------------
//
// This function provides utility
//    functions for drawing with
//    OpenGL. In it, some global
//    variables such as background
//    color are also defined. The
//    functions are static, so no
//    objects need to be created
//    to invoke them on.
// Some last minute hacks were put
//    in for a better presentation,
//    so the coding style is 
//    somewhat poor.
//
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


#include "visutils.h"




// ------------------------------------------------------------------
// This initializes the static struct variables used by VisUtils to
//    their default values.
// ------------------------------------------------------------------

double VisUtils::colInterval = 2.0;
double VisUtils::rowInterval = 4.0;

Color VisUtils::clearColor =         { 1.0, 1.0, 1.0, 0.0 }; // White
Color VisUtils::occupiedColor =      { 0.85, 0.0, 0.0, 0.0 };// Red
Color VisUtils::freeColor =          { 0.0, 0.5, 0.0, 0.0 }; // Green
Color VisUtils::naColor =            { 0.7, 0.7, 0.7, 0.0 }; // Gray
Color VisUtils::liftOccupiedColor =  { 1.0, 0.0, 0.0, 0.0 }; // Red
Color VisUtils::liftFreeColor =      { 0.0, 0.8, 0.0, 0.0 }; // Green
Color VisUtils::tiltedShuttleColor = { 0.7, 0.7, 0.7, 0.0 }; // Gray
Color VisUtils::gridColor =          { 0.0, 0.0, 0.0, 0.0 }; // Black

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^




// -------------------
void VisUtils::clear()
// -------------------
{
  // Set clear color
  glClearColor( clearColor.red,
                clearColor.green,
                clearColor.blue,
                0.0              );

  // Clear canvas
  glClear( GL_COLOR_BUFFER_BIT );
}



// ---------------------------------------------
void VisUtils::drawLiftNA( int row,
                           int col,
                           int side,
                           int numLevels,
                           int liftLevel )
// ---------------------------------------------
// This function draws a lift shaft at
//    the cell (row, col+side) where side
//    indicates either the left ('0') or right
//    ('0') vertical half of the cell
//    (row, col). This makes it possible for a
//    lift shaft to be "in between" two cells.
//    It also indicates the current level of
//    lift (0=street, -1=rotate, -2=basement).
//
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
{
  // Set color
  glColor3f( naColor.red,
             naColor.green,
             naColor.blue  );

  // Draw lift
  VisUtils::drawLift( row,
                      col,
                      side,
                      numLevels,
                      liftLevel );

}

// ---------------------------------------------
void VisUtils::drawLiftOccupied( int row,
                                 int col,
                                 int side,
                                 int numLevels,
                                 int liftLevel )
// ---------------------------------------------
// This function draws an occupied lift shaft at
//    the cell (row, col+side) where side
//    indicates either the left ('0') or right
//    ('0') vertical half of the cell
//    (row, col). This makes it possible for a
//    lift shaft to be "in between" two cells.
//    It also indicates the current level of
//    lift (0=street, -1=rotate, -2=basement).
//
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
{
  // Set color
  glColor3f( liftOccupiedColor.red,
             liftOccupiedColor.green,
             liftOccupiedColor.blue  );

  // Draw lift
  VisUtils::drawLift( row,
                      col,
                      side,
                      numLevels,
                      liftLevel );

}




// -----------------------------------------
void VisUtils::drawLiftFree( int row,
                             int col,
                             int side,
                             int numLevels,
                             int liftLevel )
// -----------------------------------------
// This function draws a free lift shaft at
//    the cell (row, col+side) where side
//    indicates either the left ('0') or
//    right ('0') vertical half of the
//    cell (row, col). This makes it
//    possible for a lift shaft to be
//    "in between" two cells. It also
//    indicates the current level of lift
//    (0=street, -1=rotate, -2=basement).
//
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
{
  // Set color
  glColor3f( liftFreeColor.red,
             liftFreeColor.green,
             liftFreeColor.blue  );

  // Draw lift
  VisUtils::drawLift( row,
                      col,
                      side,
                      numLevels,
                      liftLevel );
}




// -------------------------------------------
void VisUtils::drawHalfBayOccupied( int row,
                                    int col,
                                    int side )
// -------------------------------------------
// This function renders the half cell
//    (row, col+sice) as occupied. If side
//    is '0' it indicates the left vertical
//    half of (row, col) and if it is '1' it
//    indicates the right.
//
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
{
  // Set color
  glColor3f( occupiedColor.red,
             occupiedColor.green,
             occupiedColor.blue  );

  // Draw half bay
  VisUtils::drawHalfBay( row,
                         col,
                         side,
                         true );
}





// ---------------------------------------
void VisUtils::drawHalfBayFree( int row,
                                int col,
                                int side )
// ---------------------------------------
// This function renders the half cell
//    (row, col+sice) as free. If side
//    is '0' it indicates the left
//    vertical half of (row, col) and if
//    it is '1' it indicates the right.
//
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

{
  // Set color
  glColor3f( freeColor.red,
             freeColor.green,
             freeColor.blue  );

  // Draw half bay
  VisUtils::drawHalfBay( row,
                         col,
                         side,
                         true );
}




// ---------------------------------------
void VisUtils::drawHalfBayNA( int row,
                              int col,
                              int side )
// ---------------------------------------
// This function renders the half cell
//    (row, col+size) as not available.
//    
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
{
  // Set color
  glColor3f( naColor.red,
             naColor.green,
             naColor.blue  );

  // Draw half bay
  VisUtils::drawHalfBay( row,
                         col,
                         side,
                         false );

}




// -----------------------------------------
void VisUtils::drawTiltedShuttle( int row,
                                  int col,
                                  int side )
// -----------------------------------------
// This function should be used to indicate
//    that there is a tilted shuttle in
//    the position (row, col). It provides
//    no checking for whether (row, col)
//    is actually a valid shuttle
//    position.
//
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
{
  double x0, y0;
  double x1, y1;
  double x2, y2;
  double x3, y3;
  double diff = VisUtils::colInterval/10.0;

  if ( side == 0 )
  {
    // Tilted towards left
    x0 = col*VisUtils::colInterval-diff;
    y0 = -(row*VisUtils::rowInterval+VisUtils::rowInterval)+diff;
    x1 = x0+diff;
    y1 = y0+2*diff;
    x2 = x1;
    y2 = y1+VisUtils::rowInterval-6*diff;
    x3 = x0;
    y3 = y2+2*diff;
  }
  else // ( side == 1 )
  {
    // Tilted towards right
    x0 = col*VisUtils::colInterval+VisUtils::colInterval;
    y0 = -(row*VisUtils::rowInterval+VisUtils::rowInterval)+3*diff;
    x1 = x0+diff;
    y1 = y0-2*diff;
    x2 = x1;
    y2 = y1+VisUtils::rowInterval-2*diff;
    x3 = x0;
    y3 = y2-2*diff;
  }

  // Set fill color
  glColor3f( tiltedShuttleColor.red,
             tiltedShuttleColor.green,
             tiltedShuttleColor.blue  );
  // Draw fill
  glBegin( GL_POLYGON );
    glVertex3f( x0, y0, 0.0 );
    glVertex3f( x1, y1, 0.0 );
    glVertex3f( x2, y2, 0.0 );
    glVertex3f( x3, y3, 0.0 );
  glEnd();

  // Set border color
  glColor3f( gridColor.red,
             gridColor.green,
             gridColor.blue  );
  // Draw border
  glBegin( GL_LINE_LOOP );
    glVertex3f( x0, y0, 0.0 );
    glVertex3f( x1, y1, 0.0 );
    glVertex3f( x2, y2, 0.0 );
    glVertex3f( x3, y3, 0.0 );
  glEnd();
}




// ---------------------------------
void VisUtils::drawWalls( int rows,
                          int cols )
// ---------------------------------
{
    double stX, stY, enX, enY;
    double diff = VisUtils::colInterval/40.0;

    stX = 0.0 - diff;
    enX = cols*VisUtils::colInterval + diff;
    stY = 0.0;// + diff;
    enY = -rows*VisUtils::rowInterval;// - diff;

    // Set border color
    glColor3f( gridColor.red,
               gridColor.green,
               gridColor.blue  );
    // Draw border
    glBegin( GL_LINE_LOOP );
      glVertex3f( stX, stY, 0.0 );
      glVertex3f( stX, enY, 0.0 );
      glVertex3f( enX, enY, 0.0 );
      glVertex3f( enX, stY, 0.0 );
    glEnd();
}




// ------------------------------
double VisUtils::getColInterval()
// ------------------------------
{
  return colInterval;
}




// ------------------------------
double VisUtils::getRowInterval()
// ------------------------------
{
  return rowInterval;
}




// -----------------------------------
void VisUtils::drawHalfBay( int row,
                            int col,
                            int side,
                            bool avail )
// -----------------------------------
// This function is used as a utility
//    by the following functions:
//    1. 'drawHalfBayOccupied()'
//    2. 'drawHalfBayFree()'
//
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
{
  double lftX, topY;
  double rgtX, botY;
  double diff = VisUtils::colInterval/10.0;

  lftX = col*VisUtils::colInterval + side*(VisUtils::colInterval/2.0);
  topY = -row*VisUtils::rowInterval - 3*diff;

  rgtX = lftX+VisUtils::colInterval/2.0;
  botY = topY-VisUtils::rowInterval + 6*diff;

  if ( row == 0 && col == 5 && side == 0 )
      rgtX -= diff;
  else if ( row == 0 && col == 5 && side == 1 )
      lftX += diff;
  else if ( row == 0 && col == 6 && side == 0 )
      rgtX -= diff;
  else if ( row == 0 && col == 6 && side == 1 )
      lftX += diff;
  else if ( col == 0 && side == 0 )
      lftX += diff;
  else if ( col == 0 && side == 1 )
      rgtX -= diff;
  else if ( col == 1 && side == 0 )
      lftX += diff;
  else if ( col == 8 && side == 1 )
      rgtX -= diff;
  else if ( col == 9 && side == 0 )
      lftX += diff;
  else if ( col == 9 && side == 1 )
      rgtX -= diff;
  
  glBegin( GL_POLYGON );
    glVertex3f( lftX, botY, 0.0 );
    glVertex3f( rgtX, botY, 0.0 );
    glVertex3f( rgtX, topY, 0.0 );
    glVertex3f( lftX, topY, 0.0 );
  glEnd();

  if ( avail )
  {
    // Set border color
    glColor3f( gridColor.red,
               gridColor.green,
               gridColor.blue  );
      
    // Draw top edge
    glBegin( GL_POLYGON );
      glVertex3f( lftX, topY, 0.0 );
      glVertex3f( lftX, topY-diff, 0.0 );
      glVertex3f( rgtX, topY-diff, 0.0 );
      glVertex3f( rgtX, topY, 0.0 );
    glEnd();

    // Draw bottom edge
    glBegin( GL_POLYGON );
      glVertex3f( lftX, botY, 0.0 );
      glVertex3f( lftX, botY+diff, 0.0 );
      glVertex3f( rgtX, botY+diff, 0.0 );
      glVertex3f( rgtX, botY, 0.0 );
    glEnd();
  }
    // Set border color
    glColor3f( gridColor.red,
               gridColor.green,
               gridColor.blue  );
    
    // Draw border
    if ( side == 0 )
    {
        glBegin( GL_LINES );
          glVertex3f( rgtX, topY, 0.0 );
          glVertex3f( lftX, topY, 0.0 );
        glEnd();
        glBegin( GL_LINES );
          glVertex3f( lftX, topY, 0.0 );
          glVertex3f( lftX, botY, 0.0 );
        glEnd();
        glBegin( GL_LINES );
          glVertex3f( lftX, botY, 0.0 );
          glVertex3f( rgtX, botY, 0.0 );
        glEnd();

		if ( col == 6 )
		{
			glBegin( GL_LINES );
				glVertex3f( rgtX, topY, 0.0 );
				glVertex3f( rgtX, botY, 0.0 );
			glEnd();
		}
    }
    else if ( side == 1 )
    {
        glBegin( GL_LINES );
          glVertex3f( lftX, botY, 0.0 );
          glVertex3f( rgtX, botY, 0.0 );
        glEnd();
        glBegin( GL_LINES );
          glVertex3f( rgtX, botY, 0.0 );
          glVertex3f( rgtX, topY, 0.0 );
        glEnd();
        glBegin( GL_LINES );
          glVertex3f( rgtX, topY, 0.0 );
          glVertex3f( lftX, topY, 0.0 );
        glEnd();

		if ( row == 0 )
		{
			
			if ( col != 5 && col != 6 )
			{
				glLineStipple( 3, 0xAAAA );
				glEnable( GL_LINE_STIPPLE );
				glBegin( GL_LINES );
					glVertex3f( lftX, topY, 0.0 );
					glVertex3f( lftX, botY, 0.0 );
				glEnd();

				glDisable( GL_LINE_STIPPLE );
			}
			else if ( col == 5 )
			{
				glBegin( GL_LINES );
					glVertex3f( lftX, topY, 0.0 );
					glVertex3f( lftX, botY, 0.0 );
				glEnd();
			}

		}
    }
}




// -------------------------------------
void VisUtils::drawLift( int row,
                         int col,
                         int side,
                         int numLevels,
                         int liftLevel )
// -------------------------------------
// This function is used as a utility by
//    the following functions:
//    1. 'drawNALift()'
//    2. 'drawOccupiedLift()'
//    3. 'drawFreeLift()'
//
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
{
  double lftX, topY;
  double rgtX, botY;
  double interv;
  double border = colInterval/10.0;
  double liftHeight;

  // Calculate coordinates
  lftX = (col*colInterval) + ((side*colInterval)/2.0) - border;
  botY = -((row + 1)*rowInterval);// + 2*border;
  rgtX = lftX + colInterval + (2*border);
  topY = botY + rowInterval;// - (2*border);

  // Calculate height of 1 level
  interv = (topY - botY)/(double)numLevels;

  liftHeight = topY + (liftLevel*interv);

  // Draw lift level
  glBegin( GL_POLYGON );
    glVertex3f( lftX, liftHeight-interv, 0.0 );
    glVertex3f( rgtX, liftHeight-interv, 0.0 );
    glVertex3f( rgtX, liftHeight,     0.0 );
    glVertex3f( lftX, liftHeight,     0.0 );
  glEnd();

  // Set color for lift grid
  glColor3f( gridColor.red,
             gridColor.green,
             gridColor.blue );

  // Draw vertical grid lines
  glBegin( GL_LINES );
    glVertex3f( lftX, botY, 0.0 );
    glVertex3f( lftX, topY, 0.0 );
  glEnd();
  glBegin( GL_LINES );
    glVertex3f( rgtX, botY, 0.0 );
    glVertex3f( rgtX, topY, 0.0 );
  glEnd();

  // Draw horizontal grid lines
  for ( int i = 0; i <= numLevels; ++i )
  {
    botY = topY-i*interv;

    glBegin( GL_LINES );
      glVertex3f( lftX, botY, 0.0 );
      glVertex3f( rgtX, botY, 0.0 );
    glEnd();
  }
}
