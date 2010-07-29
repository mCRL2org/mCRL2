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
    // get size of sides
    double w, h;
    canvas->getSize( w, h );
    // get size of 1 pixel
    double pix = canvas->getPixelSize();
    // get num attributes
    int numAttr = attributeIndcs.size();

    // calc size of bounding box
    double xLft = -0.5*w+25*pix;
    double xRgt =  0.5*w-10*pix;
    double yTop =  0.5*h-10*pix;
    double yBot;
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
    // get size of sides
    double w, h;
    canvas->getSize( w, h );
    // get size of 1 pixel
    double pix = canvas->getPixelSize();
    // get num attributes
    int numAttr = attributeIndcs.size();

    // calc size of bounding box
    double xLft = -0.5*w+25*pix;
    double xRgt =  0.5*w-10*pix;
    double yBot = -0.5*h+20*pix;
    double yTop;
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
void CombnPlot::drawLabelsBC( const bool& /*inSelectMode*/ )
// -----------------------------------------------------
{
    // get size of sides
    double w, h;
    canvas->getSize( w, h );
    // get size of 1 pixel
    double pix = canvas->getPixelSize();
    // calc scaling to use
    double scaling = ( 12*pix )/(double)CHARHEIGHT;
    // number attributes
    int numAttr = attributeIndcs.size();

    // color
    VisUtils::setColorBlack();

    // y-axis labels
    double xLft = -0.51*w+3*pix;
    double xRgt = -0.50*w+12*pix;
    double yTop =  0.50*h-10*pix;
    double yBot;
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
        string max = Utils::intToStr( maxNumberPerComb );
        double x   = -0.5*w+13*pix;
        double y   =  0.5*h-10*pix;
        VisUtils::drawLabelVertBelow( texCharId, x, y, scaling, max );

        // min number
        string min = "0";
        y   = yBot;
        VisUtils::drawLabelVertAbove( texCharId, x, y, scaling, min );
    }
}


// -----------------------------------------------------
void CombnPlot::drawLabelsCP( const bool& /*inSelectMode*/ )
// -----------------------------------------------------
{
    // get size of sides
    double w, h;
    canvas->getSize( w, h );
    // get size of 1 pixel
    double pix = canvas->getPixelSize();
    // calc scaling to use
    double scaling = ( 12*pix )/(double)CHARHEIGHT;
    // number attributes
    double numAttr = attributeIndcs.size();

    // color
    VisUtils::setColorBlack();

    // x-axis label
    double x =  0.0;
    double y =  -0.5*h+9*pix;
    VisUtils::drawLabelCenter( texCharId, x, y, scaling, "Combinations" );

    if ( numAttr > 0 )
    {
        // y-axis labels

        double xLft = -0.51*w+3*pix;
        double xRgt = -0.5*w+12*pix;

        for ( size_t i = 0; i < attributeLabels.size(); ++i )
        {
            double yTop;
            if ( i > 0 )
                yTop = posRgtBot[0][i-1].y;
            else
                yTop = -0.5*h+20*pix + numAttr*(h - (20+10+10)*pix)/(double)(numAttr+1);


            double yBot = posRgtBot[0][i].y;

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
    double hCanv = canvas->getHeight();
    double pix = canvas->getPixelSize();
    int sizePositions = posBC.size();
    int numAttr = attributeIndcs.size();

    double yBot;
    if ( sizePositions > 0 )
        yBot = 0.5*hCanv-10*pix - (hCanv - (20+10+10)*pix)/(double)(numAttr+1);
    else
        yBot = 0.5*hCanv-10*pix - 0.5*(hCanv - (20+10+10)*pix);

    // selection mode
    if ( inSelectMode == true )
    {
        for( int i = 0; i < sizePositions; ++i )
        {
            double xLft = posBC[i].x - 0.5*widthBC;
            double xRgt = posBC[i].x + 0.5*widthBC;
            double yTop = posBC[i].y;

            glPushName( i );
            VisUtils::fillRect( xLft, xRgt, yTop, yBot );
            glPopName();
        }
    }
    // rendering mode
    else
    {
        ColorRGB col;
        VisUtils::mapColorCoolGreen( col );
        col.a = 0.7;

        for( int i = 0; i < sizePositions; ++i )
        {
            double xLft = posBC[i].x - 0.5*widthBC;
            double xRgt = posBC[i].x + 0.5*widthBC;
            double yTop = posBC[i].y;

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
    double pix   = canvas->getPixelSize();

    // selection mode
    if ( inSelectMode == true )
    {
        for ( size_t i = 0; i < posLftTop.size(); ++i )
        {
            // name per collumn
            glPushName( i );

            double xLft = posLftTop[i][0].x;
            double xRgt = posRgtBot[i][0].x;

            double yTop = posLftTop[i][0].y;
            double yBot = posRgtBot[i][posRgtBot[i].size()-1].y;

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
                ColorRGB col;
                double xLft = posLftTop[i][j].x;
                double yTop = posLftTop[i][j].y;

                double xRgt = posRgtBot[i][j].x;
                double yBot = posRgtBot[i][j].y;

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
        if ( mouseCombnIdx < combinations.size() )
        {
            // x-coords
            double xLft = posLftTop[mouseCombnIdx][0].x;
            double xRgt = posRgtBot[mouseCombnIdx][0].x;
            // y-coords
            double yTop = posLftTop[mouseCombnIdx][0].y;
            double yBot = posRgtBot[mouseCombnIdx][posRgtBot[mouseCombnIdx].size()-1].y;

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
    if ( posIdx < combinations.size() )
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
    // get size of sides & 1 pixel
    double w,h;
    canvas->getSize( w, h );
    double pix = canvas->getPixelSize();
    // number of attributes
    double numAttr = attributeIndcs.size();

    // calc size of bounding box
    double xLft = -0.5*w+25*pix;
    double xRgt =  0.5*w-10*pix;
    double yTop =  0.5*h-10*pix;
    double yBot;
    if ( numAttr > 0 )
        yBot = yTop - (h - (20+10+10)*pix)/(double)(numAttr+1);
    else
        yBot = yTop - 0.5*(h - (20+10+10)*pix);

    // get number of values per axis
    double numX = combinations.size();

    // get intervals for x-axis
    double fracX = 1.0;
    if ( numX > 1 )
        fracX = ( 1.0 / (double)numX )*( xRgt-xLft );

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
        double ratio = (double)numberPerComb[i]/(double)maxNumberPerComb;

        // center, top
        double x = xLft + 0.5*fracX + i*fracX;
        double y = yBot + ratio*( yTop-yBot );
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
    Attribute* attribute = NULL;

    double w, h;
    canvas->getSize( w, h );
    double pix = canvas->getPixelSize();
    int numAttr = attributeIndcs.size();

    double xLft = -0.5*w+25*pix;
    double xRgt =  0.5*w-10*pix;
    double yBot = -0.5*h+20*pix;
    double yTop;
    if ( numAttr > 0 )
        yTop = yBot + numAttr*(h - (20+10+10)*pix)/(double)(numAttr+1);
    else
        yTop = yBot + 0.5*(h - (20+10+10)*pix);

    // get number of values per axis
    double numX = combinations.size();
    double numY = attributeIndcs.size();

    // get intervals per axis
    double fracX = 1;
    if ( numX > 1 )
        fracX = (double)1 / (double)numX;

    double fracY = 1;
    if ( numY > 1)
        fracY = (double)1 / (double)numY;

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
            int card  = attribute->getSizeCurValues();
            if ( card > 0 )
            {
                double ratio = 1.0; //(double)(idx+1)/(double)(card+1);

                // left
                double x1 = xLft + i*fracX*( xRgt-xLft );
                // right
                double x2 = xLft + (i+1)*fracX*( xRgt-xLft );
                // bot
                double y2 = yTop - (j+1)*fracY*( yTop-yBot );
                // top
                double y1 = y2   + ratio*fracY*( yTop-yBot );

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
    ptr = (GLuint*) buffer;

    if ( hits > 0 )
    {
        // if necassary advance to last hit
        if ( hits > 1 )
        {
            for ( int i = 0; i < ( hits-1 ); ++i )
            {
                int number = *ptr;
                ++ptr; // number;
                ++ptr; // z1
                ++ptr; // z2
                for ( int j = 0; j < number; ++j )
                    ++ptr; // names
            }
        }

        // last hit
        ++ptr; // number
        ++ptr; // z1
        ++ptr; // z2

        int name = *ptr;

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
