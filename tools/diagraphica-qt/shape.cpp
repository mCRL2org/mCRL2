// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./shape.cpp

#include "wx.hpp" // precompiled headers

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#ifdef WIN32
#include <windows.h>
#undef __in_range // for STLport on Windows
#endif
#include <GL/glu.h>
#endif

#include "shape.h"
#include <iostream>
using namespace std;


// -- static variables ----------------------------------------------


double Shape::hdlSzeHnt =  5.0;
double Shape::minSzeHnt =  5.0;
int Shape::segNumHnt = 24;
ColorRGB Shape::colTxt = { 0.0, 0.0, 0.0, 1.0 };


// -- constructors and destructor -----------------------------------


Shape::Shape(
  Mediator* m,      const size_t& idx,
  const double& xC, const double& yC,
  const double& xD, const double& yD,
  const double& aC, const int& typ)
  : Colleague(m)
{
  // geometery
  index  = idx;
  xCtr   = xC;
  yCtr   = yC;
  xDFC   = xD;
  yDFC   = yD;
  xHge   = 0.0;
  yHge   = 0.0;
  szeTxt = 12;
  variable = "";
  checkedVariableId = -1;
  texturesGenerated = false;
  lastCanvas = NULL;

  aglCtr = aC;

  // properties
  type      = typ;
  mode      = MODE_NORMAL;
  linWth    = 1.0;
  VisUtils::mapColorMdGray(colLin);
  VisUtils::mapColorLtGray(colFil);
  hdlSze    = hdlSzeHnt;

  // degrees of freedom
  initDOF();
}


Shape::Shape(const Shape& shape)
  : Colleague(shape.mediator)
// Copy constructor.
{
  index  = shape.index;

  // geometry
  xCtr   = shape.xCtr;
  yCtr   = shape.yCtr;
  xDFC   = shape.xDFC;
  yDFC   = shape.yDFC;
  xHge   = shape.xHge;
  yHge   = shape.yHge;
  aglCtr = shape.aglCtr;

  // properties
  type   = shape.type;
  mode   = shape.mode;
  linWth = shape.linWth;
  colLin = shape.colLin;
  colFil = shape.colFil;
  hdlSze = shape.hdlSze;

  // textual properties
  variable = shape.variable;
  variableName = shape.variableName;
  note = shape.note;
  checkedVariableId = shape.checkedVariableId;
  szeTxt = shape.szeTxt;

  // degrees of freedom, invoke copy constructors
  xCtrDOF = new DOF(*shape.xCtrDOF);
  yCtrDOF = new DOF(*shape.yCtrDOF);
  wthDOF  = new DOF(*shape.wthDOF);
  hgtDOF  = new DOF(*shape.hgtDOF);
  aglDOF  = new DOF(*shape.aglDOF);
  textDOF = new DOF(*shape.textDOF);

  colDOF  = new DOF(*shape.colDOF);
  colYValues = shape.colYValues;

  opaDOF  = new DOF(*shape.opaDOF);
  opaYValues = shape.opaYValues;
}


Shape::~Shape()
{
  clearDOF();
}


// -- set functions ---------------------------------------------


void Shape::setIndex(const size_t& idx)
{
  index = idx;
}


void Shape::setCheckedId(const int& id)
{
  checkedVariableId = id;
  texturesGenerated = false;
}


void Shape::setVariable(const string& msg)
{
  variable = "";
  variable.append(msg);
  texturesGenerated = false;
}


void Shape::setVariableName(const string& msg)
{
  variableName = "";
  variableName.append(msg);
  texturesGenerated = false;
}


void Shape::setNote(const string& msg)
{
  note = "";
  note.append(msg);
  texturesGenerated = false;
}


void Shape::setTextSize(const size_t& size)
{
  szeTxt = size;
  texturesGenerated = false;
}


void Shape::setCenter(const double& xC, const double& yC)
{
  double xLeft, xRight, yTop, yBottom;
  mediator->getGridCoordinates(xLeft, xRight, yTop, yBottom);
  if (xLeft <= (xC - xDFC) && xRight >= (xC + xDFC) && yBottom <= (yC - yDFC) && yTop >= (yC + yDFC))
  {
    xCtr = xC;
    yCtr = yC;
  }
  else
  {
    if (xLeft > (xC - xDFC))
    {
      xCtr = xLeft + xDFC;
    }
    else if (xRight < (xC + xDFC))
    {
      xCtr = xRight - xDFC;
    }
    if (yBottom > (yC - yDFC))
    {
      yCtr = yBottom + yDFC;
    }
    else if (yTop < (yC + yDFC))
    {
      yCtr = yTop - yDFC;
    }
  }
}


void Shape::setDFC(const double& xD, const double& yD)
{
  xDFC = xD;
  yDFC = yD;
}


void Shape::setAngleCtr(const double& a)
{
  aglCtr = a;
}


void Shape::setHinge(const double& xH, const double& yH)
{
  xHge = xH;
  yHge = yH;
}


void Shape::addDOFColYValue(const double& y)
{
  colYValues.push_back(y);
}


void Shape::setDOFColYValue(const size_t& idx, const double& y)
{
  if (idx != NON_EXISTING && static_cast <size_t>(idx) < colYValues.size())
  {
    colYValues[idx] = y;
  }
}


void Shape::clearDOFColYValue(const size_t& idx)
{
  if (idx != NON_EXISTING && static_cast <size_t>(idx) < colYValues.size())
  {
    colYValues.erase(colYValues.begin() + idx);
  }
}


void Shape::addDOFOpaYValue(const double& y)
{
  opaYValues.push_back(y);
}


void Shape::setDOFOpaYValue(const size_t& idx, const double& y)
{
  if (idx != NON_EXISTING && static_cast <size_t>(idx) < opaYValues.size())
  {
    opaYValues[idx] = y;
  }
}


void Shape::clearDOFOpaYValue(const size_t& idx)
{
  if (idx != NON_EXISTING && static_cast <size_t>(idx) < opaYValues.size())
  {
    opaYValues.erase(opaYValues.begin() + idx);
  }
}


void Shape::setType(const int& /*typ*/)
{
  if (type == TYPE_LINE)
  {
    setTypeLine();
  }
  else if (type == TYPE_RECT)
  {
    setTypeRect();
  }
  else if (type == TYPE_ELLIPSE)
  {
    setTypeEllipse();
  }
  else if (type == TYPE_ARROW)
  {
    setTypeArrow();
  }
  else if (type == TYPE_DARROW)
  {
    setTypeDArrow();
  }
  else if (type == TYPE_NOTE)
  {
    setTypeNote();
  }
}


void Shape::setTypeNote()
{
  type = TYPE_NOTE;
}


void Shape::setTypeLine()
{
  type = TYPE_LINE;
}


void Shape::setTypeRect()
{
  type = TYPE_RECT;
}


void Shape::setTypeEllipse()
{
  type = TYPE_ELLIPSE;
}


void Shape::setTypeArrow()
{
  type = TYPE_ARROW;
}


void Shape::setTypeDArrow()
{
  type = TYPE_DARROW;
}


void Shape::setMode(const int& mde)
{
  if (mde == MODE_NORMAL)
  {
    setModeNormal();
  }
  else if (mde == MODE_EDIT)
  {
    setModeEdit();
  }
  else if (mde == MODE_EDT_DOF_XCTR)
  {
    mode = MODE_EDT_DOF_XCTR;
  }
  else if (mde == MODE_EDT_DOF_YCTR)
  {
    mode = MODE_EDT_DOF_YCTR;
  }
  else if (mde == MODE_EDT_DOF_HGT)
  {
    mode = MODE_EDT_DOF_HGT;
  }
  else if (mde == MODE_EDT_DOF_WTH)
  {
    mode = MODE_EDT_DOF_WTH;
  }
  else if (mde == MODE_EDT_DOF_AGL)
  {
    mode = MODE_EDT_DOF_AGL;
  }
  else if (mde == MODE_EDT_DOF_COL)
  {
    mode = MODE_EDT_DOF_COL;
  }
  else if (mde == MODE_EDT_DOF_OPA)
  {
    mode = MODE_EDT_DOF_OPA;
  }
  else if (mde == MODE_EDT_DOF_TEXT)
  {
    mode = MODE_EDT_DOF_TEXT;
  }
}


void Shape::setModeNormal()
{
  mode = MODE_NORMAL;
}


void Shape::setModeEdit()
{
  mode = MODE_EDIT;
}


void Shape::setModeEdtDOFXCtr()
{
  mode = MODE_EDT_DOF_XCTR;
}


void Shape::setModeEdtDOFYCtr()
{
  mode = MODE_EDT_DOF_YCTR;
}


void Shape::setModeEdtDOFHgt()
{
  mode = MODE_EDT_DOF_HGT;
}


void Shape::setModeEdtDOFWth()
{
  mode = MODE_EDT_DOF_WTH;
}


void Shape::setModeEdtDOFAgl()
{
  mode = MODE_EDT_DOF_AGL;
}


void Shape::setModeEdtDOFCol()
{
  mode = MODE_EDT_DOF_COL;
}


void Shape::setModeEdtDOFOpa()
{
  mode = MODE_EDT_DOF_OPA;
}


void Shape::setModeEdtDOFText()
{
  mode = MODE_EDT_DOF_TEXT;
}


void Shape::setLineWidth(const double& w)
{
  linWth = w;
}


void Shape::setLineColor(const ColorRGB& c)
{
  colLin = c;
}


void Shape::setLineColor(
  const double& r,
  const double& g,
  const double& b,
  const double& a)
{
  colLin.r = r;
  colLin.g = g;
  colLin.b = b;
  colLin.a = a;
}


void Shape::setLineTransp(const double& a)
{
  colLin.a = a;
}


void Shape::setFillColor(const ColorRGB& c)
{
  colFil = c;
}


void Shape::setFillColor(
  const double& r,
  const double& g,
  const double& b,
  const double& a)
{
  colFil.r = r;
  colFil.g = g;
  colFil.b = b;
  colFil.a = a;
}


void Shape::setFillTransp(const double& a)
{
  colFil.a = a;
}


void Shape::setHandleSize(const double& s)
{
  hdlSze = s;
}


void Shape::setTextures(const bool& generated)
{
  texturesGenerated = generated;
}


// -- get functions -------------------------------------------------


size_t Shape::getIndex()
{
  return index;
}


int Shape::getCheckedId()
{
  return checkedVariableId;
}


string Shape::getNote()
{
  return note;
}


string Shape::getVariable()
{
  return variable;
}


string Shape::getVariableName()
{
  return variableName;
}


size_t Shape::getTextSize()
{
  return szeTxt;
}


void Shape::getCenter(double& x, double& y)
{
  x = xCtr;
  y = yCtr;
}


double Shape::getXCtr()
{
  return xCtr;
}


double Shape::getYCtr()
{
  return yCtr;
}


void Shape::getDFC(double& x, double& y)
{
  x = xDFC;
  y = yDFC;
}


double Shape::getXDFC()
{
  return xDFC;
}


double Shape::getYDFC()
{
  return yDFC;
}


double Shape::getAngleCtr()
{
  return aglCtr;
}


void Shape::getHinge(double& x, double& y)
{
  x = xHge;
  y = yHge;
}


double Shape::getXHinge()
{
  return xHge;
}


double Shape::getYHinge()
{
  return yHge;
}


int Shape::getType()
{
  return type;
}


int Shape::getMode()
{
  return mode;
}


double Shape::getLineWidth()
{
  return linWth;
}


void Shape::getLineColor(ColorRGB& c)
{
  c = colLin;
}


void Shape::getLineColor(double& r, double& g, double& b, double& a)
{
  r = colLin.r;
  g = colLin.g;
  b = colLin.b;
  a = colLin.a;
}


double Shape::getLineTransp()
{
  return colFil.a;
}


void Shape::getFillColor(ColorRGB& c)
{
  c = colFil;
}


void Shape::getFillColor(double& r, double& g, double& b, double& a)
{
  r = colFil.r;
  g = colFil.g;
  b = colFil.b;
  a = colFil.a;
}


double Shape::getFillTransp()
{
  return colFil.a;
}


double Shape::getHandleSize()
{
  return hdlSze;
}


DOF* Shape::getDOFXCtr()
{
  return xCtrDOF;
}


DOF* Shape::getDOFYCtr()
{
  return yCtrDOF;
}


DOF* Shape::getDOFWth()
{
  return wthDOF;
}


DOF* Shape::getDOFHgt()
{
  return hgtDOF;
}


DOF* Shape::getDOFAgl()
{
  return aglDOF;
}


DOF* Shape::getDOFCol()
{
  return colDOF;
}


DOF* Shape::getDOFText()
{
  return textDOF;
}


void Shape::getDOFColYValues(vector< double > &yVals)
{
  yVals = colYValues;
}


DOF* Shape::getDOFOpa()
{
  return opaDOF;
}


void Shape::getDOFOpaYValues(vector< double > &yVals)
{
  yVals = opaYValues;
}


void Shape::getDOFAttrs(vector< Attribute* > &attrs)
// This function returns a vector of pointers to all attributes
// to which DOF's have been linked. This vector contains no
// duplicates.
{
  set< Attribute* > tempSet;
  Attribute*        tempAttr;

  // get result
  tempAttr = xCtrDOF->getAttribute();
  if (tempAttr != NULL)
  {
    tempSet.insert(tempAttr);
  }

  tempAttr = yCtrDOF->getAttribute();
  if (tempAttr != NULL)
  {
    tempSet.insert(tempAttr);
  }

  tempAttr = wthDOF->getAttribute();
  if (tempAttr != NULL)
  {
    tempSet.insert(tempAttr);
  }

  tempAttr = hgtDOF->getAttribute();
  if (tempAttr != NULL)
  {
    tempSet.insert(tempAttr);
  }

  tempAttr = aglDOF->getAttribute();
  if (tempAttr != NULL)
  {
    tempSet.insert(tempAttr);
  }

  tempAttr = colDOF->getAttribute();
  if (tempAttr != NULL)
  {
    tempSet.insert(tempAttr);
  }

  tempAttr = opaDOF->getAttribute();
  if (tempAttr != NULL)
  {
    tempSet.insert(tempAttr);
  }

  // update result
  attrs.clear();
  set< Attribute* >::iterator it;
  for (it = tempSet.begin(); it != tempSet.end(); ++it)
  {
    attrs.push_back(*it);
  }

  // clear memory
  tempSet.clear();
  tempAttr = NULL;
}


// -- visualization ---------------------------------------------


void Shape::visualize(
  const bool& inSelectMode,
  GLCanvas* canvas)
{
  // set up transf
  glPushMatrix();
  glTranslatef(xCtr, yCtr, 0.0);
  glRotatef(aglCtr, 0.0, 0.0, 1.0);

  if (mode == MODE_NORMAL)
  {
    drawNormal(inSelectMode, canvas);
  }
  else if (mode == MODE_EDIT)
  {
    drawEdit(inSelectMode, canvas);
  }
  else
  {
    drawEditDOF(inSelectMode, canvas);
  }

  // clear transf
  glPopMatrix();
}


void Shape::visualize(
  GLCanvas* canvas,
  const vector< Attribute* > attrs,
  const vector< double > attrValIdcs)
{
  double xC, yC; // center, [-1,1]
  double xD, yD; // bound dist from ctr,norm
  double aglH;   // rotation about hinge, degrees
  ColorHLS colHLS;
  ColorRGB colRGB;

  double alpha;

  xC         = xCtr;
  yC         = yCtr;
  xD         = xDFC;
  yD         = yDFC;
  aglH       = 0.0;
  colRGB     = colFil;
  colHLS.l   = 0.5;
  colHLS.s   = 1.0;

  for (size_t i = 0; i < attrs.size(); ++i)
  {
    /*
    if ( attrs[i]->getSizeCurValues() == 1 )
        alpha = 0.0;
    else
        alpha = (double)attrValIdcs[i]/( (double)attrs[i]->getSizeCurValues() - 1.0 );
    */
    if (attrs[i]->getSizeCurValues() == 0 && attrs[i]->getAttrType() == Attribute::ATTR_TYPE_CONTI)
    {
      alpha = (attrValIdcs[i] - attrs[i]->getLowerBound())/(attrs[i]->getUpperBound() - attrs[i]->getLowerBound());
    }
    else if (attrs[i]->getSizeCurValues() == 1)
    {
      alpha = 0.0;
    }
    else
    {
      alpha = (double)attrValIdcs[i]/((double)attrs[i]->getSizeCurValues() - 1.0);
    }

    if (attrs[i] == xCtrDOF->getAttribute())
    {
      xC = xCtr + (1-alpha)*xCtrDOF->getMin() + alpha*xCtrDOF->getMax();
    }

    if (attrs[i] == yCtrDOF->getAttribute())
    {
      yC = yCtr + (1-alpha)*yCtrDOF->getMin() + alpha*yCtrDOF->getMax();
    }

    if (attrs[i] == wthDOF->getAttribute())
    {
      xD = xDFC + (1-alpha)*wthDOF->getMin() + alpha*wthDOF->getMax();
    }

    if (attrs[i] == hgtDOF->getAttribute())
    {
      yD = yDFC + (1-alpha)*hgtDOF->getMin() + alpha*hgtDOF->getMax();
    }

    if (attrs[i] == aglDOF->getAttribute())
    {
      if (aglDOF->getDir() > 0)
      {
        aglH = 0.0 + (1-alpha)*aglDOF->getMin() + alpha*aglDOF->getMax();
      }
      else
      {
        aglH = 360.0 - (1-alpha)*aglDOF->getMin() + alpha*aglDOF->getMax();
      }
    }

    if (attrs[i] == colDOF->getAttribute())
    {
      double intPtVal;
      double dblPtVal;

      dblPtVal = modf(alpha*(colDOF->getSizeValues()-1), &intPtVal);

      if (intPtVal < colDOF->getSizeValues()-1)
        colHLS.h = ((1.0-dblPtVal)*colDOF->getValue((int)intPtVal)
                    + dblPtVal*colDOF->getValue((int)intPtVal+1))*360.0;
      else
      {
        colHLS.h = colDOF->getValue((int)intPtVal)*360.0;
      }

      VisUtils::hlsToRgb(colHLS, colRGB);
    }

    if (attrs[i] == opaDOF->getAttribute())
    {
      double intPtVal;
      double dblPtVal;

      dblPtVal = modf(alpha*(opaDOF->getSizeValues()-1), &intPtVal);

      if (intPtVal < opaDOF->getSizeValues()-1)
        colRGB.a = ((1.0-dblPtVal)*opaDOF->getValue((int)intPtVal)
                    + dblPtVal*opaDOF->getValue((int)intPtVal+1));
      else
      {
        colRGB.a = opaDOF->getValue((int)intPtVal);
      }
    }

    if (attrs[i] == textDOF->getAttribute() && attrs[i]->getSizeCurValues() > 0)
    {
      variable = attrs[i]->getCurValue((int) attrValIdcs[i])->getValue();
    }
  }

  // set up transf
  glPushMatrix();

  // move to center pos & rotate
  glTranslatef(xC, yC, 0.0);
  glRotatef(aglCtr, 0.0, 0.0, 1.0);

  // move to hinge pos & rotate
  glTranslatef(xHge, yHge, 0.0);
  glRotatef(aglH, 0.0, 0.0, 1.0);
  glTranslatef(-xHge, -yHge, 0.0);

  VisUtils::enableLineAntiAlias();
  VisUtils::enableBlending();


  if (type == TYPE_NOTE)
  {
    VisUtils::setColor(colRGB);
    VisUtils::fillRect(
      -xD,  xD,   // new
      yD, -yD);  // new
    VisUtils::setColor(colLin);
    VisUtils::drawRect(
      -xD,  xD,   // new
      yD, -yD);  // new
  }
  else if (type == TYPE_RECT)
  {
    //VisUtils::setColor( colFil );
    VisUtils::setColor(colRGB);
    VisUtils::fillRect(
      -xD,  xD,   // new
      yD, -yD);  // new
    VisUtils::setColor(colLin);
    VisUtils::drawRect(
      -xD,  xD,   // new
      yD, -yD);  // new
  }
  else if (type == TYPE_ELLIPSE)
  {
    //VisUtils::setColor( colFil );
    VisUtils::setColor(colRGB);
    VisUtils::fillEllipse(
      0.0, 0.0,
      xD,  yD,
      segNumHnt);
    VisUtils::setColor(colLin);
    VisUtils::drawEllipse(
      0.0, 0.0,
      xD,  yD,
      segNumHnt);
  }
  else if (type == TYPE_LINE)
  {
    //VisUtils::setColor( colLin );
    VisUtils::setColor(colRGB);
    VisUtils::drawLine(
      -xD,  xD,
      yD, -yD);
  }
  else if (type == TYPE_ARROW)
  {
    double pix = canvas->getPixelSize();

    //VisUtils::setColor( colFil );
    VisUtils::setColor(colRGB);
    VisUtils::fillArrow(
      -xD,         xD,
      yD,        -yD,
      hdlSze*pix, 2*hdlSze*pix);

    VisUtils::setColor(colLin);
    VisUtils::drawArrow(
      -xD,       xD,
      yD,      -yD,
      hdlSze*pix, 2*hdlSze*pix);
  }
  else if (type == TYPE_DARROW)
  {
    double pix = canvas->getPixelSize();

    //VisUtils::setColor( colFil );
    VisUtils::setColor(colRGB);
    VisUtils::fillDArrow(
      -xD,         xD,
      yD,        -yD,
      hdlSze*pix, 2*hdlSze*pix);
    VisUtils::setColor(colLin);
    VisUtils::drawDArrow(
      -xD,         xD,
      yD,        -yD,
      hdlSze*pix, 2*hdlSze*pix);
  }
  drawText(canvas);   // Draw the textual values of the shape
  VisUtils::disableBlending();
  VisUtils::disableLineAntiAlias();

  // clear transf
  glPopMatrix();

}


void Shape::visualize(
  GLCanvas* canvas,
  const vector< Attribute* > attrs,
  const vector< double > attrValIdcs,
  const double& pixel)
{
  double xC, yC; // center, [-1,1]
  double xD, yD; // bound dist from ctr,norm
  double aglH;   // rotation about hinge, degrees
  ColorHLS colHLS;
  ColorRGB colRGB;

  double alpha;

  xC         = xCtr;
  yC         = yCtr;
  xD         = xDFC;
  yD         = yDFC;
  aglH       = 0.0;
  colRGB     = colFil;
  colHLS.l   = 0.5;
  colHLS.s   = 1.0;

  for (size_t i = 0; i < attrs.size(); ++i)
  {
    /*
    if ( attrs[i]->getSizeCurValues() == 1 )
        alpha = 0.0;
    else
        alpha = (double)attrValIdcs[i]/( (double)attrs[i]->getSizeCurValues() - 1.0 );
    */
    if (attrs[i]->getSizeCurValues() == 0 && attrs[i]->getAttrType() == Attribute::ATTR_TYPE_CONTI)
    {
      alpha = (attrValIdcs[i] - attrs[i]->getLowerBound())/(attrs[i]->getUpperBound() - attrs[i]->getLowerBound());
    }
    else if (attrs[i]->getSizeCurValues() == 1)
    {
      alpha = 0.0;
    }
    else
    {
      alpha = (double)attrValIdcs[i]/((double)attrs[i]->getSizeCurValues() - 1.0);
    }

    if (attrs[i] == xCtrDOF->getAttribute())
    {
      xC = xCtr + (1-alpha)*xCtrDOF->getMin() + alpha*xCtrDOF->getMax();
    }

    if (attrs[i] == yCtrDOF->getAttribute())
    {
      yC = yCtr + (1-alpha)*yCtrDOF->getMin() + alpha*yCtrDOF->getMax();
    }

    if (attrs[i] == wthDOF->getAttribute())
    {
      xD = xDFC + (1-alpha)*wthDOF->getMin() + alpha*wthDOF->getMax();
    }

    if (attrs[i] == hgtDOF->getAttribute())
    {
      yD = yDFC + (1-alpha)*hgtDOF->getMin() + alpha*hgtDOF->getMax();
    }

    if (attrs[i] == aglDOF->getAttribute())
    {
      if (aglDOF->getDir() > 0)
      {
        aglH = 0.0 + (1-alpha)*aglDOF->getMin() + alpha*aglDOF->getMax();
      }
      else
      {
        aglH = 360.0 - (1-alpha)*aglDOF->getMin() + alpha*aglDOF->getMax();
      }
    }

    if (attrs[i] == colDOF->getAttribute())
    {
      double intPtVal;
      double dblPtVal;

      dblPtVal = modf(alpha*(colDOF->getSizeValues()-1), &intPtVal);

      if (intPtVal < colDOF->getSizeValues()-1)
        colHLS.h = ((1.0-dblPtVal)*colDOF->getValue((int)intPtVal)
                    + dblPtVal*colDOF->getValue((int)intPtVal+1))*360.0;
      else
      {
        colHLS.h = colDOF->getValue((int)intPtVal)*360.0;
      }

      VisUtils::hlsToRgb(colHLS, colRGB);
    }

    if (attrs[i] == opaDOF->getAttribute())
    {
      double intPtVal;
      double dblPtVal;

      dblPtVal = modf(alpha*(opaDOF->getSizeValues()-1), &intPtVal);

      if (intPtVal < opaDOF->getSizeValues()-1)
        colRGB.a = ((1.0-dblPtVal)*opaDOF->getValue((int)intPtVal)
                    + dblPtVal*opaDOF->getValue((int)intPtVal+1));
      else
      {
        colRGB.a = opaDOF->getValue((int)intPtVal);
      }
    }

    if (attrs[i] == textDOF->getAttribute() && attrs[i]->getSizeCurValues() > 0)
    {
      variable = attrs[i]->getCurValue((int) attrValIdcs[i])->getValue();
    }
  }

  // set up transf
  glPushMatrix();

  // move to center pos & rotate
  glTranslatef(xC, yC, 0.0);
  glRotatef(aglCtr, 0.0, 0.0, 1.0);

  // move to hinge pos & rotate
  glTranslatef(xHge, yHge, 0.0);
  glRotatef(aglH, 0.0, 0.0, 1.0);
  glTranslatef(-xHge, -yHge, 0.0);

  VisUtils::enableLineAntiAlias();
  VisUtils::enableBlending();


  if (type == TYPE_NOTE)
  {
    VisUtils::setColor(colRGB);
    VisUtils::fillRect(
      -xD,  xD,   // new
      yD, -yD);  // new
    VisUtils::setColor(colLin);
    VisUtils::drawRect(
      -xD,  xD,   // new
      yD, -yD);  // new
  }
  else if (type == TYPE_RECT)
  {
    //VisUtils::setColor( colFil );
    VisUtils::setColor(colRGB);
    VisUtils::fillRect(
      -xD,  xD,   // new
      yD, -yD);  // new
    VisUtils::setColor(colLin);
    VisUtils::drawRect(
      -xD,  xD,   // new
      yD, -yD);  // new
  }
  else if (type == TYPE_ELLIPSE)
  {
    //VisUtils::setColor( colFil );
    VisUtils::setColor(colRGB);
    VisUtils::fillEllipse(
      0.0, 0.0,
      xD,  yD,
      segNumHnt);
    VisUtils::setColor(colLin);
    VisUtils::drawEllipse(
      0.0, 0.0,
      xD,  yD,
      segNumHnt);
  }
  else if (type == TYPE_LINE)
  {
    //VisUtils::setColor( colLin );
    VisUtils::setColor(colRGB);
    VisUtils::drawLine(
      -xD,  xD,
      yD, -yD);
  }
  else if (type == TYPE_ARROW)
  {
    double pix = canvas->getPixelSize();

    //VisUtils::setColor( colFil );
    VisUtils::setColor(colRGB);
    VisUtils::fillArrow(
      -xD,         xD,
      yD,        -yD,
      hdlSze*pix, 2*hdlSze*pix);

    VisUtils::setColor(colLin);
    VisUtils::drawArrow(
      -xD,       xD,
      yD,      -yD,
      hdlSze*pix, 2*hdlSze*pix);
  }
  else if (type == TYPE_DARROW)
  {
    double pix = canvas->getPixelSize();

    //VisUtils::setColor( colFil );
    VisUtils::setColor(colRGB);
    VisUtils::fillDArrow(
      -xD,         xD,
      yD,        -yD,
      hdlSze*pix, 2*hdlSze*pix);
    VisUtils::setColor(colLin);
    VisUtils::drawDArrow(
      -xD,         xD,
      yD,        -yD,
      hdlSze*pix, 2*hdlSze*pix);
  }
  drawText(canvas, pixel);   // Draw the textual values of the shape
  VisUtils::disableBlending();
  VisUtils::disableLineAntiAlias();

  // clear transf
  glPopMatrix();
}


void Shape::visualize(
  GLCanvas* canvas,
  const double& opacity,
  const vector< Attribute* > attrs,
  const vector< double > attrValIdcs
)
{
  double xC, yC; // center, [-1,1]
  double xD, yD; // bound dist from ctr,norm
  double aglH;   // rotation about hinge, degrees
  ColorHLS colHLS;
  ColorRGB colRGB;

  double alpha = 0.0;

  xC         = xCtr;
  yC         = yCtr;
  xD         = xDFC;
  yD         = yDFC;
  aglH       = 0.0;
  colRGB     = colFil;
  colHLS.l   = 0.5;
  colHLS.s   = 1.0;

  for (size_t i = 0; i < attrs.size(); ++i)
  {
    /*
    if ( attrs[i]->getSizeCurValues() == 1 )
        alpha = 0.0;
    else
        alpha = (double)attrValIdcs[i]/( (double)attrs[i]->getSizeCurValues() - 1.0 );
    */
    if (attrs[i]->getAttrType() == Attribute::ATTR_TYPE_DISCR)
    {
      if (attrs[i]->getSizeCurValues() == 1)
      {
        alpha = 0.0;
      }
      else
      {
        alpha = (double)attrValIdcs[i]/((double)attrs[i]->getSizeCurValues() - 1.0);
      }
    }
    else if (attrs[i]->getAttrType() == Attribute::ATTR_TYPE_CONTI)
    {
      if (attrs[i]->getSizeCurValues() == 0)
      {
        alpha = (attrValIdcs[i] - attrs[i]->getLowerBound())/(attrs[i]->getUpperBound() - attrs[i]->getLowerBound());
      }
      else if (attrs[i]->getSizeCurValues() == 1)
      {
        alpha = 0.0;
      }
      else
      {
        alpha = (double)attrValIdcs[i]/((double)attrs[i]->getSizeCurValues() - 1.0);
      }
    }

    if (attrs[i] == xCtrDOF->getAttribute())
    {
      xC = xCtr + (1-alpha)*xCtrDOF->getMin() + alpha*xCtrDOF->getMax();
    }

    if (attrs[i] == yCtrDOF->getAttribute())
    {
      yC = yCtr + (1-alpha)*yCtrDOF->getMin() + alpha*yCtrDOF->getMax();
    }

    if (attrs[i] == wthDOF->getAttribute())
    {
      xD = xDFC + (1-alpha)*wthDOF->getMin() + alpha*wthDOF->getMax();
    }

    if (attrs[i] == hgtDOF->getAttribute())
    {
      yD = yDFC + (1-alpha)*hgtDOF->getMin() + alpha*hgtDOF->getMax();
    }

    if (attrs[i] == aglDOF->getAttribute())
    {
      if (aglDOF->getDir() > 0)
      {
        aglH = 0.0 + (1-alpha)*aglDOF->getMin() + alpha*aglDOF->getMax();
      }
      else
      {
        aglH = 360.0 - (1-alpha)*aglDOF->getMin() + alpha*aglDOF->getMax();
      }
    }

    if (attrs[i] == colDOF->getAttribute())
    {
      double intPtVal;
      double dblPtVal;

      dblPtVal = modf(alpha*(colDOF->getSizeValues()-1), &intPtVal);

      if (intPtVal < colDOF->getSizeValues()-1)
        colHLS.h = ((1.0-dblPtVal)*colDOF->getValue((int)intPtVal)
                    + dblPtVal*colDOF->getValue((int)intPtVal+1))*360.0;
      else
      {
        colHLS.h = colDOF->getValue((int)intPtVal)*360.0;
      }

      VisUtils::hlsToRgb(colHLS, colRGB);
    }

    if (attrs[i] == opaDOF->getAttribute())
    {
      double intPtVal;
      double dblPtVal;

      dblPtVal = modf(alpha*(opaDOF->getSizeValues()-1), &intPtVal);

      if (intPtVal < opaDOF->getSizeValues()-1)
        colRGB.a = ((1.0-dblPtVal)*opaDOF->getValue((int)intPtVal)
                    + dblPtVal*opaDOF->getValue((int)intPtVal+1));
      else
      {
        colRGB.a = opaDOF->getValue((int)intPtVal);
      }
    }
  }

  ColorRGB colFill = colRGB;
  colFill.a -= opacity;

  ColorRGB colLine = colLin;
  colLine.a -= opacity;

  // set up transf
  glPushMatrix();

  // move to center pos & rotate
  glTranslatef(xC, yC, 0.0);
  glRotatef(aglCtr, 0.0, 0.0, 1.0);

  // move to hinge pos & rotate
  glTranslatef(xHge, yHge, 0.0);
  glRotatef(aglH, 0.0, 0.0, 1.0);
  glTranslatef(-xHge, -yHge, 0.0);

  VisUtils::enableLineAntiAlias();
  VisUtils::enableBlending();

  if (type == TYPE_RECT)
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
  else if (type == TYPE_RECT)
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
  else if (type == TYPE_ELLIPSE)
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
  else if (type == TYPE_LINE)
  {
    //VisUtils::setColor( colLin );
    VisUtils::setColor(colFill);
    VisUtils::drawLine(
      -xD,  xD,
      yD, -yD);
  }
  else if (type == TYPE_ARROW)
  {
    double pix = canvas->getPixelSize();

    //VisUtils::setColor( colFil );
    VisUtils::setColor(colFill);
    VisUtils::fillArrow(
      -xD,         xD,
      yD,        -yD,
      hdlSze*pix, 2*hdlSze*pix);

    VisUtils::setColor(colLine);
    VisUtils::drawArrow(
      -xD,       xD,
      yD,      -yD,
      hdlSze*pix, 2*hdlSze*pix);
  }
  else if (type == TYPE_DARROW)
  {
    double pix = canvas->getPixelSize();

    //VisUtils::setColor( colFil );
    VisUtils::setColor(colFill);
    VisUtils::fillDArrow(
      -xD,         xD,
      yD,        -yD,
      hdlSze*pix, 2*hdlSze*pix);
    VisUtils::setColor(colLine);
    VisUtils::drawDArrow(
      -xD,         xD,
      yD,        -yD,
      hdlSze*pix, 2*hdlSze*pix);
  }
  drawText(canvas);   // Draw the textual values of the shape
  VisUtils::disableBlending();
  VisUtils::disableLineAntiAlias();

  // clear transf
  glPopMatrix();
}


void Shape::setTransf()
{
  // set up transf
  VisUtils::setTransf(
    xCtr,   yCtr,
    xDFC,   yDFC,
    xHge,   yHge,
    aglCtr, 0.0);
}


void Shape::clrTransf()
{
  // clear transf
  VisUtils::clrTransf();
}


// -- event handlers --------------------------------------------


void Shape::handleHit(const size_t& hdlIdx)
{
  if (mode == MODE_EDT_DOF_AGL)
  {
    handleHitEdtDOFAgl(hdlIdx);
  }
}


// -- private utility functions -------------------------------------


void Shape::initDOF()
{
  xCtrDOF = new DOF(0, "X-position");
  yCtrDOF = new DOF(1, "Y-position");
  wthDOF  = new DOF(2, "Width");
  hgtDOF  = new DOF(3, "Height");
  aglDOF  = new DOF(4, "Rotation");

  colDOF  = new DOF(5, "Color");
  colDOF->setMin(0.25);
  colDOF->setMax(0.75);
  colYValues.push_back(0.0);
  colYValues.push_back(0.0);

  opaDOF  = new DOF(6, "Opacity");
  opaDOF->setMin(0.25);
  opaDOF->setMax(0.75);
  opaYValues.push_back(0.0);
  opaYValues.push_back(0.0);

  textDOF = new DOF(7, "Text");
}


void Shape::clearDOF()
{
  // composition
  if (xCtrDOF != NULL)
  {
    delete xCtrDOF;
    xCtrDOF = NULL;
  }

  if (yCtrDOF != NULL)
  {
    delete yCtrDOF;
    yCtrDOF = NULL;
  }

  if (wthDOF != NULL)
  {
    delete wthDOF;
    wthDOF = NULL;
  }

  if (hgtDOF != NULL)
  {
    delete hgtDOF;
    hgtDOF = NULL;
  }

  if (aglDOF != NULL)
  {
    delete aglDOF;
    aglDOF = NULL;
  }

  if (colDOF != NULL)
  {
    delete colDOF;
    colDOF = NULL;
  }

  if (opaDOF != NULL)
  {
    delete opaDOF;
    opaDOF = NULL;
  }

  if (textDOF != NULL)
  {
    delete textDOF;
    textDOF = NULL;
  }
}


void Shape::handleHitEdtDOFAgl(const size_t& hdlIdx)
{
  if (hdlIdx == ID_HDL_DIR)
  {
    aglDOF->setDir(-1*aglDOF->getDir());
  }
}


// -- private visualization functions -------------------------------


void Shape::drawNormal(
  const bool& inSelectMode,
  GLCanvas* canvas)
{
  if (inSelectMode == true)
  {
    if (type == TYPE_NOTE)
    {
      VisUtils::fillRect(
        -xDFC,  xDFC,
        yDFC, -yDFC);
    }
    else if (type == TYPE_RECT)
    {
      VisUtils::fillRect(
        -xDFC,  xDFC,
        yDFC, -yDFC);
    }
    else if (type == TYPE_ELLIPSE)
    {
      VisUtils::fillEllipse(
        0.0,  0.0,
        xDFC, yDFC,
        segNumHnt);
    }
    else if (type == TYPE_LINE)
    {
      VisUtils::drawLine(
        -xDFC,  xDFC,
        yDFC, -yDFC);
    }
    else if (type == TYPE_ARROW)
    {
      double pix = canvas->getPixelSize();
      VisUtils::fillArrow(
        -xDFC,       xDFC,
        yDFC,      -yDFC,
        hdlSze*pix, 2*hdlSze*pix);
    }
    else if (type == TYPE_DARROW)
    {
      double pix = canvas->getPixelSize();
      VisUtils::fillDArrow(
        -xDFC,      xDFC,
        yDFC,     -yDFC,
        hdlSze*pix, 2*hdlSze*pix);
    }
  }
  else
  {
    VisUtils::enableLineAntiAlias();
    if (type == TYPE_NOTE)
    {
      VisUtils::setColor(colFil);
      VisUtils::fillRect(
        -xDFC,  xDFC,
        yDFC, -yDFC);
      VisUtils::setColor(colLin);
      VisUtils::drawRect(
        -xDFC,  xDFC,
        yDFC, -yDFC);
    }
    else if (type == TYPE_RECT)
    {
      VisUtils::setColor(colFil);
      VisUtils::fillRect(
        -xDFC,  xDFC,
        yDFC, -yDFC);
      VisUtils::setColor(colLin);
      VisUtils::drawRect(
        -xDFC,  xDFC,
        yDFC, -yDFC);
    }
    else if (type == TYPE_ELLIPSE)
    {
      VisUtils::setColor(colFil);
      VisUtils::fillEllipse(
        0.0,  0.0,
        xDFC, yDFC,
        segNumHnt);
      VisUtils::setColor(colLin);
      VisUtils::drawEllipse(
        0.0,  0.0,
        xDFC, yDFC,
        segNumHnt);
    }
    else if (type == TYPE_LINE)
    {
      VisUtils::setColor(colLin);
      VisUtils::drawLine(
        -xDFC,  xDFC,
        yDFC, -yDFC);
    }
    else if (type == TYPE_ARROW)
    {
      double pix = canvas->getPixelSize();

      VisUtils::setColor(colFil);
      VisUtils::fillArrow(
        -xDFC,      xDFC,
        yDFC,     -yDFC,
        hdlSze*pix, 2*hdlSze*pix);
      VisUtils::setColor(colLin);
      VisUtils::drawArrow(
        -xDFC,      xDFC,
        yDFC,     -yDFC,
        hdlSze*pix, 2*hdlSze*pix);
    }
    else if (type == TYPE_DARROW)
    {
      double pix = canvas->getPixelSize();

      VisUtils::setColor(colFil);
      VisUtils::fillDArrow(
        -xDFC,      xDFC,
        yDFC,     -yDFC,
        hdlSze*pix, 2*hdlSze*pix);
      VisUtils::setColor(colLin);
      VisUtils::drawDArrow(
        -xDFC,      xDFC,
        yDFC,     -yDFC,
        hdlSze*pix, 2*hdlSze*pix);
    }
    drawText(canvas);   // Draw the textual values of the shape
    VisUtils::disableLineAntiAlias();
  }
}


void Shape::drawText(GLCanvas* canvas)
{
  string text = note;
  if (text != "")
  {
    text.append(" ");
    text.append(variable);

    double pix = canvas->getPixelSize();

    // generate textures for drawing text, if they aren't generated yet
    if (!texturesGenerated || lastCanvas != canvas)
    {
      VisUtils::genCharTextures(
        texCharId,
        texChar);
      texturesGenerated = true;
      lastCanvas = canvas;
    }

    VisUtils::setColor(colTxt);
    VisUtils::drawLabelInBoundBox(texCharId, -xDFC, xDFC, yDFC, -yDFC, szeTxt*pix/CHARHEIGHT, text);
  }
}


void Shape::drawText(GLCanvas* canvas, double pix)
{
  string text = note;
  if (!text.empty())
  {
    text.append(" ");
    text.append(variable);
    if (pix < 0.01)
    {
      pix = 0.01;
    }
    else if (pix > 0.015)
    {
      pix = 0.01;
    }

    // generate textures for drawing text, if they aren't generated yet
    if (texturesGenerated || lastCanvas != canvas)
    {
      VisUtils::genCharTextures(
        texCharId,
        texChar);
      texturesGenerated = true;
      lastCanvas = canvas;
    }

    VisUtils::setColor(colTxt);
    VisUtils::drawLabelInBoundBox(texCharId, -xDFC, xDFC, yDFC, -yDFC, szeTxt*pix/CHARHEIGHT, text);
  }
}


void Shape::drawEdit(
  const bool& inSelectMode,
  GLCanvas* canvas)
{
  double pix      = canvas->getPixelSize();
  double hdlDelta = hdlSze*pix;

  if (inSelectMode == true)
  {
    // draw shape
    drawNormal(inSelectMode, canvas);

    glPushName(ID_HDL_CTR);
    VisUtils::fillRect(-xDFC, xDFC, yDFC, -yDFC);
    glPopName();
    glPushName(ID_HDL_TOP_LFT);
    VisUtils::fillRect(-xDFC-hdlDelta, -xDFC+hdlDelta, yDFC+hdlDelta,  yDFC-hdlDelta);
    glPopName();
    glPushName(ID_HDL_LFT);
    VisUtils::fillRect(-xDFC-hdlDelta, -xDFC+hdlDelta, 0.0+hdlDelta, 0.0-hdlDelta);
    glPopName();
    glPushName(ID_HDL_BOT_LFT);
    VisUtils::fillRect(-xDFC-hdlDelta, -xDFC+hdlDelta, -yDFC+hdlDelta, -yDFC-hdlDelta);
    glPopName();
    glPushName(ID_HDL_BOT);
    VisUtils::fillRect(0.0-hdlDelta, 0.0+hdlDelta, -yDFC+hdlDelta, -yDFC-hdlDelta);
    glPopName();
    glPushName(ID_HDL_BOT_RGT);
    VisUtils::fillRect(xDFC-hdlDelta, xDFC+hdlDelta, -yDFC+hdlDelta, -yDFC-hdlDelta);
    glPopName();
    glPushName(ID_HDL_RGT);
    VisUtils::fillRect(xDFC-hdlDelta, xDFC+hdlDelta, 0.0+hdlDelta, 0.0-hdlDelta);
    glPopName();
    glPushName(ID_HDL_TOP_RGT);
    VisUtils::fillRect(xDFC-hdlDelta, xDFC+hdlDelta, yDFC+hdlDelta, yDFC-hdlDelta);
    glPopName();
    glPushName(ID_HDL_TOP);
    VisUtils::fillRect(0.0-hdlDelta, 0.0+hdlDelta, yDFC+hdlDelta, yDFC-hdlDelta);
    glPopName();
    glPushName(ID_HDL_ROT_RGT);
    if (xDFC >= 0)
    {
      VisUtils::fillEllipse(xDFC+6.0*hdlDelta, 0.0, hdlDelta, hdlDelta, 4);
    }
    else
    {
      VisUtils::fillEllipse(xDFC-6.0*hdlDelta, 0.0, hdlDelta, hdlDelta, 4);
    }
    glPopName();
    glPushName(ID_HDL_ROT_TOP);
    if (yDFC >= 0)
    {
      VisUtils::fillEllipse(0.0, yDFC+6.0*hdlDelta, hdlDelta, hdlDelta, segNumHnt);
    }
    else
    {
      VisUtils::fillEllipse(0.0, yDFC-6.0*hdlDelta, hdlDelta, hdlDelta, segNumHnt);
    }
    glPopName();
  }
  else
  {
    // draw bounding box
    VisUtils::enableLineAntiAlias();
    VisUtils::setColorMdGray();
    VisUtils::drawRect(
      -xDFC,  xDFC,
      yDFC, -yDFC);
    VisUtils::disableLineAntiAlias();

    // draw shape
    drawNormal(inSelectMode, canvas);

    // enable antialiasing
    VisUtils::enableLineAntiAlias();

    // draw top connecting line
    VisUtils::setColorMdGray();
    if (xDFC >= 0)
    {
      VisUtils::drawLine(xDFC+6.0*hdlDelta, xDFC, 0.0, 0.0);
    }
    else
    {
      VisUtils::drawLine(xDFC-6.0*hdlDelta, xDFC, 0.0, 0.0);
    }
    // draw top connecting line
    VisUtils::setColorMdGray();
    if (yDFC >= 0)
    {
      VisUtils::drawLine(0.0, 0.0, yDFC, yDFC+6.0*hdlDelta);
    }
    else
    {
      VisUtils::drawLine(0.0, 0.0, yDFC, yDFC-6.0*hdlDelta);
    }

    // draw center
    VisUtils::setColorMdGray();
    VisUtils::drawLine(-hdlDelta, hdlDelta,      0.0,       0.0);
    VisUtils::drawLine(0.0,      0.0, hdlDelta, -hdlDelta);
    // top left
    VisUtils::setColorWhite();
    VisUtils::fillRect(-xDFC-hdlDelta, -xDFC+hdlDelta, yDFC+hdlDelta,  yDFC-hdlDelta);
    VisUtils::setColorMdGray();
    VisUtils::drawRect(-xDFC-hdlDelta, -xDFC+hdlDelta, yDFC+hdlDelta,  yDFC-hdlDelta);
    // left
    VisUtils::setColorWhite();
    VisUtils::fillRect(-xDFC-hdlDelta, -xDFC+hdlDelta, 0.0+hdlDelta, 0.0-hdlDelta);
    VisUtils::setColorMdGray();
    VisUtils::drawRect(-xDFC-hdlDelta, -xDFC+hdlDelta, 0.0+hdlDelta, 0.0-hdlDelta);
    // bottom left
    VisUtils::setColorWhite();
    VisUtils::fillRect(-xDFC-hdlDelta, -xDFC+hdlDelta, -yDFC+hdlDelta, -yDFC-hdlDelta);
    VisUtils::setColorMdGray();
    VisUtils::drawRect(-xDFC-hdlDelta, -xDFC+hdlDelta, -yDFC+hdlDelta, -yDFC-hdlDelta);
    // bottom
    VisUtils::setColorWhite();
    VisUtils::fillRect(0.0-hdlDelta, 0.0+hdlDelta, -yDFC+hdlDelta, -yDFC-hdlDelta);
    VisUtils::setColorMdGray();
    VisUtils::drawRect(0.0-hdlDelta, 0.0+hdlDelta, -yDFC+hdlDelta, -yDFC-hdlDelta);
    // bottom right
    VisUtils::setColorWhite();
    VisUtils::fillRect(xDFC-hdlDelta, xDFC+hdlDelta, -yDFC+hdlDelta, -yDFC-hdlDelta);
    VisUtils::setColorMdGray();
    VisUtils::drawRect(xDFC-hdlDelta, xDFC+hdlDelta, -yDFC+hdlDelta, -yDFC-hdlDelta);
    // right
    VisUtils::setColorWhite();
    VisUtils::fillRect(xDFC-hdlDelta, xDFC+hdlDelta, 0.0+hdlDelta, 0.0-hdlDelta);
    VisUtils::setColorMdGray();
    VisUtils::drawRect(xDFC-hdlDelta, xDFC+hdlDelta, 0.0+hdlDelta, 0.0-hdlDelta);
    // top right
    VisUtils::setColorWhite();
    VisUtils::fillRect(xDFC-hdlDelta, xDFC+hdlDelta, yDFC+hdlDelta, yDFC-hdlDelta);
    VisUtils::setColorMdGray();
    VisUtils::drawRect(xDFC-hdlDelta, xDFC+hdlDelta, yDFC+hdlDelta, yDFC-hdlDelta);
    // top
    VisUtils::setColorWhite();
    VisUtils::fillRect(0.0-hdlDelta, 0.0+hdlDelta, yDFC+hdlDelta, yDFC-hdlDelta);
    VisUtils::setColorMdGray();
    VisUtils::drawRect(0.0-hdlDelta, 0.0+hdlDelta, yDFC+hdlDelta, yDFC-hdlDelta);
    // rotation hdl rgt
    if (xDFC >= 0)
    {
      VisUtils::setColorWhite();
      VisUtils::fillEllipse(xDFC+6.0*hdlDelta, 0.0, hdlDelta, hdlDelta, 4);
      VisUtils::setColorMdGray();
      VisUtils::drawEllipse(xDFC+6.0*hdlDelta, 0.0, hdlDelta, hdlDelta, 4);
    }
    else
    {
      VisUtils::setColorWhite();
      VisUtils::fillEllipse(xDFC-6.0*hdlDelta, 0.0, hdlDelta, hdlDelta, 4);
      VisUtils::setColorMdGray();
      VisUtils::drawEllipse(xDFC-6.0*hdlDelta, 0.0, hdlDelta, hdlDelta, 4);
    }
    // rotation hdl top
    if (yDFC >= 0)
    {
      VisUtils::setColorWhite();
      VisUtils::fillEllipse(0.0, yDFC+6.0*hdlDelta, hdlDelta, hdlDelta, segNumHnt);
      VisUtils::setColorMdGray();
      VisUtils::drawEllipse(0.0, yDFC+6.0*hdlDelta, hdlDelta, hdlDelta, segNumHnt);
    }
    else
    {
      VisUtils::setColorWhite();
      VisUtils::fillEllipse(0.0, yDFC-6.0*hdlDelta, hdlDelta, hdlDelta, segNumHnt);
      VisUtils::setColorMdGray();
      VisUtils::drawEllipse(0.0, yDFC-6.0*hdlDelta, hdlDelta, hdlDelta, segNumHnt);
    }

    // disable antialiasing
    VisUtils::disableLineAntiAlias();
  }
}


void Shape::drawEditDOF(
  const bool& inSelectMode,
  GLCanvas* canvas)
{
  if (inSelectMode == true)
  {
    // draw shape
    drawNormal(inSelectMode, canvas);

    if (mode == MODE_EDT_DOF_XCTR)
    {
      drawDOFYCtr(inSelectMode, canvas);
      drawDOFWth(inSelectMode, canvas);
      drawDOFHgt(inSelectMode, canvas);
      drawDOFAgl(inSelectMode, canvas);

      drawEditDOFXCtr(inSelectMode, canvas);
    }
    else if (mode == MODE_EDT_DOF_YCTR)
    {
      drawDOFXCtr(inSelectMode, canvas);
      drawDOFWth(inSelectMode, canvas);
      drawDOFHgt(inSelectMode, canvas);
      drawDOFAgl(inSelectMode, canvas);

      drawEditDOFYCtr(inSelectMode, canvas);
    }
    else if (mode == MODE_EDT_DOF_WTH)
    {
      drawDOFXCtr(inSelectMode, canvas);
      drawDOFYCtr(inSelectMode, canvas);
      drawDOFHgt(inSelectMode, canvas);
      drawDOFAgl(inSelectMode, canvas);

      drawEditDOFWth(inSelectMode, canvas);
    }
    else if (mode == MODE_EDT_DOF_HGT)
    {
      drawDOFXCtr(inSelectMode, canvas);
      drawDOFYCtr(inSelectMode, canvas);
      drawDOFWth(inSelectMode, canvas);
      drawDOFAgl(inSelectMode, canvas);

      drawEditDOFHgt(inSelectMode, canvas);
    }
    else if (mode == MODE_EDT_DOF_AGL)
    {
      drawDOFXCtr(inSelectMode, canvas);
      drawDOFYCtr(inSelectMode, canvas);
      drawDOFWth(inSelectMode, canvas);
      drawDOFHgt(inSelectMode, canvas);

      drawEditDOFAgl(inSelectMode, canvas);
    }
    else if (mode == MODE_EDT_DOF_COL)
    {
      drawDOFXCtr(inSelectMode, canvas);
      drawDOFYCtr(inSelectMode, canvas);
      drawDOFWth(inSelectMode, canvas);
      drawDOFHgt(inSelectMode, canvas);
      drawDOFAgl(inSelectMode, canvas);
    }
    else if (mode == MODE_EDT_DOF_OPA)
    {
      drawDOFXCtr(inSelectMode, canvas);
      drawDOFYCtr(inSelectMode, canvas);
      drawDOFWth(inSelectMode, canvas);
      drawDOFHgt(inSelectMode, canvas);
      drawDOFAgl(inSelectMode, canvas);
    }
    else if (mode == MODE_EDT_DOF_TEXT)
    {
      drawDOFXCtr(inSelectMode, canvas);
      drawDOFYCtr(inSelectMode, canvas);
      drawDOFWth(inSelectMode, canvas);
      drawDOFHgt(inSelectMode, canvas);
      drawDOFAgl(inSelectMode, canvas);
    }
  }
  else
  {
    // draw bounding box
    VisUtils::enableLineAntiAlias();
    VisUtils::setColorMdGray();
    VisUtils::drawRect(
      -xDFC,  xDFC,
      yDFC, -yDFC);
    VisUtils::disableLineAntiAlias();

    // draw shape
    drawNormal(inSelectMode, canvas);

    VisUtils::enableLineAntiAlias();
    if (mode == MODE_EDT_DOF_XCTR)
    {
      drawDOFYCtr(inSelectMode, canvas);
      drawDOFWth(inSelectMode, canvas);
      drawDOFHgt(inSelectMode, canvas);
      drawDOFAgl(inSelectMode, canvas);

      drawEditDOFXCtr(inSelectMode, canvas);
    }
    else if (mode == MODE_EDT_DOF_YCTR)
    {
      drawDOFXCtr(inSelectMode, canvas);
      drawDOFWth(inSelectMode, canvas);
      drawDOFHgt(inSelectMode, canvas);
      drawDOFAgl(inSelectMode, canvas);

      drawEditDOFYCtr(inSelectMode, canvas);
    }
    else if (mode == MODE_EDT_DOF_WTH)
    {
      drawDOFXCtr(inSelectMode, canvas);
      drawDOFYCtr(inSelectMode, canvas);
      drawDOFHgt(inSelectMode, canvas);
      drawDOFAgl(inSelectMode, canvas);

      drawEditDOFWth(inSelectMode, canvas);
    }
    else if (mode == MODE_EDT_DOF_HGT)
    {
      drawDOFXCtr(inSelectMode, canvas);
      drawDOFYCtr(inSelectMode, canvas);
      drawDOFWth(inSelectMode, canvas);
      drawDOFAgl(inSelectMode, canvas);

      drawEditDOFHgt(inSelectMode, canvas);
    }
    else if (mode == MODE_EDT_DOF_AGL)
    {
      drawDOFXCtr(inSelectMode, canvas);
      drawDOFYCtr(inSelectMode, canvas);
      drawDOFWth(inSelectMode, canvas);
      drawDOFHgt(inSelectMode, canvas);

      drawEditDOFAgl(inSelectMode, canvas);
    }
    else if (mode == MODE_EDT_DOF_COL)
    {
      drawDOFXCtr(inSelectMode, canvas);
      drawDOFYCtr(inSelectMode, canvas);
      drawDOFWth(inSelectMode, canvas);
      drawDOFHgt(inSelectMode, canvas);
      drawDOFAgl(inSelectMode, canvas);
    }
    else if (mode == MODE_EDT_DOF_OPA)
    {
      drawDOFXCtr(inSelectMode, canvas);
      drawDOFYCtr(inSelectMode, canvas);
      drawDOFWth(inSelectMode, canvas);
      drawDOFHgt(inSelectMode, canvas);
      drawDOFAgl(inSelectMode, canvas);
    }
    else if (mode == MODE_EDT_DOF_TEXT)
    {
      drawDOFXCtr(inSelectMode, canvas);
      drawDOFYCtr(inSelectMode, canvas);
      drawDOFWth(inSelectMode, canvas);
      drawDOFHgt(inSelectMode, canvas);
      drawDOFAgl(inSelectMode, canvas);
    }
    VisUtils::disableLineAntiAlias();
  }
}


void Shape::drawDOFXCtr(
  const bool& inSelectMode,
  GLCanvas* canvas)
{
  double pix    = canvas->getPixelSize();
  double hdlDOF = hdlSze*pix;
  double xBeg   = xCtrDOF->getMin();
  double xEnd   = xCtrDOF->getMax();

  glPushMatrix();
  glRotatef(-aglCtr, 0.0, 0.0, 1.0);

  if (inSelectMode == true)
    {}
  else
  {
    /*
    VisUtils::setColorMdGray();
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
      VisUtils::setColorMdGray();
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
      VisUtils::setColorWhite();
      VisUtils::fillRect(
          xBeg-1.5*hdlDOF,  xBeg,
          hdlDOF,          -hdlDOF );
      VisUtils::setColorMdGray();
      VisUtils::drawRect(
          xBeg-1.5*hdlDOF,  xBeg,
          hdlDOF,          -hdlDOF );
      VisUtils::drawLine(
          xBeg,        xBeg,
          2.0*hdlDOF, -2.0*hdlDOF );
      */

      // stop
      VisUtils::setColorWhite();
      VisUtils::fillTriangle(
        xEnd,             0.0,
        xEnd+2.0*hdlDOF, -hdlDOF,
        xEnd+2.0*hdlDOF,  hdlDOF);
      VisUtils::setColorMdGray();
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
      VisUtils::setColorMdGray();
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
      VisUtils::setColorWhite();
      VisUtils::fillRect(
          xBeg,    xBeg+1.5*hdlDOF,
          hdlDOF, -hdlDOF );
      VisUtils::setColorMdGray();
      VisUtils::drawRect(
          xBeg,    xBeg+1.5*hdlDOF,
          hdlDOF, -hdlDOF );
      VisUtils::drawLine(
          xBeg,        xBeg,
          2.0*hdlDOF, -2.0*hdlDOF );
      */

      // stop
      VisUtils::setColorWhite();
      VisUtils::fillTriangle(
        xEnd-2.0*hdlDOF,  hdlDOF,
        xEnd-2.0*hdlDOF, -hdlDOF,
        xEnd,             0.0);
      VisUtils::setColorMdGray();
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
  GLCanvas* canvas)
{
  double pix    = canvas->getPixelSize();
  double hdlDOF = hdlSze*pix;
  double xBeg   = xCtrDOF->getMin();
  double xEnd   = xCtrDOF->getMax();

  glPushMatrix();
  glRotatef(-aglCtr, 0.0, 0.0, 1.0);

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
    VisUtils::setColorMdGray();
    VisUtils::drawLine(
         0.0,     0.0,
         hdlDOF, -hdlDOF );
    VisUtils::drawLine(
        -hdlDOF,  hdlDOF,
         0.0,     0.0 );
    */

    // draw starting cross
    VisUtils::setColorRed();
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
      VisUtils::setColorRed();
      VisUtils::drawLineDashed(
        xBeg, xEnd,
        0.0,  0.0);
      /*
      // start
      VisUtils::setColorRed();
      VisUtils::fillRect(
          xBeg-1.5*hdlDOF,  xBeg,
          hdlDOF,          -hdlDOF );
      VisUtils::setColorMdGray();
      VisUtils::drawRect(
          xBeg-1.5*hdlDOF,  xBeg,
          hdlDOF,          -hdlDOF );
      VisUtils::drawLine(
          xBeg,        xBeg,
          2.0*hdlDOF, -2.0*hdlDOF );
      */

      // stop
      VisUtils::setColorGreen();
      VisUtils::fillTriangle(
        xEnd,             0.0,
        xEnd+2.0*hdlDOF, -hdlDOF,
        xEnd+2.0*hdlDOF,  hdlDOF);
      VisUtils::setColorMdGray();
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
      VisUtils::setColorRed();
      VisUtils::drawLineDashed(
        xBeg, xEnd,
        0.0,  0.0);
      /*
      // start
      VisUtils::setColorGreen();
      VisUtils::fillRect(
          xBeg,    xBeg+1.5*hdlDOF,
          hdlDOF, -hdlDOF );
      VisUtils::setColorMdGray();
      VisUtils::drawRect(
          xBeg,    xBeg+1.5*hdlDOF,
          hdlDOF, -hdlDOF );
      VisUtils::setColorRed();
      VisUtils::drawLine(
          xBeg,        xBeg,
          2.0*hdlDOF, -2.0*hdlDOF );
      */

      // stop
      VisUtils::setColorGreen();
      VisUtils::fillTriangle(
        xEnd-2.0*hdlDOF,  hdlDOF,
        xEnd-2.0*hdlDOF, -hdlDOF,
        xEnd,             0.0);
      VisUtils::setColorMdGray();
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
  GLCanvas* canvas)
{
  double pix    = canvas->getPixelSize();
  double hdlDOF = hdlSze*pix;
  double yBeg   = yCtrDOF->getMin();
  double yEnd   = yCtrDOF->getMax();

  glPushMatrix();
  glRotatef(-aglCtr, 0.0, 0.0, 1.0);

  if (inSelectMode == true)
    {}
  else
  {
    /*
    // draw central handle
    VisUtils::setColorMdGray();
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
      VisUtils::setColorWhite();
      VisUtils::fillRect(
          -hdlDOF, hdlDOF,
           yBeg,   yBeg-1.5*hdlDOF );
      VisUtils::setColorMdGray();
      VisUtils::drawRect(
          -hdlDOF, hdlDOF,
           yBeg,   yBeg-1.5*hdlDOF );
      VisUtils::drawLine(
          -2.0*hdlDOF, 2.0*hdlDOF,
           yBeg,       yBeg );
      */

      // stop
      VisUtils::setColorWhite();
      VisUtils::fillTriangle(
        -hdlDOF, yEnd+2.0*hdlDOF,
        0.0,    yEnd,
        hdlDOF, yEnd+2.0*hdlDOF);
      VisUtils::setColorMdGray();
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
      VisUtils::setColorWhite();
      VisUtils::fillRect(
          hdlDOF,          -hdlDOF,
          yBeg+1.5*hdlDOF,  yBeg );
      VisUtils::setColorMdGray();
      VisUtils::drawRect(
          hdlDOF,          -hdlDOF,
          yBeg+1.5*hdlDOF,  yBeg );
      VisUtils::drawLine(
          2.0*hdlDOF, -2.0*hdlDOF,
          yBeg,        yBeg );
      */

      // stop
      VisUtils::setColorWhite();
      VisUtils::fillTriangle(
        0.0,    yEnd,
        -hdlDOF, yEnd-2.0*hdlDOF,
        hdlDOF, yEnd-2.0*hdlDOF);
      VisUtils::setColorMdGray();
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
  GLCanvas* canvas)
{
  double pix    = canvas->getPixelSize();
  double hdlDOF = hdlSze*pix;
  double yBeg   = yCtrDOF->getMin();
  double yEnd   = yCtrDOF->getMax();

  glPushMatrix();
  glRotatef(-aglCtr, 0.0, 0.0, 1.0);

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
    VisUtils::setColorRed();
    VisUtils::drawLine(
         0.0,     0.0,
         hdlDOF, -hdlDOF );
    VisUtils::drawLine(
        -hdlDOF,  hdlDOF,
         0.0,     0.0 );
    */

    // draw starting cross
    VisUtils::setColorRed();
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
      VisUtils::setColorRed();
      VisUtils::drawLineDashed(
        0.0,  0.0,
        yBeg, yEnd);

      /*
      // start
      VisUtils::setColorGreen();
      VisUtils::fillRect(
          -hdlDOF, hdlDOF,
           yBeg,   yBeg-1.5*hdlDOF );
      VisUtils::setColorMdGray();
      VisUtils::drawRect(
          -hdlDOF, hdlDOF,
           yBeg,   yBeg-1.5*hdlDOF );
      VisUtils::drawLine(
          -2.0*hdlDOF, 2.0*hdlDOF,
           yBeg,       yBeg );
      */

      // stop
      VisUtils::setColorGreen();
      VisUtils::fillTriangle(
        -hdlDOF, yEnd+2.0*hdlDOF,
        0.0,    yEnd,
        hdlDOF, yEnd+2.0*hdlDOF);
      VisUtils::setColorMdGray();
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
      VisUtils::setColorRed();
      VisUtils::drawLineDashed(
        0.0,  0.0,
        yBeg, yEnd);

      /*
      // start
      VisUtils::setColorGreen();
      VisUtils::fillRect(
          hdlDOF,          -hdlDOF,
          yBeg+1.5*hdlDOF,  yBeg );
      VisUtils::setColorMdGray();
      VisUtils::drawRect(
          hdlDOF,          -hdlDOF,
          yBeg+1.5*hdlDOF,  yBeg );
      VisUtils::drawLine(
          2.0*hdlDOF, -2.0*hdlDOF,
          yBeg,        yBeg );
      */

      // stop
      VisUtils::setColorGreen();
      VisUtils::fillTriangle(
        0.0,    yEnd,
        -hdlDOF, yEnd-2.0*hdlDOF,
        hdlDOF, yEnd-2.0*hdlDOF);
      VisUtils::setColorMdGray();
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
  GLCanvas* canvas)
{
  double pix    = canvas->getPixelSize();
  double hdlDOF = hdlSze*pix;
  double wBeg   = wthDOF->getMin();
  double wEnd   = wthDOF->getMax();

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
        xDFC+wEnd-2.0*hdlDOF,  hdlDOF,
        xDFC+wEnd-2.0*hdlDOF, -hdlDOF,
        xDFC+wEnd,             0.0);
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
        xDFC+wEnd,             0.0,
        xDFC+wEnd+2.0*hdlDOF, -hdlDOF,
        xDFC+wEnd+2.0*hdlDOF,  hdlDOF);
      glPopName();
    }
  }
  else
  {
    // draw center of shape
    VisUtils::setColorMdGray();
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
      VisUtils::setColorRed();
      VisUtils::drawLine(
        xDFC-hdlDOF,  xDFC+hdlDOF,
        hdlDOF,      -hdlDOF);
      VisUtils::drawLine(
        xDFC-hdlDOF,  xDFC+hdlDOF,
        -hdlDOF,       hdlDOF);

      // draw dashed connector to cur width
      if (xDFC < xDFC+wBeg)
        VisUtils::drawLineDashed(
          xDFC, xDFC+wBeg,
          0.0,  0.0);
      else if (wEnd+xDFC < xDFC)
        VisUtils::drawLineDashed(
          xDFC, xDFC+wEnd,
          0.0,  0.0);

      // draw connector between handles
      VisUtils::setColorRed();
      VisUtils::drawLineDashed(
        xDFC+wBeg, xDFC+wEnd,
        0.0,       0.0);

      /*
      // start
      VisUtils::setColorGreen();
      VisUtils::fillEllipse(
          xDFC+wBeg+hdlDOF, 0.0,
          hdlDOF,           hdlDOF,
          4 );
      VisUtils::setColorMdGray();
      VisUtils::drawEllipse(
          xDFC+wBeg+hdlDOF, 0.0,
          hdlDOF,           hdlDOF,
          4 );
      VisUtils::drawLine(
          xDFC+wBeg,   xDFC+wBeg,
          2.0*hdlDOF, -2.0*hdlDOF );
      */

      // stop
      VisUtils::setColorGreen();
      VisUtils::fillTriangle(
        xDFC+wEnd-2.0*hdlDOF,  hdlDOF,
        xDFC+wEnd-2.0*hdlDOF, -hdlDOF,
        xDFC+wEnd,             0.0);
      VisUtils::setColorMdGray();
      VisUtils::drawTriangle(
        xDFC+wEnd-2.0*hdlDOF,  hdlDOF,
        xDFC+wEnd-2.0*hdlDOF, -hdlDOF,
        xDFC+wEnd,             0.0);
      VisUtils::drawLine(
        xDFC+wEnd,   xDFC+wEnd,
        2.0*hdlDOF, -2.0*hdlDOF);
    }
    // pointing left
    else // ( ( wBeg > wEnd ) )
    {
      // draw cur width
      VisUtils::setColorRed();
      VisUtils::drawLine(
        xDFC-hdlDOF,  xDFC+hdlDOF,
        hdlDOF,      -hdlDOF);
      VisUtils::drawLine(
        xDFC-hdlDOF,  xDFC+hdlDOF,
        -hdlDOF,       hdlDOF);

      // draw dashed connector to cur width
      if (wBeg+xDFC < xDFC)
        VisUtils::drawLineDashed(
          xDFC, xDFC+wBeg,
          0.0,  0.0);
      else if (wEnd+xDFC > xDFC)
        VisUtils::drawLineDashed(
          xDFC, xDFC+wEnd,
          0.0,  0.0);

      // draw connector between handles
      VisUtils::setColorRed();
      VisUtils::drawLineDashed(
        xDFC+wBeg, xDFC+wEnd,
        0.0,       0.0);

      /*
      // start
      VisUtils::setColorGreen();
      VisUtils::fillEllipse(
          xDFC+wBeg-hdlDOF, 0.0,
          hdlDOF,           hdlDOF,
          4 );
      VisUtils::setColorMdGray();
      VisUtils::drawEllipse(
          xDFC+wBeg-hdlDOF, 0.0,
          hdlDOF,           hdlDOF,
          4 );
      VisUtils::drawLine(
          xDFC+wBeg,   xDFC+wBeg,
          2.0*hdlDOF, -2.0*hdlDOF );
      */

      // stop
      VisUtils::setColorGreen();
      VisUtils::fillTriangle(
        xDFC+wEnd,             0.0,
        xDFC+wEnd+2.0*hdlDOF, -hdlDOF,
        xDFC+wEnd+2.0*hdlDOF,  hdlDOF);
      VisUtils::setColorMdGray();
      VisUtils::drawTriangle(
        xDFC+wEnd,             0.0,
        xDFC+wEnd+2.0*hdlDOF, -hdlDOF,
        xDFC+wEnd+2.0*hdlDOF,  hdlDOF);
      VisUtils::drawLine(
        xDFC+wEnd,   xDFC+wEnd,
        2.0*hdlDOF, -2.0*hdlDOF);
    }
  }
}


void Shape::drawDOFWth(
  const bool& inSelectMode,
  GLCanvas* canvas)
{
  double pix    = canvas->getPixelSize();
  double hdlDOF = hdlSze*pix;
  double wBeg   = wthDOF->getMin();
  double wEnd   = wthDOF->getMax();

  if (inSelectMode == true)
    {}
  else
  {
    /*
    // draw center of shape
    VisUtils::setColorMdGray();
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
      VisUtils::setColorMdGray();
      VisUtils::drawLine(
        xDFC-hdlDOF,  xDFC+hdlDOF,
        hdlDOF,      -hdlDOF);
      VisUtils::drawLine(
        xDFC-hdlDOF,  xDFC+hdlDOF,
        -hdlDOF,       hdlDOF);

      // draw dashed connector to cur width
      if (xDFC < xDFC+wBeg)
        VisUtils::drawLineDashed(
          xDFC, xDFC+wBeg,
          0.0,  0.0);
      else if (wEnd+xDFC < xDFC)
        VisUtils::drawLineDashed(
          xDFC, xDFC+wEnd,
          0.0,  0.0);
      // draw connector between handles
      VisUtils::drawLineDashed(
        xDFC+wBeg, xDFC+wEnd,
        0.0,       0.0);

      /*
      // start
      VisUtils::setColorWhite();
      VisUtils::fillEllipse(
          xDFC+wBeg+hdlDOF, 0.0,
          hdlDOF,           hdlDOF,
          4 );
      VisUtils::setColorMdGray();
      VisUtils::drawEllipse(
          xDFC+wBeg+hdlDOF, 0.0,
          hdlDOF,           hdlDOF,
          4 );
      VisUtils::drawLine(
          xDFC+wBeg,   xDFC+wBeg,
          2.0*hdlDOF, -2.0*hdlDOF );
      */

      // stop
      VisUtils::setColorWhite();
      VisUtils::fillTriangle(
        xDFC+wEnd-2.0*hdlDOF,  hdlDOF,
        xDFC+wEnd-2.0*hdlDOF, -hdlDOF,
        xDFC+wEnd,             0.0);
      VisUtils::setColorMdGray();
      VisUtils::drawTriangle(
        xDFC+wEnd-2.0*hdlDOF,  hdlDOF,
        xDFC+wEnd-2.0*hdlDOF, -hdlDOF,
        xDFC+wEnd,             0.0);
      VisUtils::drawLine(
        xDFC+wEnd,   xDFC+wEnd,
        2.0*hdlDOF, -2.0*hdlDOF);
    }
    // pointing left
    else // ( ( wBeg > wEnd ) )
    {
      // draw cur width
      VisUtils::setColorMdGray();
      VisUtils::drawLine(
        xDFC-hdlDOF,  xDFC+hdlDOF,
        hdlDOF,      -hdlDOF);
      VisUtils::drawLine(
        xDFC-hdlDOF,  xDFC+hdlDOF,
        -hdlDOF,       hdlDOF);

      // draw dashed connector to cur width
      if (wBeg+xDFC < xDFC)
        VisUtils::drawLineDashed(
          xDFC, xDFC+wBeg,
          0.0,  0.0);
      else if (wEnd+xDFC > xDFC)
        VisUtils::drawLineDashed(
          xDFC, xDFC+wEnd,
          0.0,  0.0);
      // draw connector between handles
      VisUtils::drawLineDashed(
        xDFC+wBeg, xDFC+wEnd,
        0.0,       0.0);

      /*
      // start
      VisUtils::setColorWhite();
      VisUtils::drawEllipse(
          xDFC+wBeg-hdlDOF,  0.0,
          hdlDOF,            hdlDOF,
          4 );
      VisUtils::setColorMdGray();
      VisUtils::drawEllipse(
          xDFC+wBeg-hdlDOF, 0.0,
          hdlDOF,           hdlDOF,
          4 );
      VisUtils::drawLine(
          xDFC+wBeg,   xDFC+wBeg,
          2.0*hdlDOF, -2.0*hdlDOF );
      */

      // stop
      VisUtils::setColorWhite();
      VisUtils::fillTriangle(
        xDFC+wEnd,             0.0,
        xDFC+wEnd+2.0*hdlDOF, -hdlDOF,
        xDFC+wEnd+2.0*hdlDOF,  hdlDOF);
      VisUtils::setColorMdGray();
      VisUtils::drawTriangle(
        xDFC+wEnd,             0.0,
        xDFC+wEnd+2.0*hdlDOF, -hdlDOF,
        xDFC+wEnd+2.0*hdlDOF,  hdlDOF);
      VisUtils::drawLine(
        xDFC+wEnd,   xDFC+wEnd,
        2.0*hdlDOF, -2.0*hdlDOF);
    }
  }
}


void Shape::drawEditDOFHgt(
  const bool& inSelectMode,
  GLCanvas* canvas)
{
  double pix    = canvas->getPixelSize();
  double hdlDOF = hdlSze*pix;
  double hBeg   = hgtDOF->getMin();
  double hEnd   = hgtDOF->getMax();

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
        0.0,    yDFC+hEnd,
        -hdlDOF, yDFC+hEnd-2.0*hdlDOF,
        hdlDOF, yDFC+hEnd-2.0*hdlDOF);
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
        -hdlDOF, yDFC+hEnd+2.0*hdlDOF,
        0.0,    yDFC+hEnd,
        hdlDOF, yDFC+hEnd+2.0*hdlDOF);
      glPopName();
    }
  }
  else
  {
    // draw center of shape
    VisUtils::setColorMdGray();
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
      VisUtils::setColorRed();
      VisUtils::drawLine(
        hdlDOF,      -hdlDOF,
        yDFC+hdlDOF,  yDFC-hdlDOF);
      VisUtils::drawLine(
        hdlDOF,      -hdlDOF,
        yDFC-hdlDOF,  yDFC+hdlDOF);

      // draw dashed connector to cur height
      if (yDFC < yDFC+hBeg)
        VisUtils::drawLineDashed(
          0.0,  0.0,
          yDFC, yDFC+hBeg);
      else if (hEnd+yDFC < yDFC)
        VisUtils::drawLineDashed(
          0.0,  0.0,
          yDFC, yDFC+hEnd);

      // draw connector between handles
      VisUtils::setColorRed();
      VisUtils::drawLineDashed(
        0.0,       0.0,
        yDFC+hBeg, yDFC+hEnd);

      /*
      // start
      VisUtils::setColorGreen();
      VisUtils::fillEllipse(
          0.0,    yDFC+hBeg+hdlDOF,
          hdlDOF, hdlDOF,
          segNumHnt );
      VisUtils::setColorMdGray();
      VisUtils::drawEllipse(
          0.0,    yDFC+hBeg+hdlDOF,
          hdlDOF, hdlDOF,
          segNumHnt );
      VisUtils::drawLine(
          -2.0*hdlDOF, 2.0*hdlDOF,
          yDFC+hBeg,   yDFC+hBeg );
      */

      // stop
      VisUtils::setColorGreen();
      VisUtils::fillTriangle(
        0.0,    yDFC+hEnd,
        -hdlDOF, yDFC+hEnd-2.0*hdlDOF,
        hdlDOF, yDFC+hEnd-2.0*hdlDOF);
      VisUtils::setColorMdGray();
      VisUtils::drawTriangle(
        0.0,    yDFC+hEnd,
        -hdlDOF, yDFC+hEnd-2.0*hdlDOF,
        hdlDOF, yDFC+hEnd-2.0*hdlDOF);
      VisUtils::drawLine(
        2.0*hdlDOF, -2.0*hdlDOF,
        yDFC+hEnd,   yDFC+hEnd);
    }
    // pointing down
    else // ( ( wBeg > wEnd ) )
    {
      // draw cur height
      VisUtils::setColorRed();
      VisUtils::drawLine(
        hdlDOF,      -hdlDOF,
        yDFC+hdlDOF,  yDFC-hdlDOF);
      VisUtils::drawLine(
        hdlDOF,      -hdlDOF,
        yDFC-hdlDOF,  yDFC+hdlDOF);

      // draw dashed connector to cur width
      if (hBeg+yDFC < yDFC)
        VisUtils::drawLineDashed(
          0.0,  0.0,
          yDFC, yDFC+hBeg);
      else if (hEnd+yDFC > yDFC)
        VisUtils::drawLineDashed(
          0.0,  0.0,
          yDFC, yDFC+hEnd);

      // draw connector between handles
      VisUtils::setColorRed();
      VisUtils::drawLineDashed(
        0.0,       0.0,
        yDFC+hBeg, yDFC+hEnd);

      /*
      // start
      VisUtils::setColorGreen();
      VisUtils::fillEllipse(
          0.0,    yDFC+hBeg-hdlDOF,
          hdlDOF, hdlDOF,
          segNumHnt );
      VisUtils::setColorMdGray();
      VisUtils::drawEllipse(
          0.0,    yDFC+hBeg-hdlDOF,
          hdlDOF, hdlDOF,
          segNumHnt );
      VisUtils::drawLine(
          -2.0*hdlDOF, 2.0*hdlDOF,
          yDFC+hBeg,   yDFC+hBeg );
      */

      // stop
      VisUtils::setColorGreen();
      VisUtils::fillTriangle(
        -hdlDOF, yDFC+hEnd+2.0*hdlDOF,
        0.0,    yDFC+hEnd,
        hdlDOF, yDFC+hEnd+2.0*hdlDOF);
      VisUtils::setColorMdGray();
      VisUtils::drawTriangle(
        -hdlDOF, yDFC+hEnd+2.0*hdlDOF,
        0.0,    yDFC+hEnd,
        hdlDOF, yDFC+hEnd+2.0*hdlDOF);
      VisUtils::drawLine(
        2.0*hdlDOF, -2.0*hdlDOF,
        yDFC+hEnd,   yDFC+hEnd);
    }
  }
}


void Shape::drawDOFHgt(
  const bool& inSelectMode,
  GLCanvas* canvas)
{
  double pix    = canvas->getPixelSize();
  double hdlDOF = hdlSze*pix;
  double hBeg   = hgtDOF->getMin();
  double hEnd   = hgtDOF->getMax();

  if (inSelectMode == true)
    {}
  else
  {
    /*
    // draw center of shape
    VisUtils::setColorMdGray();
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
      VisUtils::setColorMdGray();
      VisUtils::drawLine(
        hdlDOF,      -hdlDOF,
        yDFC+hdlDOF,  yDFC-hdlDOF);
      VisUtils::drawLine(
        hdlDOF,      -hdlDOF,
        yDFC-hdlDOF,  yDFC+hdlDOF);

      // draw dashed connector to cur height
      if (yDFC < yDFC+hBeg)
        VisUtils::drawLineDashed(
          0.0,  0.0,
          yDFC, yDFC+hBeg);
      else if (hEnd+yDFC < yDFC)
        VisUtils::drawLineDashed(
          0.0,  0.0,
          yDFC, yDFC+hEnd);
      // draw connector between handles
      VisUtils::drawLineDashed(
        0.0,       0.0,
        yDFC+hBeg, yDFC+hEnd);

      /*
      // start
      VisUtils::setColorWhite();
      VisUtils::fillEllipse(
          0.0,    yDFC+hBeg+hdlDOF,
          hdlDOF, hdlDOF,
          segNumHnt );
      VisUtils::setColorMdGray();
      VisUtils::drawEllipse(
          0.0,    yDFC+hBeg+hdlDOF,
          hdlDOF, hdlDOF,
          segNumHnt );
      VisUtils::drawLine(
          -2.0*hdlDOF, 2.0*hdlDOF,
          yDFC+hBeg,   yDFC+hBeg );
      */

      // stop
      VisUtils::setColorWhite();
      VisUtils::fillTriangle(
        0.0,    yDFC+hEnd,
        -hdlDOF, yDFC+hEnd-2.0*hdlDOF,
        hdlDOF, yDFC+hEnd-2.0*hdlDOF);
      VisUtils::setColorMdGray();
      VisUtils::drawTriangle(
        0.0,    yDFC+hEnd,
        -hdlDOF, yDFC+hEnd-2.0*hdlDOF,
        hdlDOF, yDFC+hEnd-2.0*hdlDOF);
      VisUtils::drawLine(
        2.0*hdlDOF, -2.0*hdlDOF,
        yDFC+hEnd,   yDFC+hEnd);
    }
    // pointing down
    else // ( ( wBeg > wEnd ) )
    {
      // draw cur height
      VisUtils::setColorMdGray();
      VisUtils::drawLine(
        hdlDOF,      -hdlDOF,
        yDFC+hdlDOF,  yDFC-hdlDOF);
      VisUtils::drawLine(
        hdlDOF,      -hdlDOF,
        yDFC-hdlDOF,  yDFC+hdlDOF);

      // draw dashed connector to cur width
      if (hBeg+yDFC < yDFC)
        VisUtils::drawLineDashed(
          0.0,  0.0,
          yDFC, yDFC+hBeg);
      else if (hEnd+yDFC > yDFC)
        VisUtils::drawLineDashed(
          0.0,  0.0,
          yDFC, yDFC+hEnd);
      // draw connector between handles
      VisUtils::drawLineDashed(
        0.0,       0.0,
        yDFC+hBeg, yDFC+hEnd);

      /*
      // start
      VisUtils::setColorWhite();
      VisUtils::fillEllipse(
          0.0,    yDFC+hBeg-hdlDOF,
          hdlDOF, hdlDOF,
          segNumHnt );
      VisUtils::setColorMdGray();
      VisUtils::drawEllipse(
          0.0,    yDFC+hBeg-hdlDOF,
          hdlDOF, hdlDOF,
          segNumHnt );
      VisUtils::drawLine(
          -2.0*hdlDOF, 2.0*hdlDOF,
          yDFC+hBeg,   yDFC+hBeg );
      */

      // stop
      VisUtils::setColorWhite();
      VisUtils::fillTriangle(
        -hdlDOF, yDFC+hEnd+2.0*hdlDOF,
        0.0,    yDFC+hEnd,
        hdlDOF, yDFC+hEnd+2.0*hdlDOF);
      VisUtils::setColorMdGray();
      VisUtils::drawTriangle(
        -hdlDOF, yDFC+hEnd+2.0*hdlDOF,
        0.0,    yDFC+hEnd,
        hdlDOF, yDFC+hEnd+2.0*hdlDOF);
      VisUtils::drawLine(
        2.0*hdlDOF, -2.0*hdlDOF,
        yDFC+hEnd,   yDFC+hEnd);
    }
  }
}


void Shape::drawEditDOFAgl(
  const bool& inSelectMode,
  GLCanvas* canvas)
{
  double pix    = canvas->getPixelSize();
  double hdlDOF = hdlSze*pix;

  glPushMatrix();
  glRotatef(-aglCtr, 0.0, 0.0, 1.0);

  if (inSelectMode == true)
  {
    double dstHgeCtr, dstHgeHdl;
    double aglRef, aglBeg, aglEnd;
    double xBeg, yBeg;
    double xEnd, yEnd;

    // distance from hinge to center
    dstHgeCtr = Utils::dist(xCtr+xHge, yCtr+yHge, xCtr, yCtr);
    // distance from center to handles
    dstHgeHdl = dstHgeCtr + 0.5*Utils::maxx(Utils::abs(xDFC), Utils::abs(yDFC));

    if (dstHgeCtr == 0)
    {
      aglRef = 0;
    }
    else
    {
      aglRef = Utils::calcAngleDg(-xHge, -yHge);
    }
    aglBeg = aglRef + aglDOF->getMin();
    aglEnd = aglRef + aglDOF->getMax();
    // x & y coords at aglBeg, dstHgeCtr+dstCtrHdl from hinge
    xBeg = xHge+dstHgeHdl*cos(Utils::degrToRad(aglBeg));
    yBeg = yHge+dstHgeHdl*sin(Utils::degrToRad(aglBeg));
    // x & y coords at aglEnd, dstHgeCtr+dstCtrHdl from hinge
    xEnd = xHge+dstHgeHdl*cos(Utils::degrToRad(aglEnd));
    yEnd = yHge+dstHgeHdl*sin(Utils::degrToRad(aglEnd));

    glPushName(ID_HDL_HGE);
    VisUtils::fillEllipse(
      xHge,   yHge,
      hdlDOF, hdlDOF,
      segNumHnt);
    glPopName();

    // counter clockwise rotation
    if (aglDOF->getDir() > 0)
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
    dstHgeCtr = Utils::dist(xCtr+xHge, yCtr+yHge, xCtr, yCtr);
    // distance from center to handles
    dstHgeHdl = dstHgeCtr + 0.5*Utils::maxx(Utils::abs(xDFC), Utils::abs(yDFC));

    if (dstHgeCtr == 0)
    {
      aglRef = 0;
    }
    else
    {
      aglRef = Utils::calcAngleDg(-xHge, -yHge);
    }
    aglBeg = aglRef + aglDOF->getMin();
    aglEnd = aglRef + aglDOF->getMax();
    // x & y coords at aglBeg, dstHgeCtr+dstCtrHdl from hinge
    xBeg = xHge+dstHgeHdl*cos(Utils::degrToRad(aglBeg));
    yBeg = yHge+dstHgeHdl*sin(Utils::degrToRad(aglBeg));
    // x & y coords at aglEnd, dstHgeCtr+dstCtrHdl from hinge
    xEnd = xHge+dstHgeHdl*cos(Utils::degrToRad(aglEnd));
    yEnd = yHge+dstHgeHdl*sin(Utils::degrToRad(aglEnd));

    // draw center
    VisUtils::setColorMdGray();
    VisUtils::drawLine(
      -hdlDOF,  hdlDOF,
      0.0,     0.0);
    VisUtils::drawLine(
      0.0,     0.0,
      hdlDOF, -hdlDOF);

    // draw hinge
    VisUtils::setColorGreen();
    VisUtils::fillEllipse(
      xHge,   yHge,
      hdlDOF, hdlDOF,
      segNumHnt);
    VisUtils::setColorMdGray();
    VisUtils::drawEllipse(
      xHge,   yHge,
      hdlDOF, hdlDOF,
      segNumHnt);

    /*
    // connect hinge & center
    VisUtils::drawLineDashed(
        xHge, 0.0,
        yHge, 0.0 );
    */

    // counter clockwise rotation
    if (aglDOF->getDir() > 0)
    {
      VisUtils::drawLine(
        xHge, xBeg,
        yHge, yBeg);
      VisUtils::drawLine(
        xHge, xEnd,
        yHge, yEnd);
      VisUtils::setColorRed();
      VisUtils::drawArcDashed(
        xHge,      yHge,
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
      VisUtils::setColorMdGray();
      VisUtils::drawRect(
          hdlDOF, -hdlDOF,
          0.0,     1.5*hdlDOF );
      VisUtils::drawLine(
          2.0*hdlDOF, -2.0*hdlDOF,
          0.0,         0.0 );
      */

      // draw starting cross
      VisUtils::setColorRed();
      VisUtils::drawLine(
        -hdlDOF,  hdlDOF,
        hdlDOF, -hdlDOF);
      VisUtils::drawLine(
        -hdlDOF,  hdlDOF,
        -hdlDOF,  hdlDOF);

      VisUtils::setColorGreen();
      VisUtils::fillTriangle(
        2.0*hdlDOF,  0.0,
        3.0*hdlDOF, -hdlDOF,
        4.0*hdlDOF,  0.0);
      VisUtils::setColorMdGray();
      VisUtils::drawTriangle(
        2.0*hdlDOF,  0.0,
        3.0*hdlDOF, -hdlDOF,
        4.0*hdlDOF,  0.0);

      glPopMatrix();

      // end
      glPushMatrix();
      glTranslatef(xEnd, yEnd, 0.0);
      glRotatef(aglEnd, 0.0, 0.0, 1.0);
      VisUtils::setColorGreen();
      VisUtils::fillTriangle(
        0.0,     0.0,
        hdlDOF, -2.0*hdlDOF,
        -hdlDOF, -2.0*hdlDOF);
      VisUtils::setColorMdGray();
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
        xHge, xBeg,
        yHge, yBeg);
      VisUtils::drawLine(
        xHge, xEnd,
        yHge, yEnd);
      VisUtils::setColorRed();
      VisUtils::drawArcDashedCW(
        xHge,      yHge,
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
      VisUtils::setColorMdGray();
      VisUtils::drawRect(
         -hdlDOF,  hdlDOF,
          0.0,    -1.5*hdlDOF );
      VisUtils::drawLine(
          2.0*hdlDOF, -2.0*hdlDOF,
          0.0,         0.0 );
      */

      // draw starting cross
      VisUtils::setColorRed();
      VisUtils::drawLine(
        -hdlDOF,  hdlDOF,
        hdlDOF, -hdlDOF);
      VisUtils::drawLine(
        -hdlDOF,  hdlDOF,
        -hdlDOF,  hdlDOF);

      VisUtils::setColorGreen();
      VisUtils::fillTriangle(
        3.0*hdlDOF, hdlDOF,
        2.0*hdlDOF, 0.0,
        4.0*hdlDOF, 0.0);
      VisUtils::setColorMdGray();
      VisUtils::drawTriangle(
        3.0*hdlDOF, hdlDOF,
        2.0*hdlDOF, 0.0,
        4.0*hdlDOF, 0.0);

      glPopMatrix();

      // end
      glPushMatrix();
      glTranslatef(xEnd, yEnd, 0.0);
      glRotatef(aglEnd, 0.0, 0.0, 1.0);
      VisUtils::setColorGreen();
      VisUtils::fillTriangle(
        hdlDOF, 2.0*hdlDOF,
        0.0,    0.0,
        -hdlDOF, 2.0*hdlDOF);
      VisUtils::setColorMdGray();
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
  GLCanvas* canvas)
{
  double pix    = canvas->getPixelSize();
  double hdlDOF = hdlSze*pix;

  glPushMatrix();
  glRotatef(-aglCtr, 0.0, 0.0, 1.0);

  if (inSelectMode == true)
    {}
  else
  {
    double dstHgeCtr, dstHgeHdl;
    double aglRef, aglBeg, aglEnd;
    double xBeg, yBeg;
    double xEnd, yEnd;

    // distance from hinge to center
    dstHgeCtr = Utils::dist(xCtr+xHge, yCtr+yHge, xCtr, yCtr);
    // distance from center to handles
    dstHgeHdl = dstHgeCtr + 0.5*Utils::maxx(Utils::abs(xDFC), Utils::abs(yDFC));

    if (dstHgeCtr == 0)
    {
      aglRef = 0;
    }
    else
    {
      aglRef = Utils::calcAngleDg(-xHge, -yHge);
    }
    aglBeg = aglRef + aglDOF->getMin();
    aglEnd = aglRef + aglDOF->getMax();
    // x & y coords at aglBeg, dstHgeCtr+dstCtrHdl from hinge
    xBeg = xHge+dstHgeHdl*cos(Utils::degrToRad(aglBeg));
    yBeg = yHge+dstHgeHdl*sin(Utils::degrToRad(aglBeg));
    // x & y coords at aglEnd, dstHgeCtr+dstCtrHdl from hinge
    xEnd = xHge+dstHgeHdl*cos(Utils::degrToRad(aglEnd));
    yEnd = yHge+dstHgeHdl*sin(Utils::degrToRad(aglEnd));

    // draw hinge
    VisUtils::setColorMdGray();
    VisUtils::drawEllipse(
      xHge,   yHge,
      hdlDOF, hdlDOF,
      segNumHnt);

    /*
    // connect hinge & center
    VisUtils::drawLineDashed(
        xHge, 0.0,
        yHge, 0.0 );
    */

    // counter clockwise rotation
    if (aglDOF->getDir() > 0)
    {
      VisUtils::drawLineDashed(
        xHge, xBeg,
        yHge, yBeg);
      VisUtils::drawArcDashed(
        xHge,      yHge,
        aglBeg,    aglEnd,
        dstHgeHdl, segNumHnt);
      VisUtils::drawLineDashed(
        xHge, xEnd,
        yHge, yEnd);

      /*
      // start
      glPushMatrix();
      glTranslatef( xBeg, yBeg, 0.0 );
      glRotatef( aglBeg, 0.0, 0.0, 1.0 );
      VisUtils::setColorWhite();
      VisUtils::fillRect(
          hdlDOF, -hdlDOF,
          0.0,     1.5*hdlDOF );
      VisUtils::setColorMdGray();
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
      VisUtils::setColorWhite();
      VisUtils::fillTriangle(
        0.0,     0.0,
        hdlDOF, -2.0*hdlDOF,
        -hdlDOF, -2.0*hdlDOF);
      VisUtils::setColorMdGray();
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
        xHge, xBeg,
        yHge, yBeg);
      VisUtils::drawArcDashedCW(
        xHge,      yHge,
        aglBeg,    aglEnd,
        dstHgeHdl, segNumHnt);
      VisUtils::drawLineDashed(
        xHge, xEnd,
        yHge, yEnd);

      /*
      // start
      glPushMatrix();
      glTranslatef( xBeg, yBeg, 0.0 );
      glRotatef( aglBeg, 0.0, 0.0, 1.0 );
      VisUtils::setColorWhite();
      VisUtils::fillRect(
         -hdlDOF,  hdlDOF,
          0.0,    -1.5*hdlDOF );
      VisUtils::setColorMdGray();
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
      VisUtils::setColorWhite();
      VisUtils::fillTriangle(
        hdlDOF, 2.0*hdlDOF,
        0.0,    0.0,
        -hdlDOF, 2.0*hdlDOF);
      VisUtils::setColorMdGray();
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
