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
#include <QGLWidget>

#ifdef __APPLE__
# include <GLUT/glut.h>
#else
# ifdef WIN32
#  define NOMINMAX
#  include <windows.h>
# endif
# include <GL/glu.h>
#endif
#include <cstddef>
#include "graph.h"
#include "visutils.h"

class Visualizer : public QGLWidget
{
  Q_OBJECT

  public:
    // -- constructors and destructor -------------------------------
    Visualizer(
      QWidget *parent,
      Graph *m_graph);
    virtual ~Visualizer() {}

    virtual void paintGL();

    QSizeF worldSize();
    double pixelSize();
    QPointF worldCoordinate(QPointF deviceCoordinate);

    // -- set functions ---------------------------------------------
    virtual void setClearColor(
      const double& r,
      const double& g,
      const double& b);

    // -- visualization functions -----------------------------------
    virtual void visualize(const bool& inSelectMode) = 0;
    virtual void setGeomChanged(const bool& flag);
    virtual void setDataChanged(const bool& flag);

    // -- event handlers --------------------------------------------
    virtual void handleSizeEvent();

    virtual void handleMouseEvent(QMouseEvent* e);
    virtual void handleWheelEvent(QWheelEvent* /*e*/) { }
    virtual void handleMouseEnterEvent() { }
    virtual void handleMouseLeaveEvent() { }
    virtual void handleKeyEvent(QKeyEvent* e);

    virtual void enterEvent(QEvent *event) { handleMouseEnterEvent(); QGLWidget::enterEvent(event); }
    virtual void leaveEvent(QEvent *event) { handleMouseLeaveEvent(); QGLWidget::leaveEvent(event); }
    virtual void keyPressEvent(QKeyEvent *event) { handleKeyEvent(event); QGLWidget::keyPressEvent(event); }
    virtual void keyReleaseEvent(QKeyEvent *event) { handleKeyEvent(event); QGLWidget::keyReleaseEvent(event); }
    virtual void wheelEvent(QWheelEvent *event) { handleWheelEvent(event); QGLWidget::wheelEvent(event); }
    virtual void mouseMoveEvent(QMouseEvent *event) { handleMouseEvent(event); QGLWidget::mouseMoveEvent(event); }
    virtual void mousePressEvent(QMouseEvent *event) { handleMouseEvent(event); QGLWidget::mousePressEvent(event); }
    virtual void mouseReleaseEvent(QMouseEvent *event) { handleMouseEvent(event); QGLWidget::mouseReleaseEvent(event); }
    virtual void resizeEvent(QResizeEvent *event) { handleSizeEvent(); QGLWidget::resizeEvent(event); }

    QSize sizeHint() { return QSize(200,200); } // Reimplement to change preferred size
  public slots:
    void updateGL(bool inSelectMode = false);

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

    bool m_inSelectMode;

    QMouseEvent m_lastMouseEvent;   // The latest received event
    bool m_mouseDrag;               // The mouse is being dragged
    bool m_mouseDragReleased;       // The cursor was released after dragging
    QPoint m_mouseDragStart;        // The position where the drag started, only valid if (m_mouseDrag or m_mouseDragReleased)

    Qt::Key m_lastKeyCode;

    bool showMenu;

    QColor clearColor;

    Graph *m_graph;

    bool geomChanged; // canvas resized
    bool dataChanged; // data has changed

    bool texCharOK;
    GLuint texCharId[CHARSETSIZE];
    GLubyte texChar[CHARSETSIZE][CHARHEIGHT* CHARWIDTH];

    bool texCushOK;
    GLuint texCushId;
    float texCush[CUSHSIZE];
};

#endif
