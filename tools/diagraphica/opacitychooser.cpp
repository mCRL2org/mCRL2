// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./opacitychooser.cpp

#include "wx.hpp" // precompiled headers

#include "opacitychooser.h"

using namespace std;

// -- static variables ----------------------------------------------


double OpacityChooser::hdlSzeHnt =  5.0;


// -- constructors and destructor -----------------------------------


// ----------------------------
OpacityChooser::OpacityChooser(
    Mediator* m,
    Graph* g,
    GLCanvas* c )
    : Visualizer( m, g, c )
// ----------------------------
{
    hdlSize  = hdlSzeHnt;

    active  = false;
    dragIdx = -1;
}


// ------------------------------
OpacityChooser::~OpacityChooser()
// ------------------------------
{}


// -- set functions -------------------------------------------------


// -----------------------------------------------
void OpacityChooser::setActive( const bool &flag )
// -----------------------------------------------
{
    active = flag;
}


// -----------------------------
void OpacityChooser::setPoints(
    const vector< double > &opa,
    const vector< double > &y )
// -----------------------------
{
    positionsX.clear();
    for ( size_t i = 0; i < opa.size(); ++i )
        positionsX.push_back( (2.0*opa[i])-1.0 );

    positionsY = y;
}


// -- visualization functions  --------------------------------------


// -------------------------------------------------------
void OpacityChooser::visualize( const bool &inSelectMode )
// -------------------------------------------------------
{
    /*
    // check if positions are ok
    if ( geomChanged == true )
        calcPositions();
    */

    // visualize
    if ( inSelectMode == true )
    {
        GLint hits = 0;
        GLuint selectBuf[512];
        startSelectMode(
            hits,
            selectBuf,
            2.0,
            2.0 );

        drawPoints( inSelectMode );

        finishSelectMode(
            hits,
            selectBuf );
    }
    else
    {
        clear();
        drawGrayScale();
        drawPath( inSelectMode );
        drawPoints( inSelectMode );
    }
}


// -- event handlers ------------------------------------------------


// ------------------------------------------
void OpacityChooser::handleMouseLftDownEvent(
    const int &x,
    const int &y )
// ------------------------------------------
{
	mediator->handleDOFOpaActivate();
	mediator->setDOFOpacitySelected();
    if ( active == true )
    {
        Visualizer::handleMouseLftDownEvent( x, y );

        // selection moce
        visualize( true );
        // normal mode
        visualize( false );

        if ( dragIdx == -1 )
        {
            double w, h;
            double xLft, xRgt;
            double yBot, yTop;
            double xCur, yCur;

            // get size of sides
            canvas->getSize( w, h );

            // calc size of bounding box
            xLft = -0.5*w;
            xRgt =  0.5*w;
            yTop =  0.5*h;
            yBot = -0.5*h;

            // get cur mouse position
            canvas->getWorldCoords( xMouseCur, yMouseCur, xCur, yCur );

            // normalize mouse position
            // xLft is -1.0, xRgt is 1.0, yTop = 1.0 and yBot = -1.0
            xCur = xCur/xRgt;
            yCur = yCur/yTop;

            positionsX.push_back( xCur );
            positionsY.push_back( yCur );

            mediator->handleDOFOpaAdd( 0.5*(xCur+1), yCur );
        }
    }
}


// ----------------------------------------
void OpacityChooser::handleMouseLftUpEvent(
    const int &x,
    const int &y )
// ----------------------------------------
{
    if ( active == true )
    {
        Visualizer::handleMouseLftUpEvent( x, y );
        dragIdx = -1;
    }
}


// ------------------------------------------
void OpacityChooser::handleMouseRgtDownEvent(
    const int &x,
    const int &y )
// ------------------------------------------
{
    if ( active == true )
    {
        Visualizer::handleMouseRgtDownEvent( x, y );

        // selection mode
        visualize( true );
        // normal mode
        visualize( false );
    }
}


// ----------------------------------------
void OpacityChooser::handleMouseRgtUpEvent(
    const int &x,
    const int &y )
// ----------------------------------------
{
    if ( active == true )
    {
        Visualizer::handleMouseRgtUpEvent( x, y );
    }
}


// -----------------------------------------
void OpacityChooser::handleMouseMotionEvent(
    const int &x,
    const int &y )
// -----------------------------------------
{
    if ( active == true )
    {
        Visualizer::handleMouseMotionEvent( x, y );
        if ( mouseDrag == MSE_DRAG_TRUE )
            handleDrag();
    }
}


// -- utility drawing functions -------------------------------------

// ***
/*
// -------------------------
void OpacityChooser::clear()
// -------------------------
{
    VisUtils::clear( clearColor );
}
*/

// ---------------------------------
void OpacityChooser::drawGrayScale()
// ---------------------------------
{
    double w, h;
    double xLft, xRgt;
    double yBot, yTop;
    double xItv;
    ColorRGB col;

    // get size of sides
    canvas->getSize( w, h );

    // calc size of bounding box
    xLft = -0.5*w;
    xRgt =  0.5*w;
    yTop =  0.5*h;
    yBot = -0.5*h;

    xItv = (xRgt-xLft)/255.0;
    VisUtils::mapColorBlue( col );
    VisUtils::enableBlending();
    for ( int i = 0; i < 255; ++i )
    {
        col.a = pow( (i/255.0), 2 );
        /*
        VisUtils::mapColorGrayScale(
            1.0-i/255.0,
            col );
        */
        VisUtils::setColor( col );
        VisUtils::fillRect(
            xLft+i*xItv, xLft+(i+1)*xItv,
            0.5*yTop,    0.5*yBot );
    }
    VisUtils::disableBlending();
}


// ------------------------------------------------------
void OpacityChooser::drawPath( const bool &inSelectMode )
// ------------------------------------------------------
{
    double w, h;
    double xLft, xRgt;
    double yBot, yTop;
    double pix;
    double size;

    // get size of sides
    canvas->getSize( w, h );
    // get size of 1 pixel
    pix = canvas->getPixelSize();

    // calc size of bounding box
    xLft = -0.5*w;
    xRgt =  0.5*w;
    yTop =  0.5*h;
    yBot = -0.5*h;

    // size of positions
    size = positionsX.size();

    // selection mode
    if ( inSelectMode == true )
    {}
    // rendering mode
    else
    {
        VisUtils::enableLineAntiAlias();
        for ( int i = 0; i < size-1; ++i )
        {
            VisUtils::setColorBlack();
            VisUtils::drawLineDashed(
                positionsX[i]*xRgt+pix, positionsX[i+1]*xRgt+pix,
                positionsY[i]*yTop-pix, positionsY[i+1]*yTop-pix );

            VisUtils::setColorLtGray();
            VisUtils::drawLineDashed(
                positionsX[i]*xRgt, positionsX[i+1]*xRgt,
                positionsY[i]*yTop, positionsY[i+1]*yTop );
        }

        VisUtils::disableLineAntiAlias();
    }
}


// --------------------------------------------------------
void OpacityChooser::drawPoints( const bool &inSelectMode )
// --------------------------------------------------------
{
    double w, h;
    double xLft, xRgt;
    double yBot, yTop;
    double pix;
    int size;
    double hdlDOF;

    // get size of sides
    canvas->getSize( w, h );
    // get size of 1 pixel
    pix = canvas->getPixelSize();

    // calc size of bounding box
    xLft = -0.5*w;
    xRgt =  0.5*w;
    yTop =  0.5*h;
    yBot = -0.5*h;

    // size of handle
    hdlDOF = hdlSize*pix;

    // size of positions
    size = positionsX.size();

    // selection mode
    if ( inSelectMode == true )
    {
        for ( int i = 0; i < size-1; ++i )
        {
            glPushName( i );
            VisUtils::fillRect(
                positionsX[i]*xRgt-5.0*pix, positionsX[i]*xRgt+5.0*pix,
                positionsY[i]*yTop+5.0*pix, positionsY[i]*yTop-5.0*pix );
            glPopName();
        }

        if ( size == 1 )
        {
            glPushName( size-1 );
            VisUtils::fillRect(
                positionsX[size-1]*xRgt-5.0*pix, positionsX[size-1]*xRgt+5.0*pix,
                positionsY[size-1]*yTop+5.0*pix, positionsY[size-1]*yTop-5.0*pix );
            glPopName();
        }
        else if ( size > 1 )
        {
            double agl = Utils::calcAngleDg(
                positionsX[size-1]*xRgt - positionsX[size-2]*xRgt,
                positionsY[size-1]*yTop - positionsY[size-2]*yTop );

            // arrow
            glPushMatrix();
            glTranslatef(
                positionsX[size-1]*xRgt,
                positionsY[size-1]*yTop,
                0.0 );
            glRotatef( 90.0+agl, 0.0, 0.0, 1.0 );

                glPushName( size-1 );
                VisUtils::fillTriangle(
                   -hdlDOF, 2.0*hdlDOF,
                    0.0,    0.0,
                    hdlDOF, 2.0*hdlDOF );
                glPopName();

            glPopMatrix();
        }
    }
    // rendering mode
    else
    {
        VisUtils::enableLineAntiAlias();
        for ( int i = 0; i < size-1; ++i )
        {
            VisUtils::setColorBlack();
            VisUtils::drawLine(
                positionsX[i]*xRgt-4.0*pix, positionsX[i]*xRgt+6.0*pix,
                positionsY[i]*yTop+4.0*pix, positionsY[i]*yTop-6.0*pix );
            VisUtils::drawLine(
                positionsX[i]*xRgt-4.0*pix, positionsX[i]*xRgt+6.0*pix,
                positionsY[i]*yTop-6.0*pix, positionsY[i]*yTop+4.0*pix );

            if ( active == true )
                VisUtils::setColorRed();
            else
                VisUtils::setColorWhite();

            VisUtils::drawLine(
                positionsX[i]*xRgt-5.0*pix, positionsX[i]*xRgt+5.0*pix,
                positionsY[i]*yTop+5.0*pix, positionsY[i]*yTop-5.0*pix );
            VisUtils::drawLine(
                positionsX[i]*xRgt-5.0*pix, positionsX[i]*xRgt+5.0*pix,
                positionsY[i]*yTop-5.0*pix, positionsY[i]*yTop+5.0*pix );
        }

        if ( size == 1 )
        {
            VisUtils::setColorBlack();
            VisUtils::drawLine(
                positionsX[size-1]*xRgt-4.0*pix, positionsX[size-1]*xRgt+6.0*pix,
                positionsY[size-1]*yTop+4.0*pix, positionsY[size-1]*yTop-6.0*pix );
            VisUtils::drawLine(
                positionsX[size-1]*xRgt-4.0*pix, positionsX[size-1]*xRgt+6.0*pix,
                positionsY[size-1]*yTop-6.0*pix, positionsY[size-1]*yTop+4.0*pix );


            if ( active == true )
                VisUtils::setColorRed();
            else
                VisUtils::setColorWhite();

            VisUtils::drawLine(
                positionsX[size-1]*xRgt-5.0*pix, positionsX[size-1]*xRgt+5.0*pix,
                positionsY[size-1]*yTop+5.0*pix, positionsY[size-1]*yTop-5.0*pix );
            VisUtils::drawLine(
                positionsX[size-1]*xRgt-5.0*pix, positionsX[size-1]*xRgt+5.0*pix,
                positionsY[size-1]*yTop-5.0*pix, positionsY[size-1]*yTop+5.0*pix );
        }
        else if ( size > 1 )
        {
            double agl = Utils::calcAngleDg(
                positionsX[size-1]*xRgt - positionsX[size-2]*xRgt,
                positionsY[size-1]*yTop - positionsY[size-2]*yTop );

            // drop shadow
            glPushMatrix();
            glTranslatef(
                positionsX[size-1]*xRgt+pix,
                positionsY[size-1]*yTop-pix,
                0.0 );
            glRotatef( 90.0+agl, 0.0, 0.0, 1.0 );

                VisUtils::setColorBlack();
                VisUtils::drawTriangle(
                   -hdlDOF, 2.0*hdlDOF,
                    0.0,    0.0,
                    hdlDOF, 2.0*hdlDOF );
                VisUtils::drawLine(
                   -2.0*hdlDOF, 2.0*hdlDOF,
                    0.0, 0.0 );

            glPopMatrix();

            // arrow
            glPushMatrix();
            glTranslatef(
                positionsX[size-1]*xRgt,
                positionsY[size-1]*yTop,
                0.0 );
            glRotatef( 90.0+agl, 0.0, 0.0, 1.0 );

                if ( active == true )
                    VisUtils::setColorGreen();
                else
                    VisUtils::setColorWhite();
                VisUtils::fillTriangle(
                   -hdlDOF, 2.0*hdlDOF,
                    0.0,    0.0,
                    hdlDOF, 2.0*hdlDOF );

                VisUtils::setColorMdGray();
                VisUtils::drawTriangle(
                   -hdlDOF, 2.0*hdlDOF,
                    0.0,    0.0,
                    hdlDOF, 2.0*hdlDOF );
                VisUtils::drawLine(
                   -2.0*hdlDOF, 2.0*hdlDOF,
                    0.0, 0.0 );

            glPopMatrix();
        }
        VisUtils::disableLineAntiAlias();
    }
}


// -- utility event handlers ------------------------------------


// --------------------------------------------------------
void OpacityChooser::handleHits( const vector< int > &ids )
// --------------------------------------------------------
{
    if ( mouseSide == MSE_SIDE_LFT )
    {
        if ( 0 <= ids[0] && static_cast <size_t> (ids[0]) < positionsX.size() )
            dragIdx = ids[0];
    }
    else if ( mouseSide == MSE_SIDE_RGT )
    {
        if ( 0 <= ids[0] && static_cast <size_t> (ids[0]) < positionsX.size() )
        {
            /*
            positionsX.erase( positionsX.begin()+ids[0] );
            positionsY.erase( positionsY.begin()+ids[0] );
            */

            mediator->handleDOFOpaClear( ids[0] );
        }
    }
}


// ------------------------------
void OpacityChooser::handleDrag()
// ------------------------------
{
    if ( 0 <= dragIdx && static_cast <size_t> (dragIdx) < positionsX.size() )
    {
        double w, h;
        double xLft, xRgt;
        double yBot, yTop;
        double xCur, yCur;

        // get size of sides
        canvas->getSize( w, h );

        // calc size of bounding box
        xLft = -0.5*w;
        xRgt =  0.5*w;
        yTop =  0.5*h;
        yBot = -0.5*h;

        // get cur mouse position
        canvas->getWorldCoords( xMouseCur, yMouseCur, xCur, yCur );

        // normalize mouse position
        // xLft is -1.0, xRgt is 1.0, yTop = 1.0 and yBot = -1.0
        xCur = xCur/xRgt;
        yCur = yCur/yTop;

        positionsX[dragIdx] = xCur;
        positionsY[dragIdx] = yCur;

        mediator->handleDOFOpaUpdate(
            dragIdx,
            0.5*(xCur+1),
            yCur );
    }
}


// -- hit detection -------------------------------------------------


// ------------------------------
void OpacityChooser::processHits(
    GLint hits,
    GLuint buffer[] )
// ------------------------------
{
    GLuint *ptr;
    int number;
    vector< int > ids;

    ptr = (GLuint*) buffer;

    if ( hits > 0 )
    {
        // if necassary, advance to closest hit
        if ( hits > 1 )
        {
            for ( int i = 0; i < ( hits-1 ); ++i )
            {
                number = *ptr;
                ++ptr; // number;
                ++ptr; // z1
                ++ptr; // z2
                for ( int j = 0; j < number; ++j )
                    ++ptr; // names
            }
        }

        // last hit
        number = *ptr;
        ++ptr; // number
        ++ptr; // z1
        ++ptr; // z2

        for ( int i = 0; i < number; ++i )
        {
            ids.push_back( *ptr );
            ++ptr;
        }

        handleHits( ids );
    }

    ptr = NULL;
}


// -- end -----------------------------------------------------------
