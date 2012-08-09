// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./opacitychooser.cpp

#include "opacitychooser.h"

using namespace std;

// -- constructors and destructor -----------------------------------


OpacityChooser::OpacityChooser(
  QWidget *parent,
  Mediator* m,
  Graph* g):
  Visualizer(parent, g),
  Colleague(m)
{
  hdlSize  = 5.0;

  active  = false;
  dragIdx = -1;
}


OpacityChooser::~OpacityChooser()
{}


// -- set functions -------------------------------------------------


void OpacityChooser::setActive(const bool& flag)
{
  active = flag;
}


void OpacityChooser::setPoints(
  const vector< double > &opa,
  const vector< double > &y)
{
  positionsX.clear();
  for (size_t i = 0; i < opa.size(); ++i)
  {
    positionsX.push_back((2.0*opa[i])-1.0);
  }

  positionsY = y;
}


// -- visualization functions  --------------------------------------


void OpacityChooser::visualize(const bool& inSelectMode)
{
  /*
  // check if positions are ok
  if ( geomChanged == true )
      calcPositions();
  */

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

    drawPoints(inSelectMode);

    finishSelectMode(
      hits,
      selectBuf);
  }
  else
  {
    clear();
    drawGrayScale();
    drawPath(inSelectMode);
    drawPoints(inSelectMode);
  }
}


// -- event handlers ------------------------------------------------


void OpacityChooser::handleMouseEvent(QMouseEvent* e)
{
  if (e->type() == QEvent::MouseButtonPress && e->button() == Qt::LeftButton)
  {
    mediator->handleDOFOpaActivate();
    mediator->setDOFOpacitySelected();
  }
  if (active == true)
  {
    Visualizer::handleMouseEvent(e);
    if (e->type() == QEvent::MouseButtonPress && e->button() == Qt::LeftButton)
    {
      // selection mode
      updateGL(true);
      // normal mode
      updateGL();

      if (dragIdx == -1)
      {
        // get cur mouse position
        QPointF pos = worldCoordinate(e->posF());

        // normalize mouse position
        double xCur = pos.x() / (0.5 * worldSize().width());
        double yCur = pos.y() / (0.5 * worldSize().height());

        positionsX.push_back(xCur);
        positionsY.push_back(yCur);

        mediator->handleDOFOpaAdd(0.5*(xCur+1), yCur);
      }
    }
    else if (e->type() == QEvent::MouseButtonRelease && e->button() == Qt::LeftButton)
    {
      dragIdx = -1;
    }
    else if (e->type() == QEvent::MouseMove)
    {
      handleDrag();
    }
  }
}


// -- utility drawing functions -------------------------------------

// ***
/*
void OpacityChooser::clear()
{
    VisUtils::clear( clearColor );
}
*/

void OpacityChooser::drawGrayScale()
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


void OpacityChooser::drawPath(const bool& inSelectMode)
{
  double xRgt;
  double yTop;
  double pix;

  QSizeF size = worldSize();
  pix = pixelSize();

  // calc size of bounding box
  xRgt =  0.5*size.width();
  yTop =  0.5*size.height();

  // selection mode
  if (inSelectMode == true)
    {}
  // rendering mode
  else
  {
    VisUtils::enableLineAntiAlias();
    for (size_t i = 0; i < positionsX.size()-1; ++i)
    {
      VisUtils::setColor(Qt::black);
      VisUtils::drawLineDashed(
        positionsX[i]*xRgt+pix, positionsX[i+1]*xRgt+pix,
        positionsY[i]*yTop-pix, positionsY[i+1]*yTop-pix);

      VisUtils::setColor(VisUtils::lightGray);
      VisUtils::drawLineDashed(
        positionsX[i]*xRgt, positionsX[i+1]*xRgt,
        positionsY[i]*yTop, positionsY[i+1]*yTop);
    }

    VisUtils::disableLineAntiAlias();
  }
}


void OpacityChooser::drawPoints(const bool& inSelectMode)
{
  double xRgt;
  double yTop;
  double pix;
  size_t size;
  double hdlDOF;

  pix = pixelSize();

  // calc size of bounding box
  xRgt =  0.5*worldSize().width();
  yTop =  0.5*worldSize().height();

  // size of handle
  hdlDOF = hdlSize*pix;

  // size of positions
  size = positionsX.size();

  // selection mode
  if (inSelectMode == true)
  {
    for (size_t i = 0; i < size-1; ++i)
    {
      glPushName((GLuint) i);
      VisUtils::fillRect(
        positionsX[i]*xRgt-5.0*pix, positionsX[i]*xRgt+5.0*pix,
        positionsY[i]*yTop+5.0*pix, positionsY[i]*yTop-5.0*pix);
      glPopName();
    }

    if (size == 1)
    {
      glPushName((GLuint) size-1);
      VisUtils::fillRect(
        positionsX[size-1]*xRgt-5.0*pix, positionsX[size-1]*xRgt+5.0*pix,
        positionsY[size-1]*yTop+5.0*pix, positionsY[size-1]*yTop-5.0*pix);
      glPopName();
    }
    else if (size > 1)
    {
      double agl = Utils::calcAngleDg(
                     positionsX[size-1]*xRgt - positionsX[size-2]*xRgt,
                     positionsY[size-1]*yTop - positionsY[size-2]*yTop);

      // arrow
      glPushMatrix();
      glTranslatef(
        positionsX[size-1]*xRgt,
        positionsY[size-1]*yTop,
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
    for (size_t i = 0; i < size-1; ++i)
    {
      VisUtils::setColor(Qt::black);
      VisUtils::drawLine(
        positionsX[i]*xRgt-4.0*pix, positionsX[i]*xRgt+6.0*pix,
        positionsY[i]*yTop+4.0*pix, positionsY[i]*yTop-6.0*pix);
      VisUtils::drawLine(
        positionsX[i]*xRgt-4.0*pix, positionsX[i]*xRgt+6.0*pix,
        positionsY[i]*yTop-6.0*pix, positionsY[i]*yTop+4.0*pix);

      if (active == true)
      {
        VisUtils::setColor(Qt::red);
      }
      else
      {
        VisUtils::setColor(Qt::white);
      }

      VisUtils::drawLine(
        positionsX[i]*xRgt-5.0*pix, positionsX[i]*xRgt+5.0*pix,
        positionsY[i]*yTop+5.0*pix, positionsY[i]*yTop-5.0*pix);
      VisUtils::drawLine(
        positionsX[i]*xRgt-5.0*pix, positionsX[i]*xRgt+5.0*pix,
        positionsY[i]*yTop-5.0*pix, positionsY[i]*yTop+5.0*pix);
    }

    if (size == 1)
    {
      VisUtils::setColor(Qt::black);
      VisUtils::drawLine(
        positionsX[size-1]*xRgt-4.0*pix, positionsX[size-1]*xRgt+6.0*pix,
        positionsY[size-1]*yTop+4.0*pix, positionsY[size-1]*yTop-6.0*pix);
      VisUtils::drawLine(
        positionsX[size-1]*xRgt-4.0*pix, positionsX[size-1]*xRgt+6.0*pix,
        positionsY[size-1]*yTop-6.0*pix, positionsY[size-1]*yTop+4.0*pix);


      if (active == true)
      {
        VisUtils::setColor(Qt::red);
      }
      else
      {
        VisUtils::setColor(Qt::white);
      }

      VisUtils::drawLine(
        positionsX[size-1]*xRgt-5.0*pix, positionsX[size-1]*xRgt+5.0*pix,
        positionsY[size-1]*yTop+5.0*pix, positionsY[size-1]*yTop-5.0*pix);
      VisUtils::drawLine(
        positionsX[size-1]*xRgt-5.0*pix, positionsX[size-1]*xRgt+5.0*pix,
        positionsY[size-1]*yTop-5.0*pix, positionsY[size-1]*yTop+5.0*pix);
    }
    else if (size > 1)
    {
      double agl = Utils::calcAngleDg(
                     positionsX[size-1]*xRgt - positionsX[size-2]*xRgt,
                     positionsY[size-1]*yTop - positionsY[size-2]*yTop);

      // drop shadow
      glPushMatrix();
      glTranslatef(
        positionsX[size-1]*xRgt+pix,
        positionsY[size-1]*yTop-pix,
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
        positionsX[size-1]*xRgt,
        positionsY[size-1]*yTop,
        0.0);
      glRotatef(90.0+agl, 0.0, 0.0, 1.0);

      if (active == true)
      {
        VisUtils::setColor(Qt::green);
      }
      else
      {
        VisUtils::setColor(Qt::white);
      }
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


// -- utility event handlers ------------------------------------


void OpacityChooser::handleHits(const vector< int > &ids)
{
  if (m_lastMouseEvent.type() == QEvent::MouseButtonPress)
  {
    if (m_lastMouseEvent.buttons() == Qt::LeftButton)
    {
      if (0 <= ids[0] && static_cast <size_t>(ids[0]) < positionsX.size())
      {
        dragIdx = ids[0];
      }
    }
    else if (m_lastMouseEvent.buttons() == Qt::RightButton)
    {
      if (0 <= ids[0] && static_cast <size_t>(ids[0]) < positionsX.size())
      {
        /*
        positionsX.erase( positionsX.begin()+ids[0] );
        positionsY.erase( positionsY.begin()+ids[0] );
        */

        mediator->handleDOFOpaClear(ids[0]);
      }
    }
  }
}


void OpacityChooser::handleDrag()
{
  if (0 <= dragIdx && static_cast <size_t>(dragIdx) < positionsX.size())
  {
    QSizeF size = worldSize();
    QPointF pos = worldCoordinate(m_lastMouseEvent.posF());

    // normalize mouse position
    // xLft is -1.0, xRgt is 1.0, yTop = 1.0 and yBot = -1.0
    double xCur = pos.x()/(0.5*size.width());
    double yCur = pos.y()/(0.5*size.height());

    positionsX[dragIdx] = xCur;
    positionsY[dragIdx] = yCur;

    mediator->handleDOFOpaUpdate(
      dragIdx,
      0.5*(xCur+1),
      yCur);
  }
}


// -- hit detection -------------------------------------------------


void OpacityChooser::processHits(
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


// -- end -----------------------------------------------------------
