// ---------------------------------
// garagecanvas.h
// (c) 2005
// A.J. Pretorius
//
// Dept of Math and Computer Science
// Technische Universitiet Eindhoven
// a.j.pretorius@tue.nl
// ---------------------------------
//
// This class extends wxGLCanvas and
//   handles resize and paint
//   events.
//
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#ifndef GARAGECANVAS_H
#define GARAGECANVAS_H

// For compilers that support precompiled headers
#include <wx/wxprec.h>
// Else include 'wx/wx.h'
#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif
#include <wx/glcanvas.h>

#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif

#include "visutils.h"




class GarageCanvas : public wxGLCanvas
{
public:
  GarageCanvas( wxWindow* parent,
            int xPos,
            int yPos,
            int width,
            int height       );
  ~GarageCanvas();

  void InitialiseCanvas();
  void SetDataStructs( int floor[3][20],
                       int shuttle[3][2],
                       int liftH,
                       int liftO);
  void Draw();

  void OnEvtPaint( wxPaintEvent& event );
  void OnEvtSize( wxSizeEvent& event );
protected:
  // Macro declaring the begining of event table
  DECLARE_EVENT_TABLE()
private:
  void DrawGarage();

  int liftHeight;
  int liftOccupied;
  int floorState[3][20];
  int shuttleState[3][2];
};

#endif // GARAGECANVAS_H
