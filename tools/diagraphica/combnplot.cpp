// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./combnplot.cpp

#include "wx.hpp" // precompiled headers

#include "combnplot.h"

// -- constructors and destructor -----------------------------------

using namespace std;

// ------------------------
CombnPlot::CombnPlot(
    Mediator* m,
    Graph* g,
    GLCanvas* c )
    : Visualizer( m, g, c )
// ------------------------
{
    maxAttrCard      = 0;
    maxNumberPerComb = 0;

    minHgtHintPixBC =  4;
    maxWthHintPixBC = 10;

    mouseInside   = false;
    mouseCombnIdx = -1;

    diagram        = NULL;
    showDgrm       = false;
}


// --------------------
CombnPlot::~CombnPlot()
// --------------------
{
    clearValues();

    diagram = NULL;
    attrValIdcsDgrm.clear();
}


// ----------------------------------------
void CombnPlot::setDiagram( Diagram *dgrm )
// ----------------------------------------
{
    diagram = dgrm;
}


// -- set data functions --------------------------------------------


// --------------------------------
void CombnPlot::setValues(
    const vector< int > &attrIndcs,
    vector< vector< int > > &combs,
    vector< int > &number )
// --------------------------------
{
    attributeIndcs = attrIndcs;
    combinations   = combs;
    numberPerComb  = number;

    initLabels();
    calcMaxAttrCard();
    calcMaxNumberPerComb();
    calcPositions();
}


// --------------------------
void CombnPlot::clearValues()
// --------------------------
{
    attributeIndcs.clear();
    combinations.clear();
    numberPerComb.clear();

    clearPositions();
}


// -- visualization functions  --------------------------------------


// --------------------------------------------------
void CombnPlot::visualize( const bool &inSelectMode )
// --------------------------------------------------
{
    // have textures been generated
    if ( texCharOK != true )
        genCharTex();

    // check if positions are ok
    if ( geomChanged == true )
        calcPositions();

    // selection mode
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
        //drawAxes( inSelectMode );
        drawPlot( inSelectMode );

        finishSelectMode(
            hits,
            selectBuf );
    }
    // rendering mode
    else
    {
        clear();
        //setScalingTransf();
        drawPlot( inSelectMode );
        drawAxes( inSelectMode );
        drawLabels( inSelectMode );
        drawMousePos( inSelectMode );
        if ( showDgrm == true )
            drawDiagram( inSelectMode );
    }
}


// -------------------------------------------------
void CombnPlot::drawAxes( const bool &inSelectMode )
// -------------------------------------------------
{
    // bar chart
    drawAxesBC( inSelectMode );
    // combination plot
    drawAxesCP( inSelectMode );
}


// ---------------------------------------------------
void CombnPlot::drawAxesBC( const bool &inSelectMode )
// ---------------------------------------------------
{
    double w, h;
    int    numAttr;
    double xLft, xRgt;
    double yBot, yTop;
    double pix;

    // get size of sides
    canvas->getSize( w, h );
    // get size of 1 pixel
    pix = canvas->getPixelSize();
    // get num attributes
    numAttr = attributeIndcs.size();

    // calc size of bounding box
    xLft = -0.5*w+25*pix;
    xRgt =  0.5*w-10*pix;
    yTop =  0.5*h-10*pix;
    if ( numAttr > 0 )
        yBot = yTop - (h - (20+10+10)*pix)/(double)(numAttr+1);
    else
        yBot = yTop - 0.5*(h - (20+10+10)*pix);

    // rendering mode
    if ( inSelectMode != true )
    {
        // draw guides
        VisUtils::setColorLtGray();
        VisUtils::drawLine( xLft, xRgt, yTop, yTop );
        VisUtils::drawLine( xRgt, xRgt, yBot, yTop );

        // x- & y-axis
        VisUtils::setColorMdGray();
        VisUtils::drawLine( xLft, xLft, yBot, yTop );
        VisUtils::drawLine( xLft, xRgt, yBot, yBot );
    }
}


// ---------------------------------------------------
void CombnPlot::drawAxesCP( const bool &inSelectMode )
// ---------------------------------------------------
{
    double w, h;
    int    numAttr;
    double xLft, xRgt;
    double yBot, yTop;
    double pix;

    // get size of sides
    canvas->getSize( w, h );
    // get size of 1 pixel
    pix = canvas->getPixelSize();
    // get num attributes
    numAttr = attributeIndcs.size();

    // calc size of bounding box
    xLft = -0.5*w+25*pix;
    xRgt =  0.5*w-10*pix;
    yBot = -0.5*h+20*pix;
    if ( numAttr > 0 )
        yTop = yBot + numAttr*(h - (20+10+10)*pix)/(double)(numAttr+1);
    else
        yTop = yBot + 0.5*(h - (20+10+10)*pix);

    // rendering mode
    if ( inSelectMode != true )
    {
        VisUtils::setColorMdGray();

        // draw guides
        VisUtils::drawLine( xLft, xRgt, yTop, yTop );
        VisUtils::drawLine( xRgt, xRgt, yBot, yTop );

        // x- & y-axis
        VisUtils::drawLine( xLft, xLft, yBot, yTop );
        VisUtils::drawLine( xLft, xRgt, yBot, yBot );

        // subdivisions for attributes
        if ( posRgtBot.size() > 0 && posRgtBot[0].size() > 1 )
        {
            VisUtils::setColorMdGray();
            xLft += pix;
            xRgt -= pix;
            for ( size_t i = 0; i < posRgtBot[0].size()-1; ++i )
            {
                yTop = posRgtBot[0][i].y;
                yBot = yTop;
                VisUtils::drawLine( xLft, xRgt, yTop, yBot );
            }
        }
    }
}


// ---------------------------------------------------
void CombnPlot::drawLabels( const bool &inSelectMode )
// ---------------------------------------------------
{
    // bar chart
    drawLabelsBC( inSelectMode );
    // combination plot
    drawLabelsCP( inSelectMode );
}


// -----------------------------------------------------
void CombnPlot::drawLabelsBC( const bool &inSelectMode )
// -----------------------------------------------------
{
    double w, h;
    int    numAttr;
    double xLft, xRgt;
    double yTop, yBot;
    double x, y;
    double pix;
    double scaling;
    string min, max;

    // get size of sides
    canvas->getSize( w, h );
    // get size of 1 pixel
    pix = canvas->getPixelSize();
    // calc scaling to use
    scaling = ( 12*pix )/(double)CHARHEIGHT;
    // number attributes
    numAttr = attributeIndcs.size();

    // color
    VisUtils::setColorBlack();

    // y-axis labels
    xLft = -0.51*w+3*pix;
    xRgt = -0.50*w+12*pix;
    yTop =  0.50*h-10*pix;
    if ( numAttr > 0 )
        yBot = yTop - (h - (20+10+10)*pix)/(double)(numAttr+1);
    else
        yBot = yTop - 0.5*(h - (20+10+10)*pix);

    VisUtils::drawLabelInBoundBox(
                texCharId,
                xLft,      xRgt,
                yTop,      yBot,
                scaling,   "Number" );

    if ( combinations.size() > 0 )
    {
        // max number
        max = Utils::intToStr( maxNumberPerComb );
        x   = -0.5*w+13*pix;
        y   =  0.5*h-10*pix;
        VisUtils::drawLabelVertBelow( texCharId, x, y, scaling, max );

        // min number
        min = "0";
        y   = yBot;
        VisUtils::drawLabelVertAbove( texCharId, x, y, scaling, min );
    }
}


// -----------------------------------------------------
void CombnPlot::drawLabelsCP( const bool &inSelectMode )
// -----------------------------------------------------
{
    double w, h;
    double numAttr;
    double xLft, xRgt;
    double yTop, yBot;
    double x, y;
    double pix;
    double scaling;
    string min, max;

    // get size of sides
    canvas->getSize( w, h );
    // get size of 1 pixel
    pix = canvas->getPixelSize();
    // calc scaling to use
    scaling = ( 12*pix )/(double)CHARHEIGHT;
    // number attributes
    numAttr = attributeIndcs.size();

    // color
    VisUtils::setColorBlack();

    // x-axis label
    x =  0.0;
    y =  -0.5*h+9*pix;
    VisUtils::drawLabelCenter( texCharId, x, y, scaling, "Combinations" );

    if ( numAttr > 0 )
    {
        // y-axis labels

        xLft = -0.51*w+3*pix;
        xRgt = -0.5*w+12*pix;

        for ( size_t i = 0; i < attributeLabels.size(); ++i )
        {
            if ( i > 0 )
                yTop = posRgtBot[0][i-1].y;
            else
                yTop = -0.5*h+20*pix + numAttr*(h - (20+10+10)*pix)/(double)(numAttr+1);


            yBot = posRgtBot[0][i].y;

            VisUtils::drawLabelInBoundBox(
                texCharId,
                xLft,      xRgt,
                yTop,      yBot,
                scaling,   attributeLabels[i] );
        }
    }
}


// -------------------------------------------------
void CombnPlot::drawPlot( const bool &inSelectMode )
// -------------------------------------------------
{
    // bar chart
    drawPlotBC( inSelectMode );
    // combination plot
    drawPlotCP( inSelectMode );
}


// ---------------------------------------------------
void CombnPlot::drawPlotBC( const bool &inSelectMode )
// ---------------------------------------------------
{
    int    sizePositions;
    double hCanv;
    int    numAttr;
    double xLft, xRgt, yTop, yBot;
    double pix;
    ColorRGB col;

    hCanv = canvas->getHeight();
    pix = canvas->getPixelSize();
    sizePositions = posBC.size();
    numAttr = attributeIndcs.size();

    if ( sizePositions > 0 )
        yBot = 0.5*hCanv-10*pix - (hCanv - (20+10+10)*pix)/(double)(numAttr+1);
    else
        yBot = 0.5*hCanv-10*pix - 0.5*(hCanv - (20+10+10)*pix);

    // selection mode
    if ( inSelectMode == true )
    {
        for( int i = 0; i < sizePositions; ++i )
        {
            xLft = posBC[i].x - 0.5*widthBC;
            xRgt = posBC[i].x + 0.5*widthBC;
            yTop = posBC[i].y;

            glPushName( i );
            VisUtils::fillRect( xLft, xRgt, yTop, yBot );
            glPopName();
        }
    }
    // rendering mode
    else
    {
        VisUtils::mapColorCoolGreen( col );
        col.a = 0.7;

        for( int i = 0; i < sizePositions; ++i )
        {
            xLft = posBC[i].x - 0.5*widthBC;
            xRgt = posBC[i].x + 0.5*widthBC;
            yTop = posBC[i].y;

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


// ---------------------------------------------------
void CombnPlot::drawPlotCP( const bool &inSelectMode )
// ---------------------------------------------------
{
    double xLft  = 0;
    double xRgt  = 0;
    double yTop  = 0;
    double yBot  = 0;
    double pix   = 0.0;
    ColorRGB col;

    pix   = canvas->getPixelSize();

    // selection mode
    if ( inSelectMode == true )
    {
        for ( size_t i = 0; i < posLftTop.size(); ++i )
        {
            // name per collumn
            glPushName( i );

            xLft = posLftTop[i][0].x;
            xRgt = posRgtBot[i][0].x;

            yTop = posLftTop[i][0].y;
            yBot = posRgtBot[i][posRgtBot[i].size()-1].y;

            VisUtils::fillRect( xLft, xRgt, yTop, yBot );

            glPopName();
        }
    }
    // rendering mode
    else
    {
        for ( size_t i = 0; i < posLftTop.size(); ++i )
        {
            for ( size_t j = 0; j < posLftTop[i].size(); ++j )
            {
                xLft = posLftTop[i][j].x;
                yTop = posLftTop[i][j].y;

                xRgt = posRgtBot[i][j].x;
                yBot = posRgtBot[i][j].y;

                VisUtils::mapColorQualPair(
                    combinations[i][j],
                    maxAttrCard-1,
                    col );
                col.a = 0.5;
                VisUtils::setColor( col );

                if ( xRgt - xLft > pix )
                {
                    VisUtils::enableBlending();
                    VisUtils::fillRect( xLft, xRgt, yTop, yBot );
                    VisUtils::disableBlending();
                }
                else
                {
                    VisUtils::enableLineAntiAlias();
                    VisUtils::drawLine( xLft, xLft, yTop, yBot );
                    VisUtils::disableLineAntiAlias();
                }
            } // for j
        } // for i
    } // if .. else
}


// -----------------------------------------------------
void CombnPlot::drawMousePos( const bool &inSelectMode )
// -----------------------------------------------------
{
    // rendering mode
    if ( inSelectMode != true )
    {
        double xLft, xRgt, yTop, yBot;

        if ( 0 <= mouseCombnIdx &&
             mouseCombnIdx < combinations.size() )
        {
            // x-coords
            xLft = posLftTop[mouseCombnIdx][0].x;
            xRgt = posRgtBot[mouseCombnIdx][0].x;
            // y-coords
            yTop = posLftTop[mouseCombnIdx][0].y;
            yBot = posRgtBot[mouseCombnIdx][posRgtBot[mouseCombnIdx].size()-1].y;

            // draw
            VisUtils::setColorRed();
            VisUtils::drawRect( xLft, xRgt, yTop, yBot );
        }
    }
}


// ----------------------------------------------------
void CombnPlot::drawDiagram( const bool &inSelectMode )
// ----------------------------------------------------
{
    if ( inSelectMode != true )
    {
        double pix      = canvas->getPixelSize();
        double scaleTxt = (( 12*pix )/(double)CHARHEIGHT)/scaleDgrm;

        vector< Attribute* > attrs;
        for ( size_t i = 0; i < attributeIndcs.size(); ++i )
            attrs.push_back( graph->getAttribute( attributeIndcs[i] ) );

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
            attrValIdcsDgrm );

        VisUtils::setColorBlack();
        VisUtils::drawLabelRight( texCharId, -0.98, 1.1, scaleTxt, msgDgrm );

        glPopMatrix();
    }
}


// -- input event handlers ------------------------------------------


// ------------------------------------
void CombnPlot::handleMouseMotionEvent(
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
void CombnPlot::handleMouseEnterEvent()
// ------------------------------------
{
    if ( posLftTop.size() > 0 )
        mouseInside = true;
}


// ------------------------------------
void CombnPlot::handleMouseLeaveEvent()
// ------------------------------------
{
    mouseInside = false;
}
*/

// -- utility data functions ----------------------------------------


// -------------------------
void CombnPlot::initLabels()
// -------------------------
{
    attributeLabels.clear();
    for ( size_t i = 0; i < attributeIndcs.size(); ++i )
        attributeLabels.push_back(
            graph->getAttribute( attributeIndcs[i] )->getName() );
}


// ------------------------------
void CombnPlot::calcMaxAttrCard()
// ------------------------------
{
    maxAttrCard = 0;
    for ( size_t i = 0; i < attributeIndcs.size(); ++i )
    {
        if ( graph->getAttribute(
                attributeIndcs[i] )->getSizeCurValues() > maxAttrCard )
            maxAttrCard = graph->getAttribute(
                attributeIndcs[i] )->getSizeCurValues();
    }
}


// -----------------------------------
void CombnPlot::calcMaxNumberPerComb()
// -----------------------------------
{
    maxNumberPerComb = 0;
    for ( size_t i = 0; i < numberPerComb.size(); ++i )
    {
        if ( numberPerComb[i] > maxNumberPerComb )
            maxNumberPerComb = numberPerComb[i];
    }
}


// -- utility drawing functions -------------------------------------

// ***
/*
// --------------------
void CombnPlot::clear()
// --------------------
{
    VisUtils::clear( clearColor );
}
*/

// -------------------------------
void CombnPlot::setScalingTransf()
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
}


// ----------------------------------------------
void CombnPlot::displTooltip( const size_t &posIdx )
// ----------------------------------------------
{
    if ( 0 <= posIdx && posIdx < combinations.size() )
    {
        Attribute* attr = NULL;
        Value* val      = NULL;

        msgDgrm.clear();
        /*
        for ( int i = 0; i < numAttrs; ++i )
        {
            attr = graph->getAttribute( attributeIndcs[i] );
            msg.append( attr->getName() );

            if ( attr->getSizeCurValues() > 0 )
            {
                val = attr->getCurValue( combinations[posIdx][i] );
                msg.append( " = " );
                msg.append( val->getValue() );
            }
            else
                msg.append( " " );

            msg.append( "; " );
        }
        */
        // number
        msgDgrm.append( Utils::dblToStr( numberPerComb[posIdx] ) );
        msgDgrm.append( "nodes; " );
        // percentage
        msgDgrm.append( Utils::dblToStr(
                Utils::perc( numberPerComb[posIdx], graph->getSizeNodes() ) ) );
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

            showDgrm = true;

            attrValIdcsDgrm.clear();
            for ( size_t i = 0; i < attributeIndcs.size(); ++i )
                attrValIdcsDgrm.push_back( combinations[posIdx][i] );
        }

        // reset ptrs
        attr = NULL;
        val  = NULL;
    }
}


// ----------------------------
void CombnPlot::calcPositions()
// ----------------------------
{
    // update flag
    geomChanged = false;

    // bar chart
    calcPosBC();
    // combination plot
    calcPosCP();

    // diagram scale factor to draw 120 x 120 pix diagram
    double pix = canvas->getPixelSize();
    scaleDgrm = 120.0*(pix/2.0);
}


// ------------------------
void CombnPlot::calcPosBC()
// ------------------------
{
    // calc positions
    double w, h, pix;
    double numAttr;
    double xLft, xRgt, yBot, yTop;
    double numX;
    double fracX;
    double x, y;
    double ratio;

    // get size of sides & 1 pixel
    canvas->getSize( w, h );
    pix = canvas->getPixelSize();
    // number of attributes
    numAttr = attributeIndcs.size();

    // calc size of bounding box
    xLft = -0.5*w+25*pix;
    xRgt =  0.5*w-10*pix;
    yTop =  0.5*h-10*pix;
    if ( numAttr > 0 )
        yBot = yTop - (h - (20+10+10)*pix)/(double)(numAttr+1);
    else
        yBot = yTop - 0.5*(h - (20+10+10)*pix);

    // get number of values per axis
    numX = combinations.size();

    // get intervals for x-axis
    if ( numX > 1 )
        fracX = ( 1.0 / (double)numX )*( xRgt-xLft );
    else
        fracX = 1.0;

    // calc width
    //if ( fracX < maxWthHintPixBC*pix )
        widthBC = fracX;
    //else
    //    widthBC = maxWthHintPixBC*pix;

    // calc positions
    posBC.clear();
    for ( size_t i = 0; i < numberPerComb.size(); ++i )
    {
        // calc ratio
        ratio = (double)numberPerComb[i]/(double)maxNumberPerComb;

        // center, top
        x = xLft + 0.5*fracX + i*fracX;
        y = yBot + ratio*( yTop-yBot );
        if ( y-yBot < pix*minHgtHintPixBC )
            y += pix*minHgtHintPixBC;

        Position2D pos;
        pos.x = x;
        pos.y = y;
        posBC.push_back( pos );
    }
}


// ------------------------
void CombnPlot::calcPosCP()
// ------------------------
{
    // calc positions
    double w, h;
    int    numAttr;
    double xLft,  xRgt;
    double yBot,  yTop;
    double pix;
    double x1,    y1;
    double x2,    y2;
    double numX,  numY;
    double fracX, fracY;

    Attribute* attribute = NULL;
    int idx;
    int card;
    double ratio;

    canvas->getSize( w, h );
    pix = canvas->getPixelSize();
    numAttr = attributeIndcs.size();

    xLft = -0.5*w+25*pix;
    xRgt =  0.5*w-10*pix;
    yBot = -0.5*h+20*pix;
    if ( numAttr > 0 )
        yTop = yBot + numAttr*(h - (20+10+10)*pix)/(double)(numAttr+1);
    else
        yTop = yBot + 0.5*(h - (20+10+10)*pix);

    // get number of values per axis
    numX = combinations.size();
    numY = attributeIndcs.size();

    // get intervals per axis
    if ( numX > 1 )
        fracX = (double)1 / (double)numX;
    else
        fracX = 1;

    if ( numY > 1)
        fracY = (double)1 / (double)numY;
    else
        fracY = 1;

    // calc positions
    posLftTop.clear();
    posRgtBot.clear();
    for ( size_t i = 0; i < combinations.size(); ++i )
    {
        vector< Position2D > temp;
        posLftTop.push_back( temp );
        posRgtBot.push_back( temp );

        for ( size_t j = 0; j < combinations[i].size(); ++j )
        {
            // calc ratio
            attribute = graph->getAttribute( attributeIndcs[j] );
            card  = attribute->getSizeCurValues();
            if ( card > 0 )
            {
                idx   = combinations[i][j];
                ratio = 1.0; //(double)(idx+1)/(double)(card+1);

                // left
                x1 = xLft + i*fracX*( xRgt-xLft );
                // right
                x2 = xLft + (i+1)*fracX*( xRgt-xLft );
                // bot
                y2 = yTop - (j+1)*fracY*( yTop-yBot );
                // top
                y1 = y2   + ratio*fracY*( yTop-yBot );

                Position2D posLT;
                posLT.x = x1;
                posLT.y = y1;
                posLftTop[i].push_back( posLT );

                Position2D posRB;
                posRB.x = x2;
                posRB.y = y2;
                posRgtBot[i].push_back( posRB );
            }
        }
    }

    attribute = NULL;
}


// -----------------------------
void CombnPlot::clearPositions()
// -----------------------------
{
    posLftTop.clear();
    posRgtBot.clear();
}


// -- hit detection -------------------------------------------------


// -------------------------
void CombnPlot::processHits(
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

        mouseCombnIdx = name;
        displTooltip( name );
    }
    else
    {
        mouseCombnIdx = -1;
        canvas->clearToolTip();
        showDgrm = false;
    }

    ptr = NULL;
}


// -- end -----------------------------------------------------------
