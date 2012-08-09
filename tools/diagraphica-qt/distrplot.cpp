// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./distrplot.cpp

#include "distrplot.h"

// -- constructors and destructor -----------------------------------

using namespace std;

DistrPlot::DistrPlot(
  QWidget *parent,
  Graph* g)
  : Visualizer(parent, g)
{
  maxNumber    =  0;
  minHgtHintPx =  4;
  maxWthHintPx = 10;

  diagram        = 0;
  showDgrm       = false;
  attrValIdxDgrm = -1;
}


DistrPlot::~DistrPlot()
{
  clearValues();
  diagram = 0;
}


// -- set functions -------------------------------------------------


void DistrPlot::setValues(
  const size_t& idx,
  const vector< size_t > &num)
{
  clearValues();

  attrIdx = idx;
  number  = num;
  calcMaxNumber();
  calcPositions();
}


void DistrPlot::clearValues()
{
  number.clear();
  attrIdx    = 0;
  maxNumber  = 0;

  clearPositions();
}


void DistrPlot::setDiagram(Diagram* dgrm)
{
  diagram = dgrm;
}


// -- visualization functions  --------------------------------------


void DistrPlot::visualize(const bool& inSelectMode)
{
  // have textures been generated
  if (texCharOK != true)
  {
    genCharTex();
  }

  // check if positions are ok
  if (geomChanged == true)
  {
    calcPositions();
  }

  // visualize
  if (inSelectMode == true)
  {
    GLint hits = 0;
    GLuint selectBuf[512];
    startSelectMode(
      hits,
      selectBuf,
      2.0,
      2.0);

    //setScalingTransf();
    drawPlot(inSelectMode);

    finishSelectMode(
      hits,
      selectBuf);
  }
  else
  {
    clear();
    //setScalingTransf();
    drawPlot(inSelectMode);
    drawAxes(inSelectMode);
    drawLabels(inSelectMode);
    if (showDgrm == true)
    {
      drawDiagram(inSelectMode);
    }
  }
}


void DistrPlot::drawAxes(const bool& inSelectMode)
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
  if (inSelectMode != true)
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


void DistrPlot::drawLabels(const bool& /*inSelectMode*/)
{
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
    string xLabel = graph->getAttribute(attrIdx)->name().toStdString();
    x =  0.0;
    y = -0.5*size.height()+9*pix;
    VisUtils::drawLabelCenter(texCharId, x, y, scaling, xLabel);

    string max = Utils::size_tToStr(maxNumber);
    x = -0.5*size.width()+13*pix;
    y =  0.5*size.height()-10*pix;
    VisUtils::drawLabelVertBelow(texCharId, x, y, scaling, max);

    string min = "0";
    y = -0.5*size.height()+20*pix;
    VisUtils::drawLabelVertAbove(texCharId, x, y, scaling, min);
  }
}


void DistrPlot::drawPlot(const bool& inSelectMode)
{

  double hCanv = worldSize().height();
  double pix = pixelSize();
  size_t sizePositions = positions.size();

  double yBot = -0.5*hCanv + 20*pix;
  // selection mode
  if (inSelectMode == true)
  {

    for (size_t i = 0; i < sizePositions; ++i)
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
    for (size_t i = 0; i < sizePositions; ++i)
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


void DistrPlot::drawDiagram(const bool& inSelectMode)
{
  if (inSelectMode != true)
  {
    double pix      = pixelSize();
    double scaleTxt = ((12*pix)/(double)CHARHEIGHT)/scaleDgrm;

    vector< Attribute* > attrs;
    attrs.push_back(graph->getAttribute(attrIdx));
    vector< double > vals;
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
    diagram->visualize(
      inSelectMode,
      pixelSize(),
      attrs,
      vals);

    VisUtils::setColor(Qt::black);
    VisUtils::drawLabelRight(texCharId, -0.98, 1.1, scaleTxt, msgDgrm);

    glPopMatrix();
  }
}


// -- input event handlers ------------------------------------------



void DistrPlot::handleMouseEvent(QMouseEvent* e)
{
  Visualizer::handleMouseEvent(e);

  // redraw in select mode
  updateGL(true);
  // redraw in render mode
  updateGL();
}

// -- utility data functions ------------------------------------


void DistrPlot::calcMaxNumber()
{
  maxNumber = 0;
  for (size_t i = 0; i < number.size(); ++i)
  {
    if (number[i] > maxNumber)
    {
      maxNumber = number[i];
    }
  }
}


// -- utility drawing functions ---------------------------------

// ***
/*
void DistrPlot::clear()
{
    VisUtils::clear( clearColor );
}
*/

void DistrPlot::setScalingTransf()
{
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}


void DistrPlot::displTooltip(const size_t& posIdx)
{
  if (posIdx < number.size())
  {
    Attribute* attr = graph->getAttribute(attrIdx);
    string xLabel   = attr->name().toStdString();
    string value    = "";
    if (posIdx < static_cast <size_t>(attr->getSizeCurValues()))
    {
      value = attr->getCurValue(posIdx)->getValue();
    }

    msgDgrm.clear();

    // x-axis label
    /*
    msgDgrm.append( xLabel );
    msgDgrm.append( ": " );
    msgDgrm.append( value );
    msgDgrm.append( "\n" );
    */
    // y-axis label
    msgDgrm.append(Utils::size_tToStr(number[posIdx]));
    msgDgrm.append(" nodes; ");
    msgDgrm.append(Utils::dblToStr(
                     Utils::perc((int) number[posIdx], (int) graph->getSizeNodes())));
    msgDgrm.append("%");

    if (diagram == 0)
    {
      setToolTip(QString::fromStdString(msgDgrm));
    }
    else
    {
      QPointF pos = worldCoordinate(m_lastMouseEvent.posF());
      posDgrm.x = pos.x() + (pos.x() < 0 ? 1.0 : -1.0) * scaleDgrm;
      posDgrm.y = pos.y() + (pos.x() < 0 ? 1.0 : -1.0) * scaleDgrm;
      showDgrm = true;
      attrValIdxDgrm = posIdx;
    }

    // free memory
    attr = 0;
  }
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
    double yBot = -0.5*size.width()+20*pix;

    // get number of values per axis
    double numX = graph->getAttribute(attrIdx)->getSizeCurValues();

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
    for (size_t i = 0; i < number.size(); ++i)
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


void DistrPlot::processHits(
  GLint hits,
  GLuint buffer[])
{
  GLuint* ptr;
  ptr = (GLuint*) buffer;

  if (hits > 0)
  {
    // if necassary advance to last hit
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
    ++ptr; // number
    ++ptr; // z1
    ++ptr; // z2

    int name = *ptr;

    displTooltip(name);
  }
  else
  {
    setToolTip(QString());
    showDgrm = false;
  }

  ptr = 0;
}


// -- end -----------------------------------------------------------
