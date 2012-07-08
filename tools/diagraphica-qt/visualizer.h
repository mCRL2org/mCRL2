// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./visualizer.h

#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <QtCore>
#include <QtGui>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/glu.h>
#endif
#include <cstddef>
#include "colleague.h"
#include "glcanvas.h"
#include "graph.h"
#include "visutils.h"

class Visualizer : public Colleague
{
  public:
    // -- constructors and destructor -------------------------------
    Visualizer(
      Mediator* m,
      Graph* g,
      GLCanvas* c);
    virtual ~Visualizer();

    // -- set functions ---------------------------------------------
    virtual void setClearColor(
      const double& r,
      const double& g,
      const double& b);

    // -- visualization functions -----------------------------------
    virtual void visualize(const bool& inSelectMode) = 0;
    virtual void setGeomChanged(const bool& flag);
    virtual void setDataChanged(const bool& flag);
    /*
    virtual void animate() = 0;
    */

    // -- event handlers --------------------------------------------
    virtual void handleSizeEvent();

    virtual void handleMouseEvent(QMouseEvent* e);
    virtual void handleWheelEvent(QWheelEvent* /*e*/) { }
    virtual void handleMouseEnterEvent() { }
    virtual void handleMouseLeaveEvent();
    virtual void handleKeyEvent(QKeyEvent* e);

  protected:
    // -- protected utility functions -------------------------------
    virtual void clear();
    virtual void initMouse();

    virtual void startSelectMode(
      GLint hits,
      GLuint selectBuf[],
      double pickWth,
      double pickHgt);
    virtual void finishSelectMode(
      GLint hits,
      GLuint selectBuf[]);

    void genCharTex();
    void genCushTex();

    // -- hit detection ---------------------------------------------
    virtual void processHits(
      GLint hits,
      GLuint buffer[]) = 0;

    // -- mouse -----------------------------------------------------

    QMouseEvent m_lastMouseEvent;
    bool m_mouseDrag;
    QPoint m_mouseDragStart;

    int m_lastKeyCode;

    bool showMenu;

    // -- data members ----------------------------------------------
    QColor clearColor;

    Graph*    graph;  // association
    GLCanvas* canvas; // association

    // -- flags -----------------------------------------------------
    bool geomChanged; // canvas resized
    bool dataChanged; // data has changed

    // -- character textures ----------------------------------------
    bool    texCharOK;
    GLuint  texCharId[CHARSETSIZE];
    GLubyte texChar[CHARSETSIZE][CHARHEIGHT* CHARWIDTH];

    // -- cushion texture -------------------------------------------
    bool    texCushOK;
    GLuint  texCushId;
    float   texCush[CUSHSIZE];
};

#endif

// -- end -----------------------------------------------------------
