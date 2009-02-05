// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./visualizer.cpp

#include "wx.hpp" // precompiled headers

#include <iostream> // only temporary for std::clog

#include "visualizer.h"


// -- constructors and destructor -----------------------------------


// --------------------
Visualizer::Visualizer(
    Mediator* m,
    Graph* g,
    GLCanvas* c )
    : Colleague( m )
// --------------------
{
    clearColor.r = 1.0;
    clearColor.g = 1.0;
    clearColor.b = 1.0;
    clearColor.a = 1.0;

    initMouse();

    graph  = g;
    canvas = c;

    texCharOK = false;
    texCushOK = false;

    geomChanged = true;
    dataChanged = true;

    showMenu = false;
}


// ----------------------
Visualizer::~Visualizer()
// ----------------------
{
    graph  = NULL;
    canvas = NULL;
}


// -- set functions -------------------------------------------------


// ----------------------------
void Visualizer::setClearColor(
    const double &r,
    const double &g,
    const double &b )
// ----------------------------
{
    clearColor.r = r;
    clearColor.g = g;
    clearColor.b = b;
    clearColor.a = 1.0;
}

// -- helper functions ----------------------------------------------
void Visualizer::printMouseVariables()
{
    char const* mb = "DOWN";
    char const* ms = "LEFT";
    char const* mc = "SINGLE";
    char const* md = "DRAG";

    if(mouseButton == MSE_BUTTON_UP)
    	mb = "UP";
    if(mouseSide == MSE_SIDE_RGT)
    	ms = "RIGHT";
    else if (mouseSide == MSE_SIDE_MID)
    	ms = "MIDDLE";
    if(mouseClick == MSE_CLICK_DOUBLE)
    	mc = "DOUBLE";
    if(mouseDrag == MSE_DRAG_FALSE)
    	md = "NOT_DRAG";

    std::clog << "Mouse Button: " << mb << std::endl;
    std::clog << "Mouse Side: " << ms << std::endl;
    std::clog << "Mouse Click: " << mc << std::endl;
    std::clog << "Mouse Drag: " << md << std::endl << std::endl;
}


// -- visualization functions ---------------------------------------


// ------------------------------------------------
void Visualizer::setGeomChanged( const bool &flag )
// ------------------------------------------------
{
    geomChanged = flag;
}


// ------------------------------------------------
void Visualizer::setDataChanged( const bool &flag )
// ------------------------------------------------
{
    geomChanged = flag;
    dataChanged = flag;
}


// -- event handlers ------------------------------------------------


// -------------------------------
void Visualizer::handleSizeEvent()
// -------------------------------
{
    geomChanged = true;
}


// --------------------------------------
void Visualizer::handleMouseLftDownEvent(
    const int &x,
    const int &y )
// --------------------------------------
{
    mouseButton = MSE_BUTTON_DOWN;
    mouseSide   = MSE_SIDE_LFT;
    mouseClick  = MSE_CLICK_SINGLE;
    mouseDrag   = MSE_DRAG_TRUE;
    if(mouseDrag == MSE_DRAG_TRUE)

    xMouseDragBeg = x;
    yMouseDragBeg = y;
    xMouseCur     = x;
    yMouseCur     = y;
}


// ------------------------------------
void Visualizer::handleMouseLftUpEvent(
    const int &x,
    const int &y )
// ------------------------------------
{
    mouseButton = MSE_BUTTON_UP;
    mouseSide   = MSE_SIDE_LFT;
    if ( mouseClick != MSE_CLICK_DOUBLE )
        mouseClick  = MSE_CLICK_SINGLE;
    mouseDrag   = MSE_DRAG_FALSE;

    xMouseCur = x;
    yMouseCur = y;
}


// ----------------------------------------
void Visualizer::handleMouseLftDClickEvent(
    const int &x,
    const int &y )
// ----------------------------------------
{
    mouseButton = MSE_BUTTON_DOWN;
    mouseSide   = MSE_SIDE_LFT;
    mouseClick  = MSE_CLICK_DOUBLE;
    mouseDrag   = MSE_DRAG_FALSE;

    xMouseDragBeg = x;
    yMouseDragBeg = y;
    xMouseCur     = x;
    yMouseCur     = y;
}


// --------------------------------------
void Visualizer::handleMouseRgtDownEvent(
    const int &x,
    const int &y )
// --------------------------------------
{
    mouseButton = MSE_BUTTON_DOWN;
    mouseSide   = MSE_SIDE_RGT;
    mouseClick  = MSE_CLICK_SINGLE;
    mouseDrag   = MSE_DRAG_TRUE;

    xMouseDragBeg = x;
    yMouseDragBeg = y;
    xMouseCur     = x;
    yMouseCur     = y;
}


// ------------------------------------
void Visualizer::handleMouseRgtUpEvent(
    const int &x,
    const int &y )
// ------------------------------------
{
    mouseButton = MSE_BUTTON_UP;
    mouseSide   = MSE_SIDE_RGT;
    if ( mouseClick != MSE_CLICK_DOUBLE )
        mouseClick  = MSE_CLICK_SINGLE;
    mouseDrag   = MSE_DRAG_FALSE;

    xMouseCur = x;
    yMouseCur = y;
}


// ----------------------------------------
void Visualizer::handleMouseRgtDClickEvent(
    const int &x,
    const int &y )
// ----------------------------------------
{
    mouseButton = MSE_BUTTON_DOWN;
    mouseSide   = MSE_SIDE_RGT;
    mouseClick  = MSE_CLICK_DOUBLE;
    mouseDrag   = MSE_DRAG_FALSE;

    xMouseDragBeg = x;
    yMouseDragBeg = y;
    xMouseCur     = x;
    yMouseCur     = y;
}


// -------------------------------------
void Visualizer::handleMouseMotionEvent(
    const int &x,
    const int &y )
// -------------------------------------
{
    if ( mouseButton == MSE_BUTTON_DOWN )
    {
    	 mouseDrag  = MSE_DRAG_TRUE;
         mouseClick = MSE_CLICK_SINGLE;
    }
    else
    {
	mouseDrag  = MSE_DRAG_FALSE;
	mouseClick = -1;
    }

    xMouseCur = x;
    yMouseCur = y;
}


// ---------------------------------------
void Visualizer::handleMouseWheelIncEvent(
    const int &x,
    const int &y )
// ---------------------------------------
{}


// ---------------------------------------
void Visualizer::handleMouseWheelDecEvent(
    const int &x,
    const int &y )
// ---------------------------------------
{}


// -------------------------------------
void Visualizer::handleMouseEnterEvent()
// -------------------------------------
{}


// -------------------------------------
void Visualizer::handleMouseLeaveEvent()
// -------------------------------------
{
    initMouse();
}


// ------------------------------------------------------
void Visualizer::handleKeyDownEvent( const int &keyCode )
// ------------------------------------------------------
{
    keyCodeDown = keyCode;
}


// ----------------------------------------------------
void Visualizer::handleKeyUpEvent( const int &keyCode )
// ----------------------------------------------------
{
    keyCodeDown = -1;
}


// -- protected utility functions -----------------------------------


// ---------------------
void Visualizer::clear()
// ---------------------
{
    VisUtils::clear( clearColor );
}


// -------------------------
void Visualizer::initMouse()
// -------------------------
{
    mouseButton = MSE_BUTTON_UP;
    mouseSide   = MSE_SIDE_LFT;
    mouseClick  = MSE_CLICK_SINGLE;
    mouseDrag   = MSE_DRAG_FALSE;

    xMouseDragBeg = 0.0;
    yMouseDragBeg = 0.0;
    xMouseCur     = 0.0;
    yMouseCur     = 0.0;
}


// ------------------------------
void Visualizer::startSelectMode(
    GLint hits,
    GLuint selectBuf[],
    double pickWth,
    double pickHgt )
// ------------------------------
{
    GLint viewport[4];

    // ( x, y, width, height )
    glGetIntegerv( GL_VIEWPORT, viewport );

    glSelectBuffer( 512, selectBuf );
    // selection mode
    (void) glRenderMode( GL_SELECT );

    glInitNames();

    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();

    gluPickMatrix(
        (GLdouble) xMouseCur,
        (GLdouble) ( viewport[3]-yMouseCur ),
        pickWth,    // picking width
        pickHgt,    // picking height
        viewport );

    // get current size of canvas
    int width, height;
    canvas->GetSize( &width, &height );

    // casting to GLdouble ensures smooth transitions
    GLdouble aspect = (GLdouble)width / (GLdouble)height;

    // specify clipping rectangle ( left, right, bottom, top )
    if ( aspect > 1 )
	    // width > height
        gluOrtho2D( aspect*(-1), aspect*1, -1, 1 );
    else
	    // height >= width
		gluOrtho2D( -1, 1, (1/aspect)*(-1), (1/aspect)*1 );

    glMatrixMode( GL_MODELVIEW );
}


// -------------------------------
void Visualizer::finishSelectMode(
    GLint hits,
    GLuint selectBuf[] )
// -------------------------------
{
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glFlush();

    hits = glRenderMode( GL_RENDER );
    processHits( hits, selectBuf );
}


// --------------------------
void Visualizer::genCharTex()
// --------------------------
{
    VisUtils::genCharTextures(
        texCharId,
        texChar );
    texCharOK = true;
}


// --------------------------
void Visualizer::genCushTex()
// --------------------------
{
    VisUtils::genCushTextures(
        texCushId,
        texCush );
    texCushOK = true;
}


// -- end -----------------------------------------------------------
