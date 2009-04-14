// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./distrplot.cpp

#include "wx.hpp" // precompiled headers

#include "distrplot.h"

// -- constructors and destructor -----------------------------------

using namespace std;

// ----------------------------
DistrPlot::DistrPlot(
    Mediator* m,
    Graph* g,
    GLCanvas* c )
        : Visualizer( m, g, c )
// ----------------------------
{
    maxNumber    =  0;
    minHgtHintPx =  4;
    maxWthHintPx = 10;

    diagram        = NULL;
    showDgrm       = false;
    attrValIdxDgrm = -1;
}


// --------------------
DistrPlot::~DistrPlot()
// --------------------
{
    clearValues();
    diagram = NULL;
}


// -- set functions -------------------------------------------------


// ---------------------------
void DistrPlot::setValues(
    const int &idx,
    const vector< int > &num )
// ---------------------------
{
    clearValues();

    attrIdx = idx;
    number  = num;
    calcMaxNumber();
    calcPositions();
}


// --------------------------
void DistrPlot::clearValues()
// --------------------------
{
    number.clear();
    attrIdx    = 0;
    maxNumber  = 0;

    clearPositions();
}


// ----------------------------------------
void DistrPlot::setDiagram( Diagram *dgrm )
// ----------------------------------------
{
    diagram = dgrm;
}


// -- visualization functions  --------------------------------------


// --------------------------------------------------
void DistrPlot::visualize( const bool &inSelectMode )
// --------------------------------------------------
{
    // have textures been generated
    if ( texCharOK != true )
        genCharTex();

    // check if positions are ok
    if ( geomChanged == true )
        calcPositions();

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

        //setScalingTransf();
        drawPlot( inSelectMode );

        finishSelectMode(
            hits,
            selectBuf );
    }
    else
    {
        clear();
        //setScalingTransf();
        drawPlot( inSelectMode );
        drawAxes( inSelectMode );
        drawLabels( inSelectMode );
        if ( showDgrm == true )
            drawDiagram( inSelectMode );
    }
}


// -------------------------------------------------
void DistrPlot::drawAxes( const bool &inSelectMode )
// -------------------------------------------------
{
    double w, h;
    double xLft, xRgt;
    double yBot, yMid, yTop;
    double pix;

    // get size of sides
    canvas->getSize( w, h );
    // get size of 1 pixel
    pix = canvas->getPixelSize();

    // calc size of bounding box
    xLft = -0.5*w+20*pix;
    xRgt =  0.5*w-10*pix;
    yTop =  0.5*h-10*pix;
    yBot = -0.5*h+20*pix;
    yMid =  0.5*( yTop+yBot );

    // rendering mode
    if ( inSelectMode != true )
    {
        // draw guides
        VisUtils::setColorLtGray();
        VisUtils::drawLine( xLft, xRgt, yTop, yTop );
        VisUtils::drawLine( xLft, xRgt, yMid, yMid );
        VisUtils::drawLine( xRgt, xRgt, yBot, yTop );

        // x- & y-axis
        VisUtils::setColorMdGray();
        VisUtils::drawLine( xLft, xLft, yBot, yTop );
        VisUtils::drawLine( xLft, xRgt, yBot, yBot );
    }
}


// ---------------------------------------------------
void DistrPlot::drawLabels( const bool &inSelectMode )
// ---------------------------------------------------
{
    double w, h;
    double x, y;
    double pix;
    double scaling;
    string xLabel;
    string min, max;

    // get size of sides
    canvas->getSize( w, h );
    // get size of 1 pixel
    pix = canvas->getPixelSize();
    // calc scaling to use
    scaling = ( 12*pix )/(double)CHARHEIGHT;

    // color
    VisUtils::setColorBlack();

    // y-axis labels
    x = -0.5*w+9*pix;
    y =  0;
    VisUtils::drawLabelVertCenter( texCharId, x, y, scaling, "Number" );

    if ( number.size() > 0 )
    {
        // x-axis label
        xLabel = graph->getAttribute( attrIdx )->getName();
        x =  0.0;
        y = -0.5*h+9*pix;
        VisUtils::drawLabelCenter( texCharId, x, y, scaling, xLabel );

        max = Utils::intToStr( maxNumber );
        x = -0.5*w+13*pix;
        y =  0.5*h-10*pix;
        VisUtils::drawLabelVertBelow( texCharId, x, y, scaling, max );

        min = "0";
        y = -0.5*h+20*pix;
        VisUtils::drawLabelVertAbove( texCharId, x, y, scaling, min );
    }
}


// -------------------------------------------------
void DistrPlot::drawPlot( const bool &inSelectMode )
// -------------------------------------------------
{
    int    sizePositions;
    double hCanv;
    double xLft, xRgt, yTop, yBot;
    double pix;
    ColorRGB col;

    hCanv = canvas->getHeight();
    pix = canvas->getPixelSize();
    sizePositions = positions.size();

    // selection mode
    if ( inSelectMode == true )
    {
        yBot = -0.5*hCanv + 20*pix;

        for( int i = 0; i < sizePositions; ++i )
        {
            xLft = positions[i].x - 0.5*width;
            xRgt = positions[i].x + 0.5*width;
            yTop = positions[i].y;

            glPushName( i );
            VisUtils::fillRect( xLft, xRgt, yTop, yBot );
            glPopName();
        }
    }
    // rendering mode
    else
    {
        yBot = -0.5*hCanv + 20*pix;

        for( int i = 0; i < sizePositions; ++i )
        {
            xLft = positions[i].x - 0.5*width;
            xRgt = positions[i].x + 0.5*width;
            yTop = positions[i].y;

            VisUtils::mapColorCoolGreen( col );
            col.a = 0.7;
            VisUtils::setColor( col );

            if ( xRgt-xLft < pix )
                VisUtils::drawLine( xLft, xLft, yTop, yBot );
            else
            {
                VisUtils::enableBlending();
                VisUtils::fillRect( xLft, xRgt, yTop, yBot );
                VisUtils::disableBlending();
            }
        }
    }
}


// ----------------------------------------------------
void DistrPlot::drawDiagram( const bool &inSelectMode )
// ----------------------------------------------------
{
    if ( inSelectMode != true )
    {
        double pix      = canvas->getPixelSize();
        double scaleTxt = (( 12*pix )/(double)CHARHEIGHT)/scaleDgrm;

        vector< Attribute* > attrs;
        attrs.push_back( graph->getAttribute( attrIdx ) );
        vector< double > vals;
        vals.push_back( attrValIdxDgrm );

        glPushMatrix();
        glTranslatef( posDgrm.x, posDgrm.y, 0.0 );
        glScalef( scaleDgrm, scaleDgrm, scaleDgrm );

        // drop shadow
        VisUtils::setColorMdGray();
        VisUtils::fillRect(
            -1.0 + 4.0*pix/scaleDgrm,
             1.0 + 4.0*pix/scaleDgrm,
             1.0 - 4.0*pix/scaleDgrm,
            -1.0 - 4.0*pix/scaleDgrm );
        // diagram
        diagram->visualize(
            inSelectMode,
            canvas,
            attrs,
            vals );

        VisUtils::setColorBlack();
        VisUtils::drawLabelRight( texCharId, -0.98, 1.1, scaleTxt, msgDgrm );

        glPopMatrix();
    }
}


// -- input event handlers ------------------------------------------


// ------------------------------------
void DistrPlot::handleMouseMotionEvent(
    const int &x,
    const int &y )
// ------------------------------------
{
    Visualizer::handleMouseMotionEvent( x, y );

    // redraw in select mode
    visualize( true );
    // redraw in render mode
    visualize( false );
}

/*
// ------------------------------------
void DistrPlot::handleMouseEnterEvent()
// ------------------------------------
{}
*/
/*
// ------------------------------------
void DistrPlot::handleMouseLeaveEvent()
// ------------------------------------
{}
*/

// -- utility data functions ------------------------------------


// ----------------------------
void DistrPlot::calcMaxNumber()
// ----------------------------
{
    maxNumber = 0;
    for ( size_t i = 0; i < number.size(); ++i )
    {
        if ( number[i] > maxNumber )
            maxNumber = number[i];
    }
}


// -- utility drawing functions ---------------------------------

// ***
/*
// --------------------
void DistrPlot::clear()
// --------------------
{
    VisUtils::clear( clearColor );
}
*/

// -------------------------------
void DistrPlot::setScalingTransf()
// -------------------------------
{
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    double f = canvas->getScaleFactor();
    glScalef( f, f, f );
    glTranslatef(
        canvas->getXTranslation(),
        canvas->getYTranslation(),
        0.0 );
    glLoadIdentity();
}


// ----------------------------------------------
void DistrPlot::displTooltip( const size_t &posIdx )
// ----------------------------------------------
{
    if ( 0 <= posIdx && posIdx < number.size() )
    {
        Attribute* attr = graph->getAttribute( attrIdx );
        string xLabel   = attr->getName();
        string value    = "";
        if ( posIdx < static_cast <size_t> (attr->getSizeCurValues()) )
            value = attr->getCurValue( posIdx )->getValue();

        msgDgrm.clear();

        // x-axis label
        /*
        msgDgrm.append( xLabel );
        msgDgrm.append( ": " );
        msgDgrm.append( value );
        msgDgrm.append( "\n" );
        */
        // y-axis label
        msgDgrm.append( Utils::intToStr( number[posIdx] ) );
        msgDgrm.append( " nodes; " );
        msgDgrm.append( Utils::dblToStr(
            Utils::perc( number[posIdx], graph->getSizeNodes() ) ) );
        msgDgrm.append( "%" );

        if ( diagram == NULL )
        {
            // show tooltip
            canvas->showToolTip( msgDgrm );
        }
        else
        {
            // calc diagram position
            double xM, yM;
            double xD, yD;
            canvas->getWorldCoords( xMouseCur, yMouseCur, xM, yM );

            if ( xM < 0 )
                xD = xM+1.0*scaleDgrm;
            else
                xD = xM-1.0*scaleDgrm;

            if ( yM < 0 )
                yD = yM+1.0*scaleDgrm;
            else
                yD = yM-1.0*scaleDgrm;

            posDgrm.x = xD;
            posDgrm.y = yD;

            showDgrm       = true;
            attrValIdxDgrm = posIdx;
        }

        // free memory
        attr = NULL;
    }
}


// ----------------------------
void DistrPlot::calcPositions()
// ----------------------------
{
    // update flag
    geomChanged = false;

    // calc positions
    if ( number.size() > 0 )
    {
        // calc positions
        double w, h, pix;
        double xLft, xRgt, yBot,  yTop;
        double numX, numY;
        double fracX;
        double x, y;
        double ratio;

        // get size of sides & 1 pixel
        canvas->getSize( w, h );
        pix = canvas->getPixelSize();

        // calc size of bounding box
        xLft = -0.5*w+20*pix;
        xRgt =  0.5*w-10*pix;
        yTop =  0.5*h-10*pix;
        yBot = -0.5*h+20*pix;

        // get number of values per axis
        numX = graph->getAttribute( attrIdx )->getSizeCurValues();
        numY = number.size();

        // get intervals for x-axis
        if ( numX > 1 )
            fracX = ( 1.0 / (double)numX )*( xRgt-xLft );
        else
            fracX = 1.0;

        // calc width
        if ( fracX < maxWthHintPx*pix )
            width = fracX;
        else
            width = maxWthHintPx*pix;

        // calc positions
        positions.clear();
        for ( size_t i = 0; i < number.size(); ++i )
        {
            // calc ratio
            ratio = (double)number[i]/(double)maxNumber;

            // center, top
            x = xLft + 0.5*fracX + i*fracX;
            y = yBot + ratio*( yTop-yBot );
            if ( y-yBot < pix*minHgtHintPx )
                y += pix*minHgtHintPx;

            Position2D pos;
            pos.x = x;
            pos.y = y;
            positions.push_back( pos );
        }

        // diagram scale factor to draw 120 x 120 pix diagram
        scaleDgrm = 120.0*(pix/2.0);
    }
}


// -----------------------------
void DistrPlot::clearPositions()
// -----------------------------
{
    positions.clear();
    width = 0.0;
}


// -- hit detection -------------------------------------------------


// -------------------------
void DistrPlot::processHits(
    GLint hits,
    GLuint buffer[] )
// -------------------------
{
    GLuint *ptr;
    int    number;
    int    name;

    ptr = (GLuint*) buffer;

    if ( hits > 0 )
    {
        // if necassary advance to last hit
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

        name = *ptr;

        displTooltip( name );
    }
    else
    {
        canvas->clearToolTip();
        showDgrm = false;
    }

    ptr = NULL;
}


// -- end -----------------------------------------------------------
