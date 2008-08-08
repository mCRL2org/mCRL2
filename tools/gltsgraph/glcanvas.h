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
    void onMouseLftDown(wxMouseEvent& event);
    void onMouseRgtDown(wxMouseEvent& event);
    void onMouseMove(wxMouseEvent& event);


    double getPixelSize();

  private:
    LTSGraph* owner;
    Visualizer* visualizer;
    bool displayAllowed;
    double scaleFactor; 
    int oldX, oldY;
    
    void getSize(double & width, double & height);

    void pickObjects(int x, int y);
    void processHits(const GLint hits, GLuint * buffer);

  DECLARE_EVENT_TABLE()
};

#endif //GLCANVAS_H
