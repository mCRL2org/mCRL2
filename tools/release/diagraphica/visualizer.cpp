// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./visualizer.cpp

#include <iostream> // only temporary for std::clog

#include "visualizer.h"
#include "mcrl2/gui/openglwrapper.h"


Visualizer::Visualizer(
  QWidget *parent,
  Graph *graph_)
  : QGLWidget(parent),
    m_lastMouseEvent(QEvent::None, QPoint(0,0), Qt::NoButton, Qt::NoButton, Qt::NoModifier),
    m_graph(graph_)
{
  setMinimumSize(10,10);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setFocusPolicy(Qt::ClickFocus);
  clearColor = Qt::white;

  initMouse();

  m_inSelectMode = false;
  texCharOK = false;
  texCushOK = false;

  geomChanged = true;
  dataChanged = true;

  showMenu = false;
}

void Visualizer::updateGL(bool inSelectMode)
{
  m_inSelectMode = inSelectMode;
  QGLWidget::updateGL();
}

void Visualizer::paintGL()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  GLdouble aspect = (GLdouble)width() / (GLdouble)height();
  if (aspect > 1)
  {
    OpenGLWrapper::gluOrtho2D(aspect*(-1), aspect*1, -1, 1);
  }
  else
  {
    OpenGLWrapper::gluOrtho2D(-1, 1, (1/aspect)*(-1), (1/aspect)*1);
  }

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glViewport(0, 0, width(), height());

  visualize(m_inSelectMode);
  m_inSelectMode = false;
}


QSizeF Visualizer::worldSize()
// Return viewport width and height in WORLD coordinates. The 
// viewport is set up such that the shortest side has
// length 2 in world coordinates.
{
  if (height() < width())
  {
    return QSizeF(width() / (double)height() * 2.0, 2.0);
  }
  else
  {
    return QSizeF(2.0, height() / (double)width() * 2.0);
  }
}


double Visualizer::pixelSize()
// Return distance in WORLD coordinates of 1 pixel.
{
  return worldSize().width() / (double)width();
}


QPointF Visualizer::worldCoordinate(QPointF deviceCoordinate)
{
  QSizeF size = worldSize();
  double pixel = pixelSize();
  return QPointF(-0.5 * size.width() + deviceCoordinate.x() * pixel, 0.5 * size.height() - deviceCoordinate.y() * pixel);
}


void Visualizer::setClearColor(
  const double& r,
  const double& g,
  const double& b)
{
  clearColor = QColor::fromRgbF(r, g, b);
}


// -- visualization functions ---------------------------------------


void Visualizer::setGeomChanged(const bool& flag)
{
  geomChanged = flag;
}


void Visualizer::setDataChanged(const bool& flag)
{
  geomChanged = flag;
  dataChanged = flag;
}


// -- event handlers ------------------------------------------------


void Visualizer::handleSizeEvent()
{
  geomChanged = true;
}

void Visualizer::handleMouseEvent(QMouseEvent* e)
{
  m_mouseDragReleased = false;
  if (!m_mouseDrag && e->buttons() != Qt::NoButton && e->type() == QEvent::MouseMove)
  {
    m_mouseDrag = true;
    m_mouseDragStart = m_lastMouseEvent.pos();
  }
  if (m_mouseDrag && e->buttons() == Qt::NoButton)
  {
    m_mouseDrag = false;
    m_mouseDragReleased = true;
  }
  m_lastMouseEvent = QMouseEvent(e->type(), e->pos(), e->globalPos(), e->button(), e->buttons(), e->modifiers());
}

void Visualizer::handleKeyEvent(QKeyEvent* e)
{
  if (e->type() == QEvent::KeyPress)
  {
    m_lastKeyCode = Qt::Key(e->key());
  }
  else
  {
    m_lastKeyCode = Qt::Key_unknown;
  }
}


// -- protected utility functions -----------------------------------


void Visualizer::clear()
{
  VisUtils::clear(clearColor);
}


void Visualizer::initMouse()
{
  m_lastMouseEvent = QMouseEvent(QEvent::None, QPoint(0,0), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
  m_mouseDrag = false;
  m_mouseDragStart = QPoint(0,0);
}


void Visualizer::startSelectMode(
  GLint /*hits*/,
  GLuint selectBuf[],
  double pickWth,
  double pickHgt)
{
  GLint viewport[4];

  // ( x, y, width, height )
  glGetIntegerv(GL_VIEWPORT, viewport);

  glSelectBuffer(512, selectBuf);
  // selection mode
  (void) glRenderMode(GL_SELECT);

  glInitNames();

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();

  OpenGLWrapper::gluPickMatrix(m_lastMouseEvent.x(), // center x
          viewport[3] - m_lastMouseEvent.y(), // center y
          pickWth,    // picking width
          pickHgt,    // picking height
          viewport);

  // casting to GLdouble ensures smooth transitions
  GLdouble aspect = (GLdouble)width() / (GLdouble)height();

  // specify clipping rectangle ( left, right, bottom, top )
  if (aspect > 1)
    // width > height
  {
    OpenGLWrapper::gluOrtho2D(aspect*(-1), aspect*1, -1, 1);
  }
  else
    // height >= width
  {
    OpenGLWrapper::gluOrtho2D(-1, 1, (1/aspect)*(-1), (1/aspect)*1);
  }

  glMatrixMode(GL_MODELVIEW);
}


void Visualizer::finishSelectMode(
  GLint hits,
  GLuint selectBuf[])
{
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  hits = glRenderMode(GL_RENDER);
  processHits(hits, selectBuf);
}


void Visualizer::genCharTex()
{
  VisUtils::genCharTextures(
    texCharId,
    texChar);
  texCharOK = true;
}


void Visualizer::genCushTex()
{
  VisUtils::genCushTextures(
    texCushId,
    texCush);
  texCushOK = true;
}
