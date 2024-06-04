// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./combnplot.cpp

#include "combnplot.h"
#include <QToolTip>

// -- constructors and destructor -----------------------------------

CombnPlot::CombnPlot(QWidget* parent, Graph* graph, const std::vector<std::size_t> &attributeIndices):
  Visualizer(parent, graph)
{
  maxAttrCard      = 0;
  maxNumberPerComb = 0;

  minHgtHintPixBC =  4;
  maxWthHintPixBC = 10;

  mouseCombnIdx = NON_EXISTING;

  diagram        = 0;
  showDgrm       = false;

  for (std::size_t i = 0; i < attributeIndices.size(); ++i)
  {
    Attribute *attribute = m_graph->getAttribute(attributeIndices[i]);
    attributes.push_back(attribute);
    connect(attribute, SIGNAL(deleted()), this, SLOT(close()));
  }
  m_graph->calcAttrCombn(attributeIndices, combinations, numberPerComb);
  initLabels();
  calcMaxAttrCard();
  calcMaxNumberPerComb();
  calcPositions();

  setMouseTracking(true);
}


// -- set data functions --------------------------------------------

void CombnPlot::setDiagram(Diagram* dgrm)
{
  diagram = dgrm;
}


// -- visualization functions  --------------------------------------


template <Visualizer::Mode mode> void CombnPlot::drawAxesBC()
{
  QSizeF size = worldSize();
  // get size of 1 pixel
  double pix = pixelSize();
  // get num attributes
  std::size_t numAttr = attributes.size();

  // calc size of bounding box
  double xLft = -0.5*size.width()+25*pix;
  double xRgt =  0.5*size.width()-10*pix;
  double yTop =  0.5*size.height()-10*pix;
  double yBot;
  if (numAttr > 0)
  {
    yBot = yTop - (size.height() - (20+10+10)*pix)/(double)(numAttr+1);
  }
  else
  {
    yBot = yTop - 0.5*(size.height() - (20+10+10)*pix);
  }

  // rendering mode
  if constexpr (mode == Visualizing)
  {
    // draw guides
    VisUtils::setColor(VisUtils::lightGray);
    VisUtils::drawLine(xLft, xRgt, yTop, yTop);
    VisUtils::drawLine(xRgt, xRgt, yBot, yTop);

    // x- & y-axis
    VisUtils::setColor(VisUtils::mediumGray);
    VisUtils::drawLine(xLft, xLft, yBot, yTop);
    VisUtils::drawLine(xLft, xRgt, yBot, yBot);
  }
}


template <Visualizer::Mode mode> void CombnPlot::drawAxesCP()
{
  QSizeF size = worldSize();
  // get size of 1 pixel
  double pix = pixelSize();
  // get num attributes
  std::size_t numAttr = attributes.size();

  // calc size of bounding box
  double xLft = -0.5*size.width()+25*pix;
  double xRgt =  0.5*size.width()-10*pix;
  double yBot = -0.5*size.height()+20*pix;
  double yTop;
  if (numAttr > 0)
  {
    yTop = yBot + numAttr*(size.height() - (20+10+10)*pix)/(double)(numAttr+1);
  }
  else
  {
    yTop = yBot + 0.5*(size.height() - (20+10+10)*pix);
  }

  // rendering mode
  if constexpr (mode == Visualizing)
  {
    VisUtils::setColor(VisUtils::mediumGray);

    // draw guides
    VisUtils::drawLine(xLft, xRgt, yTop, yTop);
    VisUtils::drawLine(xRgt, xRgt, yBot, yTop);

    // x- & y-axis
    VisUtils::drawLine(xLft, xLft, yBot, yTop);
    VisUtils::drawLine(xLft, xRgt, yBot, yBot);

    // subdivisions for attributes
    if (posRgtBot.size() > 0 && posRgtBot[0].size() > 1)
    {
      VisUtils::setColor(VisUtils::mediumGray);
      xLft += pix;
      xRgt -= pix;
      for (std::size_t i = 0; i < posRgtBot[0].size()-1; ++i)
      {
        yTop = posRgtBot[0][i].y;
        yBot = yTop;
        VisUtils::drawLine(xLft, xRgt, yTop, yBot);
      }
    }
  }
}


template <Visualizer::Mode mode> void CombnPlot::drawAxes()
{
  // bar chart
  drawAxesBC<mode>();
  // combination plot
  drawAxesCP<mode>();
}


template <Visualizer::Mode mode> void CombnPlot::drawLabelsBC()
{
  if constexpr (mode == Marking)
  {
    return;
  }
  QSizeF size = worldSize();
  // get size of 1 pixel
  double pix = pixelSize();
  // calc scaling to use
  double scaling = (12*pix)/(double)CHARHEIGHT;
  // number attributes
  std::size_t numAttr = attributes.size();

  // color
  VisUtils::setColor(Qt::black);

  // y-axis labels
  double xLft = -0.51*size.width()+3*pix;
  double xRgt = -0.50*size.width()+12*pix;
  double yTop =  0.50*size.height()-10*pix;
  double yBot;
  if (numAttr > 0)
  {
    yBot = yTop - (size.height() - (20+10+10)*pix)/(double)(numAttr+1);
  }
  else
  {
    yBot = yTop - 0.5*(size.height() - (20+10+10)*pix);
  }

  VisUtils::drawLabelInBoundBox(
    texCharId,
    xLft,      xRgt,
    yTop,      yBot,
    scaling,   "Number");

  if (combinations.size() > 0)
  {
    // max number
    std::string max = Utils::intToStr((int) maxNumberPerComb);
    double x   = -0.5*size.width()+13*pix;
    double y   =  0.5*size.height()-10*pix;
    VisUtils::drawLabelVertBelow(texCharId, x, y, scaling, max);

    // min number
    std::string min = "0";
    y   = yBot;
    VisUtils::drawLabelVertAbove(texCharId, x, y, scaling, min);
  }
}


template <Visualizer::Mode mode> void CombnPlot::drawLabelsCP()
{
  if constexpr (mode == Marking)
  {
    return;
  }
  QSizeF size = worldSize();
  // get size of 1 pixel
  double pix = pixelSize();
  // calc scaling to use
  double scaling = (12*pix)/(double)CHARHEIGHT;
  // number attributes
  double numAttr = attributes.size();

  // color
  VisUtils::setColor(Qt::black);

  // x-axis label
  double x =  0.0;
  double y =  -0.5*size.height()+9*pix;
  VisUtils::drawLabelCenter(texCharId, x, y, scaling, "Combinations");

  if (numAttr > 0)
  {
    // y-axis labels

    double xLft = -0.51*size.width()+3*pix;
    double xRgt = -0.5*size.width()+12*pix;

    for (std::size_t i = 0; i < attributeLabels.size(); ++i)
    {
      double yTop;
      if (i > 0)
      {
        yTop = posRgtBot[0][i-1].y;
      }
      else
      {
        yTop = -0.5*size.height()+20*pix + numAttr*(size.height() - (20+10+10)*pix)/(double)(numAttr+1);
      }


      double yBot = posRgtBot[0][i].y;

      VisUtils::drawLabelInBoundBox(
        texCharId,
        xLft,      xRgt,
        yTop,      yBot,
        scaling,   attributeLabels[i]);
    }
  }
}


template <Visualizer::Mode mode> void CombnPlot::drawLabels()
{
  // bar chart
  drawLabelsBC<mode>();
  // combination plot
  drawLabelsCP<mode>();
}


template <Visualizer::Mode mode> void CombnPlot::drawPlotBC()
{
  double hCanv = worldSize().height();
  double pix = pixelSize();
  std::size_t sizePositions = posBC.size();
  std::size_t numAttr = attributes.size();

  double yBot;
  if (sizePositions > 0)
  {
    yBot = 0.5*hCanv-10*pix - (hCanv - (20+10+10)*pix)/(double)(numAttr+1);
  }
  else
  {
    yBot = 0.5*hCanv-10*pix - 0.5*(hCanv - (20+10+10)*pix);
  }

  // selection mode
  if constexpr (mode == Marking)
  {
    for (std::size_t i = 0; i < sizePositions; ++i)
    {
      double xLft = posBC[i].x - 0.5*widthBC;
      double xRgt = posBC[i].x + 0.5*widthBC;
      double yTop = posBC[i].y;

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
      double xLft = posBC[i].x - 0.5*widthBC;
      double xRgt = posBC[i].x + 0.5*widthBC;
      double yTop = posBC[i].y;

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


template <Visualizer::Mode mode> void CombnPlot::drawPlotCP()
{
  double pix   = pixelSize();

  // selection mode
  if constexpr (mode == Marking)
  {
    for (std::size_t i = 0; i < posLftTop.size(); ++i)
    {
      // name per collumn
      glPushName((GLuint) i);

      double xLft = posLftTop[i][0].x;
      double xRgt = posRgtBot[i][0].x;

      double yTop = posLftTop[i][0].y;
      double yBot = posRgtBot[i][posRgtBot[i].size()-1].y;

      VisUtils::fillRect(xLft, xRgt, yTop, yBot);

      glPopName();
    }
  }
  // rendering mode
  else
  {
    for (std::size_t i = 0; i < posLftTop.size(); ++i)
    {
      for (std::size_t j = 0; j < posLftTop[i].size(); ++j)
      {
        double xLft = posLftTop[i][j].x;
        double yTop = posLftTop[i][j].y;

        double xRgt = posRgtBot[i][j].x;
        double yBot = posRgtBot[i][j].y;

        VisUtils::setColor(VisUtils::qualPair(combinations[i][j], maxAttrCard), 0.5);

        if (xRgt - xLft > pix)
        {
          VisUtils::enableBlending();
          VisUtils::fillRect(xLft, xRgt, yTop, yBot);
          VisUtils::disableBlending();
        }
        else
        {
          VisUtils::enableLineAntiAlias();
          VisUtils::drawLine(xLft, xLft, yTop, yBot);
          VisUtils::disableLineAntiAlias();
        }
      } // for j
    } // for i
  } // if .. else
}


template <Visualizer::Mode mode> void CombnPlot::drawPlot()
{
  // bar chart
  drawPlotBC<mode>();
  // combination plot
  drawPlotCP<mode>();
}


template <Visualizer::Mode mode> void CombnPlot::drawMousePos()
{
  // rendering mode
  if constexpr (mode == Visualizing)
  {
    if (mouseCombnIdx < combinations.size())
    {
      // x-coords
      double xLft = posLftTop[mouseCombnIdx][0].x;
      double xRgt = posRgtBot[mouseCombnIdx][0].x;
      // y-coords
      double yTop = posLftTop[mouseCombnIdx][0].y;
      double yBot = posRgtBot[mouseCombnIdx][posRgtBot[mouseCombnIdx].size()-1].y;

      // draw
      VisUtils::setColor(Qt::red);
      VisUtils::drawRect(xLft, xRgt, yTop, yBot);
    }
  }
}


template <Visualizer::Mode mode> void CombnPlot::drawDiagram()
{
  if constexpr (mode == Visualizing)
  {
    double pix      = pixelSize();
    double scaleTxt = ((12*pix)/(double)CHARHEIGHT)/scaleDgrm;

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
    diagram->draw<mode>(pixelSize(), attributes, attrValIdcsDgrm);

    VisUtils::setColor(Qt::black);
    VisUtils::drawLabelRight(texCharId, -0.98, 1.1, scaleTxt, msgDgrm);

    glPopMatrix();
  }
}


template <Visualizer::Mode mode> void CombnPlot::draw()
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

  // selection mode
  if constexpr (mode == Marking)
  {
    drawPlot<mode>();
  }
  // rendering mode
  else
  {
    clear();
    drawPlot<mode>();
    drawAxes<mode>();
    drawLabels<mode>();
    drawMousePos<mode>();
    if (showDgrm)
    {
      drawDiagram<mode>();
    }
  }
}


void CombnPlot::visualize() { draw<Visualizing>(); }
void CombnPlot::mark() { draw<Marking>(); }


// -- input event handlers ------------------------------------------


void CombnPlot::mouseMoveEvent(QMouseEvent* event)
{
  Visualizer::mouseMoveEvent(event);
  SelectionList selections = getSelection();
  if (selections.empty() || selections.back().empty())
  {
    mouseCombnIdx = NON_EXISTING;
    hideTooltip();
  }
  else
  {
    mouseCombnIdx = selections.back()[0];
    showTooltip(selections.back()[0], event->position());
  }
  update();
}


// -- utility data functions ----------------------------------------


void CombnPlot::initLabels()
{
  attributeLabels.clear();
  for (std::size_t i = 0; i < attributes.size(); ++i)
    attributeLabels.push_back(attributes[i]->name().toStdString());
}


void CombnPlot::calcMaxAttrCard()
{
  maxAttrCard = 0;
  for (std::size_t i = 0; i < attributes.size(); ++i)
  {
    if (attributes[i]->getSizeCurValues() > maxAttrCard)
      maxAttrCard = attributes[i]->getSizeCurValues();
  }
}


void CombnPlot::calcMaxNumberPerComb()
{
  maxNumberPerComb = 0;
  for (std::size_t i = 0; i < numberPerComb.size(); ++i)
  {
    if (numberPerComb[i] > maxNumberPerComb)
    {
      maxNumberPerComb = numberPerComb[i];
    }
  }
}


// -- utility drawing functions -------------------------------------

void CombnPlot::showTooltip(const std::size_t& valueIndex, const QPointF& position)
{
  if (valueIndex < combinations.size())
  {
    msgDgrm = Utils::dblToStr(numberPerComb[valueIndex]) + " nodes; "
      + Utils::dblToStr(Utils::perc((double) numberPerComb[valueIndex], (double) m_graph->getSizeNodes())) + '%';

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

      attrValIdcsDgrm.clear();
      for (std::size_t i = 0; i < attributes.size(); ++i)
      {
        attrValIdcsDgrm.push_back(combinations[valueIndex][i]);
      }
    }
  }
}


void CombnPlot::hideTooltip()
{
  QToolTip::hideText();
  showDgrm = false;
}


void CombnPlot::calcPositions()
{
  // update flag
  geomChanged = false;

  // bar chart
  calcPosBC();
  // combination plot
  calcPosCP();

  // diagram scale factor to draw 120 x 120 pix diagram
  double pix = pixelSize();
  scaleDgrm = 120.0*(pix/2.0);
}


void CombnPlot::calcPosBC()
{
  QSizeF size = worldSize();
  double pix = pixelSize();
  // number of attributes
  double numAttr = attributes.size();

  // calc size of bounding box
  double xLft = -0.5*size.width()+25*pix;
  double xRgt =  0.5*size.width()-10*pix;
  double yTop =  0.5*size.height()-10*pix;
  double yBot;
  if (numAttr > 0)
  {
    yBot = yTop - (size.height() - (20+10+10)*pix)/(double)(numAttr+1);
  }
  else
  {
    yBot = yTop - 0.5*(size.height() - (20+10+10)*pix);
  }

  // get number of values per axis
  double numX = combinations.size();

  // get intervals for x-axis
  double fracX = 1.0;
  if (numX > 1)
  {
    fracX = (1.0 / (double)numX)*(xRgt-xLft);
  }

  // calc width
  //if ( fracX < maxWthHintPixBC*pix )
  widthBC = fracX;
  //else
  //    widthBC = maxWthHintPixBC*pix;

  // calc positions
  posBC.clear();
  for (std::size_t i = 0; i < numberPerComb.size(); ++i)
  {
    // calc ratio
    double ratio = (double)numberPerComb[i]/(double)maxNumberPerComb;

    // center, top
    double x = xLft + 0.5*fracX + i*fracX;
    double y = yBot + ratio*(yTop-yBot);
    if (y-yBot < pix*minHgtHintPixBC)
    {
      y += pix*minHgtHintPixBC;
    }

    Position2D pos;
    pos.x = x;
    pos.y = y;
    posBC.push_back(pos);
  }
}


void CombnPlot::calcPosCP()
{
  QSizeF size = worldSize();
  double pix = pixelSize();
  std::size_t numAttr = attributes.size();

  double xLft = -0.5*size.width()+25*pix;
  double xRgt =  0.5*size.width()-10*pix;
  double yBot = -0.5*size.height()+20*pix;
  double yTop;
  if (numAttr > 0)
  {
    yTop = yBot + numAttr*(size.height() - (20+10+10)*pix)/(double)(numAttr+1);
  }
  else
  {
     yTop = yBot + 0.5*(size.height() - (20+10+10)*pix);
  }

  // get number of values per axis
  double numX = combinations.size();
  double numY = attributes.size();

  // get intervals per axis
  double fracX = 1;
  if (numX > 1)
  {
    fracX = (double)1 / (double)numX;
  }

  double fracY = 1;
  if (numY > 1)
  {
    fracY = (double)1 / (double)numY;
  }

  // calc positions
  posLftTop.clear();
  posRgtBot.clear();
  for (std::size_t i = 0; i < combinations.size(); ++i)
  {
    std::vector< Position2D > temp;
    posLftTop.push_back(temp);
    posRgtBot.push_back(temp);

    for (std::size_t j = 0; j < combinations[i].size(); ++j)
    {
      // calc ratio
      std::size_t card  = attributes[j]->getSizeCurValues();
      if (card > 0)
      {
        double ratio = 1.0; //(double)(idx+1)/(double)(card+1);

        // left
        double x1 = xLft + i*fracX*(xRgt-xLft);
        // right
        double x2 = xLft + (i+1)*fracX*(xRgt-xLft);
        // bot
        double y2 = yTop - (j+1)*fracY*(yTop-yBot);
        // top
        double y1 = y2   + ratio*fracY*(yTop-yBot);

        Position2D posLT;
        posLT.x = x1;
        posLT.y = y1;
        posLftTop[i].push_back(posLT);

        Position2D posRB;
        posRB.x = x2;
        posRB.y = y2;
        posRgtBot[i].push_back(posRB);
      }
    }
  }
}


void CombnPlot::clearPositions()
{
  posLftTop.clear();
  posRgtBot.clear();
}


// -- hit detection -------------------------------------------------
void CombnPlot::handleSelection(const Selection&)
{
}


// -- end -----------------------------------------------------------
