#ifndef GLCANVAS_H
#define GLCANVAS_H
#include <wx/wx.h>
#include <wx/glcanvas.h>
#include "mediator.h"
#include "visualizer.h"
#include "utils.h"
#include "settings.h"

class GLCanvas: public wxGLCanvas,public Subscriber {
  public:
    GLCanvas(Mediator* owner,wxWindow* parent,Settings* ss,
      const wxSize &size=wxDefaultSize, int* attribList=NULL);
    ~GLCanvas();
    void      display(bool coll_caller=false);
    void      enableDisplay();
    void      disableDisplay();
    void      getMaxViewportDims(int *w,int *h);
    unsigned char* getPictureData(int res_x,int res_y);
    void      initialize();
    void      notify(SettingID s);
    void      resetView();
    void      reshape();
    void      setActiveTool(int t);
    void      setVisualizer(Visualizer *vis);

    void      onMouseDown(wxMouseEvent& event);
    void      onMouseEnter(wxMouseEvent& event);
    void      onMouseMove(wxMouseEvent& event);
    void      onMouseUp(wxMouseEvent& event);
    void      onMouseWheel(wxMouseEvent& event);
    void      onPaint(wxPaintEvent& event);
    void      onSize(wxSizeEvent& event);
    void      OnEraseBackground(wxEraseEvent& event);

  private:
    int	      activeTool;
    float     angleX;
    float     angleY;
    int	      currentTool;
    float     startPosZ;
    bool      collectingData;
    bool      displayAllowed;
    float     farPlane;
    float     farPlaneDefault;
    bool      lightRenderMode;
    Mediator* mediator;
    Utils::Point3D moveVector;
    float     nearPlane;
    int	      oldMouseX;
    int	      oldMouseY;
    Settings  *settings;
    Visualizer *visualizer;
    
    void      determineCurrentTool(wxMouseEvent& event);
    void      setMouseCursor();

    DECLARE_EVENT_TABLE()
};

#endif
