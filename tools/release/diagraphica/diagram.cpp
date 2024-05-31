// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./diagram.cpp

#include "diagram.h"

#include "mcrl2/gui/utilities.h"

using namespace mcrl2::gui::qt;

// -- constructors and destructor -----------------------------------


Diagram::Diagram(QObject *parent):
  QObject(parent)
{
  m_showGrid = true;
  m_snapGrid = true;

  GRID_NUM_INTERV_HINT = 30;
  ANGL_NUM_INTERV_HINT = 24;
  SIZE_BORDER          = 15;
}

Diagram &Diagram::operator=(const Diagram &other)
{
  while (!m_shapes.empty())
  {
    removeShape(0);
  }

  m_showGrid = other.m_showGrid;
  m_snapGrid = other.m_snapGrid;
  m_gridCoordinates = other.m_gridCoordinates;

  for (int i = 0; i < other.shapeCount(); i++)
  {
    Shape *shape = new Shape(*other.shape(i));
    shape->setParent(this);
    addShape(shape);
  }

  return *this;
}


// -- set functions -------------------------------------------------


void Diagram::moveShapeToBack(int index)
{
  if (index < m_shapes.size())
  {
    Shape* tmp = m_shapes[index];
    for (int i = index; i > 0; --i)
    {
      m_shapes[i] = m_shapes[i-1];
      m_shapes[i]->setIndex(i);
    }
    m_shapes[0] = tmp;
    m_shapes[0]->setIndex(0);
  }
}


void Diagram::moveShapeToFront(int index)
{
  if (index < m_shapes.size())
  {
    Shape* tmp = m_shapes[index];
    for (int i = index; i < m_shapes.size()-1; ++i)
    {
      m_shapes[i] = m_shapes[i+1];
      m_shapes[i]->setIndex(i);
    }
    m_shapes[m_shapes.size()-1] = tmp;
    m_shapes[m_shapes.size()-1]->setIndex(m_shapes.size()-1);
  }
}


void Diagram::moveShapeBackward(int index)
{
  if (0 < index && index < m_shapes.size())
  {
    swapItemsAt(m_shapes, index, index-1);
    m_shapes[index]->setIndex(index);
    m_shapes[index-1]->setIndex(index-1);
  }
}


void Diagram::moveShapeForward(int index)
{
  if (index < m_shapes.size()-1)
  {
    swapItemsAt(m_shapes, index, index+1);
    m_shapes[index]->setIndex(index);
    m_shapes[index+1]->setIndex(index+1);
  }
}


void Diagram::removeShape(int index)
{
  if (0 <= index && index < m_shapes.size())
  {
    // delete shape
    Shape* s = m_shapes[index];
    m_shapes.removeAt(index);
    delete s;
    // update indices
    for (int i = index; i < m_shapes.size(); ++i)
    {
      m_shapes[i]->setIndex(i);
    }
  }
}


// -- private utility functions -------------------------------------


template <Visualizer::Mode mode> void Diagram::drawAxes(double pixelSize)
{
  if constexpr (mode == Visualizer::Visualizing)
  {
    VisUtils::setColor(VisUtils::mediumGray);
    VisUtils::drawLine(
      0.0,                 0.0,
      1.0-pixelSize*SIZE_BORDER, -1+pixelSize*SIZE_BORDER);
    VisUtils::drawLine(
      -1.0+pixelSize*SIZE_BORDER, 1-pixelSize*SIZE_BORDER,
      0.0,                  0.0);
  }
}


template <Visualizer::Mode mode> void Diagram::drawBorder(double pixelSize)
{
  if constexpr (mode == Visualizer::Marking)
  {
    double xLft, xRgt, yTop, yBot;

    // calc margins
    xLft = -1.0 + pixelSize*SIZE_BORDER;
    xRgt =  1.0 - pixelSize*SIZE_BORDER;
    yTop =  1.0 - pixelSize*SIZE_BORDER;
    yBot = -1.0 + pixelSize*SIZE_BORDER;

    // draw
    VisUtils::fillRect(xLft, xRgt, yTop, yBot);
  }
  else
  {
    double xLft, xRgt, yTop, yBot;

    // calc margins
    xLft = -1.0 + pixelSize*SIZE_BORDER;
    xRgt =  1.0 - pixelSize*SIZE_BORDER;
    yTop =  1.0 - pixelSize*SIZE_BORDER;
    yBot = -1.0 + pixelSize*SIZE_BORDER;

    // draw
    VisUtils::setColor(Qt::white);
    VisUtils::fillRect(xLft, xRgt, yTop, yBot);

    VisUtils::setColor(VisUtils::mediumGray);
    VisUtils::drawRect(xLft, xRgt, yTop, yBot);
  }
}


template <Visualizer::Mode mode> void Diagram::drawBorderFlush()
{
  if constexpr (mode == Visualizer::Marking)
  {
    VisUtils::fillRect(-1.0, 1.0, 1.0, -1.0);
  }
  else
  {
    VisUtils::setColor(Qt::white);
    VisUtils::fillRect(-1.0, 1.0, 1.0, -1.0);

    VisUtils::setColor(VisUtils::mediumGray);
    VisUtils::drawRect(-1.0, 1.0, 1.0, -1.0);
  }

}


template <Visualizer::Mode mode> void Diagram::drawBorderFlush(const double& opacity)
{
  if constexpr (mode == Visualizer::Marking)
  {
    VisUtils::fillRect(-1.0, 1.0, 1.0, -1.0);
  }
  else
  {
    VisUtils::enableBlending();

    VisUtils::setColor(VisUtils::mediumGray, 1.0 - opacity);
    VisUtils::drawRect(-1.0, 1.0, 1.0, -1.0);

    VisUtils::disableBlending();
  }

}


template <Visualizer::Mode mode> void Diagram::drawGrid(double pixelSize)
{
  if constexpr (mode == Visualizer::Visualizing)
  {
    double numIntervals = GRID_NUM_INTERV_HINT;
    double sizeInterval;
    double xLft, xRgt, yTop, yBot;

    // calc margins
    xLft = -1.0 + pixelSize*SIZE_BORDER;
    xRgt =  1.0 - pixelSize*SIZE_BORDER;
    yTop =  1.0 - pixelSize*SIZE_BORDER;
    yBot = -1.0 + pixelSize*SIZE_BORDER;

    m_gridCoordinates.setCoords(xLft, yTop, xRgt, yBot);

    sizeInterval = (2.0-(2.0*pixelSize*SIZE_BORDER)-2.0*pixelSize)/(double)numIntervals;

    VisUtils::setColor(VisUtils::lightGray);
    // draw inside out
    for (int i = 0; i < numIntervals/2; ++i)
    {
      // vertical
      VisUtils::drawLine(
        -i*sizeInterval,          -i*sizeInterval,
        1.0-pixelSize*SIZE_BORDER-pixelSize, -1.0+pixelSize*SIZE_BORDER+pixelSize);
      VisUtils::drawLine(
        i*sizeInterval,           i*sizeInterval,
        1.0-pixelSize*SIZE_BORDER-pixelSize, -1.0+pixelSize*SIZE_BORDER+pixelSize);
      // horizontal
      VisUtils::drawLine(
        -1.0+pixelSize*SIZE_BORDER+pixelSize,  1.0-pixelSize*SIZE_BORDER-pixelSize,
        i*sizeInterval,           i*sizeInterval);
      VisUtils::drawLine(
        -1.0+pixelSize*SIZE_BORDER+pixelSize,  1.0-pixelSize*SIZE_BORDER-pixelSize,
        -i*sizeInterval,          -i*sizeInterval);
    }
  }
}


template <Visualizer::Mode mode> void Diagram::drawShapes(double pixelSize)
{
  for (int i = 0; i < m_shapes.size(); ++i)
  {
    glPushName((GLuint) i);
    m_shapes[i]->draw<mode>(pixelSize);
    glPopName();
  }
}


// -- vis functions -------------------------------------------------


template <Visualizer::Mode mode> void Diagram::draw(double pixelSize)
// Used by diagram editor.
{
  drawBorder<mode>(pixelSize);
  if (m_showGrid)
  {
    drawGrid<mode>(pixelSize);
  }
  drawShapes<mode>(pixelSize);
}
template void Diagram::draw<Visualizer::Marking>(double pixelSize);
template void Diagram::draw<Visualizer::Visualizing>(double pixelSize);


template <Visualizer::Mode mode> void Diagram::draw(double pixelSize,
  const std::vector< Attribute* > attrs,
  const std::vector< double > attrValIdcs,
  double opacity)
  // Used by visualizers.
{
  drawBorderFlush<mode>();
  if constexpr (mode == Visualizer::Visualizing)
  {
    for (int i = 0; i < m_shapes.size(); ++i)
    {
      m_shapes[i]->draw<mode>(pixelSize, attrs, attrValIdcs, opacity);
    }
  }
}
template void Diagram::draw<Visualizer::Marking> (double pixelSize,
  const std::vector< Attribute* > attrs,
  const std::vector< double > attrValIdcs, double opacity);
template void Diagram::draw<Visualizer::Visualizing>(double pixelSize,
  const std::vector< Attribute* > attrs,
  const std::vector< double > attrValIdcs, double opacity);

// -- end -----------------------------------------------------------
