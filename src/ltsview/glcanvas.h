#ifndef GLCANVAS_H
#define GLCANVAS_H

#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <wx/image.h>
#ifdef __APPLE__
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
#else
    #include <GL/gl.h>
    #include <GL/glu.h>
#endif
#include <cmath>
#include "mediator.h"
#include "utils.h"
#include "ids.h"

using namespace IDs;
using namespace Utils;

class GLCanvas: public wxGLCanvas
{
  public:
    GLCanvas( Mediator* owner, wxWindow* parent, wxWindowID id, 
	const wxPoint &pos = wxDefaultPosition,
	const wxSize &size = wxDefaultSize );
    ~GLCanvas();

    void      display( bool swapBuffers = true );
    void      enableDisplay();
    void      disableDisplay();
    void      initialize();
    void      onMouseDown( wxMouseEvent& event );
    void      onMouseEnter( wxMouseEvent& event );
    void      onMouseMove( wxMouseEvent& event);
    void      onMouseUp( wxMouseEvent& event );
    void      onMouseWheel( wxMouseEvent& event );
    void      onPaint( wxPaintEvent& event );
    void      onSize( wxSizeEvent& event );
    void      OnEraseBackground( wxEraseEvent& event );
    void      resetView();
    void      reshape();
    void      setActiveTool( int t );
    void      setDefaultPosition( float structWidth, float structHeight );

  private:
    int	      activeTool;
    float     angleX;
    float     angleY;
    int	      currentTool;
    float     startPosZ;
    float     startPosZDefault;
    bool      displayAllowed;
    float     farClippingPlane;
    float     farClippingPlaneDefault;
    Mediator* mediator;
    Point3D   moveVector;
    int	      oldMouseX;
    int	      oldMouseY;
    
    void      determineCurrentTool( wxMouseEvent& event );
    void      setMouseCursor();

    DECLARE_EVENT_TABLE()
};

#endif
