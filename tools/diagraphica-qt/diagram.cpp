// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./diagram.cpp

#include "diagram.h"

using namespace std;

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


Diagram::~Diagram()
{
  // Shapes are deleted as children
}


// -- set functions -------------------------------------------------


void Diagram::moveShapeToFront(int index)
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


void Diagram::moveShapeToBack(int index)
{
  if (index < m_shapes.size())
  {
    Shape* tmp = m_shapes[index];
    for (int i = index; i < m_shapes.size()-1; i++)
    {
      m_shapes[i] = m_shapes[i+1];
      m_shapes[i]->setIndex(i);
    }
    m_shapes[m_shapes.size()-1] = tmp;
    m_shapes[m_shapes.size()-1]->setIndex(m_shapes.size()-1);
  }
}


void Diagram::moveShapeForward(int index)
{
  if (0 < index && index < m_shapes.size())
  {
    m_shapes.swap(index, index-1);
    m_shapes[index]->setIndex(index);
    m_shapes[index-1]->setIndex(index-1);
  }
}


void Diagram::moveShapeBackward(int index)
{
  if (index < m_shapes.size()-1)
  {
    m_shapes.swap(index, index+1);
    m_shapes[index]->setIndex(index);
    m_shapes[index+1]->setIndex(index+1);
  }
}

Shape *Diagram::shape(int index)
{
  if (0 <= index && index < m_shapes.size())
  {
    return m_shapes.at(index);
  }
  return 0;
}


// -- clear functions -----------------------------------------------


void Diagram::removeShape(int index)
{
  if (0 <= index && index < m_shapes.size())
  {
    // delete shape
    Shape* s = m_shapes[index];
    m_shapes.removeAt(index);
    delete s;
    // update indices
    for (int i = index; i < m_shapes.size(); i++)
    {
      m_shapes[i]->setIndex(i);
    }
  }
}


// -- vis functions -------------------------------------------------


void Diagram::visualize(
  const bool& inSelectMode,
  double pixelSize)
// Used by diagram editor.
{
  drawBorder(inSelectMode, pixelSize);
  if (m_showGrid == true)
  {
    drawGrid(inSelectMode, pixelSize);
  }
  drawShapes(inSelectMode, pixelSize);
}


void Diagram::visualize(
  const bool& inSelectMode,
  double pixelSize,
  const vector< Attribute* > attrs,
  const vector< double > attrValIdcs)
// Used by visualizers.
{
  visualize(inSelectMode, pixelSize, attrs, attrValIdcs, 1.0);
}


void Diagram::visualize(
  const bool& inSelectMode,
  double pixelSize,
  const vector< Attribute* > attrs,
  const vector< double > attrValIdcs,
  double opacity)
// Used by visualizers.
{
  drawBorderFlush(inSelectMode);
  if (!inSelectMode)
  {
    for (int i = 0; i < m_shapes.size(); i++)
    {
      m_shapes[i]->visualize(pixelSize, opacity, attrs, attrValIdcs);
    }
  }
}


// -- private utility functions -------------------------------------


void Diagram::drawAxes(
  const bool& inSelectMode,
  double pixelSize)
{
  if (inSelectMode != true)
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


void Diagram::drawBorder(
  const bool& inSelectMode,
  double pixelSize)
{
  if (inSelectMode == true)
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


void Diagram::drawBorderFlush(
  const bool& inSelectMode)
{
  if (inSelectMode == true)
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


void Diagram::drawBorderFlush(
  const bool& inSelectMode,
  const double& opacity)
{
  if (inSelectMode == true)
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


void Diagram::drawGrid(
  const bool& inSelectMode,
  double pixelSize)
{
  if (inSelectMode != true)
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
    for (int i = 0; i < numIntervals/2; i++)
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


void Diagram::drawShapes(
  const bool& inSelectMode,
  double pixelSize)
{
  for (int i = 0; i < m_shapes.size(); i++)
  {
    glPushName((GLuint) i);
    m_shapes[i]->visualize(inSelectMode, pixelSize);
    glPopName();
  }
}


// -- end -----------------------------------------------------------
