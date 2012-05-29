// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef GLCANVAS_H
#define GLCANVAS_H

#include <QColor>
#include <QObject>

#include <wx/wx.h>
#include <wx/glcanvas.h>
#include "enums.h"
#include "ltsmanager.h"
#include "settings.h"
#include "vectors.h"

class Visualizer;

class GLCanvas: public QObject, public wxGLCanvas
{
  Q_OBJECT

  public:
    GLCanvas(wxWindow* parent,Settings* ss,LtsManager* ltsManager_,
             const wxSize& size=wxDefaultSize, int* attribList=NULL);
    ~GLCanvas() {}

  public slots:
    void display(bool coll_caller=false, bool selecting=false);
    void resetView();

  public:
    void enableDisplay();
    void disableDisplay();
    void getMaxViewportDims(int* w,int* h);
    unsigned char* getPictureData(int res_x,int res_y);
    void initialize();
    void reshape();
    void setActiveTool(int t);
    void setVisualizer(Visualizer* vis);

    void onMouseDown(wxMouseEvent& event);
    void onMouseEnter(wxMouseEvent& event);
    void onMouseMove(wxMouseEvent& event);
    void onMouseUp(wxMouseEvent& event);
    void onMouseWheel(wxMouseEvent& event);
    void onMouseDClick(wxMouseEvent& event);
    void onPaint(wxPaintEvent& event);
    void onSize(wxSizeEvent& event);
    void onEraseBackground(wxEraseEvent& event);

    void setCurrent();

  public slots:
    void setBackground(QColor value);

  signals:
    void renderingStarted();
    void renderingFinished();

  private:
    int activeTool;
    float angleX;
    float angleY;
    int currentTool;
    float startPosZ;
    bool collectingData;
    bool displayAllowed;
    float farPlane;
    float farPlaneDefault;
    bool lightRenderMode;
    Vector3D moveVector;
    float nearPlane;
    int oldMouseX;
    int oldMouseY;
    Settings* settings;
    LtsManager *ltsManager;
    Visualizer* visualizer;
    PickState selectedType;

    void determineCurrentTool(wxMouseEvent& event);
    void setMouseCursor();

    // Functions for processing hits
    void processHits(const GLint hits, GLuint* buffer, bool doubleC);
    void pickObjects(int x, int y, bool doubleC);

    DECLARE_EVENT_TABLE()
};

#endif
