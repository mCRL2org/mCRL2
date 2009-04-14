// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./examiner.cpp

#include "wx.hpp" // precompiled headers

#include "examiner.h"

#include <iostream>
using namespace std;


// -- static variables ----------------------------------------------


//ColorRGB Examiner::colClr = { 1.0, 1.0, 0.93, 1.0 };
ColorRGB Examiner::colClr = { 1.0, 1.0, 1.0, 1.0 };
ColorRGB Examiner::colTxt = { 0.0, 0.0, 0.0, 1.0 };
int Examiner::szeTxt = 12;
ColorRGB Examiner::colBdl = { 0.0, 0.0, 0.0, 0.3 };
int Examiner::hgtHstPix = 80;


// -- constructors and destructor -----------------------------------


// ------------------------
Examiner::Examiner(
    Mediator* m,
    Graph* g,
    GLCanvas* c )
    : Visualizer( m, g, c )
// ------------------------
{
    diagram = NULL;
    frame = NULL;
    VisUtils::mapColorMdGray( colFrm );

    focusFrameIdx = -1;
    offset = 0;
}


// ------------------
Examiner::~Examiner()
// ------------------
{
    // association
    diagram = NULL;
    attributes.clear();

    // composition
    if ( frame != NULL )
    {
        delete frame;
        frame = NULL;
    }

    // composition
    {
    for ( size_t i = 0; i < framesHist.size(); ++i )
        delete framesHist[i];
    }
    framesHist.clear();

    // association
    {
    for ( size_t i = 0; i < attrsHist.size(); ++i )
        attrsHist[i].clear();
    }
    attrsHist.clear();
}


// -- get functions -------------------------------------------------


// -----------------------------
ColorRGB Examiner::getColorClr()
// -----------------------------
{
    return colClr;
}


// -----------------------------
ColorRGB Examiner::getColorTxt()
// -----------------------------
{
    return colTxt;
}


// -----------------------
int Examiner::getSizeTxt()
// -----------------------
{
    return szeTxt;
}


// -----------------------------
ColorRGB Examiner::getColorBdl()
// -----------------------------
{
    return colBdl;
}


// -----------------------------
ColorRGB Examiner::getColorSel()
// -----------------------------
{
    ColorRGB col;
    VisUtils::mapColorCoolRed( col );
    return col;
}


// --------------------------
int Examiner::getIdxClstSel()
// --------------------------
{
    size_t result = -1;
    if ( 0 <= focusFrameIdx && focusFrameIdx < framesHist.size() )
        result = framesHist[focusFrameIdx]->getNode(0)->getCluster()->getIndex();
    return result;
}


// -- set functions -------------------------------------------------


// ----------------------------------------------
void Examiner::setColorClr( const ColorRGB &col )
// ----------------------------------------------
{
    colClr = col;
}


// ----------------------------------------------
void Examiner::setColorTxt( const ColorRGB &col )
// ----------------------------------------------
{
    colTxt = col;
}


// ----------------------------------------
void Examiner::setSizeTxt( const int &sze )
// ----------------------------------------
{
    szeTxt = sze;
}


// ----------------------------------------------
void Examiner::setColorBdl( const ColorRGB &col )
// ----------------------------------------------
{
    colBdl = col;
}


// ---------------------------------------
void Examiner::setDiagram( Diagram *dgrm )
// ---------------------------------------
{
    diagram = dgrm;
}


// -----------------------------------
void Examiner::setFrame(
    Cluster* frme,
    const vector< Attribute*> &attrs,
    ColorRGB col )
// -----------------------------------
{
    if ( frame != NULL )
        delete frame;
    attributes.clear();
    frame = new Cluster( *frme );

    attributes = attrs;
    colFrm = col;
}


// ----------------------
void Examiner::clrFrame()
// ----------------------
{
    if ( frame != NULL )
    {
        delete frame;
        frame = NULL;
    }

    attributes.clear();

    VisUtils::mapColorMdGray( colFrm );

    if ( 0 <= focusFrameIdx && focusFrameIdx < framesHist.size() )
    {
        ColorRGB col;
        VisUtils::mapColorCoolRed( col );
        setFrame( framesHist[focusFrameIdx], attrsHist[focusFrameIdx], col );
    }
}


// ------------------------------------
void Examiner::addFrameHist(
    Cluster* frme,
    const vector< Attribute* > &attrs )
// ------------------------------------
{
    // update flag
    dataChanged = true;

    framesHist.push_back( new Cluster( *frme ) );

    vector< Attribute * > v;
    attrsHist.push_back( attrs );
}


// --------------------------
void Examiner::clrFrameHist()
// --------------------------
{
    // update flag
    dataChanged = true;

    // composition
    {
    for ( size_t i = 0; i < framesHist.size(); ++i )
        delete framesHist[i];
    }
    framesHist.clear();

    // association
    {
    for ( size_t i = 0; i < attrsHist.size(); ++i )
        attrsHist[i].clear();
    }
    attrsHist.clear();

    focusFrameIdx = -1;
    offset = 0;

    mediator->handleMarkFrameClust( this );
}


// -----------------------------
void Examiner::clrFrameHistCur()
// -----------------------------
{
    // update flag
    dataChanged = true;

    if ( 0 <= focusFrameIdx && focusFrameIdx < framesHist.size() )
    {
        // composition
        delete framesHist[focusFrameIdx];
        framesHist.erase( framesHist.begin() + focusFrameIdx );

        // association
        attrsHist.erase( attrsHist.begin() + focusFrameIdx );

        // update focus
        focusFrameIdx = -1;

        clrFrame();

        mediator->handleUnmarkFrameClusts( this );
    }
}


// ------------------------------
int Examiner::getSizeFramesHist()
// ------------------------------
{
    return framesHist.size();
}


// -----------------------
void Examiner::clearData()
// -----------------------
{
    clearAttributes();
    clearDiagram();
    clearFrames();
}


// -------------------------------------
void Examiner::handleSendDgrmSglToSiml()
// -------------------------------------
{
    mediator->initSimulator(
        framesHist[focusFrameIdx],
        attrsHist[focusFrameIdx] );
}


// --------------------------------------
void Examiner::handleSendDgrmSglToTrace()
// --------------------------------------
{
    mediator->markTimeSeries( this, frame );
}


// --------------------------------------
void Examiner::handleSendDgrmSetToTrace()
// --------------------------------------
{
    mediator->markTimeSeries( this, framesHist );
}


// -- visualization functions  --------------------------------------


// -------------------------------------------------
void Examiner::visualize( const bool &inSelectMode )
// -------------------------------------------------
{
    clear();

    // check if positions are ok
    if ( geomChanged == true )
        calcSettingsGeomBased();
    if ( dataChanged == true )
        calcSettingsDataBased();

    if ( inSelectMode == true )
    {
        GLint hits = 0;
        GLuint selectBuf[512];
        startSelectMode(
            hits,
            selectBuf,
            2.0,
            2.0 );

        if ( diagram != NULL )
        {
            drawFrame( inSelectMode );

            if ( framesHist.size() > 0 )
            {
                drawFramesHist( inSelectMode );
                drawControls( inSelectMode );
            }
        }

        finishSelectMode(
            hits,
            selectBuf );
    }
    else
    {
        if ( diagram != NULL )
        {
            drawFrame( inSelectMode );

            if ( framesHist.size() > 0 )
            {
                drawFramesHist( inSelectMode );
                drawControls( inSelectMode );
            }
        }
    }
}


// -- event handlers ------------------------------------------------


// -----------------------------
void Examiner::handleSizeEvent()
// -----------------------------
{
    Visualizer::handleSizeEvent();

    double wth, hgt, pix;
    double bdr     = 10;

    canvas->getSize( wth, hgt );
    pix = canvas->getPixelSize();

    if ( posFramesHist.size() > 0 )
    {
        // update offset if necessary
        if ( ( posFramesHist[posFramesHist.size()-1].x + offset*pix + scaleFramesHist*1.0 ) < ( 0.5*wth - bdr*pix )  &&
             ( offset < 0 ) )
        {
            offset += ( ( 0.5*wth - bdr*pix ) - ( posFramesHist[posFramesHist.size()-1].x + scaleFramesHist*1.0 ) )/pix;

            if ( offset > 0 )
                offset = 0;
        }
    }
}


// ------------------------------------
void Examiner::handleMouseLftDownEvent(
    const int &x,
    const int &y )
// ------------------------------------
{
    Visualizer::handleMouseLftDownEvent( x, y );

    // redraw in select mode
    visualize( true );
    // redraw in render mode
    visualize( false );
}


// ----------------------------------
void Examiner::handleMouseLftUpEvent(
    const int &x,
    const int &y )
// ----------------------------------
{
    Visualizer::handleMouseLftUpEvent( x, y );

    // redraw in select mode
    visualize( true );
    // redraw in render mode
    visualize( false );
}


// --------------------------------------
void Examiner::handleMouseLftDClickEvent(
    const int &x,
    const int &y )
// --------------------------------------
{
    Visualizer::handleMouseLftDClickEvent( x, y );

    // redraw in select mode
    visualize( true );
    // redraw in render mode
    visualize( false );
}


// ------------------------------------
void Examiner::handleMouseRgtDownEvent(
    const int &x,
    const int &y )
// ------------------------------------
{
    Visualizer::handleMouseRgtDownEvent( x, y );

    // redraw in select mode
    visualize( true );
    // redraw in render mode
    visualize( false );
}


// ----------------------------------
void Examiner::handleMouseRgtUpEvent(
    const int &x,
    const int &y )
// ----------------------------------
{
    Visualizer::handleMouseRgtUpEvent( x, y );

    // redraw in select mode
    visualize( true );
    // redraw in render mode
    visualize( false );
}


// -----------------------------------
void Examiner::handleMouseMotionEvent(
    const int &x,
    const int &y )
// -----------------------------------
{
    Visualizer::handleMouseMotionEvent( x, y );

    // redraw in select mode
    visualize( true );
    // redraw in render mode
    visualize( false );
}


// ----------------------------------------------------
void Examiner::handleKeyDownEvent( const int &keyCode )
// ----------------------------------------------------
{
    Visualizer::handleKeyDownEvent( keyCode );

    if ( keyCodeDown == WXK_RIGHT || keyCodeDown == WXK_NUMPAD_RIGHT )
        handleIconRgt();
    else if ( keyCodeDown == WXK_LEFT || keyCodeDown == WXK_NUMPAD_LEFT )
        handleIconLft();

    // redraw in render mode
    visualize( false );
}


// -- utility functions ---------------------------------------------


// -----------------------------------
void Examiner::calcSettingsGeomBased()
// -----------------------------------
{
    // update flag
    geomChanged = false;

    calcPosFrame();
    calcPosFramesHist();
}


// -----------------------------------
void Examiner::calcSettingsDataBased()
// -----------------------------------
{
    // update flag
    dataChanged = false;

    calcPosFrame();
    calcPosFramesHist();
}


// --------------------------
void Examiner::calcPosFrame()
// --------------------------
{
    double wth, hgt, pix;
    double itvHist = hgtHstPix;
    double bdr     = 10;

    canvas->getSize( wth, hgt );
    pix = canvas->getPixelSize();

    // position & scaling
    posFrame.x = 0.0;
    if ( framesHist.size() > 0 )
    {
        // frame
        posFrame.y = 0.0 + 0.5*itvHist*pix;
        scaleFrame = Utils::minn( (wth - 2.0*bdr*pix), (hgt - itvHist*pix - 2.0*bdr*pix) )/2.0;
    }
    else
    {
        posFrame.y = 0.0;
        scaleFrame = Utils::minn( (wth - 2.0*bdr*pix), (hgt - 2.0*bdr*pix) )/2.0;
    }
}


// -------------------------------
void Examiner::calcPosFramesHist()
// -------------------------------
{
    double wth, hgt, pix;
    double itvHist = hgtHstPix;
    double bdr     = 10;

    canvas->getSize( wth, hgt );
    pix = canvas->getPixelSize();

    vsblHistIdxLft = 0;
    vsblHistIdxRgt = framesHist.size()-1;

    if ( framesHist.size() > 0 )
    {
        // frames history
        posFramesHist.clear();
        Position2D pos;
        pos.y = -0.5*hgt + 0.5*itvHist*pix;
        for ( size_t i = 0; i < framesHist.size(); ++i )
        {
            pos.x = -0.5*wth + bdr*pix + 0.5*itvHist*pix + i*itvHist*pix + offset*pix;
            posFramesHist.push_back( pos );

            if ( pos.x + bdr*pix < -0.5*wth )
                ++vsblHistIdxLft;
            if ( 0.5*wth < pos.x - bdr*pix )
                --vsblHistIdxRgt;
        }

        scaleFramesHist = 0.40*itvHist*pix;
    }
}


// -----------------------------
void Examiner::clearAttributes()
// -----------------------------
{
    // association
    attributes.clear();
}


// --------------------------
void Examiner::clearDiagram()
// --------------------------
{
    // association
    diagram = NULL;
}


// -------------------------
void Examiner::clearFrames()
// -------------------------
{
    // composition
    delete frame;
    frame = NULL;
}


// -- hit detection -------------------------------------------------


// --------------------------------------------------
void Examiner::handleHits( const vector< int > &ids )
// --------------------------------------------------
{
    if ( ids.size() > 0 )
    {
        if ( mouseButton == MSE_BUTTON_DOWN &&
             mouseSide == MSE_SIDE_LFT )
        {
            if ( ids[0] == ID_FRAME )
            {
                if ( ids.size() == 2 && ids[1] == ID_ICON_MORE )
                {
                    if ( mediator->getView() == Mediator::VIEW_SIM )
                    {
                        if ( frame != NULL )
                            mediator->handleSendDgrm( this, true, false, false, false, false );
                        else
                            mediator->handleSendDgrm( this, false, false, false, false, false );
                    }
                    else if ( mediator->getView() == Mediator::VIEW_TRACE )
                    {
                        if ( frame != NULL )
                            mediator->handleSendDgrm( this, false, true, true, false, false );
                        else
                            mediator->handleSendDgrm( this, false, false, false, false, false );
                    }

                    // no mouseup event is generated reset manually
                    mouseButton = MSE_BUTTON_UP;
                    mouseSide   = MSE_SIDE_LFT;
                    if ( mouseClick != MSE_CLICK_DOUBLE )
                        mouseClick  = MSE_CLICK_SINGLE;
                    mouseDrag   = MSE_DRAG_FALSE;
                }
            }
            else if ( ids[0] == ID_FRAME_HIST )
            {
                if ( focusFrameIdx == static_cast <size_t> (ids[1]) )
                {
                    focusFrameIdx = -1;
                    clrFrame();
                    mediator->handleUnmarkFrameClusts( this );
                }
                else
                {
                    focusFrameIdx = ids[1];

                    ColorRGB col;
                    VisUtils::mapColorCoolRed( col );
                    setFrame( framesHist[focusFrameIdx], attrsHist[focusFrameIdx], col );
                    mediator->handleMarkFrameClust( this );
                }
            }
            else if ( ids[ids.size()-1] == ID_ICON_CLR )
            {
                mediator->handleClearExnr( this );
            }
            else if ( ids[ids.size()-1] == ID_ICON_RWND )
            {
                handleIconRwnd();
            }
            else if ( ids[ids.size()-1] == ID_ICON_LFT )
           	{
                handleIconLft();
            }
            else if ( ids[ids.size()-1] == ID_ICON_RGT )
            {
                handleIconRgt();
            }

        }
        else if ( mouseButton == MSE_BUTTON_DOWN &&
                  mouseSide == MSE_SIDE_RGT )
        {
            if ( ids[0] == ID_FRAME )
            {
                if ( mediator->getView() == Mediator::VIEW_SIM )
                {
                    if ( frame != NULL )
                    {
                        mediator->handleSendDgrm( this, true, false, false, false, false );
                    }
                    else
                    {
                        mediator->handleSendDgrm( this, false, false, false, false, false );
                    }
                }
                else if ( mediator->getView() == Mediator::VIEW_TRACE )
                    mediator->handleSendDgrm( this, false, true, true, false, false );

                // no mouseup event is generated reset manually
                mouseButton = MSE_BUTTON_UP;
                mouseSide   = MSE_SIDE_RGT;
                if ( mouseClick != MSE_CLICK_DOUBLE )
                    mouseClick  = MSE_CLICK_SINGLE;
                mouseDrag   = MSE_DRAG_FALSE;
            }
            else if ( ids[0] == ID_FRAME_HIST )
            {
                focusFrameIdx = ids[1];

                ColorRGB col;
                VisUtils::mapColorCoolRed( col );
                setFrame( framesHist[focusFrameIdx], attrsHist[focusFrameIdx], col );

                mediator->handleClearExnrCur( this );
            }
        }
    }
}


// ----------------------------
void Examiner::handleIconRwnd()
// ----------------------------
{
    double wth, pix;

    wth = canvas->getWidth();
    pix = canvas->getPixelSize();

    if ( framesHist.size() > 0 )
    {
        if ( focusFrameIdx >= 0 )
        {
            focusFrameIdx = 0;
            offset        = 0;
            geomChanged   = true;

            ColorRGB col;
            VisUtils::mapColorCoolRed( col );
            setFrame( framesHist[focusFrameIdx], attrsHist[focusFrameIdx], col );

            mediator->handleMarkFrameClust( this );
        }
        else
        {
            offset      = 0;
            geomChanged = true;
        }
    }
    else
    {
        offset      = 0;
        geomChanged = true;
    }
}


// ---------------------------
void Examiner::handleIconLft()
// ---------------------------
{
    double wth, pix;
    double bdr = 12;
    double dLft, dRgt;

    wth = canvas->getWidth();
    pix = canvas->getPixelSize();

    if ( framesHist.size() > 0 )
    {
        if ( focusFrameIdx >= 0  && focusFrameIdx < framesHist.size() )
        {
            if ( focusFrameIdx != 0 )
                focusFrameIdx -= 1;

            dLft = ( -0.5*wth + bdr*pix ) - ( posFramesHist[focusFrameIdx].x - scaleFramesHist*1.0 );
            dRgt = ( posFramesHist[focusFrameIdx].x + scaleFramesHist*1.0 + 4*pix) - ( 0.5*wth - bdr*pix );
            if ( dRgt > 0 )
                offset -= dRgt/pix;
            else if ( dLft > 0 )
                offset += dLft/pix;

            ColorRGB col;
            VisUtils::mapColorCoolRed( col );
            setFrame( framesHist[focusFrameIdx], attrsHist[focusFrameIdx], col );

            geomChanged = true;

            mediator->handleMarkFrameClust( this );
        }
        else
        {
            if ( ( posFramesHist[0].x - scaleFramesHist*1.0 ) < ( -0.5*wth + bdr*pix ) )
                offset += 10;
                geomChanged = true;
        }
    }
    else
    {
        offset = 0;
        geomChanged = true;
    }
}


// ---------------------------
void Examiner::handleIconRgt()
// ---------------------------
{
    double wth, pix;
    double bdr = 12;
    double dLft, dRgt;

    wth = canvas->getWidth();
    pix = canvas->getPixelSize();

    if ( framesHist.size() > 0 )
    {
        if ( focusFrameIdx >= 0 && focusFrameIdx < framesHist.size() )
        {
            if ( focusFrameIdx < framesHist.size()-1 )
                focusFrameIdx += 1;

            dLft = ( -0.5*wth + bdr*pix ) - ( posFramesHist[focusFrameIdx].x - scaleFramesHist*1.0 );
            dRgt = ( posFramesHist[focusFrameIdx].x + scaleFramesHist*1.0 + 4*pix) - ( 0.5*wth - bdr*pix );
            if ( dRgt > 0 )
                offset -= dRgt/pix;
            else if ( dLft > 0 )
                offset += dLft/pix;

            ColorRGB col;
            VisUtils::mapColorCoolRed( col );
            setFrame( framesHist[focusFrameIdx], attrsHist[focusFrameIdx], col );

            geomChanged = true;

            mediator->handleMarkFrameClust( this );
        }
        else
        {
            if ( ( posFramesHist[posFramesHist.size()-1].x + scaleFramesHist*1.0 ) > ( 0.5*wth - bdr*pix ) )
                offset -= 10;
            geomChanged = true;
        }
    }
    else
    {
        offset = 0;
        geomChanged = true;
    }
}


// ------------------------
void Examiner::processHits(
    GLint hits,
    GLuint buffer[] )
// ------------------------
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
    else
        canvas->clearToolTip();

    ptr = NULL;
}


// -- utility drawing functions -------------------------------------


// -------------------
void Examiner::clear()
// -------------------
{
    VisUtils::clear( colClr );
}


// -------------------------------------------------
void Examiner::drawFrame( const bool &inSelectMode )
// -------------------------------------------------
{
    double pix;

    if ( inSelectMode == true )
    {
        pix = canvas->getPixelSize();

        glPushMatrix();
        glTranslatef( posFrame.x, posFrame.y, 0.0 );
        glScalef( scaleFrame, scaleFrame, scaleFrame );

        glPushName( ID_FRAME );
        VisUtils::fillRect( - 1.0, 1.0, 1.0, -1.0 );
        if ( focusFrameIdx >= 0 )
        {
            glPushName( ID_ICON_MORE );
            VisUtils::fillRect( -0.98, -0.98+14*pix, -0.98+14*pix, -0.98 );
            glPopName();
        }
        glPopName();

        glPopMatrix();
    }
    else
    {
        pix = canvas->getPixelSize();

        glPushMatrix();
        glTranslatef( posFrame.x, posFrame.y, 0.0 );
        glScalef( scaleFrame, scaleFrame, scaleFrame );

        VisUtils::setColor( colFrm );
        VisUtils::fillRect(
            -1.0 + 4*pix/scaleFrame,  1.0 + 4*pix/scaleFrame,
             1.0 - 4*pix/scaleFrame, -1.0 - 4*pix/scaleFrame );

        vector< double > valsFrame;
        /*
        for ( int i = 0; i < attributes.size(); ++i )
            valsFrame.push_back(
               attributes[i]->mapToValue(
                    frame->getNode(0)->getTupleVal(
                        attributes[i]->getIndex() ) )->getIndex() );
        */
        Attribute* attr;
        Node* node;
        for ( size_t i = 0; i < attributes.size(); ++i )
        {
            attr = attributes[i];
            node = frame->getNode(0);
            if ( attr->getSizeCurValues() > 0 )
                valsFrame.push_back( attr->mapToValue( node->getTupleVal( attr->getIndex() ) )->getIndex() );
            else
            {
                double val = node->getTupleVal( attr->getIndex() );
                valsFrame.push_back( val );
            }
        }
        attr = NULL;
        node = NULL;

        diagram->visualize(
            false,
            canvas,
            attributes,
            valsFrame );

        if ( focusFrameIdx >= 0 )
        {
            VisUtils::enableLineAntiAlias();
            VisUtils::setColor( colFrm );
            VisUtils::fillMoreIcon( -0.98, -0.98+14*pix, -0.98+14*pix, -0.98 );
            VisUtils::setColorLtLtGray();
            VisUtils::drawMoreIcon( -0.98, -0.98+14*pix, -0.98+14*pix, -0.98 );
            VisUtils::disableLineAntiAlias();
        }

        glPopMatrix();
    }
}


// ------------------------------------------------------
void Examiner::drawFramesHist( const bool &inSelectMode )
// ------------------------------------------------------
{
    double pix;

    if ( inSelectMode == true )
    {
        glPushName( ID_FRAME_HIST );
        //for ( int i = 0; i < framesHist.size(); ++i )
        for ( size_t i = vsblHistIdxLft; i <= vsblHistIdxRgt; ++i )
        {
            glPushMatrix();
            glTranslatef( posFramesHist[i].x, posFramesHist[i].y, 0.0 );
            glScalef( scaleFramesHist, scaleFramesHist, scaleFramesHist );

            glPushName( i );
            VisUtils::fillRect(
                -1.0,  1.0,
                 1.0, -1.0 );
            glPopName();

            glPopMatrix();
        }
        glPopName();
    }
    else
    {
        pix = canvas->getPixelSize();
        vector< double > valsFrame;

        //for ( int i = 0; i < framesHist.size(); ++i )
        for ( size_t i = vsblHistIdxLft; i <= vsblHistIdxRgt; ++i )
        {
            valsFrame.clear();
            /*
            for ( int j = 0; j < attrsHist[i].size(); ++j )
                valsFrame.push_back(
                    attrsHist[i][j]->mapToValue(
                        framesHist[i]->getNode(0)->getTupleVal(
                            attrsHist[i][j]->getIndex() ) )->getIndex() );
            */
            Attribute* attr;
            Node* node;
            for ( size_t j = 0; j < attrsHist[i].size(); ++j )
            {
                attr = attrsHist[i][j];
                node = framesHist[i]->getNode(0);
                if ( attr->getSizeCurValues() > 0 )
                    valsFrame.push_back( attr->mapToValue( node->getTupleVal( attr->getIndex() ) )->getIndex() );
                else
                {
                    double val = node->getTupleVal( attr->getIndex() );
                    valsFrame.push_back( val );
                }
            }
            attr = NULL;
            node = NULL;

            glPushMatrix();
            glTranslatef( posFramesHist[i].x, posFramesHist[i].y, 0.0 );
            glScalef( scaleFramesHist, scaleFramesHist, scaleFramesHist );

            if ( i == focusFrameIdx )
            {
                VisUtils::setColorCoolRed();
                VisUtils::fillRect(
                    -1.0 + 4*pix/scaleFramesHist,  1.0+4*pix/scaleFramesHist,
                     1.0 - 4*pix/scaleFramesHist, -1.0-4*pix/scaleFramesHist );
            }
            else
            {
                VisUtils::setColorMdGray();
                VisUtils::fillRect(
                    -1.0 + 3*pix/scaleFramesHist,  1.0+3*pix/scaleFramesHist,
                     1.0 - 3*pix/scaleFramesHist, -1.0-3*pix/scaleFramesHist );
            }

            diagram->visualize(
                false,
                canvas,
                attrsHist[i],
                valsFrame );

            glPopMatrix();
        }
    }
}


// ----------------------------------------------------
void Examiner::drawControls( const bool &inSelectMode )
// ----------------------------------------------------
{
    double wth, hgt, pix;
    double itvHist = hgtHstPix;

    canvas->getSize( wth, hgt );
    pix = canvas->getPixelSize();

    if ( inSelectMode == true )
    {
        // clear icon
        double itvSml = 6.0*pix;
        double x = 0.5*wth - itvSml - pix;
        double y = 0.5*hgt - itvSml - pix;
        glPushName( ID_ICON_CLR );
        VisUtils::fillRect( x-itvSml, x+itvSml, y+itvSml, y-itvSml );
        glPopName();

        // rewind
        glPushName( ID_ICON_RWND );
        glPushMatrix();
        glTranslatef( -18.0*pix, -0.5*hgt + itvHist*pix, 0.0 );
        VisUtils::fillRect(
            -5.0*pix,  5.0*pix,
             5.0*pix, -5.0*pix );
        glPopMatrix();
        glPopName();

        // left
        glPushName( ID_ICON_LFT );
        glPushMatrix();
        glTranslatef( -6*pix, -0.5*hgt + itvHist*pix, 0.0 );
        VisUtils::fillRect(
            -5.0*pix,  5.0*pix,
             5.0*pix, -5.0*pix );
        glPopMatrix();
        glPopName();

        // left
        glPushName( ID_ICON_LFT );
        glPushMatrix();
        glTranslatef( -0.5*wth+6*pix, -0.5*hgt + 0.5*itvHist*pix, 0.0 );
        VisUtils::fillRect(
            -5.0*pix,  5.0*pix,
             5.0*pix, -5.0*pix );
        glPopMatrix();
        glPopName();

        // play
        glPushName( ID_ICON_PLAY );
        glPushMatrix();
        glTranslatef( 6*pix, -0.5*hgt + itvHist*pix, 0.0 );
        VisUtils::fillRect(
            -5.0*pix,  5.0*pix,
             5.0*pix, -5.0*pix );
        glPopMatrix();
        glPopName();

        // right
        glPushName( ID_ICON_RGT );
        glPushMatrix();
        glTranslatef( 18*pix, -0.5*hgt + itvHist*pix, 0.0 );
        VisUtils::fillRect(
            -5.0*pix,  5.0*pix,
             5.0*pix, -5.0*pix );
        glPopMatrix();
        glPopName();

        // right
        glPushName( ID_ICON_RGT );
        glPushMatrix();
        glTranslatef( 0.5*wth-6*pix, -0.5*hgt + 0.5*itvHist*pix, 0.0 );
        VisUtils::fillRect(
            -5.0*pix,  5.0*pix,
             5.0*pix, -5.0*pix );
        glPopMatrix();
        glPopName();
    }
    else
    {
        // clear icon
        double itvSml = 6.0*pix;
        double x = 0.5*wth - itvSml - pix;
        double y = 0.5*hgt - itvSml - pix;
        double bdr = 10;
        double dLft, dRgt;

        if ( posFramesHist.size() > 1 && focusFrameIdx < 0 )
        {
            dLft = ( -0.5*wth + bdr*pix ) - ( posFramesHist[0].x - scaleFramesHist*1.0 );
            dRgt = ( posFramesHist[posFramesHist.size()-1].x + scaleFramesHist*1.0 ) - ( 0.5*wth - bdr*pix );
        }
        else
        {
            if ( 0 < focusFrameIdx && focusFrameIdx < posFramesHist.size() )
                dLft = 1;
            else
                dLft = 0;

            if ( 0 <= focusFrameIdx && focusFrameIdx < posFramesHist.size()-1 )
                dRgt = 1;
            else
                dRgt = 0;
        }

        VisUtils::enableLineAntiAlias();
        VisUtils::setColorWhite();
        VisUtils::fillClearIcon( x-itvSml, x+itvSml, y+itvSml, y-itvSml );
        VisUtils::setColorDkGray();
        VisUtils::drawClearIcon( x-itvSml, x+itvSml, y+itvSml, y-itvSml );
        VisUtils::disableLineAntiAlias();

        // border
        VisUtils::setColor( colClr );
        VisUtils::fillRect(
            -0.5*wth,               -0.5*wth + 12.0*pix,
            -0.5*hgt + itvHist*pix, -0.5*hgt );
        VisUtils::fillRect(
            0.5*wth - 12.0*pix,      0.5*wth,
            -0.5*hgt + itvHist*pix, -0.5*hgt );

        // lines
        VisUtils::setColorLtGray();
        VisUtils::drawLine(
            -0.5*wth + 6.0*pix,     -26.0*pix,
            -0.5*hgt + itvHist*pix, -0.5*hgt + itvHist*pix );
        /*
        VisUtils::drawLine(
            -0.5*wth + 6.0*pix,     -0.5*wth + 6.0*pix,
            -0.5*hgt + itvHist*pix, -0.5*hgt + 0.5*itvHist*pix + 8.0*pix );
        */
        VisUtils::drawLine(
            26.0*pix,                0.5*wth - 6.0*pix,
            -0.5*hgt + itvHist*pix, -0.5*hgt + itvHist*pix );
        /*
        VisUtils::drawLine(
            0.5*wth - 6.0*pix,       0.5*wth - 6.0*pix,
            -0.5*hgt + itvHist*pix, -0.5*hgt + 0.5*itvHist*pix + 8.0*pix );
        */

        // rewind
        glPushMatrix();
        glTranslatef( -18.0*pix, -0.5*hgt + itvHist*pix, 0.0 );

        VisUtils::enableLineAntiAlias();
        if ( dLft > 0 )
            VisUtils::setColorCoolRed();
        else
            VisUtils::setColorMdGray();
        VisUtils::fillRwndIcon(
            -5.0*pix,  5.0*pix,
             5.0*pix, -5.0*pix );
        VisUtils::setColorLtLtGray();
        VisUtils::drawRwndIcon(
            -5.0*pix,  5.0*pix,
             5.0*pix, -5.0*pix );
        VisUtils::disableLineAntiAlias();

        glPopMatrix();

        // left
        glPushMatrix();
        glTranslatef( -6*pix, -0.5*hgt + itvHist*pix, 0.0 );

        VisUtils::enableLineAntiAlias();
        if ( dLft > 0 )
            VisUtils::setColorCoolRed();
        else
            VisUtils::setColorMdGray();
        VisUtils::fillPrevIcon(
            -5.0*pix,  5.0*pix,
             5.0*pix, -5.0*pix );
        VisUtils::setColorLtLtGray();
        VisUtils::drawPrevIcon(
            -5.0*pix,  5.0*pix,
             5.0*pix, -5.0*pix );
        VisUtils::disableLineAntiAlias();

        glPopMatrix();

        // left
        if ( dLft > 0 )
        {
            glPushMatrix();
            glTranslatef( -0.5*wth+6*pix, -0.5*hgt + 0.5*itvHist*pix, 0.0 );
            VisUtils::enableLineAntiAlias();
            VisUtils::setColorCoolRed();
            VisUtils::fillPrevIcon(
                -5.0*pix,  5.0*pix,
                 5.0*pix, -5.0*pix );
            VisUtils::setColorLtLtGray();
            VisUtils::drawPrevIcon(
                -5.0*pix,  5.0*pix,
                 5.0*pix, -5.0*pix );
            VisUtils::disableLineAntiAlias();
            glPopMatrix();
        }

        // play
        glPushMatrix();
        glTranslatef( 6*pix, -0.5*hgt + itvHist*pix, 0.0 );

        VisUtils::enableLineAntiAlias();
        //VisUtils::setColorCoolRed();
        VisUtils::setColorMdGray();
        VisUtils::fillPlayIcon(
            -5.0*pix,  5.0*pix,
             5.0*pix, -5.0*pix );
        VisUtils::setColorLtLtGray();
        VisUtils::drawPlayIcon(
            -5.0*pix,  5.0*pix,
             5.0*pix, -5.0*pix );
        VisUtils::disableLineAntiAlias();

        glPopMatrix();

        // right
        glPushMatrix();
        glTranslatef( 18*pix, -0.5*hgt + itvHist*pix, 0.0 );

        VisUtils::enableLineAntiAlias();
        if ( dRgt > 0 )
            VisUtils::setColorCoolRed();
        else
            VisUtils::setColorMdGray();
        VisUtils::fillNextIcon(
            -5.0*pix,  5.0*pix,
             5.0*pix, -5.0*pix );
        VisUtils::setColorLtLtGray();
        VisUtils::drawNextIcon(
            -5.0*pix,  5.0*pix,
             5.0*pix, -5.0*pix );
        VisUtils::disableLineAntiAlias();

        glPopMatrix();

        // right
        if ( dRgt > 0 )
        {
            glPushMatrix();
            glTranslatef( 0.5*wth-6*pix, -0.5*hgt + 0.5*itvHist*pix, 0.0 );
            VisUtils::enableLineAntiAlias();
            VisUtils::setColorCoolRed();
            VisUtils::fillNextIcon(
                -5.0*pix,  5.0*pix,
                 5.0*pix, -5.0*pix );
            VisUtils::setColorLtLtGray();
            VisUtils::drawNextIcon(
                -5.0*pix,  5.0*pix,
                 5.0*pix, -5.0*pix );
            VisUtils::disableLineAntiAlias();
            glPopMatrix();
        }
    }
}


// -- implement event table -----------------------------------------


BEGIN_EVENT_TABLE( Examiner, wxEvtHandler )
END_EVENT_TABLE()


// -- end -----------------------------------------------------------
