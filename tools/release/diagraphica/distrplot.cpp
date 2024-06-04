// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./distrplot.cpp

#include "distrplot.h"
#include <QToolTip>

// -- constructors and destructor -----------------------------------

DistrPlot::DistrPlot(QWidget* parent, Graph* g, int attributeIndex):
  Visualizer(parent, g)
{
  maxNumber    =  0;
  minHgtHintPx =  4;
  maxWthHintPx = 10;

  diagram        = 0;
  showDgrm       = false;
  attrValIdxDgrm = -1;

  attribute = m_graph->getAttribute(attributeIndex);
  connect(attribute, SIGNAL(deleted()), this, SLOT(close()));
  m_graph->calcAttrDistr(attributeIndex, number);
  calcMaxNumber();
  calcPositions();

  setMouseTracking(true);
}


// -- set functions -------------------------------------------------


void DistrPlot::setDiagram(Diagram* dgrm)
{
  diagram = dgrm;
}


// -- visualization functions  --------------------------------------

template <Visualizer::Mode mode> void DistrPlot::drawAxes()
{
  QSizeF size = worldSize();
  double pix = pixelSize();

  // calc size of bounding box
  double xLft = -0.5*size.width()+20*pix;
  double xRgt =  0.5*size.width()-10*pix;
  double yTop =  0.5*size.height()-10*pix;
  double yBot = -0.5*size.height()+20*pix;
  double yMid =  0.5*(yTop+yBot);

  // rendering mode
  if constexpr (mode == Visualizing)
  {
    // draw guides
    VisUtils::setColor(VisUtils::lightGray);
    VisUtils::drawLine(xLft, xRgt, yTop, yTop);
    VisUtils::drawLine(xLft, xRgt, yMid, yMid);
    VisUtils::drawLine(xRgt, xRgt, yBot, yTop);

    // x- & y-axis
    VisUtils::setColor(VisUtils::mediumGray);
    VisUtils::drawLine(xLft, xLft, yBot, yTop);
    VisUtils::drawLine(xLft, xRgt, yBot, yBot);
  }
}


template <Visualizer::Mode mode> void DistrPlot::drawLabels()
{
  if constexpr (mode == Marking)
  {
    return;
  }
  QSizeF size = worldSize();
  double pix = pixelSize();
  // calc scaling to use
  double scaling = (12*pix)/(double)CHARHEIGHT;

  // color
  VisUtils::setColor(Qt::black);

  // y-axis labels
  double x = -0.5*size.width()+9*pix;
  double y =  0;
  VisUtils::drawLabelVertCenter(texCharId, x, y, scaling, "Number");

  if (number.size() > 0)
  {
    // x-axis label
    std::string xLabel = attribute->name().toStdString();
    x =  0.0;
    y = -0.5*size.height()+9*pix;
    VisUtils::drawLabelCenter(texCharId, x, y, scaling, xLabel);

    std::string max = Utils::size_tToStr(maxNumber);
    x = -0.5*size.width()+13*pix;
    y =  0.5*size.height()-10*pix;
    VisUtils::drawLabelVertBelow(texCharId, x, y, scaling, max);

    std::string min = "0";
    y = -0.5*size.height()+20*pix;
    VisUtils::drawLabelVertAbove(texCharId, x, y, scaling, min);
  }
}


template <Visualizer::Mode mode> void DistrPlot::drawPlot()
{

  double hCanv = worldSize().height();
  double pix = pixelSize();
  std::size_t sizePositions = positions.size();

  double yBot = -0.5*hCanv + 20*pix;
  // selection mode
  if constexpr (mode == Marking)
  {

    for (std::size_t i = 0; i < sizePositions; ++i)
    {
      double xLft = positions[i].x - 0.5*width;
      double xRgt = positions[i].x + 0.5*width;
      double yTop = positions[i].y;

      glPushName((GLuint) i);
      VisUtils::fillRect(xLft, xRgt, yTop, yBot);
      glPopName();
    }
  }
  // rendering mode
  else
  {
    for (std::size_t i = 0; i < sizePositions; ++i)
    {
      double xLft = positions[i].x - 0.5*width;
      double xRgt = positions[i].x + 0.5*width;
      double yTop = positions[i].y;

      VisUtils::setColor(VisUtils::coolGreen, 0.7);

      if (xRgt-xLft < pix)
      {
        VisUtils::drawLine(xLft, xLft, yTop, yBot);
      }
      else
      {
        VisUtils::enableBlending();
        VisUtils::fillRect(xLft, xRgt, yTop, yBot);
        VisUtils::disableBlending();
      }
    }
  }
}


template <Visualizer::Mode mode> void DistrPlot::drawDiagram()
{
  if constexpr (mode == Visualizing)
  {
    double pix      = pixelSize();
    double scaleTxt = ((12*pix)/(double)CHARHEIGHT)/scaleDgrm;

    std::vector< Attribute* > attrs;
    attrs.push_back(attribute);
    std::vector< double > vals;
    vals.push_back(attrValIdxDgrm);

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
}


template <Visualizer::Mode mode> void DistrPlot::draw()
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
    drawPlot<mode>();
    drawAxes<mode>();
    drawLabels<mode>();
    if (showDgrm)
    {
      drawDiagram<mode>();
    }
  }
}


void DistrPlot::visualize() { draw<Visualizing>(); }
void DistrPlot::mark() { draw<Marking>(); }


// -- input event handlers ------------------------------------------

void DistrPlot::mouseMoveEvent(QMouseEvent* event)
{
  Visualizer::mouseMoveEvent(event);
  SelectionList selections = getSelection();
  if (selections.empty() || selections.back().empty())
  {
    hideTooltip();
  }
  else
  {
    showTooltip(selections.back()[0], event->position());
  }
  update();
}

// -- utility data functions ------------------------------------


void DistrPlot::calcMaxNumber()
{
  maxNumber = 0;
  for (std::size_t i = 0; i < number.size(); ++i)
  {
    if (number[i] > maxNumber)
    {
      maxNumber = number[i];
    }
  }
}


// -- utility drawing functions ---------------------------------
// TODO: merge this function with the corrln and combn plots' version
void DistrPlot::showTooltip(std::size_t valueIndex, const QPointF& position)
{
  if (valueIndex < number.size())
  {
    msgDgrm.clear();

    // Todo: add value labels on a new line, do the same for corrl/combn plots
    /*if (valueIndex < attribute->getSizeCurValues())
    {
      // x-axis label
      msgDgrm = Utils::abbreviate(attribute->getCurValue(valueIndex)->getValue(), 10);
    }*/

    // y-axis label
    msgDgrm = Utils::size_tToStr(number[valueIndex]) + " nodes; "
      + Utils::dblToStr(Utils::perc((int)number[valueIndex], (int)m_graph->getSizeNodes())) + '%';

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
      attrValIdxDgrm = valueIndex;
    }
  }
}

void DistrPlot::hideTooltip()
{
  QToolTip::hideText();
  showDgrm = false;
}


void DistrPlot::calcPositions()
{
  // update flag
  geomChanged = false;

  // calc positions
  if (number.size() > 0)
  {
    QSizeF size = worldSize();
    double pix = pixelSize();

    // calc size of bounding box
    double xLft = -0.5*size.width()+20*pix;
    double xRgt =  0.5*size.width()-10*pix;
    double yTop =  0.5*size.height()-10*pix;
    double yBot = -0.5*size.height()+20*pix;

    // get number of values per axis
    double numX = attribute->getSizeCurValues();

    // get intervals for x-axis
    double fracX = 1.0;
    if (numX > 1)
    {
      fracX = (1.0 / (double)numX)*(xRgt-xLft);
    }

    // calc width
    if (fracX < maxWthHintPx*pix)
    {
      width = fracX;
    }
    else
    {
      width = maxWthHintPx*pix;
    }

    // calc positions
    positions.clear();
    for (std::size_t i = 0; i < number.size(); ++i)
    {
      // calc ratio
      double ratio = (double)number[i]/(double)maxNumber;

      // center, top
      double x = xLft + 0.5*fracX + i*fracX;
      double y = yBot + ratio*(yTop-yBot);
      if (y-yBot < pix*minHgtHintPx)
      {
        y += pix*minHgtHintPx;
      }

      Position2D pos;
      pos.x = x;
      pos.y = y;
      positions.push_back(pos);
    }

    // diagram scale factor to draw 120 x 120 pix diagram
    scaleDgrm = 120.0*(pix/2.0);
  }
}


void DistrPlot::clearPositions()
{
  positions.clear();
  width = 0.0;
}


// -- hit detection -------------------------------------------------

void DistrPlot::handleSelection(const Selection&)
{
}


// -- end -----------------------------------------------------------
