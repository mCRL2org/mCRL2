// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./corrlplot.cpp

#include "wx.hpp" // precompiled headers

#include "corrlplot.h"

using namespace std;

// -- constructors and destructor -----------------------------------


CorrlPlot::CorrlPlot(
  Mediator* m,
  Graph* g,
  GLCanvas* c)
  : Visualizer(m, g, c)
{
  minRadHintPx =  5;
  maxRadHintPx = 25;

  diagram         = NULL;
  showDgrm        = false;
  attrValIdx1Dgrm = NON_EXISTING;
  attrValIdx2Dgrm = NON_EXISTING;
}


CorrlPlot::~CorrlPlot()
{
  clearValues();
  diagram = NULL;
}


// -- set data functions --------------------------------------------


void CorrlPlot::setValues(
  const size_t& idx1,
  const size_t& idx2,
  const vector< vector< size_t > > &mapXY,
  const vector< vector< int > > &num)
{
  clearValues();
  attrIdx1 = idx1;
  attrIdx2 = idx2;
  mapXToY = mapXY;
  number  = num;
  initLabels();
  calcMaxNumber();
  calcPositions();
}


void CorrlPlot::clearValues()
{
  attrIdx1  = NON_EXISTING;
  attrIdx2  = NON_EXISTING;
  maxNumber = 0;
  maxNumX.clear();
  sumMaxNumX = 0;
  maxNumY.clear();
  sumMaxNumY = 0;
  mapXToY.clear();
  number.clear();
  clearPositions();
}


void CorrlPlot::setDiagram(Diagram* dgrm)
{
  diagram = dgrm;
}


// -- visualization functions  --------------------------------------


void CorrlPlot::visualize(const bool& inSelectMode)
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
    //drawNumberPlot( inSelectMode );
    drawPlot(inSelectMode);

    finishSelectMode(
      hits,
      selectBuf);
  }
  else
  {
    clear();
    //setScalingTransf();
    drawAxes(
      inSelectMode,
      "x-label",
      "y-label");
    drawLabels(inSelectMode);
    drawPlot(inSelectMode);
    if (showDgrm == true)
    {
      drawDiagram(inSelectMode);
    }
  }
}


void CorrlPlot::drawAxes(
  const bool& inSelectMode,
  const string& /*xLbl*/,
  const string& /*yLbl*/)
{
  // get size of sides
  double w, h;
  canvas->getSize(w, h);
  // get size of 1 pixel
  double pix = canvas->getPixelSize();

  // calc size of bounding box
  double xLft = -0.5*w+20*pix;
  double xRgt =  0.5*w-10*pix;
  double yTop =  0.5*h-10*pix;
  double yBot = -0.5*h+20*pix;

  // rendering mode
  if (inSelectMode != true)
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


void CorrlPlot::drawLabels(const bool& /*inSelectMode*/)
{
  // get size of sides
  double w, h;
  canvas->getSize(w, h);
  // get size of 1 pixel
  double pix = canvas->getPixelSize();
  // calc scaling to use
  double scaling = (12*pix)/(double)CHARHEIGHT;

  // color
  VisUtils::setColor(Qt::black);

  if (mapXToY.size() > 0)
  {
    // x-axis label
    double x =  0.0;
    double y = -0.5*h+9*pix;
    VisUtils::drawLabelCenter(texCharId, x, y, scaling, xLabel);

    // y-axis labels
    x = -0.5*w+9*pix;
    y =  0;
    VisUtils::drawLabelVertCenter(texCharId, x, y, scaling, yLabel);
  }
}


void CorrlPlot::drawPlot(const bool& inSelectMode)
{
  // selection mode
  if (inSelectMode == true)
  {
    for (size_t i = 0; i < positions.size(); ++i)
    {
      glPushName((GLuint) i);
      for (size_t j = 0; j < positions[i].size(); ++j)
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
    double pix   = canvas->getPixelSize();

    for (size_t i = 0; i < positions.size(); ++i)
    {
      for (size_t j = 0; j < positions[i].size(); ++j)
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


void CorrlPlot::drawDiagram(const bool& inSelectMode)
{
  double pix = canvas->getPixelSize();
  double scaleTxt = ((12*pix)/(double)CHARHEIGHT)/scaleDgrm;

  vector< Attribute* > attrs;
  attrs.push_back(graph->getAttribute(attrIdx1));
  attrs.push_back(graph->getAttribute(attrIdx2));

  vector< double > vals;
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
  diagram->visualize(
    inSelectMode,
    canvas,
    attrs,
    vals);

  VisUtils::setColor(Qt::black);
  VisUtils::drawLabelRight(texCharId, -0.98, 1.1, scaleTxt, msgDgrm);

  glPopMatrix();
}


// -- input event handlers ------------------------------------------


void CorrlPlot::handleMouseMotionEvent(
  const int& x,
  const int& y)
{
  Visualizer::handleMouseMotionEvent(x, y);

  // redraw in select mode
  visualize(true);
  // redraw in render mode
  visualize(false);
}

/*
void CorrlPlot::handleMouseEnterEvent()
{}
*/
/*
void CorrlPlot::handleMouseLeaveEvent()
{}
*/


// -- utility data functions ----------------------------------------


void CorrlPlot::initLabels()
{
  if (attrIdx1 != NON_EXISTING && attrIdx2 != NON_EXISTING)
  {
    xLabel = graph->getAttribute(attrIdx1)->getName();
    yLabel = graph->getAttribute(attrIdx2)->getName();
  }
  else
  {
    xLabel = "";
    yLabel = "";
  }
}


void CorrlPlot::calcMaxNumber()
{
  // init max number & totals
  sumMaxNumX = 0;
  sumMaxNumY = 0;
  maxNumber  = 0;

  // init max row & col numbers
  if (attrIdx1 != NON_EXISTING && attrIdx2 != NON_EXISTING)
  {
    size_t sizeX = graph->getAttribute(attrIdx1)->getSizeCurValues();
    size_t sizeY = graph->getAttribute(attrIdx2)->getSizeCurValues();
    {
      for (size_t i = 0; i < sizeX; ++i)
      {
        maxNumX.push_back(0);
      }
    }

    {
      for (size_t i = 0; i < sizeY; ++i)
      {
        maxNumY.push_back(0);
      }
    }
  }

  // calc max
  {
    for (size_t i = 0; i < number.size(); ++i)
    {
      for (size_t j = 0; j < number[i].size(); ++j)
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
    for (size_t i = 0; i < maxNumX.size(); ++i)
    {
      sumMaxNumX += maxNumX[i];
    }
  }
  {
    for (size_t i = 0; i < maxNumY.size(); ++i)
    {
      sumMaxNumY += maxNumY[i];
    }
  }
}


// -- utility drawing functions -------------------------------------

// ***
/*
void CorrlPlot::clear()
{
    VisUtils::clear( clearColor );
}
*/

void CorrlPlot::setScalingTransf()
{
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  double f = canvas->getScaleFactor();
  glScalef(f, f, f);
  glTranslatef(
    canvas->getXTranslation(),
    canvas->getYTranslation(),
    0.0);
}


void CorrlPlot::displTooltip(
  const int& xIdx,
  const int& yIdx)
{
  msgDgrm.clear();
  /*
  // x-axis label
  msgDgrm.append( xLabel );
  msgDgrm.append( ": " );
  msgDgrm.append( graph->getAttribute( attrIdx1 )->getCurValue( xIdx )->getValue() );
  msgDgrm.append( "; " );
  // y-axis label
  msgDgrm.append( yLabel );
  msgDgrm.append( ": " );
  msgDgrm.append( graph->getAttribute( attrIdx2 )->getCurValue( mapXToY[xIdx][yIdx] )->getValue() );
  msgDgrm.append( "; " );
  */
  // number
  msgDgrm.append(Utils::dblToStr(number[xIdx][ yIdx ]));
  msgDgrm.append(" nodes; ");
  // percentage
  msgDgrm.append(Utils::dblToStr(
                   Utils::perc((double) number[xIdx][ yIdx ], (double) graph->getSizeNodes())));
  msgDgrm.append("%");

  if (diagram == NULL)
  {
    // show tooltip
    canvas->showToolTip(msgDgrm);
  }
  else
  {
    // calc diagram position
    double xM, yM;
    double xD, yD;
    canvas->getWorldCoords(xMouseCur, yMouseCur, xM, yM);

    if (xM < 0)
    {
      xD = xM+1.0*scaleDgrm;
    }
    else
    {
      xD = xM-1.0*scaleDgrm;
    }

    if (yM < 0)
    {
      yD = yM+1.0*scaleDgrm;
    }
    else
    {
      yD = yM-1.0*scaleDgrm;
    }

    posDgrm.x = xD;
    posDgrm.y = yD;

    showDgrm       = true;
    attrValIdx1Dgrm = xIdx;
    attrValIdx2Dgrm = mapXToY[xIdx][yIdx];
  }
}


void CorrlPlot::calcPositions()
{
  // update flag
  geomChanged = false;

  if (mapXToY.size() > 0)
  {
    // get size of canvas & 1 pixel
    double w,h;
    canvas->getSize(w, h);
    double pix = canvas->getPixelSize();

    // calc sides of bounding box
    double xLft = -0.5*w+20*pix;
    double xRgt =  0.5*w-10*pix;
    double yTop =  0.5*h-10*pix;
    double yBot = -0.5*h+20*pix;

    // get number of values per axis
    double numX = graph->getAttribute(attrIdx1)->getSizeCurValues();
    double numY = graph->getAttribute(attrIdx2)->getSizeCurValues();

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
    double maxRadius;
    if ((0.5*fracX > maxRadHintPx*pix) &&
        (0.5*fracY > maxRadHintPx*pix))
    {
      maxRadius = 0.5*Utils::minn(fracX, fracY);
    }
    else
    {
      maxRadius = maxRadHintPx*pix;
      yTop -= (maxRadius-10*pix);
      if (numY > 1)
      {
        fracY = (1.0/(double)(numY))*(yTop-yBot);
      }
      else
      {
        fracY = 1.0;
      }
    }

    for (size_t i = 0; i < mapXToY.size(); ++i)
    {
      vector< Position2D > tempPos;
      positions.push_back(tempPos);
      vector< double > tempRad;
      radii.push_back(tempRad);

      for (size_t j = 0; j < mapXToY[i].size(); ++j)
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


void CorrlPlot::processHits(
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

    int name1 = *ptr;
    ++ptr; // name1
    int name2 = *ptr;

    displTooltip(name1, name2);
  }
  else
  {
    canvas->clearToolTip();
    showDgrm = false;
  }

  ptr = NULL;
}


// -- end -----------------------------------------------------------
