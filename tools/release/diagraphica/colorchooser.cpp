// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./colorchooser.cpp

#include "colorchooser.h"

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


void ColorChooser::enterEvent(QEnterEvent* event)
{
  Visualizer::enterEvent(event);
  emit activated();
  update();
}


void ColorChooser::leaveEvent(QEvent* event)
{
  Visualizer::leaveEvent(event);
  emit deactivated();
  update();
}


void ColorChooser::mouseMoveEvent(QMouseEvent* event)
{
  Visualizer::mouseMoveEvent(event);
  setCursor(m_dragIdx != NON_EXISTING ? Qt::ClosedHandCursor :
    !getSelection().empty() ? Qt::PointingHandCursor :
    Qt::ArrowCursor);
  if (event->buttons() == Qt::LeftButton && m_mouseDrag && m_dragIdx != NON_EXISTING)
  {
    movePoint(m_dragIdx, event->position());
    update();
  }
}


void ColorChooser::mousePressEvent(QMouseEvent* event)
{
  Visualizer::mousePressEvent(event);

  const SelectionList selections = getSelection();
  if (!selections.empty() && !selections.back().empty())
  {
    if (event->button() == Qt::LeftButton)
    {
      m_dragIdx = selections.back()[0];
    }
    else if (event->button() == Qt::RightButton)
    {
      removePoint(selections.back()[0]);
    }
  }

  if (event->button() == Qt::LeftButton && m_dragIdx == NON_EXISTING)
  {
    appendPoint(event->position());
    m_dragIdx = m_yCoordinates->size() - 1;
  }
  update();
}


void ColorChooser::mouseReleaseEvent(QMouseEvent* event)
{
  Visualizer::mouseReleaseEvent(event);
  if (event->button() == Qt::LeftButton)
  {
    m_dragIdx = NON_EXISTING;
  }
  update();
}


template <Visualizer::Mode mode> void ColorChooser::drawColorSpectrum()
{
  if constexpr (mode == Marking)
  {
    return;
  }
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


template <Visualizer::Mode mode> void ColorChooser::drawGrayScale()
{
  if constexpr (mode == Marking)
  {
    return;
  }
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


template <Visualizer::Mode mode> void ColorChooser::drawPath()
{
  double xRgt;
  double yTop;
  double pix;

  QSizeF size = worldSize();
  pix = pixelSize();

  xRgt =  0.5*size.width();
  yTop =  0.5*size.height();

  if constexpr (mode == Visualizing)
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


template <Visualizer::Mode mode> void ColorChooser::drawPoints()
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
  if constexpr (mode == Marking)
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


template <Visualizer::Mode mode> void ColorChooser::draw()
{
  if constexpr (mode == Marking)
  {
    drawPoints<mode>();
  }
  else
  {
    clear();
    if (m_type == HueColor)
    {
      drawColorSpectrum<mode>();
    }
    else
    {
      drawGrayScale<mode>();
    }
    drawPath<mode>();
    drawPoints<mode>();
  }
}


void ColorChooser::visualize() { draw<Visualizing>(); }
void ColorChooser::mark() { draw<Marking>(); }


void ColorChooser::appendPoint(const QPointF& position)
{
  m_dof->addValue(0.);
  m_yCoordinates->append(0.);
  movePoint(m_yCoordinates->size() - 1, position);
}


void ColorChooser::removePoint(std::size_t index)
{
  if (m_yCoordinates->size() > 2)
  {
    m_dof->removeValue(index);
    m_yCoordinates->removeAt(index);
  }
}


void ColorChooser::movePoint(std::size_t index, const QPointF& position)
{
  const QSizeF size = worldSize();
  const QPointF pos = worldCoordinate(position);

  double xCur = pos.x() / (0.5 * size.width());
  double yCur = pos.y() / (0.5 * size.height());

  m_dof->setValue(index, (xCur + 1.0) / 2.0);
  (*m_yCoordinates)[index] = yCur;
}

