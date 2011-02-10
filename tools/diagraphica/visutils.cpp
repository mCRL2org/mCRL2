// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./visutils.cpp

#include "wx.hpp" // precompiled headers

#include "visutils.h"
#include "character_set.xpm"


// -- init static variables -----------------------------------------


float   VisUtils::cushCurve =  0.3f;
float   VisUtils::cushAngle = 70.0f;
float   VisUtils::cushDepth =  1.0f;


// -- clear canvas --------------------------------------------------


// ----------------------------------------
void VisUtils::clear(const ColorRGB& col)
// ----------------------------------------
{
  glClearColor(
    col.r,
    col.g,
    col.b,
    col.a);
  glClear(
    GL_COLOR_BUFFER_BIT |
    GL_DEPTH_BUFFER_BIT);

}


// -- color ---------------------------------------------------------


// ----------------------------------------------
void VisUtils::setColor(const ColorRGB& colRGB)
// ----------------------------------------------
{
  glBegin(GL_POINTS);
  glColor4f(colRGB.r, colRGB.g, colRGB.b, colRGB.a);
  glEnd();
}


// ---------------------------
void VisUtils::setColorBlack()
// ---------------------------
{
  ColorRGB col;
  col.r = 0.0;
  col.g = 0.0;
  col.b = 0.0;
  col.a = 1.0;
  setColor(col);
}


// --------------------------
void VisUtils::setColorBlue()
// --------------------------
{
  ColorRGB col;
  col.r = 0.0;
  col.g = 0.0;
  col.b = 1.0;
  col.a = 1.0;
  setColor(col);
}


// ------------------------------
void VisUtils::setColorCoolBlue()
// ------------------------------
{
  ColorRGB col;
  col.r = 0.44;
  col.g = 0.59;
  col.b = 0.85;
  col.a = 1.0;
  setColor(col);
}


// --------------------------------
void VisUtils::setColorDkCoolBlue()
// --------------------------------
{
  ColorRGB col;
  col.r = 0.00;
  col.g = 0.31;
  col.b = 0.85;
  col.a = 1.0;
  setColor(col);
}


// ---------------------------
void VisUtils::setColorGreen()
// ---------------------------
{
  ColorRGB col;
  col.r = 0.0;
  col.g = 1.0;
  col.b = 0.0;
  col.a = 1.0;
  setColor(col);
}


// -------------------------------
void VisUtils::setColorCoolGreen()
// -------------------------------
{
  ColorRGB col;
  col.r = 0.42;//0.45;
  col.g = 0.80;//0.85;
  col.b = 0.32;//0.35;
  col.a = 1.0;
  setColor(col);
}


// ---------------------------------
void VisUtils::setColorLtCoolGreen()
// ---------------------------------
{
  ColorRGB col;
  col.r = 0.73;
  col.g = 0.95;
  col.b = 0.67;
  col.a = 1.0;
  setColor(col);
}


// ------------------------------
void VisUtils::setColorLtLtGray()
// ------------------------------
{
  ColorRGB col;
  col.r = 0.95;
  col.g = 0.95;
  col.b = 0.95;
  col.a = 1.0;
  setColor(col);
}


// ----------------------------
void VisUtils::setColorLtGray()
// ----------------------------
{
  ColorRGB col;
  col.r = 0.75;
  col.g = 0.75;
  col.b = 0.75;
  col.a = 1.0;
  setColor(col);
}


// ----------------------------
void VisUtils::setColorMdGray()
// ----------------------------
{
  ColorRGB col;
  col.r = 0.5;
  col.g = 0.5;
  col.b = 0.5;
  col.a = 1.0;
  setColor(col);
}


// ----------------------------
void VisUtils::setColorDkGray()
// ----------------------------
{
  ColorRGB col;
  col.r = 0.25;
  col.g = 0.25;
  col.b = 0.25;
  col.a = 1.0;
  setColor(col);
}


// ----------------------------
void VisUtils::setColorOrange()
// ----------------------------
{
  ColorRGB col;
  col.r = 1.0;
  col.g = 0.5;
  col.b = 0.0;
  col.a = 1.0;
  setColor(col);
}


// -------------------------
void VisUtils::setColorRed()
// -------------------------
{
  ColorRGB col;
  col.r = 1.0;
  col.g = 0.0;
  col.b = 0.0;
  col.a = 1.0;
  setColor(col);
}


// -----------------------------
void VisUtils::setColorCoolRed()
// -----------------------------
{
  ColorRGB col;
  col.r = 0.96;
  col.g = 0.25;
  col.b = 0.0;
  col.a = 1.0;
  setColor(col);
}


// ---------------------------
void VisUtils::setColorWhite()
// ---------------------------
{
  ColorRGB col;
  col.r = 1.0;
  col.g = 1.0;
  col.b = 1.0;
  col.a = 1.0;
  setColor(col);
}


// ----------------------------
void VisUtils::setColorYellow()
// ----------------------------
{
  ColorRGB col;
  col.r = 1.0;
  col.g = 1.0;
  col.b = 0.0;
  col.a = 1.0;
  setColor(col);
}


// ----------------------------------------------------
void VisUtils::setColorBlueYellow(const double& frac)
// ----------------------------------------------------
{
  ColorHLS colHLS;
  ColorRGB colRGB;

  if (frac < 0.5)
  {
    colHLS.h = 240.0;
    colHLS.s = 1.0 - frac*2.0;
  }
  else
  {
    colHLS.h = 60.0;
    colHLS.s = frac*2.0 - 1.0;
  }
  colHLS.l = 0.5;

  hlsToRgb(colHLS, colRGB);
  setColor(colRGB);
}


// ------------------------------------------
void VisUtils::mapColorBlack(ColorRGB& col)
// ------------------------------------------
{
  col.r = 0.0;
  col.g = 0.0;
  col.b = 0.0;
  col.a = 1.0;
}


// -----------------------------------------
void VisUtils::mapColorBlue(ColorRGB& col)
// -----------------------------------------
{
  col.r = 0.0;
  col.g = 0.0;
  col.b = 1.0;
  col.a = 1.0;
}


// ---------------------------------------------
void VisUtils::mapColorCoolBlue(ColorRGB& col)
// ---------------------------------------------
{
  col.r = 0.44;
  col.g = 0.59;
  col.b = 0.85;
  col.a = 1.0;
}


// -----------------------------------------------
void VisUtils::mapColorDkCoolBlue(ColorRGB& col)
// -----------------------------------------------
{
  col.r = 0.00;
  col.g = 0.31;
  col.b = 0.85;
  col.a = 1.0;
}


// ------------------------------------------
void VisUtils::mapColorGreen(ColorRGB& col)
// ------------------------------------------
{
  col.r = 0.0;
  col.g = 1.0;
  col.b = 0.0;
  col.a = 1.0;
}


// ----------------------------------------------
void VisUtils::mapColorCoolGreen(ColorRGB& col)
// ----------------------------------------------
{
  col.r = 0.42;//0.45;
  col.g = 0.80;//0.85;
  col.b = 0.32;//0.35;
  col.a = 1.0;
}


// ------------------------------------------------
void VisUtils::mapColorLtCoolGreen(ColorRGB& col)
// ------------------------------------------------
{
  col.r = 0.73;
  col.g = 0.95;
  col.b = 0.67;
  col.a = 1.0;
}


// ---------------------------------------------
void VisUtils::mapColorLtLtGray(ColorRGB& col)
// ---------------------------------------------
{
  col.r = 0.95;
  col.g = 0.95;
  col.b = 0.95;
  col.a = 1.0;
}


// -------------------------------------------
void VisUtils::mapColorLtGray(ColorRGB& col)
// -------------------------------------------
{
  col.r = 0.75;
  col.g = 0.75;
  col.b = 0.75;
  col.a = 1.0;
}


// -------------------------------------------
void VisUtils::mapColorMdGray(ColorRGB& col)
// -------------------------------------------
{
  col.r = 0.5;
  col.g = 0.5;
  col.b = 0.5;
  col.a = 1.0;
}


// -------------------------------------------
void VisUtils::mapColorDkGray(ColorRGB& col)
// -------------------------------------------
{
  col.r = 0.25;
  col.g = 0.25;
  col.b = 0.25;
  col.a = 1.0;
}


// -------------------------------------------
void VisUtils::mapColorOrange(ColorRGB& col)
// -------------------------------------------
{
  col.r = 1.0;
  col.g = 0.5;
  col.b = 0.0;
  col.a = 1.0;
}


// ----------------------------------------
void VisUtils::mapColorRed(ColorRGB& col)
// ----------------------------------------
{
  col.r = 1.0;
  col.g = 0.0;
  col.b = 0.0;
  col.a = 1.0;
}


// --------------------------------------------
void VisUtils::mapColorCoolRed(ColorRGB& col)
// --------------------------------------------
{
  col.r = 0.96;
  col.g = 0.25;
  col.b = 0.0;
  col.a = 1.0;
}


// ------------------------------------------
void VisUtils::mapColorWhite(ColorRGB& col)
// ------------------------------------------
{
  col.r = 1.0;
  col.g = 1.0;
  col.b = 1.0;
  col.a = 1.0;
}


// -------------------------------
void VisUtils::mapColorBlueYellow(
  const double& frac,
  ColorRGB& colRGB)
// -------------------------------
{
  ColorHLS colHLS;

  if (frac < 0.5)
  {
    colHLS.h = 240.0;
    colHLS.s = 1.0 - frac*2.0;
  }
  else
  {
    colHLS.h = 60.0;
    colHLS.s = frac*2.0 - 1.0;
  }
  colHLS.l = 0.5;

  hlsToRgb(colHLS, colRGB);
}


// ------------------------------
void VisUtils::mapColorGrayScale(
  const double& frac,
  ColorRGB& colRGB)
// ------------------------------
{
  ColorHLS colHLS;

  colHLS.h = 0.0;      // red, doesn't matter
  colHLS.s = 0.0;      // totally unsaturated, gray
  colHLS.l = frac*1.0; // interpolate between black & white

  hlsToRgb(colHLS, colRGB);
}


// -----------------------------
void VisUtils::mapColorSpectral(
  const double& frac,
  ColorRGB& colRGB)
// -----------------------------
{
  ColorHLS colHLS;

  colHLS.h = frac*360.0; // interpolate between hues
  colHLS.s = 1.0;        // fully saturated, gray
  colHLS.l = 0.5;        // fully saturated colors have 0.5 lightness

  hlsToRgb(colHLS, colRGB);
}


// ------------------------------
void VisUtils::mapColorQualPast1(
  const size_t& iter,
  const size_t& numr,
  ColorRGB& colRGB)
// ------------------------------
// ------------------------------------------------------------------
// www.colorbrewer.org
// ------------------------------------------------------------------
{
  // 9 ref colors
  double red[] = { 251, 179, 204, 222, 254, 255, 229, 253, 242 };
  double grn[] = { 180, 205, 235, 203, 217, 255, 216, 218, 242 };
  double blu[] = { 174, 227, 197, 228, 166, 204, 189, 236, 242 };

  if (numr <= 8)
  {
    colRGB.r = red[iter]/255.0;
    colRGB.g = grn[iter]/255.0;
    colRGB.b = blu[iter]/255.0;
  }
  else
  {
    double frac = (double)iter/(double)numr;
    double intPtVal;

    double dblPtVal = modf(frac*8.0, &intPtVal);

    colRGB.r = ((1.0-dblPtVal)*red[(int)intPtVal]
                + dblPtVal*red[(int)intPtVal+1])/255.0;

    colRGB.g = ((1.0-dblPtVal)*grn[(int)intPtVal]
                + dblPtVal*grn[(int)intPtVal+1])/255.0;

    colRGB.b = ((1.0-dblPtVal)*blu[(int)intPtVal]
                + dblPtVal*blu[(int)intPtVal+1])/255.0;
  }

  colRGB.a = 1.0;
}


// ------------------------------
void VisUtils::mapColorQualPast2(
  const size_t& iter,
  const size_t& numr,
  ColorRGB& colRGB)
// ------------------------------
// ------------------------------------------------------------------
// www.colorbrewer.org
// ------------------------------------------------------------------
{
  // 8 ref colors
  double red[] = { 179, 253, 203, 244, 230, 255, 241, 204 };
  double grn[] = { 226, 205, 213, 202, 245, 242, 226, 204 };
  double blu[] = { 205, 172, 232, 228, 201, 174, 204, 204 };

  if (numr <= 7)
  {
    colRGB.r = red[iter]/255.0;
    colRGB.g = grn[iter]/255.0;
    colRGB.b = blu[iter]/255.0;
  }
  else
  {
    double frac = (double)iter/(double)numr;
    double intPtVal;

    double dblPtVal = modf(frac*7.0, &intPtVal);

    colRGB.r = ((1.0-dblPtVal)*red[(int)intPtVal]
                + dblPtVal*red[(int)intPtVal+1])/255.0;

    colRGB.g = ((1.0-dblPtVal)*grn[(int)intPtVal]
                + dblPtVal*grn[(int)intPtVal+1])/255.0;

    colRGB.b = ((1.0-dblPtVal)*blu[(int)intPtVal]
                + dblPtVal*blu[(int)intPtVal+1])/255.0;
  }

  colRGB.a = 1.0;

}


// -----------------------------
void VisUtils::mapColorQualSet1(
  const size_t& iter,
  const size_t& numr,
  ColorRGB& colRGB)
// -----------------------------
// ------------------------------------------------------------------
// www.colorbrewer.org
// ------------------------------------------------------------------
{
  // 9 ref colors
  double red[] = { 288,  55,  77, 152, 255, 255, 166, 247, 153 };
  double grn[] = {  26, 126, 175,  78, 127, 255,  86, 129, 153 };
  double blu[] = {  28, 184,  74, 163,   0,  51,  40, 191, 153 };

  if (numr <= 8)
  {
    colRGB.r = red[iter]/255.0;
    colRGB.g = grn[iter]/255.0;
    colRGB.b = blu[iter]/255.0;
  }
  else
  {
    double frac = (double)iter/(double)numr;
    double intPtVal;

    double dblPtVal = modf(frac*8.0, &intPtVal);

    colRGB.r = ((1.0-dblPtVal)*red[(int)intPtVal]
                + dblPtVal*red[(int)intPtVal+1])/255.0;

    colRGB.g = ((1.0-dblPtVal)*grn[(int)intPtVal]
                + dblPtVal*grn[(int)intPtVal+1])/255.0;

    colRGB.b = ((1.0-dblPtVal)*blu[(int)intPtVal]
                + dblPtVal*blu[(int)intPtVal+1])/255.0;
  }

  colRGB.a = 1.0;

}


// -----------------------------
void VisUtils::mapColorQualSet2(
  const size_t& iter,
  const size_t& numr,
  ColorRGB& colRGB)
// -----------------------------
// ------------------------------------------------------------------
// www.colorbrewer.org
// ------------------------------------------------------------------
{
  // 8 ref colors
  double red[] = { 102, 252, 141, 231, 166, 255, 229, 179 };
  double grn[] = { 194, 141, 160, 138, 216, 217, 196, 179 };
  double blu[] = { 165,  98, 203, 195,  84,  47, 148, 179 };

  if (numr <= 7)
  {
    colRGB.r = red[iter]/255.0;
    colRGB.g = grn[iter]/255.0;
    colRGB.b = blu[iter]/255.0;
  }
  else
  {
    double frac = (double)iter/(double)numr;
    double intPtVal;

    double dblPtVal = modf(frac*7.0, &intPtVal);

    colRGB.r = ((1.0-dblPtVal)*red[(int)intPtVal]
                + dblPtVal*red[(int)intPtVal+1])/255.0;

    colRGB.g = ((1.0-dblPtVal)*grn[(int)intPtVal]
                + dblPtVal*grn[(int)intPtVal+1])/255.0;

    colRGB.b = ((1.0-dblPtVal)*blu[(int)intPtVal]
                + dblPtVal*blu[(int)intPtVal+1])/255.0;
  }

  colRGB.a = 1.0;

}


// -----------------------------
void VisUtils::mapColorQualSet3(
  const size_t& iter,
  const size_t& numr,
  ColorRGB& colRGB)
// -----------------------------
// ------------------------------------------------------------------
// www.colorbrewer.org
// ------------------------------------------------------------------
{
  // 12 ref colors
  double red[] = { 141, 255, 190, 251, 128, 253, 179, 252, 217, 188, 204, 255 };
  double grn[] = { 211, 255, 186, 128, 177, 180, 222, 205, 217, 128, 235, 237 };
  double blu[] = { 199, 179, 218, 114, 211,  98, 105, 229, 217, 189, 197, 111 };

  if (numr <= 11)
  {
    colRGB.r = red[iter]/255.0;
    colRGB.g = grn[iter]/255.0;
    colRGB.b = blu[iter]/255.0;
  }
  else
  {
    double frac = (double)iter/(double)numr;
    double intPtVal;

    double dblPtVal = modf(frac*11.0, &intPtVal);

    colRGB.r = ((1.0-dblPtVal)*red[(int)intPtVal]
                + dblPtVal*red[(int)intPtVal+1])/255.0;

    colRGB.g = ((1.0-dblPtVal)*grn[(int)intPtVal]
                + dblPtVal*grn[(int)intPtVal+1])/255.0;

    colRGB.b = ((1.0-dblPtVal)*blu[(int)intPtVal]
                + dblPtVal*blu[(int)intPtVal+1])/255.0;
  }

  colRGB.a = 1.0;
}


// -----------------------------
void VisUtils::mapColorQualPair(
  const size_t& iter,
  const size_t& numr,
  ColorRGB& colRGB)
// -----------------------------
// ------------------------------------------------------------------
// www.colorbrewer.org
// ------------------------------------------------------------------
{
  // 9 ref colors
  double red[] = { 166,  31, 178,  51, 251, 227, 253, 255, 202 };
  double grn[] = { 206, 120, 223, 160, 154,  26, 191, 127, 178 };
  double blu[] = { 227, 180, 138,  44, 153,  28, 111,   0, 214 };

  if (numr <= 8)
  {
    colRGB.r = red[iter]/255.0;
    colRGB.g = grn[iter]/255.0;
    colRGB.b = blu[iter]/255.0;
  }
  else
  {
    double frac = (double)iter/(double)numr;
    double intPtVal;

    double dblPtVal = modf(frac*8.0, &intPtVal);

    colRGB.r = ((1.0-dblPtVal)*red[(int)intPtVal]
                + dblPtVal*red[(int)intPtVal+1])/255.0;

    colRGB.g = ((1.0-dblPtVal)*grn[(int)intPtVal]
                + dblPtVal*grn[(int)intPtVal+1])/255.0;

    colRGB.b = ((1.0-dblPtVal)*blu[(int)intPtVal]
                + dblPtVal*blu[(int)intPtVal+1])/255.0;
  }

  colRGB.a = 1.0;
}


// -----------------------------
void VisUtils::mapColorQualDark(
  const size_t& iter,
  const size_t& numr,
  ColorRGB& colRGB)
// -----------------------------
// ------------------------------------------------------------------
// www.colorbrewer.org
// ------------------------------------------------------------------
{
  // 8 ref colors
  double red[] = { 27, 217, 117, 231, 102, 230, 166, 102 };
  double grn[] = { 158, 95, 112,  41, 166, 171, 118, 102 };
  double blu[] = { 119,  2, 179, 138,  30,   2,  29, 102 };

  if (numr <= 7)
  {
    colRGB.r = red[iter]/255.0;
    colRGB.g = grn[iter]/255.0;
    colRGB.b = blu[iter]/255.0;
  }
  else
  {
    double frac = (double)iter/(double)numr;
    double intPtVal;

    double dblPtVal = modf(frac*7.0, &intPtVal);

    colRGB.r = ((1.0-dblPtVal)*red[(int)intPtVal]
                + dblPtVal*red[(int)intPtVal+1])/255.0;

    colRGB.g = ((1.0-dblPtVal)*grn[(int)intPtVal]
                + dblPtVal*grn[(int)intPtVal+1])/255.0;

    colRGB.b = ((1.0-dblPtVal)*blu[(int)intPtVal]
                + dblPtVal*blu[(int)intPtVal+1])/255.0;
  }

  colRGB.a = 1.0;
}


// -------------------------------
void VisUtils::mapColorQualAccent(
  const size_t& iter,
  const size_t& numr,
  ColorRGB& colRGB)
// -------------------------------
// ------------------------------------------------------------------
// www.colorbrewer.org
// ------------------------------------------------------------------
{
  // 8 ref colors
  double red[] = { 127, 190, 253, 255,  56, 240, 191, 102, 153 };
  double grn[] = { 201, 174, 192, 255, 108,   2,  91, 102, 153 };
  double blu[] = { 127, 212, 134, 153, 176, 127,  23, 102, 153 };

  if (numr <= 7)
  {
    colRGB.r = red[iter]/255.0;
    colRGB.g = grn[iter]/255.0;
    colRGB.b = blu[iter]/255.0;
  }
  else
  {
    double frac = (double)iter/(double)numr;
    double intPtVal;

    double dblPtVal = modf(frac*7.0, &intPtVal);

    colRGB.r = ((1.0-dblPtVal)*red[(int)intPtVal]
                + dblPtVal*red[(int)intPtVal+1])/255.0;

    colRGB.g = ((1.0-dblPtVal)*grn[(int)intPtVal]
                + dblPtVal*grn[(int)intPtVal+1])/255.0;

    colRGB.b = ((1.0-dblPtVal)*blu[(int)intPtVal]
                + dblPtVal*blu[(int)intPtVal+1])/255.0;
  }

  colRGB.a = 1.0;
}


// ----------------------------
void VisUtils::mapColorSeqOrRd(
  const size_t& iter,
  const size_t& numr,
  ColorRGB& colRGB)
// ----------------------------
// ------------------------------------------------------------------
// www.colorbrewer.org
// ------------------------------------------------------------------
{
  // 9 ref colors
  double red[] = { 255, 254, 253, 253, 252, 239, 215, 179, 127 };
  double grn[] = { 247, 232, 212, 187, 141, 101,  48,   0,   0 };
  double blu[] = { 236, 200, 158, 132, 89,   72,  31,   0,   0 };

  if (numr <= 8)
  {
    colRGB.r = red[iter]/255.0;
    colRGB.g = grn[iter]/255.0;
    colRGB.b = blu[iter]/255.0;
  }
  else
  {
    double frac = (double)iter/(double)numr;
    double intPtVal;

    double dblPtVal = modf(frac*7.0, &intPtVal);

    colRGB.r = ((1.0-dblPtVal)*red[(int)intPtVal]
                + dblPtVal*red[(int)intPtVal+1])/255.0;

    colRGB.g = ((1.0-dblPtVal)*grn[(int)intPtVal]
                + dblPtVal*grn[(int)intPtVal+1])/255.0;

    colRGB.b = ((1.0-dblPtVal)*blu[(int)intPtVal]
                + dblPtVal*blu[(int)intPtVal+1])/255.0;
  }

  colRGB.a = 1.0;
}


// ----------------------------
void VisUtils::mapColorSeqGnBu(
  const size_t& iter,
  const size_t& numr,
  ColorRGB& colRGB)
// ----------------------------
// ------------------------------------------------------------------
// www.colorbrewer.org
// ------------------------------------------------------------------
{
  // 9 ref colors
  double red[] = { 247, 224, 204, 168, 123,  78,  43,   8,   8 };
  double grn[] = { 252, 243, 235, 221, 204, 179, 140, 104,  64 };
  double blu[] = { 240, 219, 197, 181, 196, 211, 190, 172, 129 };

  if (numr <= 8)
  {
    colRGB.r = red[iter]/255.0;
    colRGB.g = grn[iter]/255.0;
    colRGB.b = blu[iter]/255.0;
  }
  else
  {
    double frac = (double)iter/(double)numr;
    double intPtVal;

    double dblPtVal = modf(frac*7.0, &intPtVal);

    colRGB.r = ((1.0-dblPtVal)*red[(int)intPtVal]
                + dblPtVal*red[(int)intPtVal+1])/255.0;

    colRGB.g = ((1.0-dblPtVal)*grn[(int)intPtVal]
                + dblPtVal*grn[(int)intPtVal+1])/255.0;

    colRGB.b = ((1.0-dblPtVal)*blu[(int)intPtVal]
                + dblPtVal*blu[(int)intPtVal+1])/255.0;
  }

  colRGB.a = 1.0;
}


// -----------------------------
void VisUtils::mapColorSeqGreen(
  const size_t& iter,
  const size_t& numr,
  ColorRGB& colRGB)
// -----------------------------
// ------------------------------------------------------------------
// www.colorbrewer.org
// ------------------------------------------------------------------
{
  // 9 ref colors
  double red[] = { 247, 229, 199, 161, 116,  65,  35,   0,   0 };
  double grn[] = { 252, 245, 233, 217, 196, 171, 139, 109,  68 };
  double blu[] = { 245, 224, 192, 155, 118,  93,  69,  44,  27 };

  if (numr <= 8)
  {
    colRGB.r = red[iter]/255.0;
    colRGB.g = grn[iter]/255.0;
    colRGB.b = blu[iter]/255.0;
  }
  else
  {
    double frac = (double)iter/(double)numr;
    double intPtVal;

    double dblPtVal = modf(frac*7.0, &intPtVal);

    colRGB.r = ((1.0-dblPtVal)*red[(int)intPtVal]
                + dblPtVal*red[(int)intPtVal+1])/255.0;

    colRGB.g = ((1.0-dblPtVal)*grn[(int)intPtVal]
                + dblPtVal*grn[(int)intPtVal+1])/255.0;

    colRGB.b = ((1.0-dblPtVal)*blu[(int)intPtVal]
                + dblPtVal*blu[(int)intPtVal+1])/255.0;
  }

  colRGB.a = 1.0;
}


// -----------------------------
void VisUtils::mapColorSeqGreen(
  const double& alpha,
  ColorRGB& colRGB)
// -----------------------------
// ------------------------------------------------------------------
// www.colorbrewer.org
// ------------------------------------------------------------------
{
  // 9 ref colors
  double red[] = { 247, 229, 199, 161, 116,  65,  35,   0,   0 };
  double grn[] = { 252, 245, 233, 217, 196, 171, 139, 109,  68 };
  double blu[] = { 245, 224, 192, 155, 118,  93,  69,  44,  27 };

  double frac = alpha;
  double intPtVal;

  double dblPtVal = modf(frac*7.0, &intPtVal);

  colRGB.r = ((1.0-dblPtVal)*red[(int)intPtVal]
              + dblPtVal*red[(int)intPtVal+1])/255.0;

  colRGB.g = ((1.0-dblPtVal)*grn[(int)intPtVal]
              + dblPtVal*grn[(int)intPtVal+1])/255.0;

  colRGB.b = ((1.0-dblPtVal)*blu[(int)intPtVal]
              + dblPtVal*blu[(int)intPtVal+1])/255.0;
  colRGB.a = 1.0;
}


// ---------------------------
void VisUtils::mapColorSeqRed(
  const size_t& iter,
  const size_t& numr,
  ColorRGB& colRGB)
// ---------------------------
// ------------------------------------------------------------------
// www.colorbrewer.org
// ------------------------------------------------------------------
{
  // 9 ref colors
  double red[] = { 255, 254, 252, 252, 251, 239, 203, 165, 103 };
  double grn[] = { 245, 224, 187, 146, 106,  59,  24,  15,   0 };
  double blu[] = { 240, 210, 161, 114,  74,  44,  29,  21,  13 };

  if (numr <= 8)
  {
    colRGB.r = red[iter]/255.0;
    colRGB.g = grn[iter]/255.0;
    colRGB.b = blu[iter]/255.0;
  }
  else
  {
    double frac = (double)iter/(double)numr;
    double intPtVal;

    double dblPtVal = modf(frac*7.0, &intPtVal);

    colRGB.r = ((1.0-dblPtVal)*red[(int)intPtVal]
                + dblPtVal*red[(int)intPtVal+1])/255.0;

    colRGB.g = ((1.0-dblPtVal)*grn[(int)intPtVal]
                + dblPtVal*grn[(int)intPtVal+1])/255.0;

    colRGB.b = ((1.0-dblPtVal)*blu[(int)intPtVal]
                + dblPtVal*blu[(int)intPtVal+1])/255.0;
  }

  colRGB.a = 1.0;
}


// ---------------------------
void VisUtils::mapColorSeqRed(
  const double& alpha,
  ColorRGB& colRGB)
// ---------------------------
// ------------------------------------------------------------------
// www.colorbrewer.org
// ------------------------------------------------------------------
{
  // 9 ref colors
  double red[] = { 255, 254, 252, 252, 251, 239, 203, 165, 103 };
  double grn[] = { 245, 224, 187, 146, 106,  59,  24,  15,   0 };
  double blu[] = { 240, 210, 161, 114,  74,  44,  29,  21,  13 };

  double frac = alpha;
  double intPtVal;

  double dblPtVal = modf(frac*7.0, &intPtVal);

  colRGB.r = ((1.0-dblPtVal)*red[(int)intPtVal]
              + dblPtVal*red[(int)intPtVal+1])/255.0;

  colRGB.g = ((1.0-dblPtVal)*grn[(int)intPtVal]
              + dblPtVal*grn[(int)intPtVal+1])/255.0;

  colRGB.b = ((1.0-dblPtVal)*blu[(int)intPtVal]
              + dblPtVal*blu[(int)intPtVal+1])/255.0;
  colRGB.a = 1.0;
}


// ----------------------------------
void VisUtils::hlsToRgb(
  ColorHLS& colHLS,
  ColorRGB& colRGB)
// ----------------------------------
// ------------------------------------------------------------------
// Adapted from Foley et al., 1996.
// In:  h       in [0.0, 360.0]
//      l, s    in [0, 1]
// Out: r, g, b in [0, 1]
// ------------------------------------------------------------------
{
  if (colHLS.s == 0)
  {
    colRGB.r = colHLS.l;
    colRGB.g = colHLS.l;
    colRGB.b = colHLS.l;
  }
  else
  {
    double var2;
    if (colHLS.l < 0.5)
    {
      var2 = colHLS.l * (colHLS.l+colHLS.s);
    }
    else
    {
      var2 = (colHLS.l+colHLS.s) - (colHLS.s*colHLS.l);
    }

    double var1 = 2 * colHLS.l - var2;

    colRGB.r = hlsValue(var1, var2, colHLS.h + 120.0);
    colRGB.g = hlsValue(var1, var2, colHLS.h);
    colRGB.b = hlsValue(var1, var2, colHLS.h - 120.0);
  }

  colRGB.a = 1.0;
}


// -----------------------
double VisUtils::hlsValue(
  double var1,
  double var2,
  double hue)
// -----------------------
// ------------------------------------------------------------------
// Adapted from Foley et al., 1996.
// ------------------------------------------------------------------
{
  double result = 0.0;

  if (hue < 0.0)
  {
    hue += 360.0;
  }
  if (hue > 360.0)
  {
    hue -= 360.0;
  }

  if (hue < 60.0)
  {
    result = var1 + (var2 - var1) * hue/60.0;
  }
  else if (hue < 180.0)
  {
    result = var2;
  }
  else if (hue < 240.0)
  {
    result = var1 + (var2 - var1) * (240.0 - hue)/60.0;
  }
  else
  {
    result = var1;
  }

  return result;
}


// -- anti-aliasing & blending --------------------------------------


// ---------------------------------
void VisUtils::enableLineAntiAlias()
// ---------------------------------
{
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


// ----------------------------------
void VisUtils::disableLineAntiAlias()
// ----------------------------------
{
  glDisable(GL_LINE_SMOOTH);
  glDisable(GL_BLEND);
}


// ----------------------------
void VisUtils::enableBlending()
// ----------------------------
{
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


// -----------------------------
void VisUtils::disableBlending()
// -----------------------------
{
  glDisable(GL_BLEND);
}


// -- line width ------------------------------------------------


// --------------------------------------------
void VisUtils::setLineWidth(const double& px)
// --------------------------------------------
{
  glLineWidth(px);
}


// -- drawing functions ---------------------------------------------


// ---------------------------------------
void VisUtils::drawLine(
  const double& xFr, const double& xTo,
  const double& yFr, const double& yTo)
// ---------------------------------------
{
  glBegin(GL_LINES);
  glVertex2f(xFr, yFr);
  glVertex2f(xTo, yTo);
  glEnd();
}


// ---------------------------------------
void VisUtils::drawLineDashed(
  const double& xFr, const double& xTo,
  const double& yFr, const double& yTo)
// ---------------------------------------
{
  glLineStipple(2, 0xAAAA);
  glEnable(GL_LINE_STIPPLE);
  glBegin(GL_LINES);
  glVertex2f(xFr, yFr);
  glVertex2f(xTo, yTo);
  glEnd();
  glDisable(GL_LINE_STIPPLE);
}


// -----------------------------------------------
void VisUtils::drawArc(
  const double& xCtr,     const double& yCtr,
  const double& aglBegDg, const double& aglEndDg,
  const double& radius,   const int& slices)
// -----------------------------------------------
// ------------------------------------------------------------------
// This function draws a circular arc COUNTER CLOCKWIZE from
// 'aglBegDg' to 'aglEndDg'. This segment has radius equal to
// 'radius' and is centered at (xCtr, yCtr). This arc consists of
// 'slices' segments.
// ------------------------------------------------------------------
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


// -----------------------------------------------
void VisUtils::drawArcDashed(
  const double& xCtr,     const double& yCtr,
  const double& aglBegDg, const double& aglEndDg,
  const double& radius,   const int& slices)
// -----------------------------------------------
// ------------------------------------------------------------------
// This function draws a circular dashed arc COUNTER CLOCKWIZE from
// 'aglBegDg' to 'aglEndDg'. This segment has radius equal to
// 'radius' and is centered at (xCtr, yCtr). This arc consists of
// 'slices' segments.
// ------------------------------------------------------------------
{
  glLineStipple(2, 0xAAAA);
  glEnable(GL_LINE_STIPPLE);
  drawArc(
    xCtr,     yCtr,
    aglBegDg, aglEndDg,
    radius,   slices);
  glDisable(GL_LINE_STIPPLE);
}


// -----------------------------------------------
void VisUtils::drawArcCW(
  const double& xCtr,     const double& yCtr,
  const double& aglBegDg, const double& aglEndDg,
  const double& radius,   const int& slices)
// -----------------------------------------------
// ------------------------------------------------------------------
// This function draws a circular arc CLOCKWIZE from 'aglBegDg' to
// 'aglEndDg'. This segment has radius equal to 'radius' and is
// centered at (xCtr, yCtr). This arc consists of 'slices' segments.
// ------------------------------------------------------------------
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


// -----------------------------------------------
void VisUtils::drawArcDashedCW(
  const double& xCtr,     const double& yCtr,
  const double& aglBegDg, const double& aglEndDg,
  const double& radius,   const int& slices)
// -----------------------------------------------
// ------------------------------------------------------------------
// This function draws a circular dashed arc CLOCKWIZE from
// 'aglBegDg' to 'aglEndDg'. This segment has radius equal to
// 'radius' and is centered at (xCtr, yCtr). This arc consists of
// 'slices' segments.
// ------------------------------------------------------------------
{
  glLineStipple(2, 0xAAAA);
  glEnable(GL_LINE_STIPPLE);
  drawArcCW(
    xCtr,     yCtr,
    aglBegDg, aglEndDg,
    radius,   slices);
  glDisable(GL_LINE_STIPPLE);
}


// ------------------------------------------------
void VisUtils::drawArc(
  const double& xCtr,     const double& yCtr,
  const double& aglBegDg, const double& aglEndDg,
  const double& wthBeg,   const double& wthEnd,
  const double& radius,   const int& slices)
// ------------------------------------------------
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
      double xCur = xCtr + (radius+0.5*wthBeg+(i*interv))*cos(Utils::degrToRad(aglBegDg+i*slice));
      double yCur = yCtr + (radius+0.5*wthBeg+(i*interv))*sin(Utils::degrToRad(aglBegDg+i*slice));
      glVertex2f(xCur, yCur);
    }
  }
  // inside
  {
    for (int i = slices; i >= 0; --i)
    {
      double xCur = xCtr + (radius-0.5*wthBeg-(i*interv))*cos(Utils::degrToRad(aglBegDg+i*slice));
      double yCur = yCtr + (radius-0.5*wthBeg-(i*interv))*sin(Utils::degrToRad(aglBegDg+i*slice));
      glVertex2f(xCur, yCur);
    }
  }
  glEnd();
}


// ------------------------------------------------
void VisUtils::drawArc(
  const double& xCtr,     const double& yCtr,
  const double& aglBegDg, const double& aglEndDg,
  const double& wthBeg,   const double& wthEnd,
  const ColorRGB& colBeg, const ColorRGB& colEnd,
  const double& radius,   const int& slices)
// ------------------------------------------------
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
      double frac = (double)i/(double)slices;
      double r = (1-frac)*colBeg.r + frac*colEnd.r;
      double g = (1-frac)*colBeg.g + frac*colEnd.g;
      double b = (1-frac)*colBeg.b + frac*colEnd.b;
      double a = (1-frac)*colBeg.a + frac*colEnd.a;
      glColor4f(r, g, b, a);

      double xCur = xCtr + (radius+0.5*wthBeg+(i*interv))*cos(Utils::degrToRad(aglBegDg+i*slice));
      double yCur = yCtr + (radius+0.5*wthBeg+(i*interv))*sin(Utils::degrToRad(aglBegDg+i*slice));
      glVertex2f(xCur, yCur);
    }
  }
  // inside
  {
    for (int i = slices; i >= 0; --i)
    {
      double frac = (double)i/(double)slices;
      double r = (1-frac)*colBeg.r + frac*colEnd.r;
      double g = (1-frac)*colBeg.g + frac*colEnd.g;
      double b = (1-frac)*colBeg.b + frac*colEnd.b;
      double a = (1-frac)*colBeg.a + frac*colEnd.a;
      glColor4f(r, g, b, a);

      double xCur = xCtr + (radius-0.5*wthBeg-(i*interv))*cos(Utils::degrToRad(aglBegDg+i*slice));
      double yCur = yCtr + (radius-0.5*wthBeg-(i*interv))*sin(Utils::degrToRad(aglBegDg+i*slice));
      glVertex2f(xCur, yCur);
    }
  }
  glEnd();
}


// ------------------------------------------------
void VisUtils::fillArc(
  const double& xCtr,     const double& yCtr,
  const double& aglBegDg, const double& aglEndDg,
  const double& wthBeg,   const double& wthEnd,
  const double& radius,   const int& slices)
// ------------------------------------------------
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


//-------------------------------------------------
void VisUtils::fillArc(
  const double& xCtr,     const double& yCtr,
  const double& aglBegDg, const double& aglEndDg,
  const double& wthBeg,   const double& wthEnd,
  const ColorRGB& colBeg, const ColorRGB& colEnd,
  const double& radius,   const int& slices)
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
      double frac = (double)i/(double)slices;
      double r = (1-frac)*colBeg.r + frac*colEnd.r;
      double g = (1-frac)*colBeg.g + frac*colEnd.g;
      double b = (1-frac)*colBeg.b + frac*colEnd.b;
      double a = (1-frac)*colBeg.a + frac*colEnd.a;
      glColor4f(r, g, b, a);

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

// -------------------------------------
void VisUtils::drawTriangle(
  const double& x1, const double& y1,
  const double& x2, const double& y2,
  const double& x3, const double& y3)
// -------------------------------------
{
  glBegin(GL_LINE_LOOP);
  glVertex2f(x1, y1);
  glVertex2f(x2, y2);
  glVertex2f(x3, y3);
  glEnd();
}


// --------------------------------------
void VisUtils::drawTriangle(
  const double&   x1, const double& y1,
  const ColorRGB& col1,
  const double&   x2, const double& y2,
  const ColorRGB& col2,
  const double&   x3, const double& y3,
  const ColorRGB& col3)
// --------------------------------------
{
  glBegin(GL_LINE_LOOP);
  glColor4f(col1.r, col1.g, col1.b, col1.a);
  glVertex2f(x1, y1);
  glColor4f(col2.r, col2.g, col2.b, col2.a);
  glVertex2f(x2, y2);
  glColor4f(col3.r, col3.g, col3.b, col3.a);
  glVertex2f(x3, y3);
  glEnd();
}


// -------------------------------------
void VisUtils::fillTriangle(
  const double& x1, const double& y1,
  const double& x2, const double& y2,
  const double& x3, const double& y3)
// -------------------------------------
{
  glBegin(GL_POLYGON);
  glVertex2f(x1, y1);
  glVertex2f(x2, y2);
  glVertex2f(x3, y3);
  glEnd();
}


// --------------------------------------
void VisUtils::fillTriangle(
  const double&   x1, const double& y1,
  const ColorRGB& col1,
  const double&   x2, const double& y2,
  const ColorRGB& col2,
  const double&   x3, const double& y3,
  const ColorRGB& col3)
// --------------------------------------
{
  glBegin(GL_POLYGON);
  glColor4f(col1.r, col1.g, col1.b, col1.a);
  glVertex2f(x1, y1);
  glColor4f(col2.r, col2.g, col2.b, col2.a);
  glVertex2f(x2, y2);
  glColor4f(col3.r, col3.g, col3.b, col3.a);
  glVertex2f(x3, y3);
  glEnd();
}


// -----------------------------------------
void VisUtils::drawRect(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
// -----------------------------------------
{
  glBegin(GL_LINE_LOOP);
  glVertex2f(xLft, yTop);
  glVertex2f(xLft, yBot);
  glVertex2f(xRgt, yBot);
  glVertex2f(xRgt, yTop);
  glEnd();
}


// -------------------------------------------------
void VisUtils::drawRect(
  const double& xLft,        const double& xRgt,
  const double& yTop,        const double& yBot,
  const ColorRGB& colTopLft, ColorRGB& colTopRgt,
  const ColorRGB& colBotLft, ColorRGB& colBotRgt)
// -------------------------------------------------
{
  glBegin(GL_LINE_LOOP);
  glColor4f(colTopLft.r, colTopLft.g, colTopLft.b, colTopLft.a);
  glVertex2f(xLft, yTop);
  glColor4f(colBotLft.r, colBotLft.g, colBotLft.b, colBotLft.a);
  glVertex2f(xLft, yBot);
  glColor4f(colBotRgt.r, colBotRgt.g, colBotRgt.b, colBotRgt.a);
  glVertex2f(xRgt, yBot);
  glColor4f(colTopRgt.r, colTopRgt.g, colTopRgt.b, colTopRgt.a);
  glVertex2f(xRgt, yTop);
  glEnd();
}


// -----------------------------------------
void VisUtils::fillRect(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
// -----------------------------------------
{
  glBegin(GL_QUADS);
  glVertex2f(xLft, yTop);
  glVertex2f(xLft, yBot);
  glVertex2f(xRgt, yBot);
  glVertex2f(xRgt, yTop);
  glEnd();
}


// -------------------------------------------------
void VisUtils::fillRect(
  const double& xLft,        const double& xRgt,
  const double& yTop,        const double& yBot,
  const ColorRGB& colTopLft, ColorRGB& colTopRgt,
  const ColorRGB& colBotLft, ColorRGB& colBotRgt)
// -------------------------------------------------
{
  glBegin(GL_POLYGON);
  glColor4f(colTopLft.r, colTopLft.g, colTopLft.b, colTopLft.a);
  glVertex2f(xLft, yTop);
  glColor4f(colBotLft.r, colBotLft.g, colBotLft.b, colBotLft.a);
  glVertex2f(xLft, yBot);
  glColor4f(colBotRgt.r, colBotRgt.g, colBotRgt.b, colBotRgt.a);
  glVertex2f(xRgt, yBot);
  glColor4f(colTopRgt.r, colTopRgt.g, colTopRgt.b, colTopRgt.a);
  glVertex2f(xRgt, yTop);
  glEnd();
}


// ----------------------------------------
void VisUtils::drawEllipse(
  const double& xCtr, const double& yCtr,
  const double& xDOF, const double& yDOF,
  const int& slices)
// ----------------------------------------
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


// ----------------------------------------
void VisUtils::fillEllipse(
  const double& xCtr, const double& yCtr,
  const double& xDOF, const double& yDOF,
  const int& slices)
// ----------------------------------------
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


// --------------------------------------------------
void VisUtils::fillEllipse(
  const double&   xCtr,    const double&   yCtr,
  const double&   xDOFIn,  const double&   yDOFIn,
  const double&   xDOFOut, const double&   yDOFOut,
  const double&   slices,  const ColorRGB& cIn,
  const ColorRGB& cOut)
// --------------------------------------------------
{
  double slice = (2*PI)/(double)slices;

  double xCurIn  = xCtr + xDOFIn*cos(0.0);
  double yCurIn  = yCtr + yDOFIn*sin(0.0);
  double xCurOut = xCtr + xDOFOut*cos(0.0);
  double yCurOut = yCtr + yDOFOut*sin(0.0);

  for (int i = 1; i <= slices; ++i)
  {
    glBegin(GL_POLYGON);

    glColor4f(cOut.r, cOut.g, cOut.b, cOut.a);
    glVertex2f(xCurOut, yCurOut);
    glColor4f(cIn.r, cIn.g, cIn.b, cIn.a);
    glVertex2f(xCurIn,  yCurIn);

    xCurIn  = xCtr + xDOFIn*cos(i*slice);
    yCurIn  = yCtr + yDOFIn*sin(i*slice);
    xCurOut = xCtr + xDOFOut*cos(i*slice);
    yCurOut = yCtr + yDOFOut*sin(i*slice);

    glColor4f(cIn.r, cIn.g, cIn.b, cIn.a);
    glVertex2f(xCurIn,  yCurIn);
    glColor4f(cOut.r, cOut.g, cOut.b, cOut.a);
    glVertex2f(xCurOut, yCurOut);

    glEnd();
  }
}


// ----------------------------------------------------
void VisUtils::fillEllipse(
  const double&   xCtr,     const double&   yCtr,
  const double&   xDOFIn,   const double&   yDOFIn,
  const double&   xDOFOut,  const double&   yDOFOut,
  const double&   aglBegDg, const double&   aglEndDg,
  const double&   slices,   const ColorRGB& cIn,
  const ColorRGB& cOut)
// ----------------------------------------------------
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

    glColor4f(cOut.r, cOut.g, cOut.b, cOut.a);
    glVertex2f(xOutside, yOutside);
    glColor4f(cIn.r, cIn.g, cIn.b, cIn.a);
    glVertex2f(xInside, yInside);

    xInside  = xCtr + xDOFIn*cos(aglBegRd + i*sliceRd);
    yInside  = yCtr + yDOFIn*sin(aglBegRd + i*sliceRd);
    xOutside = xCtr + xDOFOut*cos(aglBegRd + i*sliceRd);
    yOutside = yCtr + yDOFOut*sin(aglBegRd + i*sliceRd);

    glColor4f(cIn.r, cIn.g, cIn.b, cIn.a);
    glVertex2f(xInside, yInside);
    glColor4f(cOut.r, cOut.g, cOut.b, cOut.a);
    glVertex2f(xOutside, yOutside);

    glEnd();
  }
}


// -------------------------------------------
void VisUtils::drawArrow(
  const double& xFr,   const double& xTo,
  const double& yFr,   const double& yTo,
  const double& wHead, const double& lHead)
// -------------------------------------------
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


// -------------------------------------------
void VisUtils::fillArrow(
  const double& xFr,   const double& xTo,
  const double& yFr,   const double& yTo,
  const double& wHead, const double& lHead)
// -------------------------------------------
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


// -------------------------------------------
void VisUtils::drawDArrow(
  const double& xFr,   const double& xTo,
  const double& yFr,   const double& yTo,
  const double& wHead, const double& lHead)
// -------------------------------------------
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


// ------------------------------------------
void VisUtils::drawArrow(
  const double& xFr,   const double& xTo,
  const double& yFr,   const double& yTo,
  const double& wBase, const double& wHead,
  const double& lHead, const ColorRGB& cFr,
  const ColorRGB& cTo)
// ------------------------------------------
{
  ColorRGB cJnc;

  // calc angle & length of arrow
  double dX     = xTo-xFr;
  double dY     = yTo-yFr;
  double angl   = Utils::calcAngleDg(dX, dY);
  double lenArw = Utils::dist(xFr, yFr, xTo, yTo);

  // calc length base
  double lenBase = lenArw-lHead;

  // calc junction color
  double alpha = lenBase/lenArw;
  cJnc.r = (1.0-alpha)*cFr.r + alpha*cTo.r;
  cJnc.g = (1.0-alpha)*cFr.g + alpha*cTo.g;
  cJnc.b = (1.0-alpha)*cFr.b + alpha*cTo.b;
  cJnc.a = (1.0-alpha)*cFr.a + alpha*cTo.a;

  glPushMatrix();
  glTranslatef(xFr, yFr, 0.0);
  glRotatef(angl, 0.0, 0.0, 1.0);

  glBegin(GL_LINE_LOOP);
  glColor4f(cFr.r, cFr.g, cFr.b, cFr.a);
  glVertex2f(0.0, 0.5*wBase);
  glColor4f(cFr.r, cFr.g, cFr.b, cFr.a);
  glVertex2f(0.0, -0.5*wBase);
  glColor4f(cJnc.r, cJnc.g, cJnc.b, cJnc.a);
  glVertex2f(lenArw-lHead, -0.5*wBase);
  glColor4f(cJnc.r, cJnc.g, cJnc.b, cJnc.a);
  glVertex2f(lenArw-lHead, -0.5*wHead);
  glColor4f(cTo.r, cTo.g, cTo.b, cTo.a);
  glVertex2f(lenArw, 0.0);
  glColor4f(cJnc.r, cJnc.g, cJnc.b, cJnc.a);
  glVertex2f(lenArw-lHead,  0.5*wHead);
  glColor4f(cJnc.r, cJnc.g, cJnc.b, cJnc.a);
  glVertex2f(lenArw-lHead,  0.5*wBase);
  glEnd();

  glPopMatrix();
}


// ------------------------------------------
void VisUtils::fillArrow(
  const double& xFr,   const double& xTo,
  const double& yFr,   const double& yTo,
  const double& wBase, const double& wHead,
  const double& lHead)
// ------------------------------------------
{
  // calc angle & length of arrow
  double dX     = xTo-xFr;
  double dY     = yTo-yFr;
  double angl   = Utils::calcAngleDg(dX, dY);
  double lenArw = Utils::dist(xFr, yFr, xTo, yTo);

  glPushMatrix();
  glTranslatef(xFr, yFr, 0.0);
  glRotatef(angl, 0.0, 0.0, 1.0);

  // base
  glBegin(GL_POLYGON);
  glVertex2f(0.0, 0.5*wBase);
  glVertex2f(0.0, -0.5*wBase);
  glVertex2f(lenArw-lHead, -0.5*wBase);
  glVertex2f(lenArw-lHead,  0.5*wBase);
  glEnd();

  // head
  glBegin(GL_POLYGON);
  glVertex2f(lenArw-lHead, -0.5*wHead);
  glVertex2f(lenArw, 0.0);
  glVertex2f(lenArw-lHead,  0.5*wHead);
  glEnd();

  glPopMatrix();
}


// ------------------------------------------
void VisUtils::fillArrow(
  const double& xFr,   const double& xTo,
  const double& yFr,   const double& yTo,
  const double& wBase, const double& wHead,
  const double& lHead, const ColorRGB& cFr,
  const ColorRGB& cTo)
// ------------------------------------------
{
  ColorRGB cJnc;

  // calc angle & length of arrow
  double dX     = xTo-xFr;
  double dY     = yTo-yFr;
  double angl   = Utils::calcAngleDg(dX, dY);
  double lenArw = Utils::dist(xFr, yFr, xTo, yTo);

  // calc length base
  double lenBase = lenArw-lHead;

  // calc junction color
  double alpha = lenBase/lenArw;
  cJnc.r = (1.0-alpha)*cFr.r + alpha*cTo.r;
  cJnc.g = (1.0-alpha)*cFr.g + alpha*cTo.g;
  cJnc.b = (1.0-alpha)*cFr.b + alpha*cTo.b;
  cJnc.a = (1.0-alpha)*cFr.a + alpha*cTo.a;

  glPushMatrix();
  glTranslatef(xFr, yFr, 0.0);
  glRotatef(angl, 0.0, 0.0, 1.0);

  // base
  glBegin(GL_POLYGON);
  glColor4f(cFr.r, cFr.g, cFr.b, cFr.a);
  glVertex2f(0.0, 0.5*wBase);
  glColor4f(cFr.r, cFr.g, cFr.b, cFr.a);
  glVertex2f(0.0, -0.5*wBase);
  glColor4f(cJnc.r, cJnc.g, cJnc.b, cJnc.a);
  glVertex2f(lenArw-lHead, -0.5*wBase);
  glColor4f(cJnc.r, cJnc.g, cJnc.b, cJnc.a);
  glVertex2f(lenArw-lHead,  0.5*wBase);
  glEnd();

  // head
  glBegin(GL_POLYGON);
  glColor4f(cJnc.r, cJnc.g, cJnc.b, cJnc.a);
  glVertex2f(lenArw-lHead, -0.5*wHead);
  glColor4f(cTo.r, cTo.g, cTo.b, cTo.a);
  glVertex2f(lenArw, 0.0);
  glColor4f(cJnc.r, cJnc.g, cJnc.b, cJnc.a);
  glVertex2f(lenArw-lHead,  0.5*wHead);
  glEnd();

  glPopMatrix();
}


// -------------------------------------------
void VisUtils::fillDArrow(
  const double& xFr,   const double& xTo,
  const double& yFr,   const double& yTo,
  const double& wHead, const double& lHead)
// -------------------------------------------
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


// ------------------------------------------------
void VisUtils::drawArrowArcCW(
  const double& xCtr,     const double& yCtr,
  const double& aglBegDg, const double& aglEndDg,
  const double& radius,   const double& slices,
  const double& wHead,    const double& lHead)
// ------------------------------------------------
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


// ------------------------------------------------
void VisUtils::fillArrowArcCW(
  const double& xCtr,     const double& yCtr,
  const double& aglBegDg, const double& aglEndDg,
  const double& radius,   const double& slices,
  const double& wHead,    const double& lHead)
// ------------------------------------------------
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


// ------------------------------------------------
void VisUtils::drawArrowArcCCW(
  const double& xCtr,     const double& yCtr,
  const double& aglBegDg, const double& aglEndDg,
  const double& radius,   const double& slices,
  const double& wHead,    const double& lHead)
// ------------------------------------------------
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


// ------------------------------------------------
void VisUtils::fillArrowArcCCW(
  const double& xCtr,     const double& yCtr,
  const double& aglBegDg, const double& aglEndDg,
  const double& radius,   const double& slices,
  const double& wHead,    const double& lHead)
// ------------------------------------------------
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


// -----------------------------------------
void VisUtils::drawFwrdIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
// -----------------------------------------
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


// -----------------------------------------
void VisUtils::fillFwrdIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
// -----------------------------------------
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


// -----------------------------------------
void VisUtils::drawNextIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
// -----------------------------------------
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


// -----------------------------------------
void VisUtils::fillNextIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
// -----------------------------------------
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


// -----------------------------------------
void VisUtils::drawPauseIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
// -----------------------------------------
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


// -----------------------------------------
void VisUtils::fillPauseIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
// -----------------------------------------
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


// -----------------------------------------
void VisUtils::drawPlayIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
// -----------------------------------------
{
  glBegin(GL_LINE_LOOP);
  glVertex2f(xLft, yTop);
  glVertex2f(xLft, yBot);
  glVertex2f(xRgt, 0.5*(yTop+yBot));
  glEnd();
}


// -----------------------------------------
void VisUtils::fillPlayIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
// -----------------------------------------
{
  glBegin(GL_POLYGON);
  glVertex2f(xLft, yTop);
  glVertex2f(xLft, yBot);
  glVertex2f(xRgt, 0.5*(yTop+yBot));
  glEnd();
}


// -----------------------------------------
void VisUtils::drawStopIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
// -----------------------------------------
{
  glBegin(GL_LINE_LOOP);
  glVertex2f(xLft + 0.125*(xRgt-xLft), yTop);
  glVertex2f(xLft + 0.125*(xRgt-xLft), yBot);
  glVertex2f(xRgt - 0.125*(xRgt-xLft), yBot);
  glVertex2f(xRgt - 0.125*(xRgt-xLft), yTop);
  glEnd();
}


// -----------------------------------------
void VisUtils::fillStopIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
// -----------------------------------------
{
  glBegin(GL_POLYGON);
  glVertex2f(xLft + 0.125*(xRgt-xLft), yTop);
  glVertex2f(xLft + 0.125*(xRgt-xLft), yBot);
  glVertex2f(xRgt - 0.125*(xRgt-xLft), yBot);
  glVertex2f(xRgt - 0.125*(xRgt-xLft), yTop);
  glEnd();
}


// -----------------------------------------
void VisUtils::drawPrevIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
// -----------------------------------------
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


// -----------------------------------------
void VisUtils::fillPrevIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
// -----------------------------------------
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


// -----------------------------------------
void VisUtils::drawRwndIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
// -----------------------------------------
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


// -----------------------------------------
void VisUtils::fillRwndIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
// -----------------------------------------
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


// -----------------------------------------
void VisUtils::drawCloseIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
// -----------------------------------------
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


// -----------------------------------------
void VisUtils::fillCloseIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
// -----------------------------------------
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


// -----------------------------------------
void VisUtils::drawMoreIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
// -----------------------------------------
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


// -----------------------------------------
void VisUtils::fillMoreIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
// -----------------------------------------
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


// -----------------------------------------
void VisUtils::drawClearIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
// -----------------------------------------
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


// -----------------------------------------
void VisUtils::fillClearIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
// -----------------------------------------
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


// -----------------------------------------
void VisUtils::drawUpIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
// -----------------------------------------
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


// -----------------------------------------
void VisUtils::fillUpIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
// -----------------------------------------
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


// -----------------------------------------
void VisUtils::drawDownIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
// -----------------------------------------
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


// -----------------------------------------
void VisUtils::fillDownIcon(
  const double& xLft, const double& xRgt,
  const double& yTop, const double& yBot)
// -----------------------------------------
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
// -----------------------------------------
void VisUtils::setTransf(
    const double &xCtr, const double &yCtr,
    const double &xDOF,  const double &yDOF,
    const double &xHge,  const double &yHge,
    const double &aglCtr )
// -----------------------------------------
// ------------------------------------------------------------------
// This function should be used when in edit mode.
// ------------------------------------------------------------------
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

// ---------------------------------------------
void VisUtils::setTransf(
  const double& xCtr,   const double& yCtr,
  const double& /*xDOF*/,   const double& /*yDOF*/,
  const double& xHge,   const double& yHge,
  const double& aglCtr, const double& aglHge)
// ---------------------------------------------
// ------------------------------------------------------------------
// This function should be used when in analysis mode.
// ------------------------------------------------------------------
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


// -----------------------
void VisUtils::clrTransf()
// -----------------------
{
  //glMatrixMode( GL_MODELVIEW );
  glPopMatrix();
}


// -- text ----------------------------------------------------------


// -----------------------------------------------------
void VisUtils::genCharTextures(
  GLuint texCharId[CHARSETSIZE],
  GLubyte texChar[CHARSETSIZE][CHARHEIGHT* CHARWIDTH])
// -----------------------------------------------------
{
  // allocate memory
  glGenTextures(CHARSETSIZE, texCharId);

  // create textures
  for (int i = 0; i < CHARSETSIZE; ++i)
  {
    // bind textures
    glBindTexture(GL_TEXTURE_2D, texCharId[i]);
    // create image
    wxImage image(characters[i]);

    int red = 0;
    int green = 0;
    int blue = 0;

    // read in texture
    int count = 0;
    for (int h = 0; h < CHARHEIGHT; ++h)
    {
      for (int w = 0; w < CHARWIDTH; ++w)
      {
        red   = (GLubyte)image.GetRed(w, h);
        green = (GLubyte)image.GetGreen(w, h);
        blue  = (GLubyte)image.GetBlue(w, h);

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


// ---------------------------------------
int VisUtils::getCharIdx(const char& c)
// ---------------------------------------
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


// -------------------------------
void VisUtils::drawLabel(
  GLuint texCharId[CHARSETSIZE],
  const double& xCoord,
  const double& yCoord,
  const double& scaling,
  const std::string& label)
// -------------------------------
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


// -------------------------------
void VisUtils::drawLabelRight(
  GLuint texCharId[CHARSETSIZE],
  const double& xCoord,
  const double& yCoord,
  const double& scaling,
  const std::string& label)
// -------------------------------
{
  drawLabel(
    texCharId,
    xCoord,
    yCoord,
    scaling,
    label);
}


// -------------------------------
void VisUtils::drawLabelLeft(
  GLuint texCharId[CHARSETSIZE],
  const double& xCoord,
  const double& yCoord,
  const double& scaling,
  const std::string& label)
// -------------------------------
{
  double translate = label.length()*CHARWIDTH*scaling;
  drawLabel(
    texCharId,
    xCoord-translate,
    yCoord,
    scaling,
    label);
}


// -------------------------------
void VisUtils::drawLabelCenter(
  GLuint texCharId[CHARSETSIZE],
  const double& xCoord,
  const double& yCoord,
  const double& scaling,
  const std::string& label)
// -------------------------------
{
  double translate = 0.5*label.length()*CHARWIDTH*scaling;
  drawLabel(
    texCharId,
    xCoord-translate,
    yCoord,
    scaling,
    label);
}


// -------------------------------
void VisUtils::drawLabelVert(
  GLuint texCharId[CHARSETSIZE],
  const double& xCoord,
  const double& yCoord,
  const double& scaling,
  const std::string& label)
// -------------------------------
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


// -------------------------------
void VisUtils::drawLabelVertAbove(
  GLuint texCharId[CHARSETSIZE],
  const double& xCoord,
  const double& yCoord,
  const double& scaling,
  const std::string& label)
// -------------------------------
{
  drawLabelVert(
    texCharId,
    xCoord,
    yCoord,
    scaling,
    label);
}


// -------------------------------
void VisUtils::drawLabelVertBelow(
  GLuint texCharId[CHARSETSIZE],
  const double& xCoord,
  const double& yCoord,
  const double& scaling,
  const std::string& label)
// -------------------------------
{
  double translate = label.length()*CHARWIDTH*scaling;
  drawLabelVert(
    texCharId,
    xCoord,
    yCoord - translate,
    scaling,
    label);
}


// --------------------------------
void VisUtils::drawLabelVertCenter(
  GLuint texCharId[CHARSETSIZE],
  const double& xCoord,
  const double& yCoord,
  const double& scaling,
  const std::string& label)
// --------------------------------
{
  double translate = 0.5*label.length()*CHARWIDTH*scaling;
  drawLabelVert(
    texCharId,
    xCoord,
    yCoord-translate,
    scaling,
    label);
}


// ----------------------------------
void VisUtils::drawLabelInBoundBox(
  GLuint texCharId[CHARSETSIZE],
  const double& xLft,
  const double& xRgt,
  const double& yTop,
  const double& yBot,
  const double& scaling,
  const std::string& label)
// ----------------------------------
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


// ----------------------------
void VisUtils::genCushTextures(
  GLuint& texCushId,
  float texCush[CUSHSIZE])
// ----------------------------
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


// ------------------------------
float VisUtils::cushionProfile1D(
  const float& x,
  const float& D,
  const float& h,
  const float& alpha,
  const float& l_ratio)
// ------------------------------
// ------------------------------------------------------------------
// Code adapted from Lucian Voinea.
// ------------------------------------------------------------------
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


// -------------------------
void VisUtils::drawCushDiag(
  const GLuint& texCushId,
  const double& xLft,
  const double& xRgt,
  const double& yTop,
  const double& yBot)
// -------------------------
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


// -------------------------
void VisUtils::drawCushHori(
  const GLuint& texCushId,
  const double& xLft,
  const double& xRgt,
  const double& yTop,
  const double& yBot)
// -------------------------
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


// -------------------------
void VisUtils::drawCushVert(
  const GLuint& texCushId,
  const double& xLft,
  const double& xRgt,
  const double& yTop,
  const double& yBot)
// -------------------------
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
