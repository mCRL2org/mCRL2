// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./visutils.cpp

#include "visutils.h"
#include "character_set.xpm"


// -- init static variables -----------------------------------------


float   VisUtils::cushCurve =  0.3f;
float   VisUtils::cushAngle = 70.0f;
float   VisUtils::cushDepth =  1.0f;


QColor interpolateRgb(QColor from, QColor to, float t)
{
  if (!from.isValid() || !to.isValid())
  {
    return QColor();
  }
  return QColor(
    from.red() + (int)((to.red() - from.red()) * t),
    from.green() + (int)((to.green() - from.green()) * t),
    from.blue() + (int)((to.blue() - from.blue()) * t),
    from.alpha() + (int)((to.alpha() - from.alpha()) * t)
    );
}

QColor interpolateHsv(QColor from, QColor to, float t, bool longPath)
{
  // TODO
  return QColor();
}


// -- clear canvas --------------------------------------------------


void VisUtils::clear(QColor col)
{
  glClearColor(col.redF(), col.greenF(), col.blueF(), col.alphaF());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


// -- color ---------------------------------------------------------


QColor VisUtils::BlueYellow::operator()(double fraction) const
{
  if (fraction < 0.5)
  {
    return QColor::fromHsvF(4/6.0, 1.0 - fraction * 2.0, 1.0);
  }
  else
  {
    return QColor::fromHsvF(1/6.0, fraction * 2.0 - 1.0, 1.0);
  }
}

QColor VisUtils::Spectral::operator()(double fraction) const
{
  return QColor::fromHsvF(fraction, 1.0, 1.0);
}

QColor VisUtils::ListColorMap::operator()(double fraction) const
{
  double scaled = fraction * (m_colors.size() - 1);
  int index = (int)scaled;
  if (index < 0)
  {
    return m_colors.first();
  }
  if (index + 1 >= m_colors.size())
  {
    return m_colors.last();
  }
  return interpolateRgb(m_colors[index], m_colors[index + 1], scaled - index);
}

QColor VisUtils::ListColorMap::operator()(int numerator, int denominator) const
{
  if (denominator <= m_colors.size())
  {
    return m_colors[numerator];
  }
  return operator()(numerator / (double)(denominator - 1));
}

const VisUtils::BlueYellow VisUtils::blueYellow;
const VisUtils::Spectral VisUtils::spectral;

const VisUtils::ListColorMap VisUtils::grayScale(QList<QColor>()
  << QColor(0, 0, 0)
  << QColor(255, 255, 255)
  );

const VisUtils::ListColorMap VisUtils::qualPast1(QList<QColor>()
  << QColor(251, 180, 174)
  << QColor(179, 205, 227)
  << QColor(204, 235, 197)
  << QColor(222, 203, 228)
  << QColor(254, 217, 166)
  << QColor(255, 255, 204)
  << QColor(229, 216, 189)
  << QColor(253, 218, 236)
  << QColor(242, 242, 242)
  );
const VisUtils::ListColorMap VisUtils::qualPast2(QList<QColor>()
  << QColor(179, 226, 205)
  << QColor(253, 205, 172)
  << QColor(203, 213, 232)
  << QColor(244, 202, 228)
  << QColor(230, 245, 201)
  << QColor(255, 242, 174)
  << QColor(241, 226, 204)
  << QColor(204, 204, 204)
  );
const VisUtils::ListColorMap VisUtils::qualSet1(QList<QColor>()
  << QColor(255, 26, 28)
  << QColor(55, 126, 184)
  << QColor(77, 175, 74)
  << QColor(152, 78, 163)
  << QColor(255, 127, 0)
  << QColor(255, 255, 51)
  << QColor(166, 86, 40)
  << QColor(247, 129, 191)
  << QColor(153, 153, 153)
  );
const VisUtils::ListColorMap VisUtils::qualSet2(QList<QColor>()
  << QColor(102, 194, 165)
  << QColor(252, 141, 98)
  << QColor(141, 160, 203)
  << QColor(231, 138, 195)
  << QColor(166, 216, 84)
  << QColor(255, 217, 47)
  << QColor(229, 196, 148)
  << QColor(179, 179, 179)
  );
const VisUtils::ListColorMap VisUtils::qualSet3(QList<QColor>()
  << QColor(141, 211, 199)
  << QColor(255, 255, 179)
  << QColor(190, 186, 218)
  << QColor(251, 128, 114)
  << QColor(128, 177, 211)
  << QColor(253, 180, 98)
  << QColor(179, 222, 105)
  << QColor(252, 205, 229)
  << QColor(217, 217, 217)
  << QColor(188, 128, 189)
  << QColor(204, 235, 197)
  << QColor(255, 237, 111)
  );
const VisUtils::ListColorMap VisUtils::qualPair(QList<QColor>()
  << QColor(166, 206, 227)
  << QColor(31, 120, 180)
  << QColor(178, 223, 138)
  << QColor(51, 160, 44)
  << QColor(251, 154, 153)
  << QColor(227, 26, 28)
  << QColor(253, 191, 111)
  << QColor(255, 127, 0)
  << QColor(202, 178, 214)
  );
const VisUtils::ListColorMap VisUtils::qualDark(QList<QColor>()
  << QColor(27, 158, 119)
  << QColor(217, 95, 2)
  << QColor(117, 112, 179)
  << QColor(231, 41, 138)
  << QColor(102, 166, 30)
  << QColor(230, 171, 2)
  << QColor(166, 118, 29)
  << QColor(102, 102, 102)
  );
const VisUtils::ListColorMap VisUtils::qualAccent(QList<QColor>()
  << QColor(127, 201, 127)
  << QColor(190, 174, 212)
  << QColor(253, 192, 134)
  << QColor(255, 255, 153)
  << QColor(56, 108, 176)
  << QColor(240, 2, 127)
  << QColor(191, 91, 23)
  << QColor(102, 102, 102)
  << QColor(153, 153, 153)
  );
const VisUtils::ListColorMap VisUtils::seqGreen(QList<QColor>()
  << QColor(247, 252, 245)
  << QColor(229, 245, 224)
  << QColor(199, 233, 192)
  << QColor(161, 217, 155)
  << QColor(116, 196, 118)
  << QColor(65, 171, 93)
  << QColor(35, 139, 69)
  << QColor(0, 109, 44)
  << QColor(0, 68, 27)
  );
const VisUtils::ListColorMap VisUtils::seqRed(QList<QColor>()
  << QColor(255, 245, 240)
  << QColor(254, 224, 210)
  << QColor(252, 187, 161)
  << QColor(252, 146, 114)
  << QColor(251, 106, 74)
  << QColor(239, 59, 44)
  << QColor(203, 24, 29)
  << QColor(165, 15, 21)
  << QColor(103, 0, 13)
  );


// -- anti-aliasing & blending --------------------------------------


void VisUtils::enableLineAntiAlias()
{
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void VisUtils::disableLineAntiAlias()
{
  glDisable(GL_LINE_SMOOTH);
  glDisable(GL_BLEND);
}


void VisUtils::enableBlending()
{
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void VisUtils::disableBlending()
{
  glDisable(GL_BLEND);
}


// -- line width ------------------------------------------------


void VisUtils::setLineWidth(const double& px)
{
  glLineWidth(px);
}


// -- drawing functions ---------------------------------------------


void VisUtils::drawLine(
  const double& xFr, const double& xTo,
  const double& yFr, const double& yTo)
{
  glBegin(GL_LINES);
  glVertex2f(xFr, yFr);
  glVertex2f(xTo, yTo);
  glEnd();
}


void VisUtils::drawLineDashed(
  const double& xFr, const double& xTo,
  const double& yFr, const double& yTo)
{
  glLineStipple(2, 0xAAAA);
  glEnable(GL_LINE_STIPPLE);
  glBegin(GL_LINES);
  glVertex2f(xFr, yFr);
  glVertex2f(xTo, yTo);
  glEnd();
  glDisable(GL_LINE_STIPPLE);
}


void VisUtils::drawArc(
  const double& xCtr,     const double& yCtr,
  const double& aglBegDg, const double& aglEndDg,
  const double& radius,   const int& slices)
// This function draws a circular arc COUNTER CLOCKWIZE from
// 'aglBegDg' to 'aglEndDg'. This segment has radius equal to
// 'radius' and is centered at (xCtr, yCtr). This arc consists of
// 'slices' segments.
{
  double slice;

  if (aglBegDg < aglEndDg)
  {
    slice = (aglEndDg-aglBegDg)/(double)slices;
  }
  else
  {
    slice = (360.0-(aglBegDg-aglEndDg))/(double)slices;
  }

  glBegin(GL_LINE_STRIP);
  for (int i = 0; i <= slices; ++i)
  {
    double xCur = xCtr + radius*cos(Utils::degrToRad(aglBegDg+i*slice));
    double yCur = yCtr + radius*sin(Utils::degrToRad(aglBegDg+i*slice));
    glVertex2f(xCur, yCur);
  }
  glEnd();
}


void VisUtils::drawArcDashed(
  const double& xCtr,     const double& yCtr,
  const double& aglBegDg, const double& aglEndDg,
  const double& radius,   const int& slices)
// This function draws a circular dashed arc COUNTER CLOCKWIZE from
// 'aglBegDg' to 'aglEndDg'. This segment has radius equal to
// 'radius' and is centered at (xCtr, yCtr). This arc consists of
// 'slices' segments.
{
  glLineStipple(2, 0xAAAA);
  glEnable(GL_LINE_STIPPLE);
  drawArc(
    xCtr,     yCtr,
    aglBegDg, aglEndDg,
    radius,   slices);
  glDisable(GL_LINE_STIPPLE);
}


void VisUtils::drawArcCW(
  const double& xCtr,     const double& yCtr,
  const double& aglBegDg, const double& aglEndDg,
  const double& radius,   const int& slices)
// This function draws a circular arc CLOCKWIZE from 'aglBegDg' to
// 'aglEndDg'. This segment has radius equal to 'radius' and is
// centered at (xCtr, yCtr). This arc consists of 'slices' segments.
{
  double slice;

  // draw arc
  if (aglBegDg < aglEndDg)
  {
    slice = (360.0-(aglEndDg-aglBegDg))/(double)slices;
  }
  else
  {
    slice = (aglBegDg-aglEndDg)/(double)slices;
  }

  glBegin(GL_LINE_STRIP);
  for (int i = 0; i <= slices; ++i)
  {
    double xCur = xCtr + radius*cos(Utils::degrToRad(aglBegDg-i*slice));
    double yCur = yCtr + radius*sin(Utils::degrToRad(aglBegDg-i*slice));
    glVertex2f(xCur, yCur);
  }
  glEnd();
}


void VisUtils::drawArcDashedCW(
  const double& xCtr,     const double& yCtr,
  const double& aglBegDg, const double& aglEndDg,
  const double& radius,   const int& slices)
// This function draws a circular dashed arc CLOCKWIZE from
// 'aglBegDg' to 'aglEndDg'. This segment has radius equal to
// 'radius' and is centered at (xCtr, yCtr). This arc consists of
// 'slices' segments.
{
  glLineStipple(2, 0xAAAA);
  glEnable(GL_LINE_STIPPLE);
  drawArcCW(
    xCtr,     yCtr,
    aglBegDg, aglEndDg,
    radius,   slices);
  glDisable(GL_LINE_STIPPLE);
}


void VisUtils::drawArc(
  const double& xCtr,     const double& yCtr,
  const double& aglBegDg, const double& aglEndDg,
  const double& wthBeg,   const double& wthEnd,
  const double& radius,   const int& slices,
  QColor colBeg,          QColor colEnd)
{
  double slice;

  if (aglBegDg < aglEndDg)
  {
    slice = (aglEndDg-aglBegDg)/(double)slices;
  }
  else
  {
    slice = (360.0-(aglBegDg-aglEndDg))/(double)slices;
  }

  double interv = 0.5*(wthEnd-wthBeg)/(double)slices;

  glBegin(GL_LINE_LOOP);
  // outside
  {
    for (int i = 0; i <= slices; ++i)
    {
      setValidColor(interpolateRgb(colBeg, colEnd, i / (double)slices));

      double xCur = xCtr + (radius+0.5*wthBeg+(i*interv))*cos(Utils::degrToRad(aglBegDg+i*slice));
      double yCur = yCtr + (radius+0.5*wthBeg+(i*interv))*sin(Utils::degrToRad(aglBegDg+i*slice));
      glVertex2f(xCur, yCur);
    }
  }
  // inside
  {
    for (int i = slices; i >= 0; --i)
    {
      setValidColor(interpolateRgb(colBeg, colEnd, i / (double)slices));

      double xCur = xCtr + (radius-0.5*wthBeg-(i*interv))*cos(Utils::degrToRad(aglBegDg+i*slice));
      double yCur = yCtr + (radius-0.5*wthBeg-(i*interv))*sin(Utils::degrToRad(aglBegDg+i*slice));
      glVertex2f(xCur, yCur);
    }
  }
  glEnd();
}


//-------------------------------------------------
void VisUtils::fillArc(
  const double& xCtr,     const double& yCtr,
  const double& aglBegDg, const double& aglEndDg,
  const double& wthBeg,   const double& wthEnd,
  const double& radius,   const int& slices,
  QColor colBeg,          QColor colEnd)
//-------------------------------------------------
{
  double slice;

  if (aglBegDg < aglEndDg)
  {
    slice = (aglEndDg-aglBegDg)/(double)slices;
  }
  else
  {
    slice = (360.0-(aglBegDg-aglEndDg))/(double)slices;
  }

  double interv = 0.5*(wthEnd-wthBeg)/(double)slices;

  glBegin(GL_QUAD_STRIP);
  {
    for (int i = 0; i <= slices; ++i)
    {
      setValidColor(interpolateRgb(colBeg, colEnd, i / (double)slices));

      // outside
      double xCur = xCtr + (radius+0.5*wthBeg+(i*interv))*cos(Utils::degrToRad(aglBegDg+i*slice));
      double yCur = yCtr + (radius+0.5*wthBeg+(i*interv))*sin(Utils::degrToRad(aglBegDg+i*slice));
      glVertex2f(xCur, yCur);

      // inside
      xCur = xCtr + (radius-0.5*wthBeg-(i*interv))*cos(Utils::degrToRad(aglBegDg+i*slice));
      yCur = yCtr + (radius-0.5*wthBeg-(i*interv))*sin(Utils::degrToRad(aglBegDg+i*slice));
      glVertex2f(xCur, yCur);
    }
  }
  glEnd();
}

void VisUtils::drawTriangle(
  const double&   x1, const double& y1,
  const double&   x2, const double& y2,
  const double&   x3, const double& y3,
  QColor col1, QColor col2, QColor col3)
{
  glBegin(GL_LINE_LOOP);
  setValidColor(col1);
  glVertex2f(x1, y1);
  setValidColor(col2);
  glVertex2f(x2, y2);
  setValidColor(col3);
  glVertex2f(x3, y3);
  glEnd();
}

void VisUtils::fillTriangle(
    const double&   x1, const double& y1,
    const double&   x2, const double& y2,
    const double&   x3, const double& y3,
    QColor col1, QColor col2, QColor col3)
{
  glBegin(GL_POLYGON);
  setValidColor(col1);
  glVertex2f(x1, y1);
  setValidColor(col2);
  glVertex2f(x2, y2);
  setValidColor(col3);
  glVertex2f(x3, y3);
  glEnd();
}


void VisUtils::drawRect(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot,
  QColor colTopLft,   QColor colTopRgt,
  QColor colBotLft,   QColor colBotRgt)
{
  glBegin(GL_LINE_LOOP);
  setValidColor(colTopLft);
  glVertex2f(xLft, yTop);
  setValidColor(colBotLft);
  glVertex2f(xLft, yBot);
  setValidColor(colBotRgt);
  glVertex2f(xRgt, yBot);
  setValidColor(colTopRgt);
  glVertex2f(xRgt, yTop);
  glEnd();
}


void VisUtils::fillRect(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot,
  QColor colTopLft,   QColor colTopRgt,
  QColor colBotLft,   QColor colBotRgt)
{
  glBegin(GL_POLYGON);
  setValidColor(colTopLft);
  glVertex2f(xLft, yTop);
  setValidColor(colBotLft);
  glVertex2f(xLft, yBot);
  setValidColor(colBotRgt);
  glVertex2f(xRgt, yBot);
  setValidColor(colTopRgt);
  glVertex2f(xRgt, yTop);
  glEnd();
}


void VisUtils::drawEllipse(
  const double& xCtr, const double& yCtr,
  const double& xDOF, const double& yDOF,
  const int& slices)
{
  double slice = (2*PI)/(double)slices;

  glBegin(GL_LINE_LOOP);
  for (int i = 0; i < slices; ++i)
  {
    double xCur = xCtr + xDOF*sin(i*slice);
    double yCur = yCtr + yDOF*cos(i*slice);
    glVertex2f(xCur, yCur);
  }
  glEnd();
}


void VisUtils::fillEllipse(
  const double& xCtr, const double& yCtr,
  const double& xDOF, const double& yDOF,
  const int& slices)
{
  double slice = (2*PI)/(double)slices;

  glBegin(GL_POLYGON);
  for (int i = 0; i < slices; ++i)
  {
    double xCur = xCtr + xDOF*sin(i*slice);
    double yCur = yCtr + yDOF*cos(i*slice);
    glVertex2f(xCur, yCur);
  }
  glEnd();
}


void VisUtils::fillEllipse(
  const double&   xCtr,    const double&   yCtr,
  const double&   xDOFIn,  const double&   yDOFIn,
  const double&   xDOFOut, const double&   yDOFOut,
  const double&   slices,  QColor cIn, QColor cOut)
{
  double slice = (2*PI)/(double)slices;

  double xCurIn  = xCtr + xDOFIn*cos(0.0);
  double yCurIn  = yCtr + yDOFIn*sin(0.0);
  double xCurOut = xCtr + xDOFOut*cos(0.0);
  double yCurOut = yCtr + yDOFOut*sin(0.0);

  for (int i = 1; i <= slices; ++i)
  {
    glBegin(GL_POLYGON);

    setColor(cOut);
    glVertex2f(xCurOut, yCurOut);
    setColor(cIn);
    glVertex2f(xCurIn,  yCurIn);

    xCurIn  = xCtr + xDOFIn*cos(i*slice);
    yCurIn  = yCtr + yDOFIn*sin(i*slice);
    xCurOut = xCtr + xDOFOut*cos(i*slice);
    yCurOut = yCtr + yDOFOut*sin(i*slice);

    setColor(cIn);
    glVertex2f(xCurIn,  yCurIn);
    setColor(cOut);
    glVertex2f(xCurOut, yCurOut);

    glEnd();
  }
}


void VisUtils::fillEllipse(
  const double&   xCtr,     const double&   yCtr,
  const double&   xDOFIn,   const double&   yDOFIn,
  const double&   xDOFOut,  const double&   yDOFOut,
  const double&   aglBegDg, const double&   aglEndDg,
  const double&   slices,   QColor cIn, QColor cOut)
{
  double aglBegRd = Utils::degrToRad(aglBegDg);

  double sliceRd;
  if (aglBegDg < aglEndDg)
  {
    sliceRd = Utils::degrToRad((aglEndDg-aglBegDg)/(double)slices);
  }
  else
  {
    sliceRd = Utils::degrToRad((360.0-(aglBegDg-aglEndDg))/(double)slices);
  }

  double xInside  = xCtr + xDOFIn*cos(aglBegRd);
  double yInside  = yCtr + yDOFIn*sin(aglBegRd);
  double xOutside = xCtr + xDOFOut*cos(aglBegRd);
  double yOutside = yCtr + yDOFOut*sin(aglBegRd);

  for (int i = 1; i <= slices; ++i)
  {
    glBegin(GL_POLYGON);

    setColor(cOut);
    glVertex2f(xOutside, yOutside);
    setColor(cIn);
    glVertex2f(xInside, yInside);

    xInside  = xCtr + xDOFIn*cos(aglBegRd + i*sliceRd);
    yInside  = yCtr + yDOFIn*sin(aglBegRd + i*sliceRd);
    xOutside = xCtr + xDOFOut*cos(aglBegRd + i*sliceRd);
    yOutside = yCtr + yDOFOut*sin(aglBegRd + i*sliceRd);

    setColor(cIn);
    glVertex2f(xInside, yInside);
    setColor(cOut);
    glVertex2f(xOutside, yOutside);

    glEnd();
  }
}


void VisUtils::drawArrow(
  const double& xFr,   const double& xTo,
  const double& yFr,   const double& yTo,
  const double& wHead, const double& lHead)
{
  // calc angle & length of arrow
  double dX   = xTo-xFr;
  double dY   = yTo-yFr;

  double angl   = Utils::calcAngleDg(dX, dY);
  double lenArw = Utils::dist(xFr, yFr, xTo, yTo);

  // calc length base
  double lenBase = lenArw - lHead;

  glPushMatrix();
  glTranslatef(xFr, yFr, 0.0);
  glRotatef(angl, 0.0, 0.0, 1.0);

  // arrow head
  drawTriangle(
    lenBase,  0.5*wHead,
    lenBase, -0.5*wHead,
    lenArw,   0.0);
  // arrow base
  drawLine(0.0, lenBase, 0.0, 0.0);

  glPopMatrix();
}


void VisUtils::drawArrow(
  const double& xFr,   const double& xTo,
  const double& yFr,   const double& yTo,
  const double& wBase, const double& wHead,
  const double& lHead, QColor cFr, QColor cTo)
{
  // calc angle & length of arrow
  double dX     = xTo-xFr;
  double dY     = yTo-yFr;
  double angl   = Utils::calcAngleDg(dX, dY);
  double lenArw = Utils::dist(xFr, yFr, xTo, yTo);

  // calc length base
  double lenBase = lenArw-lHead;

  // calc junction color
  QColor cJnc = interpolateRgb(cFr, cTo, lenBase/lenArw);

  glPushMatrix();
  glTranslatef(xFr, yFr, 0.0);
  glRotatef(angl, 0.0, 0.0, 1.0);

  glBegin(GL_LINE_LOOP);
  setValidColor(cFr);
  glVertex2f(0.0, 0.5*wBase);
  glVertex2f(0.0, -0.5*wBase);
  setValidColor(cJnc);
  glVertex2f(lenArw-lHead, -0.5*wBase);
  glVertex2f(lenArw-lHead, -0.5*wHead);
  setValidColor(cTo);
  glVertex2f(lenArw, 0.0);
  setValidColor(cJnc);
  glVertex2f(lenArw-lHead,  0.5*wHead);
  glVertex2f(lenArw-lHead,  0.5*wBase);
  glEnd();

  glPopMatrix();
}


void VisUtils::fillArrow(
  const double& xFr,   const double& xTo,
  const double& yFr,   const double& yTo,
  const double& wHead, const double& lHead)
{
  // calc angle & length of arrow
  double dX   = xTo-xFr;
  double dY   = yTo-yFr;

  double angl   = Utils::calcAngleDg(dX, dY);
  double lenArw = Utils::dist(xFr, yFr, xTo, yTo);

  // calc length base
  double lenBase = lenArw - lHead;

  glPushMatrix();
  glTranslatef(xFr, yFr, 0.0);
  glRotatef(angl, 0.0, 0.0, 1.0);

  // arrow head
  fillTriangle(
    lenBase,  0.5*wHead,
    lenBase, -0.5*wHead,
    lenArw,   0.0);
  // arrow base
  drawLine(0.0, lenBase, 0.0, 0.0);

  glPopMatrix();
}


void VisUtils::fillArrow(
  const double& xFr,   const double& xTo,
  const double& yFr,   const double& yTo,
  const double& wBase, const double& wHead,
  const double& lHead, QColor cFr, QColor cTo)
{
  // calc angle & length of arrow
  double dX     = xTo-xFr;
  double dY     = yTo-yFr;
  double angl   = Utils::calcAngleDg(dX, dY);
  double lenArw = Utils::dist(xFr, yFr, xTo, yTo);

  // calc length base
  double lenBase = lenArw-lHead;

  // calc junction color
  QColor cJnc = interpolateRgb(cFr, cTo, lenBase/lenArw);

  glPushMatrix();
  glTranslatef(xFr, yFr, 0.0);
  glRotatef(angl, 0.0, 0.0, 1.0);

  // base
  glBegin(GL_POLYGON);
  setValidColor(cFr);
  glVertex2f(0.0, 0.5*wBase);
  glVertex2f(0.0, -0.5*wBase);
  setValidColor(cJnc);
  glVertex2f(lenArw-lHead, -0.5*wBase);
  glVertex2f(lenArw-lHead,  0.5*wBase);
  glEnd();

  // head
  glBegin(GL_POLYGON);
  setValidColor(cJnc);
  glVertex2f(lenArw-lHead, -0.5*wHead);
  setValidColor(cTo);
  glVertex2f(lenArw, 0.0);
  setValidColor(cJnc);
  glVertex2f(lenArw-lHead,  0.5*wHead);
  glEnd();

  glPopMatrix();
}


void VisUtils::drawDArrow(
  const double& xFr,   const double& xTo,
  const double& yFr,   const double& yTo,
  const double& wHead, const double& lHead)
{
  // calc angle & length of arrow
  double dX   = xTo-xFr;
  double dY   = yTo-yFr;

  double angl   = Utils::calcAngleDg(dX, dY);
  double lenArw = Utils::dist(xFr, yFr, xTo, yTo);

  glPushMatrix();
  glTranslatef(xFr, yFr, 0.0);
  glRotatef(angl, 0.0, 0.0, 1.0);

  // arrow heads
  drawTriangle(
    0.0,    0.0,
    lHead,  0.5*wHead,
    lHead, -0.5*wHead);
  drawTriangle(
    lenArw-lHead,  0.5*wHead,
    lenArw-lHead, -0.5*wHead,
    lenArw,          0.0);
  // arrow base
  drawLine(lHead, lenArw-lHead, 0.0, 0.0);

  glPopMatrix();
}


void VisUtils::fillDArrow(
  const double& xFr,   const double& xTo,
  const double& yFr,   const double& yTo,
  const double& wHead, const double& lHead)
{
  // calc angle & length of arrow
  double dX   = xTo-xFr;
  double dY   = yTo-yFr;

  double angl   = Utils::calcAngleDg(dX, dY);
  double lenArw = Utils::dist(xFr, yFr, xTo, yTo);

  glPushMatrix();
  glTranslatef(xFr, yFr, 0.0);
  glRotatef(angl, 0.0, 0.0, 1.0);

  // arrow heads
  fillTriangle(
    0.0,    0.0,
    lHead,  0.5*wHead,
    lHead, -0.5*wHead);
  fillTriangle(
    lenArw-lHead,  0.5*wHead,
    lenArw-lHead, -0.5*wHead,
    lenArw,          0.0);
  // arrow base
  drawLine(lHead, lenArw-lHead, 0.0, 0.0);

  glPopMatrix();
}


void VisUtils::drawArrowArcCW(
  const double& xCtr,     const double& yCtr,
  const double& aglBegDg, const double& aglEndDg,
  const double& radius,   const double& slices,
  const double& wHead,    const double& lHead)
{
  double xCur = 0.0;
  double yCur = 0.0;
  double slice;

  // draw arc
  if (aglBegDg < aglEndDg)
  {
    slice = (aglEndDg-aglBegDg)/(double)slices;
  }
  else
  {
    slice = (360.0-(aglBegDg-aglEndDg))/(double)slices;
  }

  glBegin(GL_LINE_STRIP);
  for (int i = 0; i <= slices; ++i)
  {
    xCur = xCtr + radius*cos(Utils::degrToRad(aglBegDg+i*slice));
    yCur = yCtr + radius*sin(Utils::degrToRad(aglBegDg+i*slice));
    glVertex2f(xCur, yCur);
  }
  glEnd();

  // draw arrow head
  glPushMatrix();
  glTranslatef(xCur, yCur, 0.0);
  glRotatef(aglEndDg-90.0, 0.0, 0.0, 1.0);

  drawTriangle(
    0.0,    0.0,
    lHead, -0.5*wHead,
    lHead,  0.5*wHead);

  glPopMatrix();
}


void VisUtils::fillArrowArcCW(
  const double& xCtr,     const double& yCtr,
  const double& aglBegDg, const double& aglEndDg,
  const double& radius,   const double& slices,
  const double& wHead,    const double& lHead)
{
  double xCur = 0.0;
  double yCur = 0.0;
  double slice;

  // draw arc
  if (aglBegDg < aglEndDg)
  {
    slice = (aglEndDg-aglBegDg)/(double)slices;
  }
  else
  {
    slice = (360.0-(aglBegDg-aglEndDg))/(double)slices;
  }

  glBegin(GL_LINE_STRIP);
  for (int i = 0; i <= slices; ++i)
  {
    xCur = xCtr + radius*cos(Utils::degrToRad(aglBegDg+i*slice));
    yCur = yCtr + radius*sin(Utils::degrToRad(aglBegDg+i*slice));
    glVertex2f(xCur, yCur);
  }
  glEnd();

  // draw arrow head
  glPushMatrix();
  glTranslatef(xCur, yCur, 0.0);
  glRotatef(aglEndDg-90.0, 0.0, 0.0, 1.0);

  fillTriangle(
    0.0,    0.0,
    lHead, -0.5*wHead,
    lHead,  0.5*wHead);

  glPopMatrix();
}


void VisUtils::drawArrowArcCCW(
  const double& xCtr,     const double& yCtr,
  const double& aglBegDg, const double& aglEndDg,
  const double& radius,   const double& slices,
  const double& wHead,    const double& lHead)
{
  double xCur = 0.0;
  double yCur = 0.0;
  double slice;

  // draw arc
  if (aglBegDg < aglEndDg)
  {
    slice = (360.0-(aglEndDg-aglBegDg))/(double)slices;
  }
  else
  {
    slice = (aglBegDg-aglEndDg)/(double)slices;
  }

  glBegin(GL_LINE_STRIP);
  for (int i = 0; i <= slices; ++i)
  {
    xCur = xCtr + radius*cos(Utils::degrToRad(aglBegDg-i*slice));
    yCur = yCtr + radius*sin(Utils::degrToRad(aglBegDg-i*slice));
    glVertex2f(xCur, yCur);
  }
  glEnd();

  // draw arrow head
  glPushMatrix();
  glTranslatef(xCur, yCur, 0.0);
  glRotatef(aglEndDg+90.0, 0.0, 0.0, 1.0);

  drawTriangle(
    0.0,    0.0,
    lHead, -0.5*wHead,
    lHead,  0.5*wHead);

  glPopMatrix();
}


void VisUtils::fillArrowArcCCW(
  const double& xCtr,     const double& yCtr,
  const double& aglBegDg, const double& aglEndDg,
  const double& radius,   const double& slices,
  const double& wHead,    const double& lHead)
{
  double xCur = 0.0;
  double yCur = 0.0;
  double slice;

  // draw arc
  if (aglBegDg < aglEndDg)
  {
    slice = (360.0-(aglEndDg-aglBegDg))/(double)slices;
  }
  else
  {
    slice = (aglBegDg-aglEndDg)/(double)slices;
  }

  glBegin(GL_LINE_STRIP);
  for (int i = 0; i <= slices; ++i)
  {
    xCur = xCtr + radius*cos(Utils::degrToRad(aglBegDg-i*slice));
    yCur = yCtr + radius*sin(Utils::degrToRad(aglBegDg-i*slice));
    glVertex2f(xCur, yCur);
  }
  glEnd();

  // draw arrow head
  glPushMatrix();
  glTranslatef(xCur, yCur, 0.0);
  glRotatef(aglEndDg+90.0, 0.0, 0.0, 1.0);

  fillTriangle(
    0.0,    0.0,
    lHead, -0.5*wHead,
    lHead,  0.5*wHead);

  glPopMatrix();
}


void VisUtils::drawFwrdIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
{
  glBegin(GL_LINE_LOOP);
  glVertex2f(xLft, yTop);
  glVertex2f(xLft, yBot);
  glVertex2f(
    xLft + 0.4*(xRgt-xLft),
    0.5*(yTop+yBot));
  glEnd();

  glBegin(GL_LINE_LOOP);
  glVertex2f(
    xLft + 0.4*(xRgt-xLft),
    yTop);
  glVertex2f(
    xLft + 0.4*(xRgt-xLft),
    yBot);
  glVertex2f(
    xLft + 0.8*(xRgt-xLft),
    0.5*(yTop+yBot));
  glEnd();

  glBegin(GL_LINE_LOOP);
  glVertex2f(
    xLft + 0.8*(xRgt-xLft),
    yTop);
  glVertex2f(
    xLft + 0.8*(xRgt-xLft),
    yBot);
  glVertex2f(xRgt, yBot);
  glVertex2f(xRgt, yTop);
  glEnd();
}


void VisUtils::fillFwrdIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
{
  glBegin(GL_POLYGON);
  glVertex2f(xLft, yTop);
  glVertex2f(xLft, yBot);
  glVertex2f(
    xLft + 0.4*(xRgt-xLft),
    0.5*(yTop+yBot));
  glEnd();

  glBegin(GL_POLYGON);
  glVertex2f(
    xLft + 0.4*(xRgt-xLft),
    yTop);
  glVertex2f(
    xLft + 0.4*(xRgt-xLft),
    yBot);
  glVertex2f(
    xLft + 0.8*(xRgt-xLft),
    0.5*(yTop+yBot));
  glEnd();

  glBegin(GL_POLYGON);
  glVertex2f(
    xLft + 0.8*(xRgt-xLft),
    yTop);
  glVertex2f(
    xLft + 0.8*(xRgt-xLft),
    yBot);
  glVertex2f(xRgt, yBot);
  glVertex2f(xRgt, yTop);
  glEnd();
}


void VisUtils::drawNextIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
{
  glBegin(GL_LINE_LOOP);
  glVertex2f(xLft, yTop);
  glVertex2f(xLft, yBot);
  glVertex2f(
    xLft + 0.5*(xRgt-xLft),
    0.5*(yTop+yBot));
  glEnd();

  glBegin(GL_LINE_LOOP);
  glVertex2f(
    xLft + 0.5*(xRgt-xLft),
    yTop);
  glVertex2f(
    xLft + 0.5*(xRgt-xLft),
    yBot);
  glVertex2f(
    xRgt,
    0.5*(yTop+yBot));
  glEnd();
}


void VisUtils::fillNextIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
{
  glBegin(GL_POLYGON);
  glVertex2f(xLft, yTop);
  glVertex2f(xLft, yBot);
  glVertex2f(
    xLft + 0.5*(xRgt-xLft),
    0.5*(yTop+yBot));
  glEnd();

  glBegin(GL_POLYGON);
  glVertex2f(
    xLft + 0.5*(xRgt-xLft),
    yTop);
  glVertex2f(
    xLft + 0.5*(xRgt-xLft),
    yBot);
  glVertex2f(
    xRgt,
    0.5*(yTop+yBot));
  glEnd();
}


void VisUtils::drawPauseIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
{
  glBegin(GL_LINE_LOOP);
  glVertex2f(
    xLft + 0.125*(xRgt-xLft),
    yTop);
  glVertex2f(
    xLft + 0.125*(xRgt-xLft),
    yBot);
  glVertex2f(
    xLft + 0.4*(xRgt-xLft),
    yBot);
  glVertex2f(
    xLft + 0.4*(xRgt-xLft),
    yTop);
  glEnd();

  glBegin(GL_LINE_LOOP);
  glVertex2f(
    xRgt - 0.4*(xRgt-xLft),
    yTop);
  glVertex2f(
    xRgt - 0.4*(xRgt-xLft),
    yBot);
  glVertex2f(
    xRgt - 0.125*(xRgt-xLft),
    yBot);
  glVertex2f(
    xRgt - 0.125*(xRgt-xLft),
    yTop);
  glEnd();
}


void VisUtils::fillPauseIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
{
  glBegin(GL_POLYGON);
  glVertex2f(
    xLft + 0.125*(xRgt-xLft),
    yTop);
  glVertex2f(
    xLft + 0.125*(xRgt-xLft),
    yBot);
  glVertex2f(
    xLft + 0.4*(xRgt-xLft),
    yBot);
  glVertex2f(
    xLft + 0.4*(xRgt-xLft),
    yTop);
  glEnd();

  glBegin(GL_POLYGON);
  glVertex2f(
    xRgt - 0.4*(xRgt-xLft),
    yTop);
  glVertex2f(
    xRgt - 0.4*(xRgt-xLft),
    yBot);
  glVertex2f(
    xRgt - 0.125*(xRgt-xLft),
    yBot);
  glVertex2f(
    xRgt - 0.125*(xRgt-xLft),
    yTop);
  glEnd();
}


void VisUtils::drawPlayIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
{
  glBegin(GL_LINE_LOOP);
  glVertex2f(xLft, yTop);
  glVertex2f(xLft, yBot);
  glVertex2f(xRgt, 0.5*(yTop+yBot));
  glEnd();
}


void VisUtils::fillPlayIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
{
  glBegin(GL_POLYGON);
  glVertex2f(xLft, yTop);
  glVertex2f(xLft, yBot);
  glVertex2f(xRgt, 0.5*(yTop+yBot));
  glEnd();
}


void VisUtils::drawStopIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
{
  glBegin(GL_LINE_LOOP);
  glVertex2f(xLft + 0.125*(xRgt-xLft), yTop);
  glVertex2f(xLft + 0.125*(xRgt-xLft), yBot);
  glVertex2f(xRgt - 0.125*(xRgt-xLft), yBot);
  glVertex2f(xRgt - 0.125*(xRgt-xLft), yTop);
  glEnd();
}


void VisUtils::fillStopIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
{
  glBegin(GL_POLYGON);
  glVertex2f(xLft + 0.125*(xRgt-xLft), yTop);
  glVertex2f(xLft + 0.125*(xRgt-xLft), yBot);
  glVertex2f(xRgt - 0.125*(xRgt-xLft), yBot);
  glVertex2f(xRgt - 0.125*(xRgt-xLft), yTop);
  glEnd();
}


void VisUtils::drawPrevIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
{
  glBegin(GL_LINE_LOOP);
  glVertex2f(
    xLft,
    0.5*(yTop+yBot));
  glVertex2f(
    xLft + 0.5*(xRgt-xLft),
    yBot);
  glVertex2f(
    xLft + 0.5*(xRgt-xLft),
    yTop);
  glEnd();

  glBegin(GL_LINE_LOOP);
  glVertex2f(
    xLft + 0.5*(xRgt-xLft),
    0.5*(yTop+yBot));
  glVertex2f(xRgt, yBot);
  glVertex2f(xRgt, yTop);
  glEnd();
}


void VisUtils::fillPrevIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
{
  glBegin(GL_POLYGON);
  glVertex2f(
    xLft,
    0.5*(yTop+yBot));
  glVertex2f(
    xLft + 0.5*(xRgt-xLft),
    yBot);
  glVertex2f(
    xLft + 0.5*(xRgt-xLft),
    yTop);
  glEnd();

  glBegin(GL_POLYGON);
  glVertex2f(
    xLft + 0.5*(xRgt-xLft),
    0.5*(yTop+yBot));
  glVertex2f(xRgt, yBot);
  glVertex2f(xRgt, yTop);
  glEnd();
}


void VisUtils::drawRwndIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
{
  glBegin(GL_LINE_LOOP);
  glVertex2f(xLft, yTop);
  glVertex2f(xLft, yBot);
  glVertex2f(
    xLft + 0.2*(xRgt-xLft),
    yBot);
  glVertex2f(
    xLft + 0.2*(xRgt-xLft),
    yTop);
  glEnd();

  glBegin(GL_LINE_LOOP);
  glVertex2f(
    xLft + 0.2*(xRgt-xLft),
    0.5*(yTop+yBot));
  glVertex2f(
    xLft + 0.6*(xRgt-xLft),
    yBot);
  glVertex2f(
    xLft + 0.6*(xRgt-xLft),
    yTop);
  glEnd();

  glBegin(GL_LINE_LOOP);
  glVertex2f(
    xLft + 0.6*(xRgt-xLft),
    0.5*(yTop+yBot));
  glVertex2f(xRgt, yBot);
  glVertex2f(xRgt, yTop);
  glEnd();
}


void VisUtils::fillRwndIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
{
  glBegin(GL_POLYGON);
  glVertex2f(xLft, yTop);
  glVertex2f(xLft, yBot);
  glVertex2f(
    xLft + 0.2*(xRgt-xLft),
    yBot);
  glVertex2f(
    xLft + 0.2*(xRgt-xLft),
    yTop);
  glEnd();

  glBegin(GL_POLYGON);
  glVertex2f(
    xLft + 0.2*(xRgt-xLft),
    0.5*(yTop+yBot));
  glVertex2f(
    xLft + 0.6*(xRgt-xLft),
    yBot);
  glVertex2f(
    xLft + 0.6*(xRgt-xLft),
    yTop);
  glEnd();

  glBegin(GL_POLYGON);
  glVertex2f(
    xLft + 0.6*(xRgt-xLft),
    0.5*(yTop+yBot));
  glVertex2f(xRgt, yBot);
  glVertex2f(xRgt, yTop);
  glEnd();
}


void VisUtils::drawCloseIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
{
  double hori = 0.1*(xRgt-xLft);
  double vert = 0.1*(yTop-yBot);

  glBegin(GL_LINE_LOOP);
  // top left
  glVertex2f(xLft+1.5*hori, yTop);
  glVertex2f(xLft,          yTop-1.5*vert);
  // center left
  glVertex2f(
    xLft + 0.5*(xRgt-xLft)-1.5*hori,
    yBot + 0.5*(yTop-yBot));
  // bottom left
  glVertex2f(xLft,          yBot+1.5*vert);
  glVertex2f(xLft+1.5*hori, yBot);
  // center bottom
  glVertex2f(
    xLft + 0.5*(xRgt-xLft),
    yBot + 0.5*(yTop-yBot)-1.5*vert);
  // bottom right
  glVertex2f(xRgt-1.5*hori, yBot);
  glVertex2f(xRgt,          yBot+1.5*vert);
  // center right
  glVertex2f(
    xLft + 0.5*(xRgt-xLft)+1.5*hori,
    yBot + 0.5*(yTop-yBot));
  // top right
  glVertex2f(xRgt,          yTop-1.5*vert);
  glVertex2f(xRgt-1.5*hori, yTop);
  // center top
  glVertex2f(
    xLft + 0.5*(xRgt-xLft),
    yBot + 0.5*(yTop-yBot)+1.5*vert);
  glEnd();
}


void VisUtils::fillCloseIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
{
  double hori = 0.1*(xRgt-xLft);
  double vert = 0.1*(yTop-yBot);

  glBegin(GL_POLYGON);
  // top left
  glVertex2f(xLft+1.5*hori, yTop);
  glVertex2f(xLft,          yTop-1.5*vert);
  // bottom right
  glVertex2f(xRgt-1.5*hori, yBot);
  glVertex2f(xRgt,          yBot+1.5*vert);
  glEnd();

  glBegin(GL_POLYGON);
  // bottom left
  glVertex2f(xLft,          yBot+1.5*vert);
  glVertex2f(xLft+1.5*hori, yBot);
  // top right
  glVertex2f(xRgt,          yTop-1.5*vert);
  glVertex2f(xRgt-1.5*hori, yTop);
  glEnd();
}


void VisUtils::drawMoreIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
{
  glBegin(GL_LINE_LOOP);
  glVertex2f(xLft, yTop);
  glVertex2f(
    xLft,
    yTop - 0.2*(yTop-yBot));
  glVertex2f(
    xRgt,
    yTop - 0.2*(yTop-yBot));
  glVertex2f(xRgt, yTop);
  glEnd();

  glBegin(GL_LINE_LOOP);
  glVertex2f(
    xLft,
    yTop - 0.3*(yTop-yBot));
  glVertex2f(
    xLft + 0.5*(xRgt-xLft),
    yBot);
  glVertex2f(
    xRgt,
    yTop - 0.3*(yTop-yBot));
  glEnd();
}


void VisUtils::fillMoreIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
{
  glBegin(GL_POLYGON);
  glVertex2f(xLft, yTop);
  glVertex2f(
    xLft,
    yTop - 0.2*(yTop-yBot));
  glVertex2f(
    xRgt,
    yTop - 0.2*(yTop-yBot));
  glVertex2f(xRgt, yTop);
  glEnd();

  glBegin(GL_POLYGON);
  glVertex2f(
    xLft,
    yTop - 0.3*(yTop-yBot));
  glVertex2f(
    xLft + 0.5*(xRgt-xLft),
    yBot);
  glVertex2f(
    xRgt,
    yTop - 0.3*(yTop-yBot));
  glEnd();
}


void VisUtils::drawClearIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
{
  glBegin(GL_LINE_STRIP);
  glVertex2f(xLft + 0.3*(xRgt-xLft), yTop - 0.6*(yTop-yBot));
  glVertex2f(xLft + 0.3*(xRgt-xLft), yBot);
  glVertex2f(xRgt,                   yBot);
  glVertex2f(xRgt,                   yTop - 0.3*(yTop-yBot));
  glVertex2f(xLft + 0.6*(xRgt-xLft), yTop - 0.3*(yTop-yBot));
  glEnd();

  glBegin(GL_LINES);
  glVertex2f(xLft,                   yTop);
  glVertex2f(xLft + 0.6*(xRgt-xLft), yTop-0.6*(yTop-yBot));
  glVertex2f(xLft,                   yTop-0.6*(yTop-yBot));
  glVertex2f(xLft + 0.6*(xRgt-xLft), yTop);
  glEnd();
}


void VisUtils::fillClearIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
{
  glBegin(GL_POLYGON);
  glVertex2f(xLft + 0.3*(xRgt-xLft), yTop - 0.3*(yTop-yBot));
  glVertex2f(xLft + 0.3*(xRgt-xLft), yBot);
  glVertex2f(xRgt,                   yBot);
  glVertex2f(xRgt,                   yTop - 0.3*(yTop-yBot));
  glEnd();

  glBegin(GL_LINES);
  glVertex2f(xLft,                   yTop);
  glVertex2f(xLft + 0.6*(xRgt-xLft), yTop-0.6*(yTop-yBot));
  glVertex2f(xLft,                   yTop-0.6*(yTop-yBot));
  glVertex2f(xLft + 0.6*(xRgt-xLft), yTop);
  glEnd();
}


void VisUtils::drawUpIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
{
  glBegin(GL_LINE_LOOP);
  glVertex2f(
    xLft + 0.5*(xRgt-xLft),
    yTop);
  glVertex2f(
    xLft,
    yTop - 0.5*(yTop-yBot));
  glVertex2f(
    xRgt,
    yTop - 0.5*(yTop-yBot));
  glEnd();

  glBegin(GL_LINE_LOOP);
  glVertex2f(
    xLft + 0.5*(xRgt-xLft),
    yTop - 0.5*(yTop-yBot));
  glVertex2f(
    xLft,
    yBot);
  glVertex2f(
    xRgt,
    yBot);
  glEnd();
}


void VisUtils::fillUpIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
{
  glBegin(GL_POLYGON);
  glVertex2f(
    xLft + 0.5*(xRgt-xLft),
    yTop);
  glVertex2f(
    xLft,
    yTop - 0.5*(yTop-yBot));
  glVertex2f(
    xRgt,
    yTop - 0.5*(yTop-yBot));
  glEnd();

  glBegin(GL_POLYGON);
  glVertex2f(
    xLft + 0.5*(xRgt-xLft),
    yTop - 0.5*(yTop-yBot));
  glVertex2f(
    xLft,
    yBot);
  glVertex2f(
    xRgt,
    yBot);
  glEnd();
}


void VisUtils::drawDownIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
{
  glBegin(GL_LINE_LOOP);
  glVertex2f(
    xLft,
    yTop);
  glVertex2f(
    xLft + 0.5*(xRgt-xLft),
    yTop - 0.5*(yTop-yBot));
  glVertex2f(
    xRgt,
    yTop);
  glEnd();

  glBegin(GL_LINE_LOOP);
  glVertex2f(
    xLft,
    yTop - 0.5*(yTop-yBot));
  glVertex2f(
    xLft + 0.5*(xRgt-xLft),
    yBot);
  glVertex2f(
    xRgt,
    yTop - 0.5*(yTop-yBot));
  glEnd();
}


void VisUtils::fillDownIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
{
  glBegin(GL_POLYGON);
  glVertex2f(
    xLft,
    yTop);
  glVertex2f(
    xLft + 0.5*(xRgt-xLft),
    yTop - 0.5*(yTop-yBot));
  glVertex2f(
    xRgt,
    yTop);
  glEnd();

  glBegin(GL_POLYGON);
  glVertex2f(
    xLft,
    yTop - 0.5*(yTop-yBot));
  glVertex2f(
    xLft + 0.5*(xRgt-xLft),
    yBot);
  glVertex2f(
    xRgt,
    yTop - 0.5*(yTop-yBot));
  glEnd();
}


// -- transformations -----------------------------------------------

/*
void VisUtils::setTransf(
    const double &xCtr, const double &yCtr,
    const double &xDOF,  const double &yDOF,
    const double &xHge,  const double &yHge,
    const double &aglCtr )
// This function should be used when in edit mode.
{
    double dX = xCtr-xHge;
    double dY = yCtr-yHge;

    // modelview matrix
    //glMatrixMode( GL_MODELVIEW );
    glPushMatrix();

    // move to center pos & rotate
    glTranslatef( xHge, yHge, 0.0 );
    glRotatef( aglCtr, 0.0, 0.0, 1.0 );

    // move to center
    glTranslatef( dX, dY, 0.0 );
}
*/

void VisUtils::setTransf(
  const double& xCtr,   const double& yCtr,
  const double& /*xDOF*/,   const double& /*yDOF*/,
  const double& xHge,   const double& yHge,
  const double& aglCtr, const double& aglHge)
// This function should be used when in analysis mode.
{
  double dX = xCtr-xHge;
  double dY = yCtr-yHge;

  // modelview matrix
  //glMatrixMode( GL_MODELVIEW );
  glPushMatrix();

  // move to hinge pos & rotate
  glTranslatef(xHge, yHge, 0.0);
  glRotatef(aglHge, 0.0, 0.0, 1.0);

  // move to center pos & rotate
  glTranslatef(dX, dY, 0.0);
  glRotatef(aglCtr, 0.0, 0.0, 1.0);
}


void VisUtils::clrTransf()
{
  //glMatrixMode( GL_MODELVIEW );
  glPopMatrix();
}


// -- text ----------------------------------------------------------


void VisUtils::genCharTextures(
  GLuint texCharId[CHARSETSIZE],
  GLubyte texChar[CHARSETSIZE][CHARHEIGHT* CHARWIDTH])
{
  // allocate memory
  glGenTextures(CHARSETSIZE, texCharId);

  // create textures
  for (int i = 0; i < CHARSETSIZE; ++i)
  {
    // bind textures
    glBindTexture(GL_TEXTURE_2D, texCharId[i]);
    // create image
    QImage image(characters[i]);

    int red = 0;
    int green = 0;
    int blue = 0;

    // read in texture
    int count = 0;
    for (int h = 0; h < CHARHEIGHT; ++h)
    {
      for (int w = 0; w < CHARWIDTH; ++w)
      {
        QColor pixel(image.pixel(w, h));
        red   = (GLubyte)pixel.red();
        green = (GLubyte)pixel.green();
        blue  = (GLubyte)pixel.blue();

        texChar[i][count] = (GLubyte)(255.0-(red+green+blue)/3.0);

        ++count;
      }
    }

    gluBuild2DMipmaps(
      GL_TEXTURE_2D,
      GL_ALPHA,
      16,
      32,
      GL_ALPHA,
      GL_UNSIGNED_BYTE,
      texChar[i]);
  }
}


int VisUtils::getCharIdx(const char& c)
{
  int result = 80;
  int asci = (int) c;

  // lowercase alphabet
  if (97 <= asci && asci <= 122)
  {
    result = asci - 97;
  }
  // uppercase alphabet
  else if (65 <= asci && asci <= 90)
  {
    result = asci - 65 + 26;
  }
  // numbers 0 -- 9
  else if (48 <= asci && asci <= 58)
  {
    result = asci - 48 + 52;
  }
  // other characters
  else
  {
    switch (c)
    {
      case ' ':
        result = 62;
        break;
      case '_':
        result = 63;
        break;
      case '-':
        result = 64;
        break;
      case '|':
        result = 65;
        break;
      case '/':
        result = 66;
        break;
      case '\\':
        result = 67;
        break;
      case '"':
        result = 68;
        break;
      case '\'':
        result = 69;
        break;
      case ':':
        result = 70;
        break;
      case ';':
        result = 71;
        break;
      case '.':
        result = 72;
        break;
      case ',':
        result = 73;
        break;
      case '(':
        result = 74;
        break;
      case ')':
        result = 75;
        break;
      case '[':
        result = 76;
        break;
      case ']':
        result = 77;
        break;
      case '{':
        result = 78;
        break;
      case '}':
        result = 79;
        break;
      case '?':
        result = 80;
        break;
      case '<':
        result = 81;
        break;
      case '>':
        result = 82;
        break;
      case '+':
        result = 83;
        break;
      case '=':
        result = 84;
        break;
      case '*':
        result = 85;
        break;
      case '&':
        result = 86;
        break;
      case '^':
        result = 87;
        break;
      case '%':
        result = 88;
        break;
      case '$':
        result = 89;
        break;
      case '#':
        result = 90;
        break;
      case '@':
        result = 91;
        break;
      case '!':
        result = 92;
        break;
      case '`':
        result = 93;
        break;
      case '~':
        result = 94;
        break;
      default :
        result = 80;
        break;

    }
  }
  return result;
}


void VisUtils::drawLabel(
  GLuint texCharId[CHARSETSIZE],
  const double& xCoord,
  const double& yCoord,
  const double& scaling,
  const std::string& label)
{
  if (label.size() > 0)
  {
    // enable texture mapping
    glEnable(GL_TEXTURE_2D);

    for (size_t i = 0; i < label.length(); ++i)
    {
      double xLft = xCoord + i*scaling*CHARWIDTH;
      double xRgt = xCoord + (i+1)*scaling*CHARWIDTH;
      double yTop = yCoord + 0.5*scaling*CHARHEIGHT;
      double yBot = yCoord - 0.5*scaling*CHARHEIGHT;

      // bind textures
      glBindTexture(GL_TEXTURE_2D, texCharId[ getCharIdx(label[i]) ]);

      // set texture mapping parameters
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      /*
      glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
      glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
      */
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glBegin(GL_QUADS);
      glTexCoord2f(0.0, 0.0);
      glVertex3f(xLft, yTop, 0.5);
      glTexCoord2f(0.0, 1.0);
      glVertex3f(xLft, yBot, 0.5);
      glTexCoord2f(1.0, 1.0);
      glVertex3f(xRgt, yBot, 0.5);
      glTexCoord2f(1.0, 0.0);
      glVertex3f(xRgt, yTop, 0.5);
      glEnd();

      glDisable(GL_BLEND);
    }

    glDisable(GL_TEXTURE_2D);
  }
}


void VisUtils::drawLabelRight(
  GLuint texCharId[CHARSETSIZE],
  const double& xCoord,
  const double& yCoord,
  const double& scaling,
  const std::string& label)
{
  drawLabel(
    texCharId,
    xCoord,
    yCoord,
    scaling,
    label);
}


void VisUtils::drawLabelLeft(
  GLuint texCharId[CHARSETSIZE],
  const double& xCoord,
  const double& yCoord,
  const double& scaling,
  const std::string& label)
{
  double translate = label.length()*CHARWIDTH*scaling;
  drawLabel(
    texCharId,
    xCoord-translate,
    yCoord,
    scaling,
    label);
}


void VisUtils::drawLabelCenter(
  GLuint texCharId[CHARSETSIZE],
  const double& xCoord,
  const double& yCoord,
  const double& scaling,
  const std::string& label)
{
  double translate = 0.5*label.length()*CHARWIDTH*scaling;
  drawLabel(
    texCharId,
    xCoord-translate,
    yCoord,
    scaling,
    label);
}


void VisUtils::drawLabelVert(
  GLuint texCharId[CHARSETSIZE],
  const double& xCoord,
  const double& yCoord,
  const double& scaling,
  const std::string& label)
{
  if (label.size() > 0)
  {
    // enable texture mapping
    glEnable(GL_TEXTURE_2D);

    for (size_t i = 0; i < label.length(); ++i)
    {
      double xLft = xCoord - 0.5*scaling*CHARHEIGHT;
      double xRgt = xCoord + 0.5*scaling*CHARHEIGHT;
      double yTop = yCoord + (i+1)*scaling*CHARWIDTH;
      double yBot = yCoord + i*scaling*CHARWIDTH;

      // bind textures
      glBindTexture(GL_TEXTURE_2D, texCharId[ getCharIdx(label[i]) ]);

      // set texture mapping parameters
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glBegin(GL_QUADS);
      glTexCoord2f(0.0, 0.0);
      glVertex2f(xLft, yBot);
      glTexCoord2f(0.0, 1.0);
      glVertex2f(xRgt, yBot);
      glTexCoord2f(1.0, 1.0);
      glVertex2f(xRgt, yTop);
      glTexCoord2f(1.0, 0.0);
      glVertex2f(xLft, yTop);
      glEnd();

      glDisable(GL_BLEND);
    }

    glDisable(GL_TEXTURE_2D);
  }
}


void VisUtils::drawLabelVertAbove(
  GLuint texCharId[CHARSETSIZE],
  const double& xCoord,
  const double& yCoord,
  const double& scaling,
  const std::string& label)
{
  drawLabelVert(
    texCharId,
    xCoord,
    yCoord,
    scaling,
    label);
}


void VisUtils::drawLabelVertBelow(
  GLuint texCharId[CHARSETSIZE],
  const double& xCoord,
  const double& yCoord,
  const double& scaling,
  const std::string& label)
{
  double translate = label.length()*CHARWIDTH*scaling;
  drawLabelVert(
    texCharId,
    xCoord,
    yCoord - translate,
    scaling,
    label);
}


void VisUtils::drawLabelVertCenter(
  GLuint texCharId[CHARSETSIZE],
  const double& xCoord,
  const double& yCoord,
  const double& scaling,
  const std::string& label)
{
  double translate = 0.5*label.length()*CHARWIDTH*scaling;
  drawLabelVert(
    texCharId,
    xCoord,
    yCoord-translate,
    scaling,
    label);
}


void VisUtils::drawLabelInBoundBox(
  GLuint texCharId[CHARSETSIZE],
  const double& xLft,
  const double& xRgt,
  const double& yTop,
  const double& yBot,
  const double& scaling,
  const std::string& label)
{
  double w = xRgt - xLft;
  double h = yTop - yBot;
  double r = w/h;

  double charWidth = (CHARWIDTH*scaling);
  double lblLength = label.size()*charWidth;
  std::string cropLbl = label;

  if (r >= 1.0)   // longer than tall or short label
  {
    int numToCrop = (int)ceil((lblLength-w)/charWidth);
    if (0 < numToCrop && static_cast <size_t>(numToCrop) < cropLbl.size())
    {
      ssize_t eraseSize = cropLbl.size() - numToCrop;
      if (eraseSize > 0)
      {
        cropLbl.erase(eraseSize);
        cropLbl.append(".");
      }
    }

    drawLabelCenter(
      texCharId,
      xLft + (xRgt-xLft)/2.0,
      yBot + (yTop-yBot)/2.0,
      scaling,
      cropLbl);
  }
  else // taller than long
  {
    int numToCrop = (int)ceil((lblLength-h)/charWidth);
    if (0 < numToCrop && static_cast  <size_t>(numToCrop) < cropLbl.size())
    {
      ssize_t eraseSize = cropLbl.size() - numToCrop - 2;
      if (eraseSize > 2)
      {
        cropLbl.erase(eraseSize);
        cropLbl.append("..");
      }
    }

    if (cropLbl.size() > 2)
    {
      drawLabelVertCenter(
        texCharId,
        xLft + (xRgt-xLft)/2.0,
        yBot + (yTop-yBot)/2.0,
        scaling,
        cropLbl);
    }
    else
    {
      drawLabelCenter(
        texCharId,
        xLft + (xRgt-xLft)/2.0,
        yBot + (yTop-yBot)/2.0,
        scaling,
        cropLbl);
    }
  }
}


// -- cushions ------------------------------------------------------


void VisUtils::genCushTextures(
  GLuint& texCushId,
  float texCush[CUSHSIZE])
{
  glGenTextures(1, &texCushId);
  glBindTexture(GL_TEXTURE_1D, texCushId);

  // define texture
  for (int i = 0; i < CUSHSIZE; ++i)
  {
    texCush[i] = cushionProfile1D(
                   (float)i,
                   CUSHSIZE,
                   (float)cushCurve*CUSHSIZE,
                   cushAngle,
                   cushDepth);
  }

  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);

  glTexImage1D(
    GL_TEXTURE_1D,
    0,
    GL_LUMINANCE,
    CUSHSIZE,
    0,
    GL_LUMINANCE,
    GL_FLOAT,
    texCush);
}


float VisUtils::cushionProfile1D(
  const float& x,
  const float& D,
  const float& h,
  const float& alpha,
  const float& l_ratio)
// Code adapted from Lucian Voinea.
{
  double pi = 3.1415926535;
  float alphaNew = (float)(pi*alpha)/180;

  float a= -(float)(4*h)/(D*D);
  float b= (float)(4*h)/D;
  float r = -(2.0*a*x+b)*cos(alphaNew)+sin(alphaNew);
  r= (float)r/pow(4.0f*a*a*x*x+4.0f*a*b*x+b*b+1.0f,0.5f);
  if (r>1)
  {
    r=1;
  }
  return (r*l_ratio);
}


void VisUtils::drawCushDiag(
  const GLuint& texCushId,
  const double& xLft,
  const double& xRgt,
  const double& yTop,
  const double& yBot)
{
  // enable texture mapping
  glEnable(GL_TEXTURE_1D);
  glBindTexture(GL_TEXTURE_1D, texCushId);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  // map texture
  glBegin(GL_QUADS);
  glTexCoord1f(0.5);
  glVertex2f(xLft, yTop);
  glTexCoord1f(0.0);
  glVertex2f(xLft, yBot);
  glTexCoord1f(0.5);
  glVertex2f(xRgt, yBot);
  glTexCoord1f(1.0);
  glVertex2f(xRgt, yTop);
  glEnd();

  // disable texture mapping
  glDisable(GL_TEXTURE_1D);
}


void VisUtils::drawCushHori(
  const GLuint& texCushId,
  const double& xLft,
  const double& xRgt,
  const double& yTop,
  const double& yBot)
{
  // enable texture mapping
  glEnable(GL_TEXTURE_1D);
  glBindTexture(GL_TEXTURE_1D, texCushId);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  // map texture
  glBegin(GL_QUADS);
  glTexCoord1f(0.0);
  glVertex2f(xLft, yTop);
  glTexCoord1f(0.0);
  glVertex2f(xLft, yBot);
  glTexCoord1f(1.0);
  glVertex2f(xRgt, yBot);
  glTexCoord1f(1.0);
  glVertex2f(xRgt, yTop);
  glEnd();

  // disable texture mapping
  glDisable(GL_TEXTURE_1D);
}


void VisUtils::drawCushVert(
  const GLuint& texCushId,
  const double& xLft,
  const double& xRgt,
  const double& yTop,
  const double& yBot)
{
  // enable texture mapping
  glEnable(GL_TEXTURE_1D);
  glBindTexture(GL_TEXTURE_1D, texCushId);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  // map texture
  glBegin(GL_QUADS);
  glTexCoord1f(1.0);
  glVertex2f(xLft, yTop);
  glTexCoord1f(0.0);
  glVertex2f(xLft, yBot);
  glTexCoord1f(0.0);
  glVertex2f(xRgt, yBot);
  glTexCoord1f(1.0);
  glVertex2f(xRgt, yTop);
  glEnd();

  // disable texture mapping
  glDisable(GL_TEXTURE_1D);
}


// -- end -----------------------------------------------------------
