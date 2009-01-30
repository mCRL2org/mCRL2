// Author(s): Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file glcanvas.h
/// \brief OpenGL rendering canvas

#ifndef GLCANVAS_H
#define GLCANVAS_H

#include <wx/glcanvas.h>

#ifndef LTSGRAPH_H
  #include "ltsgraph.h"
#else
  class LTSGraph;
#endif

#ifndef VISUALIZER_H
  #include "visualizer.h"
#else
  class Visualizer;
#endif

class GLCanvas : public wxGLCanvas
{
  public:
    GLCanvas(
          LTSGraph* app, 
          wxWindow* parent, 
          const wxSize &size = wxDefaultSize, 
          int* attribList = NULL);

    ~GLCanvas();

    void display();
    void initialize();
    void setVisualizer(Visualizer *vis);

    void onPaint(wxPaintEvent& event);
    void onSize(wxSizeEvent& event);
    void onEraseBackground(wxEraseEvent& event);
    void reshape();
   
    // Mouse event handlers
    void onMouseEnter(wxMouseEvent& event);
    void onMouseLeave(wxFocusEvent& event);
    void onMouseLftDown(wxMouseEvent& event);
    void onMouseRgtDown(wxMouseEvent& event);
    void onMouseMove(wxMouseEvent& event);
    void onMouseLftUp(wxMouseEvent& event);
    void onMouseRgtUp(wxMouseEvent& event);


    double getPixelSize();
    double getAspectRatio() const;
  private:
    LTSGraph* owner;
    Visualizer* visualizer;
    bool displayAllowed;
    double scaleFactor; 
    int oldX, oldY;
    
    void getSize(double & width, double & height);

    void pickObjects(int x, int y, bool ctrl);
    void processHits(const GLint hits, GLuint * buffer, bool ctrl);

  DECLARE_EVENT_TABLE()
};

#endif //GLCANVAS_H
