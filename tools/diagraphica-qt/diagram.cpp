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


Diagram::Diagram(
  Mediator* m)
  : Colleague(m)
{
  showGrid = true;
  snapGrid = true;

  GRID_NUM_INTERV_HINT = 30;
  ANGL_NUM_INTERV_HINT = 24;
  SIZE_BORDER          = 15;
}


Diagram::~Diagram()
{
  // composition
  for (size_t i = 0; i < shapes.size(); ++i)
  {
    delete shapes[i];
    shapes[i] = NULL;
  }
  shapes.clear();
}


// -- set functions -------------------------------------------------


void Diagram::addShape(Shape* s)
{
  shapes.push_back(s);
}


void Diagram::moveShapeToFront(const size_t& idx)
{
  if (idx < shapes.size())
  {
    Shape* tmp = shapes[idx];
    for (size_t i = idx; i > 0; --i)
    {
      shapes[i] = shapes[i-1];
      shapes[i]->setIndex(i);
    }
    shapes[0] = tmp;
    shapes[0]->setIndex(0);
    tmp = NULL;
  }
}


void Diagram::moveShapeToBack(const size_t& idx)
{
  if (idx < shapes.size())
  {
    Shape* tmp = shapes[idx];
    for (size_t i = idx; i < shapes.size()-1; ++i)
    {
      shapes[i] = shapes[i+1];
      shapes[i]->setIndex(i);
    }
    shapes[shapes.size()-1] = tmp;
    shapes[shapes.size()-1]->setIndex(shapes.size()-1);
    tmp = NULL;
  }
}


void Diagram::moveShapeForward(const size_t& idx)
{
  if (0 < idx && idx < shapes.size())
  {
    Shape* tmp = shapes[idx];

    shapes[idx] = shapes[idx-1];
    shapes[idx]->setIndex(idx);

    shapes[idx-1] = tmp;
    shapes[idx-1]->setIndex(idx-1);

    tmp = NULL;
  }
}


void Diagram::moveShapeBackward(const size_t& idx)
{
  if (idx < shapes.size()-1)
  {
    Shape* tmp = shapes[idx];

    shapes[idx] = shapes[idx+1];
    shapes[idx]->setIndex(idx);

    shapes[idx+1] = tmp;
    shapes[idx+1]->setIndex(idx+1);

    tmp = NULL;
  }
}


void Diagram::setShowGrid(const bool& flag)
{
  showGrid = flag;
}


void Diagram::setSnapGrid(const bool& flag)
{
  snapGrid = flag;
}


// -- get functions -------------------------------------------------


size_t Diagram::getSizeShapes()
{
  return shapes.size();
}


Shape* Diagram::getShape(const size_t& idx)
{
  Shape* result = NULL;
  if (idx < shapes.size())
  {
    result = shapes[idx];
  }
  return result;
}


bool Diagram::getSnapGrid()
{
  return snapGrid;
}


double Diagram::getGridInterval(GLCanvas* canvas)
{
  double numIntervals = GRID_NUM_INTERV_HINT;
  double sizeInterval;
  double pix;

  // get pixel size
  pix = canvas->getPixelSize();

  sizeInterval = (2.0-(2.0*pix*SIZE_BORDER))/(double)numIntervals;

  return sizeInterval;
}


double Diagram::getAngleInterval()
{
  double numIntervals = ANGL_NUM_INTERV_HINT;
  double sizeInterval = 360.0/(double)numIntervals;

  return sizeInterval;
}

void Diagram::getGridCoordinates(double& xLeft, double& xRight, double& yTop, double& yBottom)
{
  xLeft = gridXLeft;
  xRight = gridXRight;
  yTop = gridYTop;
  yBottom = gridYBottom;
}


// -- clear functions -----------------------------------------------


void Diagram::deleteShape(const size_t& idx)
{
  if (idx < shapes.size())
  {
    // delete shape
    Shape* s = shapes[idx];
    shapes.erase(shapes.begin()+idx);
    delete s;
    s = NULL;
    // update indices
    for (size_t i = idx; i < shapes.size(); ++i)
    {
      shapes[i]->setIndex(i);
    }
  }
}


// -- vis functions -------------------------------------------------


void Diagram::visualize(
  const bool& inSelectMode,
  GLCanvas* canvas)
// Used by diagram editor.
{
  drawBorder(inSelectMode, canvas);
  if (showGrid == true)
  {
    drawGrid(inSelectMode, canvas);
  }
  drawShapes(inSelectMode, canvas);
}


void Diagram::visualize(
  const bool& inSelectMode,
  GLCanvas* canvas,
  const vector< Attribute* > attrs,
  const vector< double > attrValIdcs)
// Used by visualizers.
{
  if (inSelectMode == true)
  {
    drawBorderFlush(inSelectMode, canvas);
  }
  else
  {
    drawBorderFlush(inSelectMode, canvas);
    for (size_t i = 0; i < shapes.size(); ++i)
    {
      shapes[i]->visualize(canvas, 1.0, attrs, attrValIdcs);
    }
  }
}


void Diagram::visualize(
  const bool& inSelectMode,
  GLCanvas* canvas,
  const vector< Attribute* > attrs,
  const vector< double > attrValIdcs,
  double opacity)
// Used by visualizers.
{
  if (inSelectMode == true)
  {
    drawBorderFlush(inSelectMode, canvas);
  }
  else
  {
    drawBorderFlush(inSelectMode, canvas, opacity);
    for (size_t i = 0; i < shapes.size(); ++i)
    {
      shapes[i]->visualize(canvas, opacity, attrs, attrValIdcs);
    }
  }
}


// -- private utility functions -------------------------------------


void Diagram::initGridSettings()
{
  showGrid   = true;
  snapGrid   = false;
}


void Diagram::drawAxes(
  const bool& inSelectMode,
  GLCanvas* canvas)
{
  if (inSelectMode != true)
  {
    double pix = canvas->getPixelSize();

    VisUtils::setColor(VisUtils::mediumGray);
    VisUtils::drawLine(
      0.0,                 0.0,
      1.0-pix*SIZE_BORDER, -1+pix*SIZE_BORDER);
    VisUtils::drawLine(
      -1.0+pix*SIZE_BORDER, 1-pix*SIZE_BORDER,
      0.0,                  0.0);
  }
}


void Diagram::drawBorder(
  const bool& inSelectMode,
  GLCanvas* canvas)
{
  if (inSelectMode == true)
  {
    double pix;
    double xLft, xRgt, yTop, yBot;

    // get pixel size
    pix = canvas->getPixelSize();

    // calc margins
    xLft = -1.0 + pix*SIZE_BORDER;
    xRgt =  1.0 - pix*SIZE_BORDER;
    yTop =  1.0 - pix*SIZE_BORDER;
    yBot = -1.0 + pix*SIZE_BORDER;

    // draw
    VisUtils::fillRect(xLft, xRgt, yTop, yBot);
  }
  else
  {
    double pix;
    double xLft, xRgt, yTop, yBot;

    // get pixel size
    pix = canvas->getPixelSize();

    // calc margins
    xLft = -1.0 + pix*SIZE_BORDER;
    xRgt =  1.0 - pix*SIZE_BORDER;
    yTop =  1.0 - pix*SIZE_BORDER;
    yBot = -1.0 + pix*SIZE_BORDER;

    // draw
    VisUtils::setColor(Qt::white);
    VisUtils::fillRect(xLft, xRgt, yTop, yBot);

    VisUtils::setColor(VisUtils::mediumGray);
    VisUtils::drawRect(xLft, xRgt, yTop, yBot);
  }
}


void Diagram::drawBorderFlush(
  const bool& inSelectMode,
  GLCanvas* /*canvas*/)
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
  GLCanvas* /*canvas*/,
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
  GLCanvas* canvas)
{
  if (inSelectMode != true)
  {
    double numIntervals = GRID_NUM_INTERV_HINT;
    double sizeInterval;
    double pix;
    double xLft, xRgt, yTop, yBot;

    // get pixel size
    pix = canvas->getPixelSize();

    // calc margins
    xLft = -1.0 + pix*SIZE_BORDER;
    xRgt =  1.0 - pix*SIZE_BORDER;
    yTop =  1.0 - pix*SIZE_BORDER;
    yBot = -1.0 + pix*SIZE_BORDER;

    gridXLeft = xLft;
    gridXRight = xRgt;
    gridYTop = yTop;
    gridYBottom = yBot;

    sizeInterval = (2.0-(2.0*pix*SIZE_BORDER)-2.0*pix)/(double)numIntervals;

    VisUtils::setColor(VisUtils::lightGray);
    // draw inside out
    for (int i = 0; i < numIntervals/2; ++i)
    {
      // vertical
      VisUtils::drawLine(
        -i*sizeInterval,          -i*sizeInterval,
        1.0-pix*SIZE_BORDER-pix, -1.0+pix*SIZE_BORDER+pix);
      VisUtils::drawLine(
        i*sizeInterval,           i*sizeInterval,
        1.0-pix*SIZE_BORDER-pix, -1.0+pix*SIZE_BORDER+pix);
      // horizontal
      VisUtils::drawLine(
        -1.0+pix*SIZE_BORDER+pix,  1.0-pix*SIZE_BORDER-pix,
        i*sizeInterval,           i*sizeInterval);
      VisUtils::drawLine(
        -1.0+pix*SIZE_BORDER+pix,  1.0-pix*SIZE_BORDER-pix,
        -i*sizeInterval,          -i*sizeInterval);
    }
  }
}


void Diagram::drawShapes(
  const bool& inSelectMode,
  GLCanvas* canvas)
{

  if (inSelectMode == true)
  {
    for (size_t i = 0; i < shapes.size(); ++i)
    {
      glPushName((GLuint) i);
      shapes[i]->visualize(inSelectMode, canvas);
      glPopName();
    }
  }
  else
  {
    for (size_t i = 0; i < shapes.size(); ++i)
    {
      shapes[i]->visualize(inSelectMode, canvas);
    }
  }

}


// -- end -----------------------------------------------------------
