// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file garagecanvas.h

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

#include <wx/wx.h>
#include <wx/glcanvas.h>

#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif

#undef __in_range // For STLport

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
  // Macro declaring the beginning of event table
  DECLARE_EVENT_TABLE()
private:
  void DrawGarage();

  int liftHeight;
  int liftOccupied;
  int floorState[3][20];
  int shuttleState[3][2];
};

#endif // GARAGECANVAS_H
