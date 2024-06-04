// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./corrlplot.cpp

#include "corrlplot.h"
#include <QToolTip>

// -- constructors and destructor -----------------------------------


CorrlPlot::CorrlPlot(QWidget *parent, Graph* graph, int attributeIndex1, int attributeIndex2):
  Visualizer(parent, graph)
{
  minRadHintPx =  5;
  maxRadHintPx = 25;

  diagram         = 0;
  showDgrm        = false;
  attrValIdx1Dgrm = NON_EXISTING;
  attrValIdx2Dgrm = NON_EXISTING;

  attribute1 = m_graph->getAttribute(attributeIndex1);
  attribute2 = m_graph->getAttribute(attributeIndex2);
  connect(attribute1, SIGNAL(deleted()), this, SLOT(close()));
  connect(attribute2, SIGNAL(deleted()), this, SLOT(close()));

  m_graph->calcAttrCorrl(attributeIndex1, attributeIndex2, mapXToY, number);
  initLabels();
  calcMaxNumber();
  calcPositions();

  setMouseTracking(true);
}


// -- set data functions --------------------------------------------

void CorrlPlot::setDiagram(Diagram* dgrm)
{
  diagram = dgrm;
}


// -- visualization functions  --------------------------------------


template <Visualizer::Mode mode> void CorrlPlot::drawAxes()
{
  QSizeF size = worldSize();
  double pix = pixelSize();

  // calc size of bounding box
  double xLft = -0.5*size.width()+20*pix;
  double xRgt =  0.5*size.width()-10*pix;
  double yTop =  0.5*size.height()-10*pix;
  double yBot = -0.5*size.height()+20*pix;

  // rendering mode
  if (mode == Visualizing)
  {
    // draw guides
    VisUtils::setColor(VisUtils::lightGray);
    VisUtils::drawLine(xLft, xRgt, yTop, yTop);
    VisUtils::drawLine(xRgt, xRgt, yBot, yTop);
    VisUtils::setColor(VisUtils::lightGray);
    VisUtils::enableLineAntiAlias();
    VisUtils::drawLine(xLft, xRgt, yBot, yTop);
    VisUtils::disableLineAntiAlias();

    // x- & y-axis
    VisUtils::setColor(VisUtils::mediumGray);
    VisUtils::drawLine(xLft, xLft, yBot, yTop);
    VisUtils::drawLine(xLft, xRgt, yBot, yBot);
  }
}


template <Visualizer::Mode mode> void CorrlPlot::drawLabels()
{
  if (mode == Marking)
  {
    return;
  }
  QSizeF size = worldSize();
  double pix = pixelSize();
  // calc scaling to use
  double scaling = (12*pix)/(double)CHARHEIGHT;

  // color
  VisUtils::setColor(Qt::black);

  if (mapXToY.size() > 0)
  {
    // x-axis label
    double x =  0.0;
    double y = -0.5*size.height()+9*pix;
    VisUtils::drawLabelCenter(texCharId, x, y, scaling, xLabel);

    // y-axis labels
    x = -0.5*size.width()+9*pix;
    y =  0;
    VisUtils::drawLabelVertCenter(texCharId, x, y, scaling, yLabel);
  }
}


template <Visualizer::Mode mode> void CorrlPlot::drawPlot()
{
  // selection mode
  if constexpr (mode == Marking)
  {
    for (std::size_t i = 0; i < positions.size(); ++i)
    {
      glPushName((GLuint) i);
      for (std::size_t j = 0; j < positions[i].size(); ++j)
      {
        double x   = positions[i][j].x;
        double y   = positions[i][j].y;
        double rad = radii[i][j];

        glPushName((GLuint) j);
        //VisUtils::fillCirc( x, y, rad, 21);
        VisUtils::fillEllipse(x, y, rad, rad, 21);
        glPopName();
      }
      glPopName();
    }
  }
  // rendering mode
  else
  {
    double pix   = pixelSize();

    for (std::size_t i = 0; i < positions.size(); ++i)
    {
      for (std::size_t j = 0; j < positions[i].size(); ++j)
      {
        double x   = positions[i][j].x;
        double y   = positions[i][j].y;
        double rad = radii[i][j];

        VisUtils::setColor(VisUtils::coolGreen, 0.35);

        VisUtils::enableBlending();
        //VisUtils::fillCirc( x, y, rad, 21);
        VisUtils::fillEllipse(x, y, rad, rad, 21);
        VisUtils::disableBlending();

        VisUtils::enableLineAntiAlias();
        //VisUtils::drawCirc( x, y, rad, 21);
        VisUtils::drawEllipse(x, y, rad, rad, 21);
        VisUtils::setColor(Qt::black, 0.1);
        //VisUtils::drawCirc( x, y, rad, 21);
        VisUtils::drawEllipse(x, y, rad, rad, 21);
        VisUtils::disableLineAntiAlias();

        VisUtils::setColor(Qt::black);
        VisUtils::fillRect(x-pix, x+pix, y+pix, y-pix);
      }
    }
  }
}


template <Visualizer::Mode mode> void CorrlPlot::drawDiagram()
{
  if constexpr (mode == Marking)
  {
    return;
  }
  double pix = pixelSize();
  double scaleTxt = ((12*pix)/(double)CHARHEIGHT)/scaleDgrm;

  std::vector< Attribute* > attrs;
  attrs.push_back(attribute1);
  attrs.push_back(attribute2);

  std::vector< double > vals;
  vals.push_back(attrValIdx1Dgrm);
  vals.push_back(attrValIdx2Dgrm);

  glPushMatrix();
  glTranslatef(posDgrm.x, posDgrm.y, 0.0);
  glScalef(scaleDgrm, scaleDgrm, scaleDgrm);

  // drop shadow
  VisUtils::setColor(VisUtils::mediumGray);
  VisUtils::fillRect(
    -1.0 + 4.0*pix/scaleDgrm,
    1.0 + 4.0*pix/scaleDgrm,
    1.0 - 4.0*pix/scaleDgrm,
    -1.0 - 4.0*pix/scaleDgrm);
  // diagram
  diagram->draw<mode>(pixelSize(), attrs, vals);

  VisUtils::setColor(Qt::black);
  VisUtils::drawLabelRight(texCharId, -0.98, 1.1, scaleTxt, msgDgrm);

  glPopMatrix();
}


template <Visualizer::Mode mode> void CorrlPlot::draw()
{
  // have textures been generated
  if (!texCharOK)
  {
    genCharTex();
  }

  // check if positions are ok
  if (geomChanged)
  {
    calcPositions();
  }

  // visualize
  if constexpr (mode == Marking)
  {
    drawPlot<mode>();
  }
  else
  {
    clear();
    drawAxes<mode>();
    drawLabels<mode>();
    drawPlot<mode>();
    if (showDgrm)
    {
      drawDiagram<mode>();
    }
  }
}


void CorrlPlot::visualize() { draw<Visualizing>(); }
void CorrlPlot::mark() { draw<Marking>(); }


// -- input event handlers ------------------------------------------


void CorrlPlot::mouseMoveEvent(QMouseEvent* event)
{
  Visualizer::mouseMoveEvent(event);
  const SelectionList selections = getSelection();
  if (selections.empty() || selections.back().size() < 2)
  {
    hideTooltip();
  }
  else
  {
    const Selection& selection = selections.back();
    showTooltip(selection[0], selection[1], event->position());
  }
  update();
}


// -- utility data functions ----------------------------------------


void CorrlPlot::initLabels()
{
  xLabel = attribute1->name().toStdString();
  yLabel = attribute2->name().toStdString();
}


void CorrlPlot::calcMaxNumber()
{
  // init max number & totals
  sumMaxNumX = 0;
  sumMaxNumY = 0;
  maxNumber  = 0;

  // init max row & col numbers
  std::size_t sizeX = attribute1->getSizeCurValues();
  std::size_t sizeY = attribute2->getSizeCurValues();
  for (std::size_t i = 0; i < sizeX; ++i)
  {
    maxNumX.push_back(0);
  }

  for (std::size_t i = 0; i < sizeY; ++i)
  {
    maxNumY.push_back(0);
  }

  // calc max
  {
    for (std::size_t i = 0; i < number.size(); ++i)
    {
      for (std::size_t j = 0; j < number[i].size(); ++j)
      {
        if (number[i][j] > maxNumber)
        {
          maxNumber = number[i][j];
        }
        if (number[i][j] > maxNumX[i])
        {
          maxNumX[i] = number[i][j];
        }
        if (number[i][j] > maxNumY[ mapXToY[i][j] ])
        {
          maxNumY[j] = number[i][j];
        }
      }
    }
  }

  // calc totals
  {
    for (std::size_t i = 0; i < maxNumX.size(); ++i)
    {
      sumMaxNumX += maxNumX[i];
    }
  }
  {
    for (std::size_t i = 0; i < maxNumY.size(); ++i)
    {
      sumMaxNumY += maxNumY[i];
    }
  }
}


// -- utility drawing functions -------------------------------------

void CorrlPlot::showTooltip(std::size_t xIndex, std::size_t yIndex, const QPointF& position)
{
  msgDgrm = Utils::dblToStr(number[xIndex][yIndex]) + " nodes; "
    + Utils::dblToStr(Utils::perc((double) number[xIndex][yIndex], (double) m_graph->getSizeNodes())) + '%';

  if (diagram == 0)
  {
    QToolTip::showText(QCursor::pos(), QString::fromStdString(msgDgrm));
  }
  else
  {
    QPointF pos = worldCoordinate(position);
    posDgrm.x = pos.x() + (pos.x() < 0 ? 1.0 : -1.0) * scaleDgrm;
    posDgrm.y = pos.y() + (pos.y() < 0 ? 1.0 : -1.0) * scaleDgrm;
    showDgrm = true;
    attrValIdx1Dgrm = xIndex;
    attrValIdx2Dgrm = mapXToY[xIndex][yIndex];
  }
}


void CorrlPlot::hideTooltip()
{
  QToolTip::hideText();
  showDgrm = false;
}


void CorrlPlot::calcPositions()
{
  // update flag
  geomChanged = false;

  if (mapXToY.size() > 0)
  {
    QSizeF size = worldSize();
    double pix = pixelSize();

    // calc sides of bounding box
    double xLft = -0.5*size.width()+20*pix;
    double xRgt =  0.5*size.width()-10*pix;
    double yTop =  0.5*size.height()-10*pix;
    double yBot = -0.5*size.height()+20*pix;

    // get number of values per axis
    double numX = attribute1->getSizeCurValues();
    double numY = attribute2->getSizeCurValues();

    // calc intervals per axis
    double fracX = 1.0;
    if (numX > 1)
    {
      fracX = (1.0/(double)(numX))*(xRgt-xLft);
    }

    double fracY = 1.0;
    if (numY > 1)
    {
      fracY = (1.0/(double)(numY))*(yTop-yBot);
    }

    // clear prev positions
    positions.clear();
    radii.clear();

    // calc positions
    double maxRadius = Utils::maxx(maxRadHintPx*pix, 0.5*Utils::minn(fracX, fracY));

    for (std::size_t i = 0; i < mapXToY.size(); ++i)
    {
      std::vector< Position2D > tempPos;
      positions.push_back(tempPos);
      std::vector< double > tempRad;
      radii.push_back(tempRad);

      for (std::size_t j = 0; j < mapXToY[i].size(); ++j)
      {
        // fraction of total number
        double frac = (double)number[i][j]/(double)maxNumber;

        // radius
        double maxArea = PI*maxRadius*maxRadius;
        double area    = frac*maxArea;
        double radius  = sqrt(area/PI);

        if (radius < minRadHintPx*pix)
        {
          radius = minRadHintPx*pix;
        }
        radii[i].push_back(radius);

        double x = xLft + 0.5*fracX + i*fracX;
        double y = yBot + 0.5*fracY + mapXToY[i][j]*fracY;
        Position2D pos;
        pos.x = x;
        pos.y = y;
        positions[i].push_back(pos);
      }
    }

    // diagram scale factor to draw 120 x 120 pix diagram
    scaleDgrm = 120.0*(pix/2.0);
  }
}


// -- utility data functions ----------------------------------------


void CorrlPlot::clearPositions()
{
  positions.clear();
  radii.clear();
}


// -- hit detection -------------------------------------------------
void CorrlPlot::handleSelection(const Selection&)
{
}


// -- end -----------------------------------------------------------
