// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./visutils.h

#ifndef VISUTILS_H
#define VISUTILS_H

#include <QtCore>
#include <QtGui>

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
#include <string>
#include "utils.h"


static inline QColor alpha(QColor color, double alpha) { color.setAlphaF(alpha); return color; }

// -- structs for positions -----------------------------------------

struct Position3D
{
  double x;
  double y;
  double z;
};

struct Position2D
{
  double x;
  double y;
};


// -- data for character textures -----------------------------------

const int CHARSETSIZE = 95;
const int CHARWIDTH   = 16;
const int CHARHEIGHT  = 32;

// -- data for cushion textures -------------------------------------

const int CUSHSIZE    = 256;

QColor interpolateRgb(QColor from, QColor to, float t);
QColor interpolateHsv(QColor from, QColor to, float t, bool longPath = false);

class VisUtils
{
  template<class T> struct ColorMap
  {
    QColor operator()(int numerator, int denominator) const { return T::operator()(numerator / (double)denominator); }
  };

  class ListColorMap
  {
    public:
      ListColorMap(QList<QColor> colors): m_colors(QVector<QColor>::fromList(colors)) {}
      QColor operator()(double fraction) const;
      QColor operator()(int numerator, int denominator) const;

    private:
      QVector<QColor> m_colors;
  };

  public:
    static void clear(QColor col);

    // -- anti-aliasing & blending ----------------------------------
    static void enableLineAntiAlias();
    static void disableLineAntiAlias();
    static void enableBlending();
    static void disableBlending();

    // -- line width ------------------------------------------------
    static void setLineWidth(const double& px);

    // -- color -----------------------------------------------------
    static void setValidColor(QColor color) { if (color.isValid()) setColor(color); }
    static void setColor(QColor color) { setColor(color, color.alphaF()); }
    static void setColor(QColor color, double alpha) { glColor4f(color.redF(), color.greenF(), color.blueF(), alpha); }

    static const QRgb coolBlue = 0x7097d9;
    static const QRgb darkCoolBlue = 0x004fd9;
    static const QRgb coolGreen = 0x6bcc51;
    static const QRgb lightCoolGreen = 0xbaf3ab;
    static const QRgb lightLightGray = 0xf3f3f3;
    static const QRgb lightGray = 0xbfbfbf;
    static const QRgb mediumGray = 0x808080;
    static const QRgb darkGray = 0x404040;
    static const QRgb orange = 0xff8000;
    static const QRgb coolRed = 0xf54000;

    static const struct BlueYellow: public ColorMap<BlueYellow> { BlueYellow(){} QColor operator()(double fraction) const; } blueYellow;
    static const struct Spectral: public ColorMap<Spectral> { Spectral(){} QColor operator()(double fraction) const; } spectral;

    static const ListColorMap grayScale;
    static const ListColorMap qualPast1;
    static const ListColorMap qualPast2;
    static const ListColorMap qualSet1;
    static const ListColorMap qualSet2;
    static const ListColorMap qualSet3;
    static const ListColorMap qualPair;
    static const ListColorMap qualDark;
    static const ListColorMap qualAccent;
    static const ListColorMap seqGreen;
    static const ListColorMap seqRed;

    // -- drawing functions -----------------------------------------
    static void drawLine(
      const double& xFr, const double& xTo,
      const double& yFr, const double& yTo);
    static void drawLineDashed(
      const double& xFr, const double& xTo,
      const double& yFr, const double& yTo);

    static void drawArc(
      const double& xCtr,     const double& yCtr,
      const double& aglBegDg, const double& aglEndDg,
      const double& radius,   const int& slices);
    static void drawArcDashed(
      const double& xCtr,     const double& yCtr,
      const double& aglBegDg, const double& aglEndDg,
      const double& radius,   const int& slices);
    static void drawArcCW(
      const double& xCtr,     const double& yCtr,
      const double& aglBegDg, const double& aglEndDg,
      const double& radius,   const int& slices);
    static void drawArcDashedCW(
      const double& xCtr,     const double& yCtr,
      const double& aglBegDg, const double& aglEndDg,
      const double& radius,   const int& slices);
    static void drawArc(
      const double& xCtr,       const double& yCtr,
      const double& aglBegDg,   const double& aglEndDg,
      const double& wthBeg,     const double& wthEnd,
      const double& radius,     const int& slices,
      QColor colBeg = QColor(), QColor colEnd = QColor());
    static void fillArc(
      const double& xCtr,       const double& yCtr,
      const double& aglBegDg,   const double& aglEndDg,
      const double& wthBeg,     const double& wthEnd,
      const double& radius,     const int& slices,
      QColor colBeg = QColor(), QColor colEnd = QColor());

    static void drawTriangle(
      const double&   x1,     const double& y1,
      const double&   x2,     const double& y2,
      const double&   x3,     const double& y3,
      QColor col1 = QColor(), QColor col2 = QColor(), QColor col3 = QColor());
    static void fillTriangle(
      const double&   x1,     const double& y1,
      const double&   x2,     const double& y2,
      const double&   x3,     const double& y3,
      QColor col1 = QColor(), QColor col2 = QColor(), QColor col3 = QColor());

    static void drawRect(
      const double& xLft,          const double& xRgt,
      const double& yTop,          const double& yBot,
      QColor colTopLft = QColor(), QColor colTopRgt = QColor(),
      QColor colBotLft = QColor(), QColor colBotRgt = QColor());
    static void fillRect(
      const double& xLft,          const double& xRgt,
      const double& yTop,          const double& yBot,
      QColor colTopLft = QColor(), QColor colTopRgt = QColor(),
      QColor colBotLft = QColor(), QColor colBotRgt = QColor());

    static void drawEllipse(
      const double& xCtr, const double& yCtr,
      const double& xDOF, const double& yDOF,
      const int& slices);
    static void fillEllipse(
      const double& xCtr, const double& yCtr,
      const double& xDOF, const double& yDOF,
      const int& slices);
    static void fillEllipse(
      const double&   xCtr,    const double&   yCtr,
      const double&   xDOFIn,  const double&   yDOFIn,
      const double&   xDOFOut, const double&   yDOFOut,
      const double&   slices,  QColor cIn, QColor cOut);
    static void fillEllipse(
      const double&   xCtr,     const double&   yCtr,
      const double&   xDOFIn,   const double&   yDOFIn,
      const double&   xDOFOut,  const double&   yDOFOut,
      const double&   aglBegDg, const double& aglEndDg,
      const double&   slices,   QColor cIn, QColor cOut);

    static void drawArrow(
      const double& xFr,     const double& xTo,
      const double& yFr,     const double& yTo,
      const double& wHead, const double& lHead);
    static void drawArrow(
      const double& xFr,   const double&   xTo,
      const double& yFr,   const double&   yTo,
      const double& wBase, const double&   wHead,
      const double& lHead, QColor cFr = QColor(), QColor cTo = QColor());
    static void fillArrow(
      const double& xFr,     const double& xTo,
      const double& yFr,     const double& yTo,
      const double& wHead, const double& lHead);
    static void fillArrow(
      const double& xFr,   const double& xTo,
      const double& yFr,   const double& yTo,
      const double& wBase, const double& wHead,
      const double& lHead, QColor cFr = QColor(), QColor cTo = QColor());

    static void drawDArrow(
      const double& xFr,   const double& xTo,
      const double& yFr,   const double& yTo,
      const double& wHead, const double& lHead);
    static void fillDArrow(
      const double& xFr,   const double& xTo,
      const double& yFr,   const double& yTo,
      const double& wHead, const double& lHead);

    static void drawArrowArcCW(
      const double& xCtr,     const double& yCtr,
      const double& aglBegDg, const double& aglEndDg,
      const double& radius,   const double& slices,
      const double& wHead,    const double& lHead);
    static void fillArrowArcCW(
      const double& xCtr,     const double& yCtr,
      const double& aglBegDg, const double& aglEndDg,
      const double& radius,   const double& slices,
      const double& wHead,    const double& lHead);
    static void drawArrowArcCCW(
      const double& xCtr,     const double& yCtr,
      const double& aglBegDg, const double& aglEndDg,
      const double& radius,   const double& slices,
      const double& wHead,    const double& lHead);
    static void fillArrowArcCCW(
      const double& xCtr,     const double& yCtr,
      const double& aglBegDg, const double& aglEndDg,
      const double& radius,   const double& slices,
      const double& wHead,    const double& lHead);

    static void drawFwrdIcon(
      const double& xLft, const double& xRgt,
      const double& yTop, const double& yBot);
    static void fillFwrdIcon(
      const double& xLft, const double& xRgt,
      const double& yTop, const double& yBot);
    static void drawNextIcon(
      const double& xLft, const double& xRgt,
      const double& yTop, const double& yBot);
    static void fillNextIcon(
      const double& xLft, const double& xRgt,
      const double& yTop, const double& yBot);
    static void drawPauseIcon(
      const double& xLft, const double& xRgt,
      const double& yTop, const double& yBot);
    static void fillPauseIcon(
      const double& xLft, const double& xRgt,
      const double& yTop, const double& yBot);
    static void drawPlayIcon(
      const double& xLft, const double& xRgt,
      const double& yTop, const double& yBot);
    static void fillPlayIcon(
      const double& xLft, const double& xRgt,
      const double& yTop, const double& yBot);
    static void drawStopIcon(
      const double& xLft, const double& xRgt,
      const double& yTop, const double& yBot);
    static void fillStopIcon(
      const double& xLft, const double& xRgt,
      const double& yTop, const double& yBot);
    static void drawPrevIcon(
      const double& xLft, const double& xRgt,
      const double& yTop, const double& yBot);
    static void fillPrevIcon(
      const double& xLft, const double& xRgt,
      const double& yTop, const double& yBot);
    static void drawRwndIcon(
      const double& xLft, const double& xRgt,
      const double& yTop, const double& yBot);
    static void fillRwndIcon(
      const double& xLft, const double& xRgt,
      const double& yTop, const double& yBot);
    static void drawCloseIcon(
      const double& xLft, const double& xRgt,
      const double& yTop, const double& yBot);
    static void fillCloseIcon(
      const double& xLft, const double& xRgt,
      const double& yTop, const double& yBot);
    static void drawMoreIcon(
      const double& xLft, const double& xRgt,
      const double& yTop, const double& yBot);
    static void fillMoreIcon(
      const double& xLft, const double& xRgt,
      const double& yTop, const double& yBot);
    static void drawClearIcon(
      const double& xLft, const double& xRgt,
      const double& uTop, const double& yBot);
    static void fillClearIcon(
      const double& xLft, const double& xRgt,
      const double& uTop, const double& yBot);
    static void drawUpIcon(
      const double& xLft, const double& xRgt,
      const double& yTop, const double& yBot);
    static void fillUpIcon(
      const double& xLft, const double& xRgt,
      const double& yTop, const double& yBot);
    static void drawDownIcon(
      const double& xLft, const double& xRgt,
      const double& yTop, const double& yBot);
    static void fillDownIcon(
      const double& xLft, const double& xRgt,
      const double& yTop, const double& yBot);

    // -- transformations -------------------------------------------
    /*
    static void setTransf(
        const double &xCtr, const double &yCtr,
        const double &xDOF, const double &yDOF,
        const double &xHge, const double &yHge,
        const double &aglCtr );
    */
    static void setTransf(
      const double& xCtr,   const double& yCtr,
      const double& xDOF,   const double& yDOF,
      const double& xHge,   const double& yHge,
      const double& aglCtr, const double& aglHge);  // remove

    static void clrTransf();

    // -- text ------------------------------------------------------
    static void genCharTextures(
      GLuint texCharId[CHARSETSIZE],
      GLubyte texChar[CHARSETSIZE][CHARHEIGHT* CHARWIDTH]);
    static int getCharIdx(const char& c);

    static void drawLabel(
      GLuint texCharId[CHARSETSIZE],
      const double& xCoord,
      const double& yCoord,
      const double& scaling,
      const std::string& label);
    static void drawLabelRight(
      GLuint texCharId[CHARSETSIZE],
      const double& xCoord,
      const double& yCoord,
      const double& scaling,
      const std::string& label);
    static void drawLabelLeft(
      GLuint texCharId[CHARSETSIZE],
      const double& xCoord,
      const double& yCoord,
      const double& scaling,
      const std::string& label);
    static void drawLabelCenter(
      GLuint texCharId[CHARSETSIZE],
      const double& xCoord,
      const double& yCoord,
      const double& scaling,
      const std::string& label);

    static void drawLabelVert(
      GLuint texCharId[CHARSETSIZE],
      const double& xCoord,
      const double& yCoord,
      const double& scaling,
      const std::string& label);
    static void drawLabelVertAbove(
      GLuint texCharId[CHARSETSIZE],
      const double& xCoord,
      const double& yCoord,
      const double& scaling,
      const std::string& label);
    static void drawLabelVertBelow(
      GLuint texCharId[CHARSETSIZE],
      const double& xCoord,
      const double& yCoord,
      const double& scaling,
      const std::string& label);
    static void drawLabelVertCenter(
      GLuint texCharId[CHARSETSIZE],
      const double& xCoord,
      const double& yCoord,
      const double& scaling,
      const std::string& label);

    static void drawLabelInBoundBox(
      GLuint texCharId[CHARSETSIZE],
      const double& xLft,
      const double& xRgt,
      const double& yTop,
      const double& yBot,
      const double& scaling,
      const std::string& label);

    // -- cushions --------------------------------------------------

    static void genCushTextures(
      GLuint& texCushId,
      float texCush[CUSHSIZE]);
    static float cushionProfile1D(
      const float& x,
      const float& D,
      const float& h,
      const float& alpha,
      const float& l_ratio);

    static void drawCushDiag(
      const GLuint& texCushId,
      const double& xLft,
      const double& xRgt,
      const double& yTop,
      const double& yBot);
    static void drawCushHori(
      const GLuint& texCushId,
      const double& xLft,
      const double& xRgt,
      const double& yTop,
      const double& yBot);
    static void drawCushVert(
      const GLuint& texCushId,
      const double& xLft,
      const double& xRgt,
      const double& yTop,
      const double& yBot);

    enum
    {
      COL_MAP_QUAL_PAST_1,
      COL_MAP_QUAL_PAST_2,
      COL_MAP_QUAL_SET_1,
      COL_MAP_QUAL_SET_2,
      COL_MAP_QUAL_SET_3,
      COL_MAP_QUAL_PAIR,
      COL_MAP_QUAL_DARK,
      COL_MAP_QUAL_ACCENT,
      BLEND_HARD,
      BLEND_LINEAR,
      BLEND_CONCAVE,
      BLEND_CONVEX,
      BLEND_OSCILLATE
    };

  private:
    // -- cushion textures ------------------------------------------
    static float   cushCurve;
    static float   cushAngle;
    static float   cushDepth;
};

#endif

// -- end -----------------------------------------------------------
