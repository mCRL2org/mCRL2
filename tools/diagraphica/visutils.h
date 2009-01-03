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


#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
	#ifdef WIN32
		#include <windows.h>
	  #undef __in_range
	#endif
    #include <GL/glu.h>
#endif
#include <string>
#include <wx/bitmap.h>
#include <wx/image.h>
#include "utils.h"


// -- structs for color ---------------------------------------------

struct ColorHLS
{
    double h;
    double l;
    double s;
};

struct ColorRGB
{
    double r;
    double g;
    double b;
    double a;
};


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
const std::string CHARPATH = "Chars/";

// -- data for cushion textures -------------------------------------

const int CUSHSIZE    = 256;

// -- data for gradient textures ------------------------------------

const int GRADSIZE    = 256;
const std::string GRADPATH = "Grads/";

class VisUtils
{
public:
    // -- clear canvas ----------------------------------------------
    static void clear( const ColorRGB &col );
    
    // -- anti-aliasing & blending ----------------------------------
    static void enableLineAntiAlias();
    static void disableLineAntiAlias();
    static void enableBlending();
    static void disableBlending();

    // -- line width ------------------------------------------------
    static void setLineWidth( const double &px );

    // -- color -----------------------------------------------------
    static void setColor( const ColorRGB &colRGB );
    
    static void setColorBlack();
    static void setColorBlue();
    static void setColorCoolBlue();
    static void setColorDkCoolBlue();
    static void setColorGreen();
    static void setColorCoolGreen();
    static void setColorLtCoolGreen();
    static void setColorLtLtGray();
    static void setColorLtGray();
    static void setColorMdGray();
    static void setColorDkGray();
    static void setColorOrange();
    static void setColorRed();
    static void setColorCoolRed();
    static void setColorWhite();
    static void setColorYellow();
    static void setColorBlueYellow( const double &frac );
    
    static void mapColorBlack( ColorRGB &col );
    static void mapColorBlue( ColorRGB &col );
    static void mapColorCoolBlue( ColorRGB &col );
    static void mapColorDkCoolBlue( ColorRGB &col );
    static void mapColorGreen( ColorRGB &col );
    static void mapColorCoolGreen( ColorRGB &col );
    static void mapColorLtCoolGreen( ColorRGB &col );
    static void mapColorLtLtGray( ColorRGB &col );
    static void mapColorLtGray( ColorRGB &col );
    static void mapColorMdGray( ColorRGB &col );
    static void mapColorDkGray( ColorRGB &col );
    static void mapColorOrange( ColorRGB &col );
    static void mapColorRed( ColorRGB &col );
    static void mapColorCoolRed( ColorRGB &col );
    static void mapColorWhite( ColorRGB &col );
    
    static void mapColorBlueYellow( 
        const double &frac,
        ColorRGB &colRGB );
    static void mapColorGrayScale(
        const double &frac,
        ColorRGB &colRGB );
    static void mapColorSpectral(
        const double &frac,
        ColorRGB &colRGB );
    
    static void mapColorQualPast1(
        const int &iter,
        const int &numr,
        ColorRGB &colRGB );
    static void mapColorQualPast2(
        const int &iter,
        const int &numr,
        ColorRGB &colRGB );
    static void mapColorQualSet1(
        const int &iter,
        const int &numr,
        ColorRGB &colRGB );
    static void mapColorQualSet2(
        const int &iter,
        const int &numr,
        ColorRGB &colRGB );
    static void mapColorQualSet3(
        const int &iter,
        const int &numr,
        ColorRGB &colRGB );
    static void mapColorQualPair(
        const int &iter,
        const int &numr,
        ColorRGB &colRGB );
    static void mapColorQualDark(
        const int &iter,
        const int &numr,
        ColorRGB &colRGB );
    static void mapColorQualAccent(
        const int &iter,
        const int &numr,
        ColorRGB &colRGB );

    static void mapColorSeqOrRd(
        const int &iter,
        const int &numr,
        ColorRGB &colRGB );
    static void mapColorSeqGnBu(
        const int &iter,
        const int &numr,
        ColorRGB &colRGB );
    static void mapColorSeqGreen(
        const int &iter,
        const int &numr,
        ColorRGB &colRGB );
    static void mapColorSeqGreen(
        const double &alpha,
        ColorRGB &colRGB );
    static void mapColorSeqRed(
        const int &iter,
        const int &numr,
        ColorRGB &colRGB );
    static void mapColorSeqRed(
        const double &alpha,
        ColorRGB &colRGB );
    
    static void hlsToRgb( 
        ColorHLS &colHLS,
        ColorRGB &colRGB );
    static double hlsValue(
        double var1, 
        double var2, 
        double hue );

    // -- drawing functions -----------------------------------------
    static void drawLine( 
        const double &xFr, const double &xTo,
        const double &yFr, const double &yTo );
    static void drawLineDashed(
        const double &xFr, const double &xTo,
        const double &yFr, const double &yTo );

    static void drawArc(
        const double &xCtr,     const double &yCtr,
        const double &aglBegDg, const double &aglEndDg,
        const double &radius,   const int &slices );
    static void drawArcDashed(
        const double &xCtr,     const double &yCtr,
        const double &aglBegDg, const double &aglEndDg,
        const double &radius,   const int &slices );
    static void drawArcCW(
        const double &xCtr,     const double &yCtr,
        const double &aglBegDg, const double &aglEndDg,
        const double &radius,   const int &slices );
    static void drawArcDashedCW(
        const double &xCtr,     const double &yCtr,
        const double &aglBegDg, const double &aglEndDg,
        const double &radius,   const int &slices );
    static void drawArc(
        const double &xCtr,     const double &yCtr,
        const double &aglBegDg, const double &aglEndDg,
        const double &wthBeg,   const double &wthEnd,
        const double &radius,   const int &slices );
    static void drawArc(
        const double &xCtr,     const double &yCtr,
        const double &aglBegDg, const double &aglEndDg,
        const double &wthBeg,   const double &wthEnd,
        const ColorRGB &colBeg, const ColorRGB &colEnd,
        const double &radius,   const int &slices );
    static void fillArc(
        const double &xCtr,     const double &yCtr,
        const double &aglBegDg, const double &aglEndDg,
        const double &wthBeg,   const double &wthEnd,
        const double &radius,   const int &slices );
    static void fillArc(
        const double &xCtr,     const double &yCtr,
        const double &aglBegDg, const double &aglEndDg,
        const double &wthBeg,   const double &wthEnd,
        const ColorRGB &colBeg, const ColorRGB &colEnd,
        const double &radius,   const int &slices );
    
    static void drawTriangle(
        const double &x1, const double &y1,
        const double &x2, const double &y2,
        const double &x3, const double &y3 );
    static void drawTriangle(
        const double   &x1, const double &y1,
        const ColorRGB &col1,
        const double   &x2, const double &y2,
        const ColorRGB &col2,
        const double   &x3, const double &y3,
        const ColorRGB &col3 );
    static void fillTriangle(
        const double &x1, const double &y1,
        const double &x2, const double &y2,
        const double &x3, const double &y3 );
    static void fillTriangle(
        const double   &x1, const double &y1,
        const ColorRGB &col1,
        const double   &x2, const double &y2,
        const ColorRGB &col2,
        const double   &x3, const double &y3,
        const ColorRGB &col3 );

    static void drawRect(
        const double &xLft, const double &xRgt,
        const double &yTop, const double &yBot );
    static void drawRect(
        const double &xLft,        const double &xRgt,
        const double &yTop,        const double &yBot,
        const ColorRGB &colTopLft, ColorRGB &colTopRgt,
        const ColorRGB &colBotLft, ColorRGB &colBotRgt );
    static void fillRect(
        const double &xLft, const double &xRgt,
        const double &yTop, const double &yBot );
    static void fillRect(
        const double &xLft,        const double &xRgt,
        const double &yTop,        const double &yBot,
        const ColorRGB &colTopLft, ColorRGB &colTopRgt,
        const ColorRGB &colBotLft, ColorRGB &colBotRgt );
    
    static void drawEllipse(
        const double &xCtr, const double &yCtr,
        const double &xDOF, const double &yDOF,
        const int &slices );
    static void fillEllipse(
        const double &xCtr, const double &yCtr,
        const double &xDOF, const double &yDOF,
        const int &slices );
    static void fillEllipse(
        const double   &xCtr,    const double   &yCtr,
        const double   &xDOFIn,  const double   &yDOFIn,
        const double   &xDOFOut, const double   &yDOFOut,
        const double   &slices,  const ColorRGB &cIn,
        const ColorRGB &cOut );
    static void fillEllipse(
        const double   &xCtr,     const double   &yCtr,
        const double   &xDOFIn,   const double   &yDOFIn,
        const double   &xDOFOut,  const double   &yDOFOut,
        const double   &aglBegDg, const double &aglEndDg,
        const double   &slices,   const ColorRGB &cIn,
        const ColorRGB &cOut );
    
    static void drawArrow(
        const double &xFr,     const double &xTo,
        const double &yFr,     const double &yTo,
        const double &wHead, const double &lHead );
    static void fillArrow(
        const double &xFr,     const double &xTo,
        const double &yFr,     const double &yTo,
        const double &wHead, const double &lHead );
    static void drawArrow(
        const double &xFr,   const double   &xTo,
        const double &yFr,   const double   &yTo,
        const double &wBase, const double   &wHead,
        const double &lHead, const ColorRGB &cFr, 
        const ColorRGB &cTo );
    static void fillArrow(
        const double &xFr,   const double &xTo,
        const double &yFr,   const double &yTo,
        const double &wBase, const double &wHead,
        const double &lHead );
    static void fillArrow(
        const double &xFr,   const double &xTo,
        const double &yFr,   const double &yTo,
        const double &wBase, const double &wHead,
        const double &lHead, const ColorRGB &cFr, 
        const ColorRGB &cTo );
    
    static void drawDArrow(
        const double &xFr,   const double &xTo,
        const double &yFr,   const double &yTo,
        const double &wHead, const double &lHead );
    static void fillDArrow(
        const double &xFr,   const double &xTo,
        const double &yFr,   const double &yTo,
        const double &wHead, const double &lHead );

    static void drawArrowArcCW(
        const double &xCtr,     const double &yCtr,
        const double &aglBegDg, const double &aglEndDg,
        const double &radius,   const double &slices,
        const double &wHead,    const double &lHead );
    static void fillArrowArcCW(
        const double &xCtr,     const double &yCtr,
        const double &aglBegDg, const double &aglEndDg,
        const double &radius,   const double &slices,
        const double &wHead,    const double &lHead );
    static void drawArrowArcCCW(
        const double &xCtr,     const double &yCtr,
        const double &aglBegDg, const double &aglEndDg,
        const double &radius,   const double &slices,
        const double &wHead,    const double &lHead );
    static void fillArrowArcCCW(
        const double &xCtr,     const double &yCtr,
        const double &aglBegDg, const double &aglEndDg,
        const double &radius,   const double &slices,
        const double &wHead,    const double &lHead );
    
    static void drawFwrdIcon(
        const double &xLft, const double &xRgt,
        const double &yTop, const double &yBot );
    static void fillFwrdIcon(
        const double &xLft, const double &xRgt,
        const double &yTop, const double &yBot );
    static void drawNextIcon(
        const double &xLft, const double &xRgt,
        const double &yTop, const double &yBot );
    static void fillNextIcon(
        const double &xLft, const double &xRgt,
        const double &yTop, const double &yBot );
    static void drawPauseIcon(
        const double &xLft, const double &xRgt,
        const double &yTop, const double &yBot );
    static void fillPauseIcon(
        const double &xLft, const double &xRgt,
        const double &yTop, const double &yBot );
    static void drawPlayIcon(
        const double &xLft, const double &xRgt,
        const double &yTop, const double &yBot );
    static void fillPlayIcon(
        const double &xLft, const double &xRgt,
        const double &yTop, const double &yBot );
    static void drawStopIcon(
        const double &xLft, const double &xRgt,
        const double &yTop, const double &yBot );
    static void fillStopIcon(
        const double &xLft, const double &xRgt,
        const double &yTop, const double &yBot );
    static void drawPrevIcon(
        const double &xLft, const double &xRgt,
        const double &yTop, const double &yBot );
    static void fillPrevIcon(
        const double &xLft, const double &xRgt,
        const double &yTop, const double &yBot );
    static void drawRwndIcon(
        const double &xLft, const double &xRgt,
        const double &yTop, const double &yBot );
    static void fillRwndIcon(
        const double &xLft, const double &xRgt,
        const double &yTop, const double &yBot );
    static void drawCloseIcon(
        const double &xLft, const double &xRgt,
        const double &yTop, const double &yBot );
    static void fillCloseIcon(
        const double &xLft, const double &xRgt,
        const double &yTop, const double &yBot );
    static void drawMoreIcon(
        const double &xLft, const double &xRgt,
        const double &yTop, const double &yBot );
    static void fillMoreIcon(
        const double &xLft, const double &xRgt,
        const double &yTop, const double &yBot );
    static void drawClearIcon(
        const double &xLft, const double &xRgt,
        const double &uTop, const double &yBot );
    static void fillClearIcon(
        const double &xLft, const double &xRgt,
        const double &uTop, const double &yBot );
    static void drawUpIcon(
        const double &xLft, const double &xRgt,
        const double &yTop, const double &yBot );
    static void fillUpIcon(
        const double &xLft, const double &xRgt,
        const double &yTop, const double &yBot );
    static void drawDownIcon(
        const double &xLft, const double &xRgt,
        const double &yTop, const double &yBot );
    static void fillDownIcon(
        const double &xLft, const double &xRgt,
        const double &yTop, const double &yBot );

    // -- transformations -------------------------------------------
    /*
    static void setTransf(
        const double &xCtr, const double &yCtr,
        const double &xDOF, const double &yDOF,
        const double &xHge, const double &yHge,
        const double &aglCtr );
    */
    static void setTransf(
        const double &xCtr,   const double &yCtr,
        const double &xDOF,   const double &yDOF,
        const double &xHge,   const double &yHge,
        const double &aglCtr, const double &aglHge ); // remove

    static void clrTransf();

    // -- text ------------------------------------------------------
    static void genCharTextures(
        GLuint texCharId[CHARSETSIZE],
        GLubyte texChar[CHARSETSIZE][CHARHEIGHT*CHARWIDTH] );
    static int getCharIdx( const char &c );
    
    static void drawLabel(
        GLuint texCharId[CHARSETSIZE],
        const double &xCoord,
        const double &yCoord,
        const double &scaling,
        const std::string &label );
    static void drawLabelRight(
        GLuint texCharId[CHARSETSIZE],
        const double &xCoord,
        const double &yCoord,
        const double &scaling,
        const std::string &label );
    static void drawLabelLeft(
        GLuint texCharId[CHARSETSIZE],
        const double &xCoord,
        const double &yCoord,
        const double &scaling,
        const std::string &label );
    static void drawLabelCenter(
        GLuint texCharId[CHARSETSIZE],
        const double &xCoord,
        const double &yCoord,
        const double &scaling,
        const std::string &label );
    
    static void drawLabelVert(
        GLuint texCharId[CHARSETSIZE],
        const double &xCoord,
        const double &yCoord,
        const double &scaling,
        const std::string &label );
    static void drawLabelVertAbove(
        GLuint texCharId[CHARSETSIZE],
        const double &xCoord,
        const double &yCoord,
        const double &scaling,
        const std::string &label );
    static void drawLabelVertBelow(
        GLuint texCharId[CHARSETSIZE],
        const double &xCoord,
        const double &yCoord,
        const double &scaling,
        const std::string &label );
    static void drawLabelVertCenter(
        GLuint texCharId[CHARSETSIZE],
        const double &xCoord,
        const double &yCoord,
        const double &scaling,
        const std::string &label );

    static void drawLabelInBoundBox(
        GLuint texCharId[CHARSETSIZE],
        const double &xLft,
        const double &xRgt,
        const double &yTop,
        const double &yBot,
        const double &scaling,
        const std::string &label );

    // -- cushions --------------------------------------------------
    
    static void genCushTextures(
        GLuint &texCushId,
        float texCush[CUSHSIZE] );
    static float cushionProfile1D(
        const float &x,
        const float &D,
        const float &h,
        const float &alpha,
        const float &l_ratio );
    
    static void drawCushDiag(
        const GLuint &texCushId,
        const double &xLft,
        const double &xRgt,
        const double &yTop,
        const double &yBot );
    static void drawCushHori(
        const GLuint &texCushId,
        const double &xLft,
        const double &xRgt,
        const double &yTop,
        const double &yBot );
    static void drawCushVert(
        const GLuint &texCushId,
        const double &xLft,
        const double &xRgt,
        const double &yTop,
        const double &yBot );

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
