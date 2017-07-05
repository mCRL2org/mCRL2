// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./colorchooser.cpp

#include "colorchooser.h"

using namespace std;

static const double handleSize = 5.0;

ColorChooser::ColorChooser(QWidget *parent, DOF *dof, QList<double> *yCoordinates, ColorChooser::ColorType type):
  Visualizer(parent, 0),
  m_dof(dof),
  m_yCoordinates(yCoordinates),
  m_type(type),
  m_dragIdx(NON_EXISTING)
{
  setMouseTracking(true);
  setMinimumSize(100,50);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}


void ColorChooser::visualize(const bool& inSelectMode)
{
  if (inSelectMode)
  {
    GLint hits = 0;
    GLuint selectBuf[512];
    startSelectMode(hits, selectBuf, 2.0, 2.0);

    drawPoints(inSelectMode);

    finishSelectMode(hits, selectBuf);
  }
  else
  {
    clear();
    if (m_type == HueColor)
    {
      drawColorSpectrum();
    }
    else
    {
      drawGrayScale();
    }
    drawPath(inSelectMode);
    drawPoints(inSelectMode);
  }
}

void ColorChooser::handleMouseEnterEvent()
{
  emit activated();
  // normal mode
  updateGL();
}

void ColorChooser::handleMouseLeaveEvent()
{
  emit deactivated();
  // normal mode
  updateGL();
}


void ColorChooser::handleMouseEvent(QMouseEvent* e)
{
  Visualizer::handleMouseEvent(e);
  if (e->type() != QEvent::MouseMove || e->buttons() != Qt::NoButton)
  {
    // selection mode
    updateGL(true);

    if (e->button() == Qt::LeftButton)
    {
      if (e->type() == QEvent::MouseButtonPress)
      {
        if (m_dragIdx == NON_EXISTING)
        {
          QPointF pos = worldCoordinate(e->localPos());

          double xCur = pos.x() / (0.5 * worldSize().width());
          double yCur = pos.y() / (0.5 * worldSize().height());

          m_dof->addValue((xCur + 1.0) / 2.0);
          m_yCoordinates->append(yCur);
        }
      }
      if (e->type() == QEvent::MouseButtonRelease)
      {
        m_dragIdx = NON_EXISTING;
      }
    }

    if (m_mouseDrag && e->buttons() == Qt::LeftButton)
    {
      handleDrag();
    }

    // normal mode
    updateGL();
  }
}

void ColorChooser::drawColorSpectrum()
{
  QSizeF size = worldSize();

  // calc size of bounding box
  double xLft = -0.5*size.width();
  double xRgt =  0.5*size.width();
  double yTop =  0.5*size.height();
  double yBot = -0.5*size.height();

  double xItv = (xRgt-xLft)/255.0;
  for (int i = 0; i < 255; ++i)
  {
    VisUtils::setColor(VisUtils::spectral(i / 255.0));
    VisUtils::fillRect(xLft+i*xItv, xLft+(i+1)*xItv, yTop, yBot);
  }
}


void ColorChooser::drawGrayScale()
{
  QSizeF size = worldSize();

  // calc size of bounding box
  double xLft = -0.5*size.width();
  double xRgt =  0.5*size.width();
  double yTop =  0.5*size.height();
  double yBot = -0.5*size.height();

  double xItv = (xRgt-xLft)/255.0;
  VisUtils::enableBlending();
  for (int i = 0; i < 255; ++i)
  {
    VisUtils::setColor(Qt::blue, pow((i/255.0), 2));
    VisUtils::fillRect(
      xLft+i*xItv, xLft+(i+1)*xItv,
      0.5*yTop,    0.5*yBot);
  }
  VisUtils::disableBlending();
}


void ColorChooser::drawPath(const bool& inSelectMode)
{
  double xRgt;
  double yTop;
  double pix;

  QSizeF size = worldSize();
  pix = pixelSize();

  xRgt =  0.5*size.width();
  yTop =  0.5*size.height();

  if (!inSelectMode)
  {
    VisUtils::enableLineAntiAlias();
    for (int i = 0; i < m_yCoordinates->size()-1; ++i)
    {
      VisUtils::setColor(Qt::black);
      VisUtils::drawLineDashed(
        xPosition(i)*xRgt+pix, xPosition(i+1)*xRgt+pix,
        yPosition(i)*yTop-pix, yPosition(i+1)*yTop-pix);

      VisUtils::setColor(VisUtils::lightGray);
      VisUtils::drawLineDashed(
        xPosition(i)*xRgt, xPosition(i+1)*xRgt,
        yPosition(i)*yTop, yPosition(i+1)*yTop);
    }
    VisUtils::disableLineAntiAlias();
  }
}


void ColorChooser::drawPoints(const bool& inSelectMode)
{
  double xRgt;
  double yTop;
  double pix;
  std::size_t    size;
  double hdlDOF;

  pix = pixelSize();

  // calc size of bounding box
  xRgt =  0.5*worldSize().width();
  yTop =  0.5*worldSize().height();

  // size of handle
  hdlDOF = handleSize*pix;

  // size of positions
  size = m_yCoordinates->size();

  // selection mode
  if (inSelectMode)
  {
    for (std::size_t i = 0; i < size-1; ++i)
    {
      glPushName((GLuint) i);
      VisUtils::fillRect(
        xPosition(i)*xRgt-5.0*pix, xPosition(i)*xRgt+5.0*pix,
        yPosition(i)*yTop+5.0*pix, yPosition(i)*yTop-5.0*pix);
      glPopName();
    }

    if (size == 1)
    {
      glPushName((GLuint) size-1);
      VisUtils::fillRect(
        xPosition(size-1)*xRgt-5.0*pix, xPosition(size-1)*xRgt+5.0*pix,
        yPosition(size-1)*yTop+5.0*pix, yPosition(size-1)*yTop-5.0*pix);
      glPopName();
    }
    else if (size > 1)
    {
      double agl = Utils::calcAngleDg(
                     xPosition(size-1)*xRgt - xPosition(size-2)*xRgt,
                     yPosition(size-1)*yTop - yPosition(size-2)*yTop);

      // arrow
      glPushMatrix();
      glTranslatef(
        xPosition(size-1)*xRgt,
        yPosition(size-1)*yTop,
        0.0);
      glRotatef(90.0+agl, 0.0, 0.0, 1.0);

      glPushName((GLuint) size-1);
      VisUtils::fillTriangle(
        -hdlDOF, 2.0*hdlDOF,
        0.0,    0.0,
        hdlDOF, 2.0*hdlDOF);
      glPopName();

      glPopMatrix();
    }
  }
  // rendering mode
  else
  {
    VisUtils::enableLineAntiAlias();
    for (std::size_t i = 0; i < size-1; ++i)
    {
      VisUtils::setColor(Qt::black);
      VisUtils::drawLine(
        xPosition(i)*xRgt-4.0*pix, xPosition(i)*xRgt+6.0*pix,
        yPosition(i)*yTop+4.0*pix, yPosition(i)*yTop-6.0*pix);
      VisUtils::drawLine(
        xPosition(i)*xRgt-4.0*pix, xPosition(i)*xRgt+6.0*pix,
        yPosition(i)*yTop-6.0*pix, yPosition(i)*yTop+4.0*pix);

      VisUtils::setColor(underMouse() ? Qt::red : Qt::white);

      VisUtils::drawLine(
        xPosition(i)*xRgt-5.0*pix, xPosition(i)*xRgt+5.0*pix,
        yPosition(i)*yTop+5.0*pix, yPosition(i)*yTop-5.0*pix);
      VisUtils::drawLine(
        xPosition(i)*xRgt-5.0*pix, xPosition(i)*xRgt+5.0*pix,
        yPosition(i)*yTop-5.0*pix, yPosition(i)*yTop+5.0*pix);
    }

    if (size == 1)
    {
      VisUtils::setColor(Qt::black);
      VisUtils::drawLine(
        xPosition(size-1)*xRgt-4.0*pix, xPosition(size-1)*xRgt+6.0*pix,
        yPosition(size-1)*yTop+4.0*pix, yPosition(size-1)*yTop-6.0*pix);
      VisUtils::drawLine(
        xPosition(size-1)*xRgt-4.0*pix, xPosition(size-1)*xRgt+6.0*pix,
        yPosition(size-1)*yTop-6.0*pix, yPosition(size-1)*yTop+4.0*pix);


      VisUtils::setColor(underMouse() ? Qt::red : Qt::white);

      VisUtils::drawLine(
        xPosition(size-1)*xRgt-5.0*pix, xPosition(size-1)*xRgt+5.0*pix,
        yPosition(size-1)*yTop+5.0*pix, yPosition(size-1)*yTop-5.0*pix);
      VisUtils::drawLine(
        xPosition(size-1)*xRgt-5.0*pix, xPosition(size-1)*xRgt+5.0*pix,
        yPosition(size-1)*yTop-5.0*pix, yPosition(size-1)*yTop+5.0*pix);
    }
    else if (size > 1)
    {
      double agl = Utils::calcAngleDg(
                     xPosition(size-1)*xRgt - xPosition(size-2)*xRgt,
                     yPosition(size-1)*yTop - yPosition(size-2)*yTop);

      // drop shadow
      glPushMatrix();
      glTranslatef(
        xPosition(size-1)*xRgt+pix,
        yPosition(size-1)*yTop-pix,
        0.0);
      glRotatef(90.0+agl, 0.0, 0.0, 1.0);

      VisUtils::setColor(Qt::black);
      VisUtils::drawTriangle(
        -hdlDOF, 2.0*hdlDOF,
        0.0,    0.0,
        hdlDOF, 2.0*hdlDOF);
      VisUtils::drawLine(
        -2.0*hdlDOF, 2.0*hdlDOF,
        0.0, 0.0);

      glPopMatrix();

      // arrow
      glPushMatrix();
      glTranslatef(
        xPosition(size-1)*xRgt,
        yPosition(size-1)*yTop,
        0.0);
      glRotatef(90.0+agl, 0.0, 0.0, 1.0);

      VisUtils::setColor(underMouse() ? Qt::green : Qt::white);

      VisUtils::fillTriangle(
        -hdlDOF, 2.0*hdlDOF,
        0.0,    0.0,
        hdlDOF, 2.0*hdlDOF);

      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawTriangle(
        -hdlDOF, 2.0*hdlDOF,
        0.0,    0.0,
        hdlDOF, 2.0*hdlDOF);
      VisUtils::drawLine(
        -2.0*hdlDOF, 2.0*hdlDOF,
        0.0, 0.0);

      glPopMatrix();
    }
    VisUtils::disableLineAntiAlias();
  }
}


void ColorChooser::handleHits(const vector< int > &ids)
{
  if (m_lastMouseEvent.type() == QEvent::MouseButtonPress)
  {
    int id = ids[0];
    if (m_lastMouseEvent.button() == Qt::LeftButton)
    {
      if (0 <= id && id < m_yCoordinates->size())
      {
        m_dragIdx = id;
      }
    }
    else if (m_lastMouseEvent.button() == Qt::RightButton)
    {
      if (0 <= id && id < m_yCoordinates->size() && m_yCoordinates->size() > 2)
      {
        m_dof->removeValue(id);
        m_yCoordinates->removeAt(id);
      }
    }
  }
}


void ColorChooser::handleDrag()
{
  if (m_dragIdx != NON_EXISTING && m_dragIdx < (std::size_t)m_yCoordinates->size())
  {
    QSizeF size = worldSize();
    QPointF pos = worldCoordinate(m_lastMouseEvent.localPos());

    double xCur = pos.x()/(0.5*size.width());
    double yCur = pos.y()/(0.5*size.height());

    m_dof->setValue(m_dragIdx, (xCur + 1.0) / 2.0);
    (*m_yCoordinates)[m_dragIdx] = yCur;
    updateGL(true);
    updateGL();
  }
}


void ColorChooser::processHits(
  GLint hits,
  GLuint buffer[])
{
  GLuint* ptr;
  vector< int > ids;

  ptr = (GLuint*) buffer;

  if (hits > 0)
  {
    // if necassary, advance to closest hit
    if (hits > 1)
    {
      for (int i = 0; i < (hits-1); ++i)
      {
        int number = *ptr;
        ++ptr; // number;
        ++ptr; // z1
        ++ptr; // z2
        for (int j = 0; j < number; ++j)
        {
          ++ptr;  // names
        }
      }
    }

    // last hit
    int number = *ptr;
    ++ptr; // number
    ++ptr; // z1
    ++ptr; // z2

    for (int i = 0; i < number; ++i)
    {
      ids.push_back(*ptr);
      ++ptr;
    }

    handleHits(ids);
  }

  ptr = 0;
}
