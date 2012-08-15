// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./shape.cpp

#ifdef __APPLE__
# include <GLUT/glut.h>
#else
# ifdef WIN32
#  define NOMINMAX
#  include <windows.h>
#  undef __in_range // for STLport on Windows
# endif
# include <GL/glu.h>
#endif

#include "shape.h"
#include "diagram.h"
#include <iostream>
using namespace std;


// -- static variables ----------------------------------------------


double Shape::hdlSzeHnt =  5.0;
double Shape::minSzeHnt =  5.0;
int Shape::segNumHnt = 24;
QColor Shape::colTxt = Qt::black;


// -- constructors and destructor -----------------------------------


Shape::Shape(
    Diagram *parentDiagram, size_t index,
    double xCenter,         double yCenter,
    double xDistance,       double yDistance,
    double angle,           int    shapeType,
    double xHinge,          double yHinge)
{
  m_parentDiagram     = parentDiagram;
  m_index             = index;

  // geometery
  m_xCenter           = xCenter;
  m_yCenter           = yCenter;
  m_xDistance         = xDistance;
  m_yDistance         = yDistance;
  m_angle             = angle;
  m_xHinge            = xHinge;
  m_yHinge            = yHinge;

  // properties
  m_shapeType         = shapeType;
  m_drawMode          = MODE_NORMAL;
  m_textSize          = 12;

  m_lineWidth         = 1.0;
  m_lineColor         = VisUtils::mediumGray;
  m_fillColor         = VisUtils::lightGray;

  m_variableValue     = "";
  m_note              = "";
  m_texturesGenerated = false;


  // degrees of freedom
  initDOF();
}


Shape::Shape(const Shape& shape)
// Copy constructor.
{
  m_parentDiagram     = shape.m_parentDiagram;
  m_index             = shape.m_index;

  // geometry
  m_xCenter           = shape.m_xCenter;
  m_yCenter           = shape.m_yCenter;
  m_xDistance         = shape.m_xDistance;
  m_yDistance         = shape.m_yDistance;
  m_angle             = shape.m_angle;
  m_xHinge            = shape.m_xHinge;
  m_yHinge            = shape.m_yHinge;

  // properties
  m_shapeType         = shape.m_shapeType;
  m_drawMode          = shape.m_drawMode;
  m_textSize          = shape.m_textSize;

  m_lineWidth         = shape.m_lineWidth;
  m_lineColor         = shape.m_lineColor;
  m_fillColor         = shape.m_fillColor;

  // textual properties
  m_variableValue     = shape.m_variableValue;
  m_note              = shape.m_note;
  m_texturesGenerated = false;

  // degrees of freedom, invoke copy constructors
  m_xCenterDOF        = new DOF(*shape.m_xCenterDOF);
  m_yCenterDOF        = new DOF(*shape.m_yCenterDOF);
  m_widthDOF          = new DOF(*shape.m_widthDOF);
  m_heightDOF         = new DOF(*shape.m_heightDOF);
  m_angleDOF          = new DOF(*shape.m_angleDOF);
  m_textDOF           = new DOF(*shape.m_textDOF);

  m_colorDOF          = new DOF(*shape.m_colorDOF);
  m_colorYValues      = shape.m_colorYValues;

  m_opacityDOF        = new DOF(*shape.m_opacityDOF);
  m_opacityYValues    = shape.m_opacityYValues;
}


Shape::~Shape()
{
  clearDOF();
}


// -- set functions ---------------------------------------------


void Shape::setDOFColYValue(int index, double value)
{
  if (0 <= index && index < m_colorYValues.size())
  {
    m_colorYValues[index] = value;
  }
}


void Shape::removeDOFColYValue(int index)
{
  if (0 <= index && index < m_colorYValues.size())
  {
    m_colorYValues.removeAt(index);
  }
}


void Shape::setDOFOpaYValue(int index, double value)
{
  if (0 <= index && index < m_opacityYValues.size())
  {
    m_opacityYValues[index] = value;
  }
}


void Shape::removeDOFOpaYValue(int index)
{
  if (0 <= index && index < m_opacityYValues.size())
  {
    m_opacityYValues.removeAt(index);
  }
}


// -- visualization ---------------------------------------------


void Shape::visualize(
    const bool& inSelectMode,
    double pixelSize)
{
  // set up transf
  glPushMatrix();
  glTranslatef(m_xCenter, m_yCenter, 0.0);
  glRotatef(m_angle, 0.0, 0.0, 1.0);

  if (m_drawMode == MODE_NORMAL)
  {
    drawNormal(inSelectMode, pixelSize);
  }
  else if (m_drawMode == MODE_EDIT)
  {
    drawEdit(inSelectMode, pixelSize);
  }
  else
  {
    drawEditDOF(inSelectMode, pixelSize);
  }

  // clear transf
  glPopMatrix();
}


void Shape::visualize(
    double pixelSize,
    const double& opacity,
    const vector< Attribute* > attrs,
    const vector< double > attrValIdcs
    )
{
  double xC, yC; // center, [-1,1]
  double xD, yD; // bound dist from ctr,norm
  double aglH;   // rotation about hinge, degrees

  double alpha = 0.0;

  xC         = m_xCenter;
  yC         = m_yCenter;
  xD         = m_xDistance;
  yD         = m_yDistance;
  aglH       = 0.0;
  QColor colFill = m_fillColor;

  for (size_t i = 0; i < attrs.size(); ++i)
  {
    /*
    if ( attrs[i]->getSizeCurValues() == 1 )
        alpha = 0.0;
    else
        alpha = (double)attrValIdcs[i]/( (double)attrs[i]->getSizeCurValues() - 1.0 );
    */
    if (attrs[i]->getSizeCurValues() == 1)
    {
      alpha = 0.0;
    }
    else
    {
      alpha = (double)attrValIdcs[i]/((double)attrs[i]->getSizeCurValues() - 1.0);
    }

    if (attrs[i] == m_xCenterDOF->attribute())
    {
      xC = m_xCenter + (1-alpha)*m_xCenterDOF->getMin() + alpha*m_xCenterDOF->getMax();
    }

    if (attrs[i] == m_yCenterDOF->attribute())
    {
      yC = m_yCenter + (1-alpha)*m_yCenterDOF->getMin() + alpha*m_yCenterDOF->getMax();
    }

    if (attrs[i] == m_widthDOF->attribute())
    {
      xD = m_xDistance + (1-alpha)*m_widthDOF->getMin() + alpha*m_widthDOF->getMax();
    }

    if (attrs[i] == m_heightDOF->attribute())
    {
      yD = m_yDistance + (1-alpha)*m_heightDOF->getMin() + alpha*m_heightDOF->getMax();
    }

    if (attrs[i] == m_angleDOF->attribute())
    {
      if (m_angleDOF->direction() > 0)
      {
        aglH = 0.0 + (1-alpha)*m_angleDOF->getMin() + alpha*m_angleDOF->getMax();
      }
      else
      {
        aglH = 360.0 - (1-alpha)*m_angleDOF->getMin() + alpha*m_angleDOF->getMax();
      }
    }

    if (attrs[i] == m_colorDOF->attribute())
    {
      double intPtVal;
      double dblPtVal;

      dblPtVal = modf(alpha*(m_colorDOF->valueCount()-1), &intPtVal);

      if (intPtVal < m_colorDOF->valueCount()-1)
      {
        colFill = QColor::fromHsvF((1.0-dblPtVal) * m_colorDOF->value((int)intPtVal) + dblPtVal * m_colorDOF->value((int)intPtVal+1), 1.0, 1.0);
      }
      else
      {
        colFill = QColor::fromHsvF(m_colorDOF->value((int)intPtVal), 1.0, 1.0);
      }
    }

    if (attrs[i] == m_opacityDOF->attribute())
    {
      double intPtVal;
      double dblPtVal;

      dblPtVal = modf(alpha*(m_opacityDOF->valueCount()-1), &intPtVal);

      if (intPtVal < m_opacityDOF->valueCount()-1)
      {
        colFill.setAlphaF((1.0-dblPtVal)*m_opacityDOF->value((int)intPtVal) + dblPtVal*m_opacityDOF->value((int)intPtVal+1));
      }
      else
      {
        colFill.setAlphaF(m_opacityDOF->value((int)intPtVal));
      }
    }
  }

  colFill.setAlphaF(colFill.alphaF() * opacity);

  QColor colLine = m_lineColor;
  colLine.setAlphaF(colLine.alphaF() * opacity);

  // set up transf
  glPushMatrix();

  // move to center pos & rotate
  glTranslatef(xC, yC, 0.0);
  glRotatef(m_angle, 0.0, 0.0, 1.0);

  // move to hinge pos & rotate
  glTranslatef(m_xHinge, m_yHinge, 0.0);
  glRotatef(aglH, 0.0, 0.0, 1.0);
  glTranslatef(-m_xHinge, -m_yHinge, 0.0);

  VisUtils::enableLineAntiAlias();
  VisUtils::enableBlending();

  if (m_shapeType == TYPE_RECT)
  {
    //VisUtils::setColor( colFil );
    VisUtils::setColor(colFill);
    VisUtils::fillRect(
          -xD,  xD,   // new
          yD, -yD);  // new
    VisUtils::setColor(colLine);
    VisUtils::drawRect(
          -xD,  xD,   // new
          yD, -yD);  // new
  }
  else if (m_shapeType == TYPE_RECT)
  {
    //VisUtils::setColor( colFil );
    VisUtils::setColor(colFill);
    VisUtils::fillRect(
          -xD,  xD,   // new
          yD, -yD);  // new
    VisUtils::setColor(colLine);
    VisUtils::drawRect(
          -xD,  xD,   // new
          yD, -yD);  // new
  }
  else if (m_shapeType == TYPE_ELLIPSE)
  {
    //VisUtils::setColor( colFil );
    VisUtils::setColor(colFill);
    VisUtils::fillEllipse(
          0.0, 0.0,
          xD,  yD,
          segNumHnt);
    VisUtils::setColor(colLine);
    VisUtils::drawEllipse(
          0.0, 0.0,
          xD,  yD,
          segNumHnt);
  }
  else if (m_shapeType == TYPE_LINE)
  {
    //VisUtils::setColor( colLin );
    VisUtils::setColor(colFill);
    VisUtils::drawLine(
          -xD,  xD,
          yD, -yD);
  }
  else if (m_shapeType == TYPE_ARROW)
  {
    //VisUtils::setColor( colFil );
    VisUtils::setColor(colFill);
    VisUtils::fillArrow(
          -xD,         xD,
          yD,        -yD,
          hdlSzeHnt*pixelSize, 2*hdlSzeHnt*pixelSize);

    VisUtils::setColor(colLine);
    VisUtils::drawArrow(
          -xD,       xD,
          yD,      -yD,
          hdlSzeHnt*pixelSize, 2*hdlSzeHnt*pixelSize);
  }
  else if (m_shapeType == TYPE_DARROW)
  {
    //VisUtils::setColor( colFil );
    VisUtils::setColor(colFill);
    VisUtils::fillDArrow(
          -xD,         xD,
          yD,        -yD,
          hdlSzeHnt*pixelSize, 2*hdlSzeHnt*pixelSize);
    VisUtils::setColor(colLine);
    VisUtils::drawDArrow(
          -xD,         xD,
          yD,        -yD,
          hdlSzeHnt*pixelSize, 2*hdlSzeHnt*pixelSize);
  }
  drawText(pixelSize);   // Draw the textual values of the shape
  VisUtils::disableBlending();
  VisUtils::disableLineAntiAlias();

  // clear transf
  glPopMatrix();
}


void Shape::setTransf()
{
  // set up transf
  VisUtils::setTransf(
        m_xCenter,   m_yCenter,
        m_xDistance,   m_yDistance,
        m_xHinge,   m_yHinge,
        m_angle, 0.0);
}


void Shape::clrTransf()
{
  // clear transf
  VisUtils::clrTransf();
}


// -- event handlers --------------------------------------------


void Shape::handleHit(const size_t& hdlIdx)
{
  if (m_drawMode == MODE_EDIT_DOF_AGL)
  {
    handleHitEdtDOFAgl(hdlIdx);
  }
}


// -- private utility functions -------------------------------------


void Shape::initDOF()
{
  m_xCenterDOF = new DOF(0, "Horizontal position");
  m_yCenterDOF = new DOF(1, "Vertical position");
  m_widthDOF  = new DOF(2, "Width");
  m_heightDOF  = new DOF(3, "Height");
  m_angleDOF  = new DOF(4, "Rotation");

  m_colorDOF  = new DOF(5, "Color");
  m_colorDOF->setMin(0.25);
  m_colorDOF->setMax(0.75);
  m_colorYValues.push_back(0.0);
  m_colorYValues.push_back(0.0);

  m_opacityDOF  = new DOF(6, "Opacity");
  m_opacityDOF->setMin(0.25);
  m_opacityDOF->setMax(0.75);
  m_opacityYValues.push_back(0.0);
  m_opacityYValues.push_back(0.0);

  m_textDOF = new DOF(7, "Text");
}


void Shape::clearDOF()
{
  // composition
  if (m_xCenterDOF != 0)
  {
    delete m_xCenterDOF;
    m_xCenterDOF = 0;
  }

  if (m_yCenterDOF != 0)
  {
    delete m_yCenterDOF;
    m_yCenterDOF = 0;
  }

  if (m_widthDOF != 0)
  {
    delete m_widthDOF;
    m_widthDOF = 0;
  }

  if (m_heightDOF != 0)
  {
    delete m_heightDOF;
    m_heightDOF = 0;
  }

  if (m_angleDOF != 0)
  {
    delete m_angleDOF;
    m_angleDOF = 0;
  }

  if (m_colorDOF != 0)
  {
    delete m_colorDOF;
    m_colorDOF = 0;
  }

  if (m_opacityDOF != 0)
  {
    delete m_opacityDOF;
    m_opacityDOF = 0;
  }

  if (m_textDOF != 0)
  {
    delete m_textDOF;
    m_textDOF = 0;
  }
}


void Shape::handleHitEdtDOFAgl(const size_t& hdlIdx)
{
  if (hdlIdx == ID_HDL_DIR)
  {
    m_angleDOF->setDirection(-1*m_angleDOF->direction());
  }
}


// -- private visualization functions -------------------------------


void Shape::drawNormal(
    const bool& inSelectMode,
    double pixelSize)
{
  if (inSelectMode == true)
  {
    if (m_shapeType == TYPE_NOTE)
    {
      VisUtils::fillRect(
            -m_xDistance,  m_xDistance,
            m_yDistance, -m_yDistance);
    }
    else if (m_shapeType == TYPE_RECT)
    {
      VisUtils::fillRect(
            -m_xDistance,  m_xDistance,
            m_yDistance, -m_yDistance);
    }
    else if (m_shapeType == TYPE_ELLIPSE)
    {
      VisUtils::fillEllipse(
            0.0,  0.0,
            m_xDistance, m_yDistance,
            segNumHnt);
    }
    else if (m_shapeType == TYPE_LINE)
    {
      VisUtils::drawLine(
            -m_xDistance,  m_xDistance,
            m_yDistance, -m_yDistance);
    }
    else if (m_shapeType == TYPE_ARROW)
    {
      VisUtils::fillArrow(
            -m_xDistance,       m_xDistance,
            m_yDistance,      -m_yDistance,
            hdlSzeHnt*pixelSize, 2*hdlSzeHnt*pixelSize);
    }
    else if (m_shapeType == TYPE_DARROW)
    {
      VisUtils::fillDArrow(
            -m_xDistance,      m_xDistance,
            m_yDistance,     -m_yDistance,
            hdlSzeHnt*pixelSize, 2*hdlSzeHnt*pixelSize);
    }
  }
  else
  {
    VisUtils::enableLineAntiAlias();
    if (m_shapeType == TYPE_NOTE)
    {
      VisUtils::setColor(m_fillColor);
      VisUtils::fillRect(
            -m_xDistance,  m_xDistance,
            m_yDistance, -m_yDistance);
      VisUtils::setColor(m_lineColor);
      VisUtils::drawRect(
            -m_xDistance,  m_xDistance,
            m_yDistance, -m_yDistance);
    }
    else if (m_shapeType == TYPE_RECT)
    {
      VisUtils::setColor(m_fillColor);
      VisUtils::fillRect(
            -m_xDistance,  m_xDistance,
            m_yDistance, -m_yDistance);
      VisUtils::setColor(m_lineColor);
      VisUtils::drawRect(
            -m_xDistance,  m_xDistance,
            m_yDistance, -m_yDistance);
    }
    else if (m_shapeType == TYPE_ELLIPSE)
    {
      VisUtils::setColor(m_fillColor);
      VisUtils::fillEllipse(
            0.0,  0.0,
            m_xDistance, m_yDistance,
            segNumHnt);
      VisUtils::setColor(m_lineColor);
      VisUtils::drawEllipse(
            0.0,  0.0,
            m_xDistance, m_yDistance,
            segNumHnt);
    }
    else if (m_shapeType == TYPE_LINE)
    {
      VisUtils::setColor(m_lineColor);
      VisUtils::drawLine(
            -m_xDistance,  m_xDistance,
            m_yDistance, -m_yDistance);
    }
    else if (m_shapeType == TYPE_ARROW)
    {
      VisUtils::setColor(m_fillColor);
      VisUtils::fillArrow(
            -m_xDistance,      m_xDistance,
            m_yDistance,     -m_yDistance,
            hdlSzeHnt*pixelSize, 2*hdlSzeHnt*pixelSize);
      VisUtils::setColor(m_lineColor);
      VisUtils::drawArrow(
            -m_xDistance,      m_xDistance,
            m_yDistance,     -m_yDistance,
            hdlSzeHnt*pixelSize, 2*hdlSzeHnt*pixelSize);
    }
    else if (m_shapeType == TYPE_DARROW)
    {
      VisUtils::setColor(m_fillColor);
      VisUtils::fillDArrow(
            -m_xDistance,      m_xDistance,
            m_yDistance,     -m_yDistance,
            hdlSzeHnt*pixelSize, 2*hdlSzeHnt*pixelSize);
      VisUtils::setColor(m_lineColor);
      VisUtils::drawDArrow(
            -m_xDistance,      m_xDistance,
            m_yDistance,     -m_yDistance,
            hdlSzeHnt*pixelSize, 2*hdlSzeHnt*pixelSize);
    }
    drawText(pixelSize);   // Draw the textual values of the shape
    VisUtils::disableLineAntiAlias();
  }
}


void Shape::drawText(double pixelSize)
{
  if (!m_note.isEmpty() || m_variableValue.isEmpty())
  {
    QString text = m_note;
    if (!text.isEmpty() && !m_variableValue.isEmpty())
      text.append(" ");
    text.append(m_variableValue);


    if (!m_texturesGenerated)
    {
      VisUtils::genCharTextures(m_texCharId, m_texChar);
      m_texturesGenerated = true;
    }

    VisUtils::setColor(colTxt);
    VisUtils::drawLabelInBoundBox(m_texCharId, -m_xDistance, m_xDistance, m_yDistance, -m_yDistance, m_textSize*pixelSize/CHARHEIGHT, text.toStdString());
  }
}


void Shape::drawEdit(
    const bool& inSelectMode,
    double pixelSize)
{
  double hdlDelta = hdlSzeHnt*pixelSize;

  if (inSelectMode == true)
  {
    // draw shape
    drawNormal(inSelectMode, pixelSize);

    glPushName(ID_HDL_CTR);
    VisUtils::fillRect(-m_xDistance, m_xDistance, m_yDistance, -m_yDistance);
    glPopName();
    glPushName(ID_HDL_TOP_LFT);
    VisUtils::fillRect(-m_xDistance-hdlDelta, -m_xDistance+hdlDelta, m_yDistance+hdlDelta,  m_yDistance-hdlDelta);
    glPopName();
    glPushName(ID_HDL_LFT);
    VisUtils::fillRect(-m_xDistance-hdlDelta, -m_xDistance+hdlDelta, 0.0+hdlDelta, 0.0-hdlDelta);
    glPopName();
    glPushName(ID_HDL_BOT_LFT);
    VisUtils::fillRect(-m_xDistance-hdlDelta, -m_xDistance+hdlDelta, -m_yDistance+hdlDelta, -m_yDistance-hdlDelta);
    glPopName();
    glPushName(ID_HDL_BOT);
    VisUtils::fillRect(0.0-hdlDelta, 0.0+hdlDelta, -m_yDistance+hdlDelta, -m_yDistance-hdlDelta);
    glPopName();
    glPushName(ID_HDL_BOT_RGT);
    VisUtils::fillRect(m_xDistance-hdlDelta, m_xDistance+hdlDelta, -m_yDistance+hdlDelta, -m_yDistance-hdlDelta);
    glPopName();
    glPushName(ID_HDL_RGT);
    VisUtils::fillRect(m_xDistance-hdlDelta, m_xDistance+hdlDelta, 0.0+hdlDelta, 0.0-hdlDelta);
    glPopName();
    glPushName(ID_HDL_TOP_RGT);
    VisUtils::fillRect(m_xDistance-hdlDelta, m_xDistance+hdlDelta, m_yDistance+hdlDelta, m_yDistance-hdlDelta);
    glPopName();
    glPushName(ID_HDL_TOP);
    VisUtils::fillRect(0.0-hdlDelta, 0.0+hdlDelta, m_yDistance+hdlDelta, m_yDistance-hdlDelta);
    glPopName();
    glPushName(ID_HDL_ROT_RGT);
    if (m_xDistance >= 0)
    {
      VisUtils::fillEllipse(m_xDistance+6.0*hdlDelta, 0.0, hdlDelta, hdlDelta, 4);
    }
    else
    {
      VisUtils::fillEllipse(m_xDistance-6.0*hdlDelta, 0.0, hdlDelta, hdlDelta, 4);
    }
    glPopName();
    glPushName(ID_HDL_ROT_TOP);
    if (m_yDistance >= 0)
    {
      VisUtils::fillEllipse(0.0, m_yDistance+6.0*hdlDelta, hdlDelta, hdlDelta, segNumHnt);
    }
    else
    {
      VisUtils::fillEllipse(0.0, m_yDistance-6.0*hdlDelta, hdlDelta, hdlDelta, segNumHnt);
    }
    glPopName();
  }
  else
  {
    // draw bounding box
    VisUtils::enableLineAntiAlias();
    VisUtils::setColor(VisUtils::mediumGray);
    VisUtils::drawRect(
          -m_xDistance,  m_xDistance,
          m_yDistance, -m_yDistance);
    VisUtils::disableLineAntiAlias();

    // draw shape
    drawNormal(inSelectMode, pixelSize);

    // enable antialiasing
    VisUtils::enableLineAntiAlias();

    // draw top connecting line
    VisUtils::setColor(VisUtils::mediumGray);
    if (m_xDistance >= 0)
    {
      VisUtils::drawLine(m_xDistance+6.0*hdlDelta, m_xDistance, 0.0, 0.0);
    }
    else
    {
      VisUtils::drawLine(m_xDistance-6.0*hdlDelta, m_xDistance, 0.0, 0.0);
    }
    // draw top connecting line
    VisUtils::setColor(VisUtils::mediumGray);
    if (m_yDistance >= 0)
    {
      VisUtils::drawLine(0.0, 0.0, m_yDistance, m_yDistance+6.0*hdlDelta);
    }
    else
    {
      VisUtils::drawLine(0.0, 0.0, m_yDistance, m_yDistance-6.0*hdlDelta);
    }

    // draw center
    VisUtils::setColor(VisUtils::mediumGray);
    VisUtils::drawLine(-hdlDelta, hdlDelta,      0.0,       0.0);
    VisUtils::drawLine(0.0,      0.0, hdlDelta, -hdlDelta);
    // top left
    VisUtils::setColor(Qt::white);
    VisUtils::fillRect(-m_xDistance-hdlDelta, -m_xDistance+hdlDelta, m_yDistance+hdlDelta,  m_yDistance-hdlDelta);
    VisUtils::setColor(VisUtils::mediumGray);
    VisUtils::drawRect(-m_xDistance-hdlDelta, -m_xDistance+hdlDelta, m_yDistance+hdlDelta,  m_yDistance-hdlDelta);
    // left
    VisUtils::setColor(Qt::white);
    VisUtils::fillRect(-m_xDistance-hdlDelta, -m_xDistance+hdlDelta, 0.0+hdlDelta, 0.0-hdlDelta);
    VisUtils::setColor(VisUtils::mediumGray);
    VisUtils::drawRect(-m_xDistance-hdlDelta, -m_xDistance+hdlDelta, 0.0+hdlDelta, 0.0-hdlDelta);
    // bottom left
    VisUtils::setColor(Qt::white);
    VisUtils::fillRect(-m_xDistance-hdlDelta, -m_xDistance+hdlDelta, -m_yDistance+hdlDelta, -m_yDistance-hdlDelta);
    VisUtils::setColor(VisUtils::mediumGray);
    VisUtils::drawRect(-m_xDistance-hdlDelta, -m_xDistance+hdlDelta, -m_yDistance+hdlDelta, -m_yDistance-hdlDelta);
    // bottom
    VisUtils::setColor(Qt::white);
    VisUtils::fillRect(0.0-hdlDelta, 0.0+hdlDelta, -m_yDistance+hdlDelta, -m_yDistance-hdlDelta);
    VisUtils::setColor(VisUtils::mediumGray);
    VisUtils::drawRect(0.0-hdlDelta, 0.0+hdlDelta, -m_yDistance+hdlDelta, -m_yDistance-hdlDelta);
    // bottom right
    VisUtils::setColor(Qt::white);
    VisUtils::fillRect(m_xDistance-hdlDelta, m_xDistance+hdlDelta, -m_yDistance+hdlDelta, -m_yDistance-hdlDelta);
    VisUtils::setColor(VisUtils::mediumGray);
    VisUtils::drawRect(m_xDistance-hdlDelta, m_xDistance+hdlDelta, -m_yDistance+hdlDelta, -m_yDistance-hdlDelta);
    // right
    VisUtils::setColor(Qt::white);
    VisUtils::fillRect(m_xDistance-hdlDelta, m_xDistance+hdlDelta, 0.0+hdlDelta, 0.0-hdlDelta);
    VisUtils::setColor(VisUtils::mediumGray);
    VisUtils::drawRect(m_xDistance-hdlDelta, m_xDistance+hdlDelta, 0.0+hdlDelta, 0.0-hdlDelta);
    // top right
    VisUtils::setColor(Qt::white);
    VisUtils::fillRect(m_xDistance-hdlDelta, m_xDistance+hdlDelta, m_yDistance+hdlDelta, m_yDistance-hdlDelta);
    VisUtils::setColor(VisUtils::mediumGray);
    VisUtils::drawRect(m_xDistance-hdlDelta, m_xDistance+hdlDelta, m_yDistance+hdlDelta, m_yDistance-hdlDelta);
    // top
    VisUtils::setColor(Qt::white);
    VisUtils::fillRect(0.0-hdlDelta, 0.0+hdlDelta, m_yDistance+hdlDelta, m_yDistance-hdlDelta);
    VisUtils::setColor(VisUtils::mediumGray);
    VisUtils::drawRect(0.0-hdlDelta, 0.0+hdlDelta, m_yDistance+hdlDelta, m_yDistance-hdlDelta);
    // rotation hdl rgt
    if (m_xDistance >= 0)
    {
      VisUtils::setColor(Qt::white);
      VisUtils::fillEllipse(m_xDistance+6.0*hdlDelta, 0.0, hdlDelta, hdlDelta, 4);
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawEllipse(m_xDistance+6.0*hdlDelta, 0.0, hdlDelta, hdlDelta, 4);
    }
    else
    {
      VisUtils::setColor(Qt::white);
      VisUtils::fillEllipse(m_xDistance-6.0*hdlDelta, 0.0, hdlDelta, hdlDelta, 4);
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawEllipse(m_xDistance-6.0*hdlDelta, 0.0, hdlDelta, hdlDelta, 4);
    }
    // rotation hdl top
    if (m_yDistance >= 0)
    {
      VisUtils::setColor(Qt::white);
      VisUtils::fillEllipse(0.0, m_yDistance+6.0*hdlDelta, hdlDelta, hdlDelta, segNumHnt);
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawEllipse(0.0, m_yDistance+6.0*hdlDelta, hdlDelta, hdlDelta, segNumHnt);
    }
    else
    {
      VisUtils::setColor(Qt::white);
      VisUtils::fillEllipse(0.0, m_yDistance-6.0*hdlDelta, hdlDelta, hdlDelta, segNumHnt);
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawEllipse(0.0, m_yDistance-6.0*hdlDelta, hdlDelta, hdlDelta, segNumHnt);
    }

    // disable antialiasing
    VisUtils::disableLineAntiAlias();
  }
}


void Shape::drawEditDOF(
    const bool& inSelectMode,
    double pixelSize)
{
  if (inSelectMode == true)
  {
    // draw shape
    drawNormal(inSelectMode, pixelSize);

    if (m_drawMode == MODE_EDIT_DOF_XCTR)
    {
      drawDOFYCtr(inSelectMode, pixelSize);
      drawDOFWth(inSelectMode, pixelSize);
      drawDOFHgt(inSelectMode, pixelSize);
      drawDOFAgl(inSelectMode, pixelSize);

      drawEditDOFXCtr(inSelectMode, pixelSize);
    }
    else if (m_drawMode == MODE_EDIT_DOF_YCTR)
    {
      drawDOFXCtr(inSelectMode, pixelSize);
      drawDOFWth(inSelectMode, pixelSize);
      drawDOFHgt(inSelectMode, pixelSize);
      drawDOFAgl(inSelectMode, pixelSize);

      drawEditDOFYCtr(inSelectMode, pixelSize);
    }
    else if (m_drawMode == MODE_EDIT_DOF_WTH)
    {
      drawDOFXCtr(inSelectMode, pixelSize);
      drawDOFYCtr(inSelectMode, pixelSize);
      drawDOFHgt(inSelectMode, pixelSize);
      drawDOFAgl(inSelectMode, pixelSize);

      drawEditDOFWth(inSelectMode, pixelSize);
    }
    else if (m_drawMode == MODE_EDIT_DOF_HGT)
    {
      drawDOFXCtr(inSelectMode, pixelSize);
      drawDOFYCtr(inSelectMode, pixelSize);
      drawDOFWth(inSelectMode, pixelSize);
      drawDOFAgl(inSelectMode, pixelSize);

      drawEditDOFHgt(inSelectMode, pixelSize);
    }
    else if (m_drawMode == MODE_EDIT_DOF_AGL)
    {
      drawDOFXCtr(inSelectMode, pixelSize);
      drawDOFYCtr(inSelectMode, pixelSize);
      drawDOFWth(inSelectMode, pixelSize);
      drawDOFHgt(inSelectMode, pixelSize);

      drawEditDOFAgl(inSelectMode, pixelSize);
    }
    else if (m_drawMode == MODE_EDIT_DOF_COL)
    {
      drawDOFXCtr(inSelectMode, pixelSize);
      drawDOFYCtr(inSelectMode, pixelSize);
      drawDOFWth(inSelectMode, pixelSize);
      drawDOFHgt(inSelectMode, pixelSize);
      drawDOFAgl(inSelectMode, pixelSize);
    }
    else if (m_drawMode == MODE_EDIT_DOF_OPA)
    {
      drawDOFXCtr(inSelectMode, pixelSize);
      drawDOFYCtr(inSelectMode, pixelSize);
      drawDOFWth(inSelectMode, pixelSize);
      drawDOFHgt(inSelectMode, pixelSize);
      drawDOFAgl(inSelectMode, pixelSize);
    }
    else if (m_drawMode == MODE_EDIT_DOF_TEXT)
    {
      drawDOFXCtr(inSelectMode, pixelSize);
      drawDOFYCtr(inSelectMode, pixelSize);
      drawDOFWth(inSelectMode, pixelSize);
      drawDOFHgt(inSelectMode, pixelSize);
      drawDOFAgl(inSelectMode, pixelSize);
    }
  }
  else
  {
    // draw bounding box
    VisUtils::enableLineAntiAlias();
    VisUtils::setColor(VisUtils::mediumGray);
    VisUtils::drawRect(
          -m_xDistance,  m_xDistance,
          m_yDistance, -m_yDistance);
    VisUtils::disableLineAntiAlias();

    // draw shape
    drawNormal(inSelectMode, pixelSize);

    VisUtils::enableLineAntiAlias();
    if (m_drawMode == MODE_EDIT_DOF_XCTR)
    {
      drawDOFYCtr(inSelectMode, pixelSize);
      drawDOFWth(inSelectMode, pixelSize);
      drawDOFHgt(inSelectMode, pixelSize);
      drawDOFAgl(inSelectMode, pixelSize);

      drawEditDOFXCtr(inSelectMode, pixelSize);
    }
    else if (m_drawMode == MODE_EDIT_DOF_YCTR)
    {
      drawDOFXCtr(inSelectMode, pixelSize);
      drawDOFWth(inSelectMode, pixelSize);
      drawDOFHgt(inSelectMode, pixelSize);
      drawDOFAgl(inSelectMode, pixelSize);

      drawEditDOFYCtr(inSelectMode, pixelSize);
    }
    else if (m_drawMode == MODE_EDIT_DOF_WTH)
    {
      drawDOFXCtr(inSelectMode, pixelSize);
      drawDOFYCtr(inSelectMode, pixelSize);
      drawDOFHgt(inSelectMode, pixelSize);
      drawDOFAgl(inSelectMode, pixelSize);

      drawEditDOFWth(inSelectMode, pixelSize);
    }
    else if (m_drawMode == MODE_EDIT_DOF_HGT)
    {
      drawDOFXCtr(inSelectMode, pixelSize);
      drawDOFYCtr(inSelectMode, pixelSize);
      drawDOFWth(inSelectMode, pixelSize);
      drawDOFAgl(inSelectMode, pixelSize);

      drawEditDOFHgt(inSelectMode, pixelSize);
    }
    else if (m_drawMode == MODE_EDIT_DOF_AGL)
    {
      drawDOFXCtr(inSelectMode, pixelSize);
      drawDOFYCtr(inSelectMode, pixelSize);
      drawDOFWth(inSelectMode, pixelSize);
      drawDOFHgt(inSelectMode, pixelSize);

      drawEditDOFAgl(inSelectMode, pixelSize);
    }
    else if (m_drawMode == MODE_EDIT_DOF_COL)
    {
      drawDOFXCtr(inSelectMode, pixelSize);
      drawDOFYCtr(inSelectMode, pixelSize);
      drawDOFWth(inSelectMode, pixelSize);
      drawDOFHgt(inSelectMode, pixelSize);
      drawDOFAgl(inSelectMode, pixelSize);
    }
    else if (m_drawMode == MODE_EDIT_DOF_OPA)
    {
      drawDOFXCtr(inSelectMode, pixelSize);
      drawDOFYCtr(inSelectMode, pixelSize);
      drawDOFWth(inSelectMode, pixelSize);
      drawDOFHgt(inSelectMode, pixelSize);
      drawDOFAgl(inSelectMode, pixelSize);
    }
    else if (m_drawMode == MODE_EDIT_DOF_TEXT)
    {
      drawDOFXCtr(inSelectMode, pixelSize);
      drawDOFYCtr(inSelectMode, pixelSize);
      drawDOFWth(inSelectMode, pixelSize);
      drawDOFHgt(inSelectMode, pixelSize);
      drawDOFAgl(inSelectMode, pixelSize);
    }
    VisUtils::disableLineAntiAlias();
  }
}


void Shape::drawDOFXCtr(
    const bool& inSelectMode,
    double pixelSize)
{
  double hdlDOF = hdlSzeHnt*pixelSize;
  double xBeg   = m_xCenterDOF->getMin();
  double xEnd   = m_xCenterDOF->getMax();

  glPushMatrix();
  glRotatef(-m_angle, 0.0, 0.0, 1.0);

  if (inSelectMode == true)
  {}
  else
  {
    /*
    VisUtils::setColor(VisUtils::mediumGray);
    VisUtils::drawLine(
         0.0,     0.0,
         hdlDOF, -hdlDOF );
    VisUtils::drawLine(
        -hdlDOF,  hdlDOF,
         0.0,     0.0 );
    */

    // pointing right
    if (xEnd < xBeg)
    {
      VisUtils::setColor(VisUtils::mediumGray);
      if (0.0 < xEnd)
        VisUtils::drawLineDashed(
              0.0,  xEnd,
              0.0,  0.0);
      else if (xBeg < 0.0)
        VisUtils::drawLineDashed(
              xBeg, 0.0,
              0.0,  0.0);
      VisUtils::drawLineDashed(
            xBeg, xEnd,
            0.0,  0.0);

      /*
      // start
      VisUtils::setColor(Qt::white);
      VisUtils::fillRect(
          xBeg-1.5*hdlDOF,  xBeg,
          hdlDOF,          -hdlDOF );
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawRect(
          xBeg-1.5*hdlDOF,  xBeg,
          hdlDOF,          -hdlDOF );
      VisUtils::drawLine(
          xBeg,        xBeg,
          2.0*hdlDOF, -2.0*hdlDOF );
      */

      // stop
      VisUtils::setColor(Qt::white);
      VisUtils::fillTriangle(
            xEnd,             0.0,
            xEnd+2.0*hdlDOF, -hdlDOF,
            xEnd+2.0*hdlDOF,  hdlDOF);
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawTriangle(
            xEnd,             0.0,
            xEnd+2.0*hdlDOF, -hdlDOF,
            xEnd+2.0*hdlDOF,  hdlDOF);
      VisUtils::drawLine(
            xEnd,        xEnd,
            2.0*hdlDOF, -2.0*hdlDOF);
    }
    // pointing right
    else // ( xBeg <= xEnd )
    {
      VisUtils::setColor(VisUtils::mediumGray);
      if (0.0 < xBeg)
        VisUtils::drawLineDashed(
              0.0, xBeg,
              0.0, 0.0);
      else if (xEnd < 0.0)
        VisUtils::drawLineDashed(
              xEnd, 0.0,
              0.0,  0.0);
      VisUtils::drawLineDashed(
            xBeg, xEnd,
            0.0,  0.0);

      /*
      // start
      VisUtils::setColor(Qt::white);
      VisUtils::fillRect(
          xBeg,    xBeg+1.5*hdlDOF,
          hdlDOF, -hdlDOF );
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawRect(
          xBeg,    xBeg+1.5*hdlDOF,
          hdlDOF, -hdlDOF );
      VisUtils::drawLine(
          xBeg,        xBeg,
          2.0*hdlDOF, -2.0*hdlDOF );
      */

      // stop
      VisUtils::setColor(Qt::white);
      VisUtils::fillTriangle(
            xEnd-2.0*hdlDOF,  hdlDOF,
            xEnd-2.0*hdlDOF, -hdlDOF,
            xEnd,             0.0);
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawTriangle(
            xEnd-2.0*hdlDOF,  hdlDOF,
            xEnd-2.0*hdlDOF, -hdlDOF,
            xEnd,             0.0);
      VisUtils::drawLine(
            xEnd,        xEnd,
            2.0*hdlDOF, -2.0*hdlDOF);
    }
  }

  glPopMatrix();
}


void Shape::drawEditDOFXCtr(
    const bool& inSelectMode,
    double pixelSize)
{
  double hdlDOF = hdlSzeHnt*pixelSize;
  double xBeg   = m_xCenterDOF->getMin();
  double xEnd   = m_xCenterDOF->getMax();

  glPushMatrix();
  glRotatef(-m_angle, 0.0, 0.0, 1.0);

  if (inSelectMode == true)
  {
    // pointing right
    if (xEnd <= xBeg)
    {
      /*
      // start
      glPushName( ID_HDL_DOF_BEG );
      VisUtils::fillRect(
          xBeg-1.5*hdlDOF,  xBeg,
          hdlDOF,          -hdlDOF );
      glPopName();
      */

      // stop
      glPushName(ID_HDL_DOF_END);
      VisUtils::fillTriangle(
            xEnd,             0.0,
            xEnd+2.0*hdlDOF, -hdlDOF,
            xEnd+2.0*hdlDOF,  hdlDOF);
      glPopName();
    }
    // pointing right
    else // ( xBeg < xEnd )
    {
      /*
      // start
      glPushName( ID_HDL_DOF_BEG );
      VisUtils::fillRect(
          xBeg,    xBeg+1.5*hdlDOF,
          hdlDOF, -hdlDOF );
      glPopName();
      */

      // stop
      glPushName(ID_HDL_DOF_END);
      VisUtils::fillTriangle(
            xEnd-2.0*hdlDOF,  hdlDOF,
            xEnd-2.0*hdlDOF, -hdlDOF,
            xEnd,             0.0);
      glPopName();
    }
  }
  else
  {
    /*
    // draw center
    VisUtils::setColor(VisUtils::mediumGray);
    VisUtils::drawLine(
         0.0,     0.0,
         hdlDOF, -hdlDOF );
    VisUtils::drawLine(
        -hdlDOF,  hdlDOF,
         0.0,     0.0 );
    */

    // draw starting cross
    VisUtils::setColor(Qt::red);
    VisUtils::drawLine(
          -hdlDOF,  hdlDOF,
          hdlDOF, -hdlDOF);
    VisUtils::drawLine(
          -hdlDOF,  hdlDOF,
          -hdlDOF,  hdlDOF);

    // pointing left
    if (xEnd <= xBeg)
    {
      if (0.0 < xEnd)
        VisUtils::drawLineDashed(
              0.0,  xEnd,
              0.0,  0.0);
      else if (xBeg < 0.0)
        VisUtils::drawLineDashed(
              xBeg, 0.0,
              0.0,  0.0);
      VisUtils::setColor(Qt::red);
      VisUtils::drawLineDashed(
            xBeg, xEnd,
            0.0,  0.0);
      /*
      // start
      VisUtils::setColor(Qt::red);
      VisUtils::fillRect(
          xBeg-1.5*hdlDOF,  xBeg,
          hdlDOF,          -hdlDOF );
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawRect(
          xBeg-1.5*hdlDOF,  xBeg,
          hdlDOF,          -hdlDOF );
      VisUtils::drawLine(
          xBeg,        xBeg,
          2.0*hdlDOF, -2.0*hdlDOF );
      */

      // stop
      VisUtils::setColor(Qt::green);
      VisUtils::fillTriangle(
            xEnd,             0.0,
            xEnd+2.0*hdlDOF, -hdlDOF,
            xEnd+2.0*hdlDOF,  hdlDOF);
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawTriangle(
            xEnd,             0.0,
            xEnd+2.0*hdlDOF, -hdlDOF,
            xEnd+2.0*hdlDOF,  hdlDOF);
      VisUtils::drawLine(
            xEnd,        xEnd,
            2.0*hdlDOF, -2.0*hdlDOF);
    }
    // pointing right
    else // ( xBeg < xEnd )
    {
      if (0.0 < xBeg)
        VisUtils::drawLineDashed(
              0.0, xBeg,
              0.0, 0.0);
      else if (xEnd < 0.0)
        VisUtils::drawLineDashed(
              xEnd, 0.0,
              0.0,  0.0);
      VisUtils::setColor(Qt::red);
      VisUtils::drawLineDashed(
            xBeg, xEnd,
            0.0,  0.0);
      /*
      // start
      VisUtils::setColor(Qt::green);
      VisUtils::fillRect(
          xBeg,    xBeg+1.5*hdlDOF,
          hdlDOF, -hdlDOF );
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawRect(
          xBeg,    xBeg+1.5*hdlDOF,
          hdlDOF, -hdlDOF );
      VisUtils::setColor(Qt::red);
      VisUtils::drawLine(
          xBeg,        xBeg,
          2.0*hdlDOF, -2.0*hdlDOF );
      */

      // stop
      VisUtils::setColor(Qt::green);
      VisUtils::fillTriangle(
            xEnd-2.0*hdlDOF,  hdlDOF,
            xEnd-2.0*hdlDOF, -hdlDOF,
            xEnd,             0.0);
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawTriangle(
            xEnd-2.0*hdlDOF,  hdlDOF,
            xEnd-2.0*hdlDOF, -hdlDOF,
            xEnd,             0.0);
      VisUtils::drawLine(
            xEnd,        xEnd,
            2.0*hdlDOF, -2.0*hdlDOF);
    }
  }

  glPopMatrix();
}


void Shape::drawDOFYCtr(
    const bool& inSelectMode,
    double pixelSize)
{
  double hdlDOF = hdlSzeHnt*pixelSize;
  double yBeg   = m_yCenterDOF->getMin();
  double yEnd   = m_yCenterDOF->getMax();

  glPushMatrix();
  glRotatef(-m_angle, 0.0, 0.0, 1.0);

  if (inSelectMode == true)
  {}
  else
  {
    /*
    // draw central handle
    VisUtils::setColor(VisUtils::mediumGray);
    VisUtils::drawLine(
         0.0,     0.0,
         hdlDOF, -hdlDOF );
    VisUtils::drawLine(
        -hdlDOF,  hdlDOF,
         0.0,     0.0 );
    */

    // pointing down
    if (yEnd < yBeg)
    {
      if (0.0 < yEnd)
        VisUtils::drawLineDashed(
              0.0, 0.0,
              0.0, yEnd);
      else if (yBeg < 0.0)
        VisUtils::drawLineDashed(
              0.0, 0.0,
              0.0, yBeg);
      VisUtils::drawLineDashed(
            0.0,  0.0,
            yBeg, yEnd);

      /*
      // start
      VisUtils::setColor(Qt::white);
      VisUtils::fillRect(
          -hdlDOF, hdlDOF,
           yBeg,   yBeg-1.5*hdlDOF );
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawRect(
          -hdlDOF, hdlDOF,
           yBeg,   yBeg-1.5*hdlDOF );
      VisUtils::drawLine(
          -2.0*hdlDOF, 2.0*hdlDOF,
           yBeg,       yBeg );
      */

      // stop
      VisUtils::setColor(Qt::white);
      VisUtils::fillTriangle(
            -hdlDOF, yEnd+2.0*hdlDOF,
            0.0,    yEnd,
            hdlDOF, yEnd+2.0*hdlDOF);
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawTriangle(
            -hdlDOF, yEnd+2.0*hdlDOF,
            0.0,    yEnd,
            hdlDOF, yEnd+2.0*hdlDOF);
      VisUtils::drawLine(
            2.0*hdlDOF, -2.0*hdlDOF,
            yEnd,        yEnd);
    }
    // pointing up
    else // ( yBeg <= yEnd )
    {
      if (0.0 < yBeg)
        VisUtils::drawLineDashed(
              0.0, 0.0,
              0.0, yBeg);
      else if (yEnd < 0.0)
        VisUtils::drawLineDashed(
              0.0, 0.0,
              0.0, yEnd);
      VisUtils::drawLineDashed(
            0.0,  0.0,
            yBeg, yEnd);

      /*
      // start
      VisUtils::setColor(Qt::white);
      VisUtils::fillRect(
          hdlDOF,          -hdlDOF,
          yBeg+1.5*hdlDOF,  yBeg );
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawRect(
          hdlDOF,          -hdlDOF,
          yBeg+1.5*hdlDOF,  yBeg );
      VisUtils::drawLine(
          2.0*hdlDOF, -2.0*hdlDOF,
          yBeg,        yBeg );
      */

      // stop
      VisUtils::setColor(Qt::white);
      VisUtils::fillTriangle(
            0.0,    yEnd,
            -hdlDOF, yEnd-2.0*hdlDOF,
            hdlDOF, yEnd-2.0*hdlDOF);
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawTriangle(
            0.0,    yEnd,
            -hdlDOF, yEnd-2.0*hdlDOF,
            hdlDOF, yEnd-2.0*hdlDOF);
      VisUtils::drawLine(
            2.0*hdlDOF, -2.0*hdlDOF,
            yEnd,        yEnd);
    }
  }

  glPopMatrix();
}


void Shape::drawEditDOFYCtr(
    const bool& inSelectMode,
    double pixelSize)
{
  double hdlDOF = hdlSzeHnt*pixelSize;
  double yBeg   = m_yCenterDOF->getMin();
  double yEnd   = m_yCenterDOF->getMax();

  glPushMatrix();
  glRotatef(-m_angle, 0.0, 0.0, 1.0);

  if (inSelectMode == true)
  {
    // pointing down
    if (yEnd < yBeg)
    {
      // start
      glPushName(ID_HDL_DOF_BEG);
      VisUtils::fillRect(
            -hdlDOF, hdlDOF,
            yBeg,   yBeg-1.5*hdlDOF);
      glPopName();

      // stop
      glPushName(ID_HDL_DOF_END);
      VisUtils::fillTriangle(
            -hdlDOF, yEnd+2.0*hdlDOF,
            0.0,    yEnd,
            hdlDOF, yEnd+2.0*hdlDOF);
      glPopName();
    }
    // pointing up
    else // ( yBeg <= yEnd )
    {
      /*
      // start
      glPushName( ID_HDL_DOF_BEG );
      VisUtils::fillRect(
          hdlDOF,          -hdlDOF,
          yBeg+1.5*hdlDOF,  yBeg );
      glPopName();
      */

      // stop
      glPushName(ID_HDL_DOF_END);
      VisUtils::fillTriangle(
            0.0,    yEnd,
            -hdlDOF, yEnd-2.0*hdlDOF,
            hdlDOF, yEnd-2.0*hdlDOF);
      glPopName();
    }
  }
  else
  {
    /*
    // draw center
    VisUtils::setColor(Qt::red);
    VisUtils::drawLine(
         0.0,     0.0,
         hdlDOF, -hdlDOF );
    VisUtils::drawLine(
        -hdlDOF,  hdlDOF,
         0.0,     0.0 );
    */

    // draw starting cross
    VisUtils::setColor(Qt::red);
    VisUtils::drawLine(
          -hdlDOF,  hdlDOF,
          hdlDOF, -hdlDOF);
    VisUtils::drawLine(
          -hdlDOF,  hdlDOF,
          -hdlDOF,  hdlDOF);

    // pointing down
    if (yEnd < yBeg)
    {
      if (0.0 < yEnd)
        VisUtils::drawLineDashed(
              0.0, 0.0,
              0.0, yEnd);
      else if (yBeg < 0.0)
        VisUtils::drawLineDashed(
              0.0, 0.0,
              0.0, yBeg);
      VisUtils::setColor(Qt::red);
      VisUtils::drawLineDashed(
            0.0,  0.0,
            yBeg, yEnd);

      /*
      // start
      VisUtils::setColor(Qt::green);
      VisUtils::fillRect(
          -hdlDOF, hdlDOF,
           yBeg,   yBeg-1.5*hdlDOF );
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawRect(
          -hdlDOF, hdlDOF,
           yBeg,   yBeg-1.5*hdlDOF );
      VisUtils::drawLine(
          -2.0*hdlDOF, 2.0*hdlDOF,
           yBeg,       yBeg );
      */

      // stop
      VisUtils::setColor(Qt::green);
      VisUtils::fillTriangle(
            -hdlDOF, yEnd+2.0*hdlDOF,
            0.0,    yEnd,
            hdlDOF, yEnd+2.0*hdlDOF);
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawTriangle(
            -hdlDOF, yEnd+2.0*hdlDOF,
            0.0,    yEnd,
            hdlDOF, yEnd+2.0*hdlDOF);
      VisUtils::drawLine(
            2.0*hdlDOF, -2.0*hdlDOF,
            yEnd,        yEnd);
    }
    // pointing up
    else // ( yBeg <= yEnd )
    {
      if (0.0 < yBeg)
        VisUtils::drawLineDashed(
              0.0, 0.0,
              0.0, yBeg);
      else if (yEnd < 0.0)
        VisUtils::drawLineDashed(
              0.0, 0.0,
              0.0, yEnd);
      VisUtils::setColor(Qt::red);
      VisUtils::drawLineDashed(
            0.0,  0.0,
            yBeg, yEnd);

      /*
      // start
      VisUtils::setColor(Qt::green);
      VisUtils::fillRect(
          hdlDOF,          -hdlDOF,
          yBeg+1.5*hdlDOF,  yBeg );
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawRect(
          hdlDOF,          -hdlDOF,
          yBeg+1.5*hdlDOF,  yBeg );
      VisUtils::drawLine(
          2.0*hdlDOF, -2.0*hdlDOF,
          yBeg,        yBeg );
      */

      // stop
      VisUtils::setColor(Qt::green);
      VisUtils::fillTriangle(
            0.0,    yEnd,
            -hdlDOF, yEnd-2.0*hdlDOF,
            hdlDOF, yEnd-2.0*hdlDOF);
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawTriangle(
            0.0,    yEnd,
            -hdlDOF, yEnd-2.0*hdlDOF,
            hdlDOF, yEnd-2.0*hdlDOF);
      VisUtils::drawLine(
            2.0*hdlDOF, -2.0*hdlDOF,
            yEnd,        yEnd);
    }
  }

  glPopMatrix();
}


void Shape::drawEditDOFWth(
    const bool& inSelectMode,
    double pixelSize)
{
  double hdlDOF = hdlSzeHnt*pixelSize;
  double wBeg   = m_widthDOF->getMin();
  double wEnd   = m_widthDOF->getMax();

  if (inSelectMode == true)
  {
    // pointing right
    if (wBeg <= wEnd)
    {
      /*
      // start
      glPushName( ID_HDL_DOF_BEG );
      VisUtils::fillEllipse(
          xDFC+wBeg+hdlDOF, 0.0,
          hdlDOF,           hdlDOF,
          4 );
      glPopName();
      */

      // stop
      glPushName(ID_HDL_DOF_END);
      VisUtils::fillTriangle(
            m_xDistance+wEnd-2.0*hdlDOF,  hdlDOF,
            m_xDistance+wEnd-2.0*hdlDOF, -hdlDOF,
            m_xDistance+wEnd,             0.0);
      glPopName();
    }
    // pointing left
    else // ( ( wBeg > wEnd ) )
    {
      /*
      // start
      glPushName( ID_HDL_DOF_BEG );
      VisUtils::fillEllipse(
          xDFC+wBeg-hdlDOF, 0.0,
          hdlDOF,           hdlDOF,
          4 );
      glPopName();
      */

      // stop
      glPushName(ID_HDL_DOF_END);
      VisUtils::fillTriangle(
            m_xDistance+wEnd,             0.0,
            m_xDistance+wEnd+2.0*hdlDOF, -hdlDOF,
            m_xDistance+wEnd+2.0*hdlDOF,  hdlDOF);
      glPopName();
    }
  }
  else
  {
    // draw center of shape
    VisUtils::setColor(VisUtils::mediumGray);
    VisUtils::drawLine(
          -hdlDOF, hdlDOF,
          0.0,    0.0);
    VisUtils::drawLine(
          0.0,    0.0,
          -hdlDOF, hdlDOF);

    // pointing right
    if (wBeg <= wEnd)
    {
      // draw cur width
      VisUtils::setColor(Qt::red);
      VisUtils::drawLine(
            m_xDistance-hdlDOF,  m_xDistance+hdlDOF,
            hdlDOF,      -hdlDOF);
      VisUtils::drawLine(
            m_xDistance-hdlDOF,  m_xDistance+hdlDOF,
            -hdlDOF,       hdlDOF);

      // draw dashed connector to cur width
      if (m_xDistance < m_xDistance+wBeg)
        VisUtils::drawLineDashed(
              m_xDistance, m_xDistance+wBeg,
              0.0,  0.0);
      else if (wEnd+m_xDistance < m_xDistance)
        VisUtils::drawLineDashed(
              m_xDistance, m_xDistance+wEnd,
              0.0,  0.0);

      // draw connector between handles
      VisUtils::setColor(Qt::red);
      VisUtils::drawLineDashed(
            m_xDistance+wBeg, m_xDistance+wEnd,
            0.0,       0.0);

      /*
      // start
      VisUtils::setColor(Qt::green);
      VisUtils::fillEllipse(
          xDFC+wBeg+hdlDOF, 0.0,
          hdlDOF,           hdlDOF,
          4 );
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawEllipse(
          xDFC+wBeg+hdlDOF, 0.0,
          hdlDOF,           hdlDOF,
          4 );
      VisUtils::drawLine(
          xDFC+wBeg,   xDFC+wBeg,
          2.0*hdlDOF, -2.0*hdlDOF );
      */

      // stop
      VisUtils::setColor(Qt::green);
      VisUtils::fillTriangle(
            m_xDistance+wEnd-2.0*hdlDOF,  hdlDOF,
            m_xDistance+wEnd-2.0*hdlDOF, -hdlDOF,
            m_xDistance+wEnd,             0.0);
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawTriangle(
            m_xDistance+wEnd-2.0*hdlDOF,  hdlDOF,
            m_xDistance+wEnd-2.0*hdlDOF, -hdlDOF,
            m_xDistance+wEnd,             0.0);
      VisUtils::drawLine(
            m_xDistance+wEnd,   m_xDistance+wEnd,
            2.0*hdlDOF, -2.0*hdlDOF);
    }
    // pointing left
    else // ( ( wBeg > wEnd ) )
    {
      // draw cur width
      VisUtils::setColor(Qt::red);
      VisUtils::drawLine(
            m_xDistance-hdlDOF,  m_xDistance+hdlDOF,
            hdlDOF,      -hdlDOF);
      VisUtils::drawLine(
            m_xDistance-hdlDOF,  m_xDistance+hdlDOF,
            -hdlDOF,       hdlDOF);

      // draw dashed connector to cur width
      if (wBeg+m_xDistance < m_xDistance)
        VisUtils::drawLineDashed(
              m_xDistance, m_xDistance+wBeg,
              0.0,  0.0);
      else if (wEnd+m_xDistance > m_xDistance)
        VisUtils::drawLineDashed(
              m_xDistance, m_xDistance+wEnd,
              0.0,  0.0);

      // draw connector between handles
      VisUtils::setColor(Qt::red);
      VisUtils::drawLineDashed(
            m_xDistance+wBeg, m_xDistance+wEnd,
            0.0,       0.0);

      /*
      // start
      VisUtils::setColor(Qt::green);
      VisUtils::fillEllipse(
          xDFC+wBeg-hdlDOF, 0.0,
          hdlDOF,           hdlDOF,
          4 );
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawEllipse(
          xDFC+wBeg-hdlDOF, 0.0,
          hdlDOF,           hdlDOF,
          4 );
      VisUtils::drawLine(
          xDFC+wBeg,   xDFC+wBeg,
          2.0*hdlDOF, -2.0*hdlDOF );
      */

      // stop
      VisUtils::setColor(Qt::green);
      VisUtils::fillTriangle(
            m_xDistance+wEnd,             0.0,
            m_xDistance+wEnd+2.0*hdlDOF, -hdlDOF,
            m_xDistance+wEnd+2.0*hdlDOF,  hdlDOF);
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawTriangle(
            m_xDistance+wEnd,             0.0,
            m_xDistance+wEnd+2.0*hdlDOF, -hdlDOF,
            m_xDistance+wEnd+2.0*hdlDOF,  hdlDOF);
      VisUtils::drawLine(
            m_xDistance+wEnd,   m_xDistance+wEnd,
            2.0*hdlDOF, -2.0*hdlDOF);
    }
  }
}


void Shape::drawDOFWth(
    const bool& inSelectMode,
    double pixelSize)
{
  double hdlDOF = hdlSzeHnt*pixelSize;
  double wBeg   = m_widthDOF->getMin();
  double wEnd   = m_widthDOF->getMax();

  if (inSelectMode == true)
  {}
  else
  {
    /*
    // draw center of shape
    VisUtils::setColor(VisUtils::mediumGray);
    VisUtils::drawLine(
        -hdlDOF, hdlDOF,
         0.0,    0.0  );
    VisUtils::drawLine(
         0.0,    0.0,
        -hdlDOF, hdlDOF );
    */

    // pointing right
    if (wBeg <= wEnd)
    {
      // draw cur width
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawLine(
            m_xDistance-hdlDOF,  m_xDistance+hdlDOF,
            hdlDOF,      -hdlDOF);
      VisUtils::drawLine(
            m_xDistance-hdlDOF,  m_xDistance+hdlDOF,
            -hdlDOF,       hdlDOF);

      // draw dashed connector to cur width
      if (m_xDistance < m_xDistance+wBeg)
        VisUtils::drawLineDashed(
              m_xDistance, m_xDistance+wBeg,
              0.0,  0.0);
      else if (wEnd+m_xDistance < m_xDistance)
        VisUtils::drawLineDashed(
              m_xDistance, m_xDistance+wEnd,
              0.0,  0.0);
      // draw connector between handles
      VisUtils::drawLineDashed(
            m_xDistance+wBeg, m_xDistance+wEnd,
            0.0,       0.0);

      /*
      // start
      VisUtils::setColor(Qt::white);
      VisUtils::fillEllipse(
          xDFC+wBeg+hdlDOF, 0.0,
          hdlDOF,           hdlDOF,
          4 );
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawEllipse(
          xDFC+wBeg+hdlDOF, 0.0,
          hdlDOF,           hdlDOF,
          4 );
      VisUtils::drawLine(
          xDFC+wBeg,   xDFC+wBeg,
          2.0*hdlDOF, -2.0*hdlDOF );
      */

      // stop
      VisUtils::setColor(Qt::white);
      VisUtils::fillTriangle(
            m_xDistance+wEnd-2.0*hdlDOF,  hdlDOF,
            m_xDistance+wEnd-2.0*hdlDOF, -hdlDOF,
            m_xDistance+wEnd,             0.0);
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawTriangle(
            m_xDistance+wEnd-2.0*hdlDOF,  hdlDOF,
            m_xDistance+wEnd-2.0*hdlDOF, -hdlDOF,
            m_xDistance+wEnd,             0.0);
      VisUtils::drawLine(
            m_xDistance+wEnd,   m_xDistance+wEnd,
            2.0*hdlDOF, -2.0*hdlDOF);
    }
    // pointing left
    else // ( ( wBeg > wEnd ) )
    {
      // draw cur width
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawLine(
            m_xDistance-hdlDOF,  m_xDistance+hdlDOF,
            hdlDOF,      -hdlDOF);
      VisUtils::drawLine(
            m_xDistance-hdlDOF,  m_xDistance+hdlDOF,
            -hdlDOF,       hdlDOF);

      // draw dashed connector to cur width
      if (wBeg+m_xDistance < m_xDistance)
        VisUtils::drawLineDashed(
              m_xDistance, m_xDistance+wBeg,
              0.0,  0.0);
      else if (wEnd+m_xDistance > m_xDistance)
        VisUtils::drawLineDashed(
              m_xDistance, m_xDistance+wEnd,
              0.0,  0.0);
      // draw connector between handles
      VisUtils::drawLineDashed(
            m_xDistance+wBeg, m_xDistance+wEnd,
            0.0,       0.0);

      /*
      // start
      VisUtils::setColor(Qt::white);
      VisUtils::drawEllipse(
          xDFC+wBeg-hdlDOF,  0.0,
          hdlDOF,            hdlDOF,
          4 );
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawEllipse(
          xDFC+wBeg-hdlDOF, 0.0,
          hdlDOF,           hdlDOF,
          4 );
      VisUtils::drawLine(
          xDFC+wBeg,   xDFC+wBeg,
          2.0*hdlDOF, -2.0*hdlDOF );
      */

      // stop
      VisUtils::setColor(Qt::white);
      VisUtils::fillTriangle(
            m_xDistance+wEnd,             0.0,
            m_xDistance+wEnd+2.0*hdlDOF, -hdlDOF,
            m_xDistance+wEnd+2.0*hdlDOF,  hdlDOF);
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawTriangle(
            m_xDistance+wEnd,             0.0,
            m_xDistance+wEnd+2.0*hdlDOF, -hdlDOF,
            m_xDistance+wEnd+2.0*hdlDOF,  hdlDOF);
      VisUtils::drawLine(
            m_xDistance+wEnd,   m_xDistance+wEnd,
            2.0*hdlDOF, -2.0*hdlDOF);
    }
  }
}


void Shape::drawEditDOFHgt(
    const bool& inSelectMode,
    double pixelSize)
{
  double hdlDOF = hdlSzeHnt*pixelSize;
  double hBeg   = m_heightDOF->getMin();
  double hEnd   = m_heightDOF->getMax();

  if (inSelectMode == true)
  {
    // pointing up
    if (hBeg <= hEnd)
    {
      /*
      // start
      glPushName( ID_HDL_DOF_BEG );
      VisUtils::fillEllipse(
          0.0,    yDFC+hBeg+hdlDOF,
          hdlDOF, hdlDOF,
          segNumHnt );
      glPopName();
      */

      // stop
      glPushName(ID_HDL_DOF_END);
      VisUtils::fillTriangle(
            0.0,    m_yDistance+hEnd,
            -hdlDOF, m_yDistance+hEnd-2.0*hdlDOF,
            hdlDOF, m_yDistance+hEnd-2.0*hdlDOF);
      glPopName();
    }
    // pointing down
    else // ( ( wBeg > wEnd ) )
    {
      /*
      // start
      glPushName( ID_HDL_DOF_BEG );
      VisUtils::fillEllipse(
          0.0,    yDFC+hBeg-hdlDOF,
          hdlDOF, hdlDOF,
          segNumHnt );
      glPopName();
      */

      // stop
      glPushName(ID_HDL_DOF_END);
      VisUtils::fillTriangle(
            -hdlDOF, m_yDistance+hEnd+2.0*hdlDOF,
            0.0,    m_yDistance+hEnd,
            hdlDOF, m_yDistance+hEnd+2.0*hdlDOF);
      glPopName();
    }
  }
  else
  {
    // draw center of shape
    VisUtils::setColor(VisUtils::mediumGray);
    VisUtils::drawLine(
          -hdlDOF, hdlDOF,
          0.0,    0.0);
    VisUtils::drawLine(
          0.0,    0.0,
          -hdlDOF, hdlDOF);

    // pointing up
    if (hBeg <= hEnd)
    {
      // draw cur height
      VisUtils::setColor(Qt::red);
      VisUtils::drawLine(
            hdlDOF,      -hdlDOF,
            m_yDistance+hdlDOF,  m_yDistance-hdlDOF);
      VisUtils::drawLine(
            hdlDOF,      -hdlDOF,
            m_yDistance-hdlDOF,  m_yDistance+hdlDOF);

      // draw dashed connector to cur height
      if (m_yDistance < m_yDistance+hBeg)
        VisUtils::drawLineDashed(
              0.0,  0.0,
              m_yDistance, m_yDistance+hBeg);
      else if (hEnd+m_yDistance < m_yDistance)
        VisUtils::drawLineDashed(
              0.0,  0.0,
              m_yDistance, m_yDistance+hEnd);

      // draw connector between handles
      VisUtils::setColor(Qt::red);
      VisUtils::drawLineDashed(
            0.0,       0.0,
            m_yDistance+hBeg, m_yDistance+hEnd);

      /*
      // start
      VisUtils::setColor(Qt::green);
      VisUtils::fillEllipse(
          0.0,    yDFC+hBeg+hdlDOF,
          hdlDOF, hdlDOF,
          segNumHnt );
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawEllipse(
          0.0,    yDFC+hBeg+hdlDOF,
          hdlDOF, hdlDOF,
          segNumHnt );
      VisUtils::drawLine(
          -2.0*hdlDOF, 2.0*hdlDOF,
          yDFC+hBeg,   yDFC+hBeg );
      */

      // stop
      VisUtils::setColor(Qt::green);
      VisUtils::fillTriangle(
            0.0,    m_yDistance+hEnd,
            -hdlDOF, m_yDistance+hEnd-2.0*hdlDOF,
            hdlDOF, m_yDistance+hEnd-2.0*hdlDOF);
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawTriangle(
            0.0,    m_yDistance+hEnd,
            -hdlDOF, m_yDistance+hEnd-2.0*hdlDOF,
            hdlDOF, m_yDistance+hEnd-2.0*hdlDOF);
      VisUtils::drawLine(
            2.0*hdlDOF, -2.0*hdlDOF,
            m_yDistance+hEnd,   m_yDistance+hEnd);
    }
    // pointing down
    else // ( ( wBeg > wEnd ) )
    {
      // draw cur height
      VisUtils::setColor(Qt::red);
      VisUtils::drawLine(
            hdlDOF,      -hdlDOF,
            m_yDistance+hdlDOF,  m_yDistance-hdlDOF);
      VisUtils::drawLine(
            hdlDOF,      -hdlDOF,
            m_yDistance-hdlDOF,  m_yDistance+hdlDOF);

      // draw dashed connector to cur width
      if (hBeg+m_yDistance < m_yDistance)
        VisUtils::drawLineDashed(
              0.0,  0.0,
              m_yDistance, m_yDistance+hBeg);
      else if (hEnd+m_yDistance > m_yDistance)
        VisUtils::drawLineDashed(
              0.0,  0.0,
              m_yDistance, m_yDistance+hEnd);

      // draw connector between handles
      VisUtils::setColor(Qt::red);
      VisUtils::drawLineDashed(
            0.0,       0.0,
            m_yDistance+hBeg, m_yDistance+hEnd);

      /*
      // start
      VisUtils::setColor(Qt::green);
      VisUtils::fillEllipse(
          0.0,    yDFC+hBeg-hdlDOF,
          hdlDOF, hdlDOF,
          segNumHnt );
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawEllipse(
          0.0,    yDFC+hBeg-hdlDOF,
          hdlDOF, hdlDOF,
          segNumHnt );
      VisUtils::drawLine(
          -2.0*hdlDOF, 2.0*hdlDOF,
          yDFC+hBeg,   yDFC+hBeg );
      */

      // stop
      VisUtils::setColor(Qt::green);
      VisUtils::fillTriangle(
            -hdlDOF, m_yDistance+hEnd+2.0*hdlDOF,
            0.0,    m_yDistance+hEnd,
            hdlDOF, m_yDistance+hEnd+2.0*hdlDOF);
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawTriangle(
            -hdlDOF, m_yDistance+hEnd+2.0*hdlDOF,
            0.0,    m_yDistance+hEnd,
            hdlDOF, m_yDistance+hEnd+2.0*hdlDOF);
      VisUtils::drawLine(
            2.0*hdlDOF, -2.0*hdlDOF,
            m_yDistance+hEnd,   m_yDistance+hEnd);
    }
  }
}


void Shape::drawDOFHgt(
    const bool& inSelectMode,
    double pixelSize)
{
  double hdlDOF = hdlSzeHnt*pixelSize;
  double hBeg   = m_heightDOF->getMin();
  double hEnd   = m_heightDOF->getMax();

  if (inSelectMode == true)
  {}
  else
  {
    /*
    // draw center of shape
    VisUtils::setColor(VisUtils::mediumGray);
    VisUtils::drawLine(
        -hdlDOF, hdlDOF,
         0.0,    0.0  );
    VisUtils::drawLine(
         0.0,    0.0,
        -hdlDOF, hdlDOF );
    */

    // pointing up
    if (hBeg <= hEnd)
    {
      // draw cur height
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawLine(
            hdlDOF,      -hdlDOF,
            m_yDistance+hdlDOF,  m_yDistance-hdlDOF);
      VisUtils::drawLine(
            hdlDOF,      -hdlDOF,
            m_yDistance-hdlDOF,  m_yDistance+hdlDOF);

      // draw dashed connector to cur height
      if (m_yDistance < m_yDistance+hBeg)
        VisUtils::drawLineDashed(
              0.0,  0.0,
              m_yDistance, m_yDistance+hBeg);
      else if (hEnd+m_yDistance < m_yDistance)
        VisUtils::drawLineDashed(
              0.0,  0.0,
              m_yDistance, m_yDistance+hEnd);
      // draw connector between handles
      VisUtils::drawLineDashed(
            0.0,       0.0,
            m_yDistance+hBeg, m_yDistance+hEnd);

      /*
      // start
      VisUtils::setColor(Qt::white);
      VisUtils::fillEllipse(
          0.0,    yDFC+hBeg+hdlDOF,
          hdlDOF, hdlDOF,
          segNumHnt );
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawEllipse(
          0.0,    yDFC+hBeg+hdlDOF,
          hdlDOF, hdlDOF,
          segNumHnt );
      VisUtils::drawLine(
          -2.0*hdlDOF, 2.0*hdlDOF,
          yDFC+hBeg,   yDFC+hBeg );
      */

      // stop
      VisUtils::setColor(Qt::white);
      VisUtils::fillTriangle(
            0.0,    m_yDistance+hEnd,
            -hdlDOF, m_yDistance+hEnd-2.0*hdlDOF,
            hdlDOF, m_yDistance+hEnd-2.0*hdlDOF);
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawTriangle(
            0.0,    m_yDistance+hEnd,
            -hdlDOF, m_yDistance+hEnd-2.0*hdlDOF,
            hdlDOF, m_yDistance+hEnd-2.0*hdlDOF);
      VisUtils::drawLine(
            2.0*hdlDOF, -2.0*hdlDOF,
            m_yDistance+hEnd,   m_yDistance+hEnd);
    }
    // pointing down
    else // ( ( wBeg > wEnd ) )
    {
      // draw cur height
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawLine(
            hdlDOF,      -hdlDOF,
            m_yDistance+hdlDOF,  m_yDistance-hdlDOF);
      VisUtils::drawLine(
            hdlDOF,      -hdlDOF,
            m_yDistance-hdlDOF,  m_yDistance+hdlDOF);

      // draw dashed connector to cur width
      if (hBeg+m_yDistance < m_yDistance)
        VisUtils::drawLineDashed(
              0.0,  0.0,
              m_yDistance, m_yDistance+hBeg);
      else if (hEnd+m_yDistance > m_yDistance)
        VisUtils::drawLineDashed(
              0.0,  0.0,
              m_yDistance, m_yDistance+hEnd);
      // draw connector between handles
      VisUtils::drawLineDashed(
            0.0,       0.0,
            m_yDistance+hBeg, m_yDistance+hEnd);

      /*
      // start
      VisUtils::setColor(Qt::white);
      VisUtils::fillEllipse(
          0.0,    yDFC+hBeg-hdlDOF,
          hdlDOF, hdlDOF,
          segNumHnt );
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawEllipse(
          0.0,    yDFC+hBeg-hdlDOF,
          hdlDOF, hdlDOF,
          segNumHnt );
      VisUtils::drawLine(
          -2.0*hdlDOF, 2.0*hdlDOF,
          yDFC+hBeg,   yDFC+hBeg );
      */

      // stop
      VisUtils::setColor(Qt::white);
      VisUtils::fillTriangle(
            -hdlDOF, m_yDistance+hEnd+2.0*hdlDOF,
            0.0,    m_yDistance+hEnd,
            hdlDOF, m_yDistance+hEnd+2.0*hdlDOF);
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawTriangle(
            -hdlDOF, m_yDistance+hEnd+2.0*hdlDOF,
            0.0,    m_yDistance+hEnd,
            hdlDOF, m_yDistance+hEnd+2.0*hdlDOF);
      VisUtils::drawLine(
            2.0*hdlDOF, -2.0*hdlDOF,
            m_yDistance+hEnd,   m_yDistance+hEnd);
    }
  }
}


void Shape::drawEditDOFAgl(
    const bool& inSelectMode,
    double pixelSize)
{
  double hdlDOF = hdlSzeHnt*pixelSize;

  glPushMatrix();
  glRotatef(-m_angle, 0.0, 0.0, 1.0);

  if (inSelectMode == true)
  {
    double dstHgeCtr, dstHgeHdl;
    double aglRef, aglBeg, aglEnd;
    double xBeg, yBeg;
    double xEnd, yEnd;

    // distance from hinge to center
    dstHgeCtr = Utils::dist(m_xCenter+m_xHinge, m_yCenter+m_yHinge, m_xCenter, m_yCenter);
    // distance from center to handles
    dstHgeHdl = dstHgeCtr + 0.5*Utils::maxx(Utils::abs(m_xDistance), Utils::abs(m_yDistance));

    if (dstHgeCtr == 0)
    {
      aglRef = 0;
    }
    else
    {
      aglRef = Utils::calcAngleDg(-m_xHinge, -m_yHinge);
    }
    aglBeg = aglRef + m_angleDOF->getMin();
    aglEnd = aglRef + m_angleDOF->getMax();
    // x & y coords at aglBeg, dstHgeCtr+dstCtrHdl from hinge
    xBeg = m_xHinge+dstHgeHdl*cos(Utils::degrToRad(aglBeg));
    yBeg = m_yHinge+dstHgeHdl*sin(Utils::degrToRad(aglBeg));
    // x & y coords at aglEnd, dstHgeCtr+dstCtrHdl from hinge
    xEnd = m_xHinge+dstHgeHdl*cos(Utils::degrToRad(aglEnd));
    yEnd = m_yHinge+dstHgeHdl*sin(Utils::degrToRad(aglEnd));

    glPushName(ID_HDL_HGE);
    VisUtils::fillEllipse(
          m_xHinge,   m_yHinge,
          hdlDOF, hdlDOF,
          segNumHnt);
    glPopName();

    // counter clockwise rotation
    if (m_angleDOF->direction() > 0)
    {
      // start
      glPushMatrix();
      glTranslatef(xBeg, yBeg, 0.0);
      glRotatef(aglBeg, 0.0, 0.0, 1.0);

      /*
      glPushName( ID_HDL_DOF_BEG );
      VisUtils::fillRect(
          hdlDOF, -hdlDOF,
          0.0,     1.5*hdlDOF );
      glPopName();
      */

      glPushName(ID_HDL_DIR);
      VisUtils::fillTriangle(
            2.0*hdlDOF,  0.0,
            3.0*hdlDOF, -hdlDOF,
            4.0*hdlDOF,  0.0);
      glPopName();

      glPopMatrix();

      // end
      glPushMatrix();
      glTranslatef(xEnd, yEnd, 0.0);
      glRotatef(aglEnd, 0.0, 0.0, 1.0);

      glPushName(ID_HDL_DOF_END);
      VisUtils::fillTriangle(
            0.0,     0.0,
            hdlDOF, -2.0*hdlDOF,
            -hdlDOF, -2.0*hdlDOF);
      glPopName();

      glPopMatrix();
    }
    // clockwise rotation
    else
    {
      // start
      glPushMatrix();
      glTranslatef(xBeg, yBeg, 0.0);
      glRotatef(aglBeg, 0.0, 0.0, 1.0);

      /*
      glPushName( ID_HDL_DOF_BEG );
      VisUtils::fillRect(
         -hdlDOF,  hdlDOF,
          0.0,    -1.5*hdlDOF );
      glPopName();
      */

      glPushName(ID_HDL_DIR);
      VisUtils::fillTriangle(
            3.0*hdlDOF, hdlDOF,
            2.0*hdlDOF, 0.0,
            4.0*hdlDOF, 0.0);
      glPopName();

      glPopMatrix();

      // end
      glPushMatrix();
      glTranslatef(xEnd, yEnd, 0.0);
      glRotatef(aglEnd, 0.0, 0.0, 1.0);

      glPushName(ID_HDL_DOF_END);
      VisUtils::fillTriangle(
            hdlDOF, 2.0*hdlDOF,
            0.0,    0.0,
            -hdlDOF, 2.0*hdlDOF);
      glPopName();

      glPopMatrix();
    }
  }
  else
  {
    double dstHgeCtr, dstHgeHdl;
    double aglRef, aglBeg, aglEnd;
    double xBeg, yBeg;
    double xEnd, yEnd;

    // distance from hinge to center
    dstHgeCtr = Utils::dist(m_xCenter+m_xHinge, m_yCenter+m_yHinge, m_xCenter, m_yCenter);
    // distance from center to handles
    dstHgeHdl = dstHgeCtr + 0.5*Utils::maxx(Utils::abs(m_xDistance), Utils::abs(m_yDistance));

    if (dstHgeCtr == 0)
    {
      aglRef = 0;
    }
    else
    {
      aglRef = Utils::calcAngleDg(-m_xHinge, -m_yHinge);
    }
    aglBeg = aglRef + m_angleDOF->getMin();
    aglEnd = aglRef + m_angleDOF->getMax();
    // x & y coords at aglBeg, dstHgeCtr+dstCtrHdl from hinge
    xBeg = m_xHinge+dstHgeHdl*cos(Utils::degrToRad(aglBeg));
    yBeg = m_yHinge+dstHgeHdl*sin(Utils::degrToRad(aglBeg));
    // x & y coords at aglEnd, dstHgeCtr+dstCtrHdl from hinge
    xEnd = m_xHinge+dstHgeHdl*cos(Utils::degrToRad(aglEnd));
    yEnd = m_yHinge+dstHgeHdl*sin(Utils::degrToRad(aglEnd));

    // draw center
    VisUtils::setColor(VisUtils::mediumGray);
    VisUtils::drawLine(
          -hdlDOF,  hdlDOF,
          0.0,     0.0);
    VisUtils::drawLine(
          0.0,     0.0,
          hdlDOF, -hdlDOF);

    // draw hinge
    VisUtils::setColor(Qt::green);
    VisUtils::fillEllipse(
          m_xHinge,   m_yHinge,
          hdlDOF, hdlDOF,
          segNumHnt);
    VisUtils::setColor(VisUtils::mediumGray);
    VisUtils::drawEllipse(
          m_xHinge,   m_yHinge,
          hdlDOF, hdlDOF,
          segNumHnt);

    /*
    // connect hinge & center
    VisUtils::drawLineDashed(
        xHge, 0.0,
        yHge, 0.0 );
    */

    // counter clockwise rotation
    if (m_angleDOF->direction() > 0)
    {
      VisUtils::drawLine(
            m_xHinge, xBeg,
            m_yHinge, yBeg);
      VisUtils::drawLine(
            m_xHinge, xEnd,
            m_yHinge, yEnd);
      VisUtils::setColor(Qt::red);
      VisUtils::drawArcDashed(
            m_xHinge,      m_yHinge,
            aglBeg,    aglEnd,
            dstHgeHdl, segNumHnt);

      // start
      glPushMatrix();
      glTranslatef(xBeg, yBeg, 0.0);
      glRotatef(aglBeg, 0.0, 0.0, 1.0);

      /*
      VisUtils::fillRect(
          hdlDOF, -hdlDOF,
          0.0,     1.5*hdlDOF );
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawRect(
          hdlDOF, -hdlDOF,
          0.0,     1.5*hdlDOF );
      VisUtils::drawLine(
          2.0*hdlDOF, -2.0*hdlDOF,
          0.0,         0.0 );
      */

      // draw starting cross
      VisUtils::setColor(Qt::red);
      VisUtils::drawLine(
            -hdlDOF,  hdlDOF,
            hdlDOF, -hdlDOF);
      VisUtils::drawLine(
            -hdlDOF,  hdlDOF,
            -hdlDOF,  hdlDOF);

      VisUtils::setColor(Qt::green);
      VisUtils::fillTriangle(
            2.0*hdlDOF,  0.0,
            3.0*hdlDOF, -hdlDOF,
            4.0*hdlDOF,  0.0);
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawTriangle(
            2.0*hdlDOF,  0.0,
            3.0*hdlDOF, -hdlDOF,
            4.0*hdlDOF,  0.0);

      glPopMatrix();

      // end
      glPushMatrix();
      glTranslatef(xEnd, yEnd, 0.0);
      glRotatef(aglEnd, 0.0, 0.0, 1.0);
      VisUtils::setColor(Qt::green);
      VisUtils::fillTriangle(
            0.0,     0.0,
            hdlDOF, -2.0*hdlDOF,
            -hdlDOF, -2.0*hdlDOF);
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawTriangle(
            0.0,     0.0,
            hdlDOF, -2.0*hdlDOF,
            -hdlDOF, -2.0*hdlDOF);
      VisUtils::drawLine(
            2.0*hdlDOF, -2.0*hdlDOF,
            0.0,         0.0);
      glPopMatrix();
    }
    // clockwise rotation
    else
    {
      VisUtils::drawLine(
            m_xHinge, xBeg,
            m_yHinge, yBeg);
      VisUtils::drawLine(
            m_xHinge, xEnd,
            m_yHinge, yEnd);
      VisUtils::setColor(Qt::red);
      VisUtils::drawArcDashedCW(
            m_xHinge,      m_yHinge,
            aglBeg,    aglEnd,
            dstHgeHdl, segNumHnt);

      // start
      glPushMatrix();
      glTranslatef(xBeg, yBeg, 0.0);
      glRotatef(aglBeg, 0.0, 0.0, 1.0);

      /*
      VisUtils::fillRect(
         -hdlDOF,  hdlDOF,
          0.0,    -1.5*hdlDOF );
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawRect(
         -hdlDOF,  hdlDOF,
          0.0,    -1.5*hdlDOF );
      VisUtils::drawLine(
          2.0*hdlDOF, -2.0*hdlDOF,
          0.0,         0.0 );
      */

      // draw starting cross
      VisUtils::setColor(Qt::red);
      VisUtils::drawLine(
            -hdlDOF,  hdlDOF,
            hdlDOF, -hdlDOF);
      VisUtils::drawLine(
            -hdlDOF,  hdlDOF,
            -hdlDOF,  hdlDOF);

      VisUtils::setColor(Qt::green);
      VisUtils::fillTriangle(
            3.0*hdlDOF, hdlDOF,
            2.0*hdlDOF, 0.0,
            4.0*hdlDOF, 0.0);
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawTriangle(
            3.0*hdlDOF, hdlDOF,
            2.0*hdlDOF, 0.0,
            4.0*hdlDOF, 0.0);

      glPopMatrix();

      // end
      glPushMatrix();
      glTranslatef(xEnd, yEnd, 0.0);
      glRotatef(aglEnd, 0.0, 0.0, 1.0);
      VisUtils::setColor(Qt::green);
      VisUtils::fillTriangle(
            hdlDOF, 2.0*hdlDOF,
            0.0,    0.0,
            -hdlDOF, 2.0*hdlDOF);
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawTriangle(
            hdlDOF, 2.0*hdlDOF,
            0.0,    0.0,
            -hdlDOF, 2.0*hdlDOF);
      VisUtils::drawLine(
            2.0*hdlDOF, -2.0*hdlDOF,
            0.0,         0.0);
      glPopMatrix();
    }
  }

  glPopMatrix();
}


void Shape::drawDOFAgl(
    const bool& inSelectMode,
    double pixelSize)
{
  double hdlDOF = hdlSzeHnt*pixelSize;

  glPushMatrix();
  glRotatef(-m_angle, 0.0, 0.0, 1.0);

  if (inSelectMode == true)
  {}
  else
  {
    double dstHgeCtr, dstHgeHdl;
    double aglRef, aglBeg, aglEnd;
    double xBeg, yBeg;
    double xEnd, yEnd;

    // distance from hinge to center
    dstHgeCtr = Utils::dist(m_xCenter+m_xHinge, m_yCenter+m_yHinge, m_xCenter, m_yCenter);
    // distance from center to handles
    dstHgeHdl = dstHgeCtr + 0.5*Utils::maxx(Utils::abs(m_xDistance), Utils::abs(m_yDistance));

    if (dstHgeCtr == 0)
    {
      aglRef = 0;
    }
    else
    {
      aglRef = Utils::calcAngleDg(-m_xHinge, -m_yHinge);
    }
    aglBeg = aglRef + m_angleDOF->getMin();
    aglEnd = aglRef + m_angleDOF->getMax();
    // x & y coords at aglBeg, dstHgeCtr+dstCtrHdl from hinge
    xBeg = m_xHinge+dstHgeHdl*cos(Utils::degrToRad(aglBeg));
    yBeg = m_yHinge+dstHgeHdl*sin(Utils::degrToRad(aglBeg));
    // x & y coords at aglEnd, dstHgeCtr+dstCtrHdl from hinge
    xEnd = m_xHinge+dstHgeHdl*cos(Utils::degrToRad(aglEnd));
    yEnd = m_yHinge+dstHgeHdl*sin(Utils::degrToRad(aglEnd));

    // draw hinge
    VisUtils::setColor(VisUtils::mediumGray);
    VisUtils::drawEllipse(
          m_xHinge,   m_yHinge,
          hdlDOF, hdlDOF,
          segNumHnt);

    /*
    // connect hinge & center
    VisUtils::drawLineDashed(
        xHge, 0.0,
        yHge, 0.0 );
    */

    // counter clockwise rotation
    if (m_angleDOF->direction() > 0)
    {
      VisUtils::drawLineDashed(
            m_xHinge, xBeg,
            m_yHinge, yBeg);
      VisUtils::drawArcDashed(
            m_xHinge,      m_yHinge,
            aglBeg,    aglEnd,
            dstHgeHdl, segNumHnt);
      VisUtils::drawLineDashed(
            m_xHinge, xEnd,
            m_yHinge, yEnd);

      /*
      // start
      glPushMatrix();
      glTranslatef( xBeg, yBeg, 0.0 );
      glRotatef( aglBeg, 0.0, 0.0, 1.0 );
      VisUtils::setColor(Qt::white);
      VisUtils::fillRect(
          hdlDOF, -hdlDOF,
          0.0,     1.5*hdlDOF );
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawRect(
          hdlDOF, -hdlDOF,
          0.0,     1.5*hdlDOF );
      VisUtils::drawLine(
          2.0*hdlDOF, -2.0*hdlDOF,
          0.0,         0.0 );
      glPopMatrix();
      */

      // end
      glPushMatrix();
      glTranslatef(xEnd, yEnd, 0.0);
      glRotatef(aglEnd, 0.0, 0.0, 1.0);
      VisUtils::setColor(Qt::white);
      VisUtils::fillTriangle(
            0.0,     0.0,
            hdlDOF, -2.0*hdlDOF,
            -hdlDOF, -2.0*hdlDOF);
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawTriangle(
            0.0,     0.0,
            hdlDOF, -2.0*hdlDOF,
            -hdlDOF, -2.0*hdlDOF);
      VisUtils::drawLine(
            2.0*hdlDOF, -2.0*hdlDOF,
            0.0,         0.0);
      glPopMatrix();
    }
    // clockwise rotation
    else
    {
      VisUtils::drawLineDashed(
            m_xHinge, xBeg,
            m_yHinge, yBeg);
      VisUtils::drawArcDashedCW(
            m_xHinge,      m_yHinge,
            aglBeg,    aglEnd,
            dstHgeHdl, segNumHnt);
      VisUtils::drawLineDashed(
            m_xHinge, xEnd,
            m_yHinge, yEnd);

      /*
      // start
      glPushMatrix();
      glTranslatef( xBeg, yBeg, 0.0 );
      glRotatef( aglBeg, 0.0, 0.0, 1.0 );
      VisUtils::setColor(Qt::white);
      VisUtils::fillRect(
         -hdlDOF,  hdlDOF,
          0.0,    -1.5*hdlDOF );
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawRect(
         -hdlDOF,  hdlDOF,
          0.0,    -1.5*hdlDOF );
      VisUtils::drawLine(
          2.0*hdlDOF, -2.0*hdlDOF,
          0.0,         0.0 );
      glPopMatrix();
      */

      // end
      glPushMatrix();
      glTranslatef(xEnd, yEnd, 0.0);
      glRotatef(aglEnd, 0.0, 0.0, 1.0);
      VisUtils::setColor(Qt::white);
      VisUtils::fillTriangle(
            hdlDOF, 2.0*hdlDOF,
            0.0,    0.0,
            -hdlDOF, 2.0*hdlDOF);
      VisUtils::setColor(VisUtils::mediumGray);
      VisUtils::drawTriangle(
            hdlDOF, 2.0*hdlDOF,
            0.0,    0.0,
            -hdlDOF, 2.0*hdlDOF);
      VisUtils::drawLine(
            2.0*hdlDOF, -2.0*hdlDOF,
            0.0,         0.0);
      glPopMatrix();
    }
  }

  glPopMatrix();
}


// -- end -----------------------------------------------------------
