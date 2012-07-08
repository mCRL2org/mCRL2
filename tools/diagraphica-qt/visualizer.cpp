// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./visualizer.cpp

#include "wx.hpp" // precompiled headers

#include <iostream> // only temporary for std::clog

#include "visualizer.h"


// -- constructors and destructor -----------------------------------


Visualizer::Visualizer(
  Mediator* m,
  Graph* g,
  GLCanvas* c)
  : Colleague(m),
    m_lastMouseEvent(QEvent::None, QPoint(0,0), Qt::NoButton, Qt::NoButton, Qt::NoModifier)
{
  clearColor = Qt::white;

  initMouse();

  graph  = g;
  canvas = c;

  texCharOK = false;
  texCushOK = false;

  geomChanged = true;
  dataChanged = true;

  showMenu = false;
}


Visualizer::~Visualizer()
{
  graph  = NULL;
  canvas = NULL;
}


// -- set functions -------------------------------------------------


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
  m_lastMouseEvent = QMouseEvent(e->type(), e->pos(), e->globalPos(), e->button(), e->buttons(), e->modifiers());
  if (!m_mouseDrag && e->buttons() != Qt::NoButton)
  {
    m_mouseDrag = true;
    m_mouseDragStart = e->pos();
  }
  if (m_mouseDrag && e->buttons() == Qt::NoButton)
  {
    m_mouseDrag = false;
  }
}

void Visualizer::handleMouseLeaveEvent()
{
  initMouse();
}

void Visualizer::handleKeyEvent(QKeyEvent* e)
{
  if (e->type() == QEvent::KeyPress)
  {
    m_lastKeyCode = e->key();
  }
  else
  {
    m_lastKeyCode = -1;
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

  gluPickMatrix(
    (GLdouble) m_lastMouseEvent.x(),
    (GLdouble)(viewport[3]-m_lastMouseEvent.y()),
    pickWth,    // picking width
    pickHgt,    // picking height
    viewport);

  // get current size of canvas
  int width, height;
  canvas->GetSize(&width, &height);

  // casting to GLdouble ensures smooth transitions
  GLdouble aspect = (GLdouble)width / (GLdouble)height;

  // specify clipping rectangle ( left, right, bottom, top )
  if (aspect > 1)
    // width > height
  {
    gluOrtho2D(aspect*(-1), aspect*1, -1, 1);
  }
  else
    // height >= width
  {
    gluOrtho2D(-1, 1, (1/aspect)*(-1), (1/aspect)*1);
  }

  glMatrixMode(GL_MODELVIEW);
}


void Visualizer::finishSelectMode(
  GLint hits,
  GLuint selectBuf[])
{
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glFlush();

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


// -- end -----------------------------------------------------------
