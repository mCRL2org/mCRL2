// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./timeseries.cpp

#include "wx.hpp" // precompiled headers

#include "timeseries.h"

#include <iostream>
using namespace std;


// -- static variables ----------------------------------------------


bool TimeSeries::useShading = false;
//ColorRGB TimeSeries::colClr = { 1.0, 1.0, 0.93, 1.0 };
ColorRGB TimeSeries::colClr = { 1.0, 1.0, 1.0, 1.0 };
ColorRGB TimeSeries::colTxt = { 0.0, 0.0, 0.0, 1.0 };
int TimeSeries::szeTxt = 12;
ColorRGB TimeSeries::colMrk = { 0.73, 0.89, 1.0, 1.0 };
//ColorRGB TimeSeries::colMrk = { 0.84, 0.93, 1.0, 1.0 };
int TimeSeries::itvAnim = 350;


// -- constructors and destructor -----------------------------------


// ----------------------------
TimeSeries::TimeSeries(
    Mediator* m,
    Graph* g,
    GLCanvas* c )
        : Visualizer( m, g, c )
// ----------------------------
{
    critSect = false;

    ySpacePxl     = 6.0;
    minPixPerNode = 4.0;
    actPixPerNode = 4.0;
    wdwStartIdx   = 0;

    shiftStartIdx = -1;
    dragStartIdx  = -1;

    mouseOverIdx = -1;

    dragDir = DRAG_DIR_NULL;

    timerAnim = new wxTimer();
    timerAnim->SetOwner( this, ID_TIMER );

    animIdxDgrm = -1;
    animFrame = itemsMarked.end();
}


// ----------------------
TimeSeries::~TimeSeries()
// ----------------------
{
    clearDiagram();
    clearAttributes();

    delete timerAnim;
    timerAnim = NULL;
}


// -- get functions -------------------------------------------------


// -----------------------------
bool TimeSeries::getUseShading()
// -----------------------------
{
    return useShading;
}


// ---------------------------------------------------
void TimeSeries::getIdcsClstMarked( set< int > &idcs )
// ---------------------------------------------------
{
    idcs.clear();
    set< int >::iterator it;
    for ( it = itemsMarked.begin(); it != itemsMarked.end(); ++it )
        idcs.insert(  graph->getNode( *it )->getCluster()->getIndex() );
}


// --------------------------------
void TimeSeries::getIdcsClstMarked(
    set< int > &idcs ,
    ColorRGB &col )
// --------------------------------
{
    getIdcsClstMarked( idcs );
    col = colMrk;
}


// ----------------------------
void TimeSeries::getIdxMseOver(
    int &idxLeaf,
    set< int > &idcsBndl,
    ColorRGB &colLeaf )
// ----------------------------
{
    idxLeaf = -1;
    idcsBndl.clear();
    if ( 0 <= mouseOverIdx && mouseOverIdx < graph->getSizeNodes() )
    {
        Node* node = graph->getNode( mouseOverIdx );
        idxLeaf = node->getCluster()->getIndex();
        for ( int i = 0; i < node->getSizeInEdges(); ++i )
            idcsBndl.insert( node->getInEdge(i)->getBundle()->getIndex() );
        node = NULL;
    }
    VisUtils::mapColorCoolBlue( colLeaf );
}


// ----------------------------
void TimeSeries::getCurrIdxDgrm(
    int &idxLeaf,
    set< int > &idcsBndl,
    ColorRGB &colLeaf )
// ----------------------------
{
    idxLeaf = -1;
    idcsBndl.clear();
    if ( 0 <= currIdxDgrm && currIdxDgrm < graph->getSizeNodes() )
    {
        Node* node = graph->getNode( currIdxDgrm );
        idxLeaf = node->getCluster()->getIndex();
        for ( int i = 0; i < node->getSizeInEdges(); ++i )
            idcsBndl.insert( node->getInEdge(i)->getBundle()->getIndex() );
        node = NULL;
    }
    VisUtils::mapColorCoolBlue( colLeaf );
}


// -----------------------------
void TimeSeries::getAnimIdxDgrm(
    int &idxLeaf,
    set< int > &idcsBndl,
    ColorRGB &colLeaf )
// -----------------------------
{
    if ( animFrame != itemsMarked.end() )
    {
        Node* nodeFr;
        Node* nodeTo;
        Edge* edgeIn;

        idxLeaf = -1;
        idcsBndl.clear();

        if ( 0 <= *animFrame && *animFrame < graph->getSizeNodes() )
        {
            nodeTo = graph->getNode( *animFrame );
            set< int >::iterator it = itemsMarked.begin();
            if ( nodeTo->getIndex() == *it )
                it = itemsMarked.end();
            else
                it = animFrame;
            nodeFr = graph->getNode( *(--it) );

            idxLeaf = nodeTo->getCluster()->getIndex();
            for ( int i = 0; i < nodeTo->getSizeInEdges(); ++i )
            {
                edgeIn = nodeTo->getInEdge( i );
                if ( edgeIn->getInNode() == nodeFr )
                    idcsBndl.insert( edgeIn->getBundle()->getIndex() );
            }
        }
        VisUtils::mapColorCoolBlue( colLeaf );

        nodeFr = NULL;
        nodeTo = NULL;
        edgeIn = NULL;
    }
}


// ------------------------------------------------
void TimeSeries::getAttrIdcs( vector< int > &idcs )
// ------------------------------------------------
{
    idcs.clear();
    for ( size_t i = 0; i < attributes.size(); ++i )
        idcs.push_back( attributes[i]->getIndex() );
}


// -- set functions -------------------------------------------------


// -------------------------------------------------
void TimeSeries::setUseShading( const bool &useShd )
// -------------------------------------------------
{
    useShading = useShd;
}


// ------------------------------------------------
void TimeSeries::setColorClr( const ColorRGB &col )
// ------------------------------------------------
{
    colClr = col;
}


// ------------------------------------------------
void TimeSeries::setColorTxt( const ColorRGB &col )
// ------------------------------------------------
{
    colTxt = col;
}


// ------------------------------------------
void TimeSeries::setSizeTxt( const int &sze )
// ------------------------------------------
{
    szeTxt = sze;
}


// -----------------------------------------
void TimeSeries::setDiagram( Diagram *dgrm )
// -----------------------------------------
{
    clearDiagram();
    diagram = dgrm;

    dataChanged = true;
}


// ------------------------------------------------------------
void TimeSeries::initAttributes( const vector< int > attrIdcs )
// ------------------------------------------------------------
{
    // clear existing attributes
    clearAttributes();

    // init new attributes
    for ( size_t i = 0; i < attrIdcs.size(); ++i )
        attributes.push_back( graph->getAttribute( attrIdcs[i] ) );

    dataChanged = true;
 }


// -------------------------
void TimeSeries::clearData()
// -------------------------
{
    wdwStartIdx = 0;
}


// -----------------------------------------
void TimeSeries::markItems( Cluster* frame )
// -----------------------------------------
{
    if ( frame->getSizeNodes() > 0 )
    {
        // get index of animation frame
        int prevAnimIdx;
        if ( animFrame != itemsMarked.end() )
            prevAnimIdx = *animFrame;
        else
            prevAnimIdx = -1;

        // update marked items
        itemsMarked.clear();
        for ( int i = 0; i < frame->getSizeNodes(); ++i )
            itemsMarked.insert( frame->getNode(i)->getIndex() );

        // update animation frame
        animFrame = itemsMarked.find( prevAnimIdx );
        if ( animFrame == itemsMarked.end() )
            animFrame = itemsMarked.begin();
    }
}


// ----------------------------------------------------------
void TimeSeries::markItems( const vector< Cluster* > frames )
// ----------------------------------------------------------
{
    Cluster* frame;

    // get index of animation frame
    int prevAnimIdx;
    if ( animFrame != itemsMarked.end() )
        prevAnimIdx = *animFrame;
    else
        prevAnimIdx = -1;

    // update marked items
    for ( size_t i = 0; i < frames.size(); ++i )
    {
        frame = frames[i];

        if ( i == 0 && frame->getSizeNodes() > 0 )
            itemsMarked.clear();

        for ( int j = 0; j < frame->getSizeNodes(); ++j )
            itemsMarked.insert( frame->getNode(j)->getIndex() );
    }
    frame = NULL;

    // update animation frame
    animFrame = itemsMarked.find( prevAnimIdx );
    if ( animFrame == itemsMarked.end() )
        animFrame = itemsMarked.begin();
}


// ---------------------------------------
void TimeSeries::handleSendDgrmSglToExnr()
// ---------------------------------------
{
    Cluster* frame;
    vector< Attribute* > attrs;

    frame = new Cluster();
    frame->addNode( graph->getNode( currIdxDgrm ) );
    for ( int i = 0; i < graph->getSizeAttributes(); ++i )
    {
        if ( graph->getAttribute( i )->getSizeCurValues() > 0 )
            attrs.push_back( graph->getAttribute( i ) );
    }
    mediator->addToExaminer( frame, attrs );

    delete frame;
    frame = NULL;
    attrs.clear();
}


// -- visualization functions  --------------------------------------


// ---------------------------------------------------
void TimeSeries::visualize( const bool &inSelectMode )
// ---------------------------------------------------
{
    if ( critSect != true )
    {
        // have textures been generated
        if ( texCharOK != true )
            genCharTex();

        // check if positions are ok
        if ( geomChanged == true )
            calcSettingsGeomBased();
        // check if data are ok
        if ( dataChanged == true )
            calcSettingsDataBased();

        // clear canvas
        clear();

        // visualize
        if ( inSelectMode == true )
        {
            double wth, hgt;
            canvas->getSize( wth, hgt );

            GLint hits = 0;
            GLuint selectBuf[512];
            startSelectMode(
                hits,
                selectBuf,
                0.0125,
                0.0125 );

            glPushName( ID_CANVAS );
            VisUtils::fillRect( -0.5*wth, 0.5*wth, 0.5*wth, -0.5*wth );

            if ( timerAnim->IsRunning() != true )
            {
                drawMarkedItems( inSelectMode );
                drawSlider( inSelectMode );
            }
            if ( dragStatus != DRAG_STATUS_ITMS )
                drawDiagrams( inSelectMode );

            glPopName();

            finishSelectMode(
                hits,
                selectBuf );
        }
        else
        {
            drawAxes( inSelectMode );
            drawMarkedItems( inSelectMode );
            drawSlider( inSelectMode );
            drawScale( inSelectMode );
            drawAttrVals( inSelectMode );
            //drawAxes( inSelectMode );
            drawLabels( inSelectMode );
            drawMouseOver( inSelectMode );
            drawDiagrams( inSelectMode );
        }
    }
}


// -- event handlers ------------------------------------------------


// --------------------------------------
void TimeSeries::handleMouseLftDownEvent(
    const int &x,
    const int &y )
// --------------------------------------
{
    Visualizer::handleMouseLftDownEvent( x, y );

    // redraw in select mode
    visualize( true );
    // redraw in render mode
    visualize( false );
}


// ------------------------------------
void TimeSeries::handleMouseLftUpEvent(
    const int &x,
    const int &y )
// ------------------------------------
{
    Visualizer::handleMouseLftUpEvent( x, y );

    // redraw in select mode
    visualize( true );
    // redraw in render mode
    visualize( false );
}


// ----------------------------------------
void TimeSeries::handleMouseLftDClickEvent(
    const int &x,
    const int &y )
// ----------------------------------------
{
    Visualizer::handleMouseLftDClickEvent( x, y );

    // redraw in select mode
    visualize( true );
    // redraw in render mode
    visualize( false );
}


// --------------------------------------
void TimeSeries::handleMouseRgtDownEvent(
    const int &x,
    const int &y )
// --------------------------------------
{
    Visualizer::handleMouseRgtDownEvent( x, y );

    // redraw in select mode
    visualize( true );
    // redraw in render mode
    visualize( false );
}


// ------------------------------------
void TimeSeries::handleMouseRgtUpEvent(
    const int &x,
    const int &y )
// ------------------------------------
{
    Visualizer::handleMouseRgtUpEvent( x, y );

    // redraw in select mode
    visualize( true );
    // redraw in render mode
    visualize( false );
}


// -------------------------------------
void TimeSeries::handleMouseMotionEvent(
    const int &x,
    const int &y )
// -------------------------------------
{
    Visualizer::handleMouseMotionEvent( x, y );

    // redraw in select mode
    visualize( true );
    // redraw in render mode
    visualize( false );

    xMousePrev = xMouseCur;
    yMousePrev = yMouseCur;
}



// ---------------------------------------
void TimeSeries::handleMouseWheelIncEvent(
    const int &x,
    const int &y )
// ---------------------------------------
{
    Visualizer::handleMouseWheelIncEvent( x, y );

    if ( timerAnim->IsRunning() != true )
    {
        mouseOverIdx = -1;

        // zoom out
        double pix  = canvas->getPixelSize();
        double dist = posSliderBotRgt.x - posSliderTopLft.x;

        double diff = actPixPerNode;
        actPixPerNode = (dist/pix)/(double)(nodesWdwScale-nodesItvSlider);
        diff -= actPixPerNode;

        if ( actPixPerNode > minPixPerNode )
        {
            actPixPerNode = minPixPerNode;
            if ( diff > 0 )
                wdwStartIdx += (int)(0.5*(nodesItvSlider-1));
        }
        else if ( actPixPerNode < 0 )
            actPixPerNode = minPixPerNode;
        else
            wdwStartIdx += (int)(0.5*nodesItvSlider);

        geomChanged = true;

        // redraw in render mode
        visualize( false );
    }
}


// ---------------------------------------
void TimeSeries::handleMouseWheelDecEvent(
    const int &x,
    const int &y )
// ---------------------------------------
{
    Visualizer::handleMouseWheelDecEvent( x, y );

    if ( timerAnim->IsRunning() != true )
    {
        mouseOverIdx = -1;

        // zoom in
        double pix  = canvas->getPixelSize();
        double dist = posSliderBotRgt.x - posSliderTopLft.x;

        // update pixels per node
        actPixPerNode = (dist/pix)/(double)(nodesWdwScale+nodesItvSlider);
        if ( actPixPerNode < itvSliderPerNode )
        {
            actPixPerNode = itvSliderPerNode;
            wdwStartIdx -= (int)(0.5*itvSliderPerNode);
        }
        else
        {
            // update position
            wdwStartIdx -= (int)(0.5*nodesItvSlider);
            if ( wdwStartIdx + nodesWdwScale+nodesItvSlider > graph->getSizeNodes()-1)
                wdwStartIdx = (graph->getSizeNodes()-1) - (nodesWdwScale+nodesItvSlider);
            if ( wdwStartIdx < 0 )
                wdwStartIdx = 0;
        }

        geomChanged = true;

        // redraw in render mode
        visualize( false );
    }
}


// -------------------------------------
void TimeSeries::handleMouseLeaveEvent()
// -------------------------------------
{
    Visualizer::initMouse();

    // reset mouse roll-over
    mouseOverIdx = -1;
    mediator->handleMarkFrameClust( this );

    // redraw in render mode
    visualize( false );
}


// -----------------------------------------------------
void TimeSeries::handleKeyDownEvent( const int &keyCode )
// -----------------------------------------------------
{
    Visualizer::handleKeyDownEvent( keyCode );

    if ( keyCodeDown == WXK_RIGHT || keyCodeDown == WXK_NUMPAD_RIGHT )
    {
        // move to right
        if ( (wdwStartIdx + 1 + nodesWdwScale ) <= (graph->getSizeNodes()-1) )
            wdwStartIdx += 1;
        else if ( (wdwStartIdx + 1 + nodesWdwScale ) > (graph->getSizeNodes()-1) )
            wdwStartIdx = (graph->getSizeNodes()-1) - nodesWdwScale;
    }
    else if ( keyCodeDown == WXK_LEFT || keyCodeDown == WXK_NUMPAD_LEFT )
    {
        // move to left
        if ( (wdwStartIdx + 1) < 0 )
            wdwStartIdx = 0;
        else if ( (wdwStartIdx - 1) >= 0 )
            wdwStartIdx -= 1;
    }
    else if ( keyCodeDown == WXK_HOME || keyCodeDown == WXK_NUMPAD_HOME )
    {
        // move to beginning
        wdwStartIdx = 0;
    }
    else if ( keyCodeDown == WXK_END || keyCodeDown == WXK_NUMPAD_END )
    {
        // move to end
        wdwStartIdx = (graph->getSizeNodes()-1) - nodesWdwScale;
    }
    else if ( keyCodeDown == WXK_PAGEUP || keyCodeDown == WXK_NUMPAD_PAGEUP || keyCodeDown == WXK_NUMPAD9 )
    {
        // move one window toward beginning
        if ( wdwStartIdx - nodesWdwScale < 0 )
            wdwStartIdx = 0;
        else
            wdwStartIdx -= nodesWdwScale;

    }
    else if ( keyCodeDown == WXK_PAGEDOWN || keyCodeDown == WXK_NUMPAD_PAGEDOWN || keyCodeDown == WXK_NUMPAD3 )
    {
        // move one window toward end
        if ( (wdwStartIdx + 2*nodesWdwScale ) <= (graph->getSizeNodes()-1) )
            wdwStartIdx += nodesWdwScale;
        else
            wdwStartIdx = (graph->getSizeNodes()-1) - nodesWdwScale;
    }
    else if ( keyCodeDown == WXK_ESCAPE )
    {
        if ( timerAnim->IsRunning() )
            timerAnim->Stop();
        else
            itemsMarked.clear();
    }

    // redraw in render mode
    visualize( false );
}


// ----------------------------------------------------
void TimeSeries::handleKeyUpEvent( const int &keyCode )
// ----------------------------------------------------
{
    Visualizer::handleKeyUpEvent( keyCode );

    if ( keyCode == WXK_SHIFT )
    {
        shiftStartIdx = -1;
    }
}


// -- utility functions -----------------------------------------


// -------------------------------------
void TimeSeries::calcSettingsGeomBased()
// -------------------------------------
{
    critSect = true;

    // update flag
    geomChanged = false;
    // calculate positions
    calcPositions();

    critSect = false;
}


// -------------------------------------
void TimeSeries::calcSettingsDataBased()
// -------------------------------------
{
    critSect = true;

    // update flag
    dataChanged = false;
    // calculate positions
    calcPositions();

    critSect = false;
}


// -----------------------------
void TimeSeries::calcPositions()
// -----------------------------
{
    double yItv = 0.0;

    double prevItvWdwPerNode = itvWdwPerNode;
    double prevScaleLft      = posScaleTopLft.x;

    // calc general info
    double pix = canvas->getPixelSize();
    double cWth, cHgt;
    canvas->getSize( cWth, cHgt );
    double xLft = -0.5*cWth;
    double xRgt =  0.5*cWth;
    double yTop =  0.5*cHgt;
    double yBot = -0.5*cHgt;

    // calc positions of slider at top
    posSliderTopLft.x = xLft + 5.0*pix;
    posSliderTopLft.y = yTop - 6.0*pix;
    posSliderBotRgt.x = xRgt - 5.0*pix;
    posSliderBotRgt.y = yTop - 6.0*pix - 6.0*ySpacePxl*pix;
    // calc intervals of slider
    if ( graph->getSizeNodes() > 0 )
    {
        nodesItvSlider = 1;
        double distPx = (posSliderBotRgt.x - posSliderTopLft.x)/pix;
        itvSlider = 0.0;

        while ( itvSlider == 0.0 )
        {
            double fact = (double)(graph->getSizeNodes())/(double)nodesItvSlider;
            double itvPx = distPx/fact;

            if ( itvPx >= 5 )
                itvSlider = itvPx*pix;
            else
                nodesItvSlider *= 10;
        }

        itvSliderPerNode = (posSliderBotRgt.x - posSliderTopLft.x)/(double)(graph->getSizeNodes());
    }
    else
        itvSlider = cWth;

    // calc size of visible window
    if ( graph->getSizeNodes() > 0 )
    {
        double distPx = (posSliderBotRgt.x - posSliderTopLft.x)/pix;

        nodesWdwScale = int (distPx/actPixPerNode);
        if ( graph->getSizeNodes() < nodesWdwScale )
            nodesWdwScale = graph->getSizeNodes();

        itvWdwPerNode = (posSliderBotRgt.x - posSliderTopLft.x)/(double)nodesWdwScale;
    }
    else
        nodesWdwScale = 0;

    // calc intervals of scale
    if ( graph->getSizeNodes() > 0 )
    {
        nodesItvScale = 1;
        double distPx = (posScaleBotRgt.x - posScaleTopLft.x)/pix;

        double itvPx = (double)distPx/(double)(nodesWdwScale/nodesItvScale);
        while ( itvPx <= 3 )
        {
            nodesItvScale *= 10;
            itvPx = (double)distPx/(double)(nodesWdwScale/nodesItvScale);
        }
    }
    else
        nodesItvScale = int (cWth);

    // calc positions of scale at bottom
    posScaleTopLft.x = xLft + 5.0*pix;
    posScaleTopLft.y = yBot + 3.5*ySpacePxl*pix + 6.0*pix;
    posScaleBotRgt.x = xRgt - 5.0*pix;
    posScaleBotRgt.y = yBot + 6.0*pix;

    // calc positions of attribute grids
    posAxesTopLft.clear();
    posAxesBotRgt.clear();

    if ( attributes.size() > 0 )
    {
        double yDist = cHgt
            - (6.0*ySpacePxl + 6.0)*pix // slider at top
            - (3.5*ySpacePxl + 6.0)*pix // scale at bottom
            - 2.0*pix;                  // spacing
        yItv = yDist/(double)attributes.size();
    }

    Position2D pos;
    for ( size_t i = 0; i < attributes.size(); ++i )
    {
        pos.x = posScaleTopLft.x;
        pos.y = yTop
            - (6.0*ySpacePxl + 6.0)*pix // slider at top
            - 0.5*ySpacePxl*pix         // space between attrs
            - i*yItv                    // top of interval
            - 3.0*pix;                  // bit of a hack
        posAxesTopLft.push_back( pos );

        pos.x = posScaleBotRgt.x;
        pos.y = yTop
            - (6.0*ySpacePxl + 6.0)*pix // slider at top
            - (i+1)*yItv;               // bot of interval
        posAxesBotRgt.push_back( pos );
    }

    // calc positions of values
    posValues.clear();
    Attribute* attr;
    Node* node;
    for ( size_t i = 0; i < attributes.size(); ++i )
    {
        attr = attributes[i];
        vector< Position2D > v;

        if ( attr->getSizeCurValues() == 0 )
        {
            double rge;
            if ( attr->getLowerBound() < 0 )
                rge = 2.0*Utils::maxx( Utils::abs( attr->getLowerBound() ), Utils::abs( attr->getUpperBound() ) );
            else
                rge = attr->getUpperBound() - attr->getLowerBound();

            for ( int j = 0; j< graph->getSizeNodes(); ++j )
            {
                node = graph->getNode( j );
                double alphaHgt;
                if ( attr->getLowerBound() < 0 )
                    alphaHgt = 0.5 + node->getTupleVal( attr->getIndex() )/rge;
                else
                    alphaHgt = ( node->getTupleVal( attr->getIndex() ) - attr->getLowerBound() )/rge;

                pos.x = posAxesTopLft[i].x + j*itvWdwPerNode;
                pos.y = posAxesBotRgt[i].y + alphaHgt*(yItv - 0.5*ySpacePxl*pix - 3.0*pix);

                v.push_back( pos );
            }
        }
        else
        {
            for ( int j = 0; j< graph->getSizeNodes(); ++j )
            {
                node = graph->getNode( j );

                double alphaHgt;
                if ( attr->getSizeCurValues() == 1 )
                    alphaHgt = 1.0;
                else
                    alphaHgt = (double)attr->mapToValue( node->getTupleVal( attr->getIndex() ) )->getIndex()
                            /
                            (double)(attr->getSizeCurValues()-1);
                pos.x = posAxesTopLft[i].x + j*itvWdwPerNode;
                pos.y = posAxesBotRgt[i].y + alphaHgt*(yItv - 0.5*ySpacePxl*pix - 3.0*pix);

                v.push_back( pos );
            }
        }
        posValues.push_back( v );
    }

    // diagram scale factor to draw 120 x 120 pix diagram
    scaleDgrm = 120.0*(pix/2.0);

    // update positions of diagrams
    map< int, Position2D >::iterator it;
    for ( it = showDgrm.begin(); it != showDgrm.end(); ++it )
    {
        double prevCorrIdx = (it->second.x-prevScaleLft)/prevItvWdwPerNode;
        it->second.x = posScaleTopLft.x + prevCorrIdx*itvWdwPerNode;
    }

    // clear memory
    attr = NULL;
    node = NULL;
}


// ----------------------------
void TimeSeries::clearDiagram()
// ----------------------------
{
    // association
    diagram = NULL;
}


// -------------------------------
void TimeSeries::clearAttributes()
// -------------------------------
{
    // association
    attributes.clear();
}


// -- utility event handlers ----------------------------------------


// ----------------------------------------
void TimeSeries::onTimer( wxTimerEvent &e )
// ----------------------------------------
{
    ++animFrame;
    if ( animFrame == itemsMarked.end() )
        animFrame = itemsMarked.begin();

    //mediator->handleAnimFrameBundl( this );
    mediator->handleAnimFrameClust( this );

    visualize( false );
    canvas->Refresh();
}


// -----------------------------------------------------
void TimeSeries::handleRwndDiagram( const int &dgrmIdx )
// -----------------------------------------------------
{
    animFrame = itemsMarked.begin();

    map< int, Position2D >::iterator it;
    it = showDgrm.find( dgrmIdx );

    int idx = *animFrame;
    Position2D pos = it->second;

    showDgrm.erase( it );
    showDgrm.insert( pair< int, Position2D >( idx, pos ) );

    animIdxDgrm = idx;
    currIdxDgrm = idx;

    mediator->handleAnimFrameClust( this );

    visualize( false );
    canvas->Refresh();
}


// -----------------------------------------------------
void TimeSeries::handlePrevDiagram( const int &dgrmIdx )
// -----------------------------------------------------
{
    if ( timerAnim->IsRunning() )
        timerAnim->Stop();

    if ( animFrame == itemsMarked.begin() )
        animFrame = --itemsMarked.end();
    else
        --animFrame;

    if ( *animFrame != animIdxDgrm )
    {
        map< int, Position2D >::iterator it;
        it = showDgrm.find( animIdxDgrm );
        if ( it == showDgrm.end() )
            it = --showDgrm.end();

        int idx = *animFrame;
        Position2D pos = it->second;

        showDgrm.erase( it );
        showDgrm.insert( pair< int, Position2D >( idx, pos ) );

        mediator->handleAnimFrameClust( this );

        animIdxDgrm = idx;
        currIdxDgrm = idx;
    }
}


// -----------------------------------------------------
void TimeSeries::handlePlayDiagram( const int &dgrmIdx )
// -----------------------------------------------------
{
    if ( dgrmIdx == animIdxDgrm )
    {
        if ( timerAnim->IsRunning() )
        {
            timerAnim->Stop();

            if ( *animFrame != animIdxDgrm )
            {
                map< int, Position2D >::iterator it;
                it = showDgrm.find( animIdxDgrm );

                int idx = *animFrame;
                Position2D pos = it->second;

                showDgrm.erase( it );
                showDgrm.insert( pair< int, Position2D >( idx, pos ) );

                animIdxDgrm = idx;
                currIdxDgrm = idx;
            }
        }
        else
            timerAnim->Start( itvAnim );
    }
    else
    {
        animIdxDgrm = dgrmIdx;
        animFrame = itemsMarked.begin();
        timerAnim->Start( itvAnim );
    }
}


// -----------------------------------------------------
void TimeSeries::handleNextDiagram( const int &dgrmIdx )
// -----------------------------------------------------
{
    if ( timerAnim->IsRunning() )
        timerAnim->Stop();

    if ( animFrame == itemsMarked.end() )
        animFrame = itemsMarked.find( dgrmIdx );

    ++animFrame;
    if ( animFrame == itemsMarked.end() )
        animFrame = itemsMarked.begin();

    if ( *animFrame != animIdxDgrm )
    {
        map< int, Position2D >::iterator it;
        it = showDgrm.find( animIdxDgrm );
        if ( it == showDgrm.end() )
            it = showDgrm.begin();

        int idx = *animFrame;
        Position2D pos = it->second;

        showDgrm.erase( it );
        showDgrm.insert( pair< int, Position2D >( idx, pos ) );

        mediator->handleAnimFrameClust( this );

        animIdxDgrm = idx;
        currIdxDgrm = idx;
    }
}


// -- hit detection -------------------------------------------------


// ----------------------------------------------------
void TimeSeries::handleHits( const vector< int > &ids )
// ----------------------------------------------------
{
   if ( ids.size() > 1 )
    {
        // mouse button down
        if ( mouseButton == MSE_BUTTON_DOWN )
        {
            mouseOverIdx = -1;

            if ( mouseDrag == MSE_DRAG_TRUE )
            {
                if ( mouseSide == MSE_SIDE_LFT )
                {
                    if ( dragStatus == DRAG_STATUS_SLDR )
                        handleDragSliderHdl();
                    else if ( dragStatus == DRAG_STATUS_SLDR_LFT )
                        handleDragSliderHdlLft();
                    else if ( dragStatus == DRAG_STATUS_SLDR_RGT )
                        handleDragSliderHdlRgt();
                    else if ( dragStatus == DRAG_STATUS_ITMS )
                    {
                        if ( ids.size() > 2 )
                            handleDragItems( ids[2] );
                    }
                    else if ( dragStatus == DRAG_STATUS_DGRM )
                    {
                        if ( ids.size() > 2 )
                            handleDragDiagram( ids[2] );
                    }
                }
            }
        }
        // mouse button up
        else
        {
            dragDistNodes = 0.0;
            dragStatus = DRAG_STATUS_NONE;

            if ( mouseSide == MSE_SIDE_LFT && ids.size() > 2 && ids[1] == ID_DIAGRAM )
            {
            	if ( ids.size() == 4 && mouseClick == MSE_CLICK_SINGLE )
                {
                	if ( ids[3] == ID_DIAGRAM_CLSE )
                    {
                       	handleShowDiagram( ids[2] );
                    }
                    else if ( ids[3] == ID_DIAGRAM_MORE )
                    {
                    	currIdxDgrm = ids[2];
                       	mediator->handleSendDgrm( this, false, false, false, true, false );
                    }
                    else if ( ids[3] == ID_DIAGRAM_RWND )
                      	handleRwndDiagram( ids[2] );
                    else if ( ids[3] == ID_DIAGRAM_PREV )
                       	handlePrevDiagram( ids[2] );
                    else if ( ids[3] == ID_DIAGRAM_PLAY )
                       	handlePlayDiagram( ids[2] );
                    else if ( ids[3] == ID_DIAGRAM_NEXT )
                       	handleNextDiagram( ids[2] );
                }
                else
                	dragStatus = DRAG_STATUS_DGRM;

                mouseOverIdx = -1;
                currIdxDgrm = ids[2];

                if ( currIdxDgrm >= 0 && timerAnim->IsRunning() != true )
                {
                    ColorRGB col;
                    Cluster* frame = new Cluster();
                    vector< Attribute* > attrs;

                    VisUtils::mapColorCoolBlue( col );
                    frame->addNode( graph->getNode( currIdxDgrm ) );

                    for ( int i = 0; i < graph->getSizeAttributes(); ++i )
                    {
                        //if ( graph->getAttribute( i )->getSizeCurValues() > 0 )
                            attrs.push_back( graph->getAttribute( i ) );
                    }

                    mediator->handleShowFrame( frame, attrs, col );
                    mediator->handleMarkFrameClust( this );

                    delete frame;
                    frame = NULL;
                }
            }
            else if ( mouseClick == MSE_CLICK_SINGLE && mouseSide == MSE_SIDE_LFT && ids[1] == ID_SLIDER )
            {
                if ( ids.size() == 3 )
                {
                	if ( ids[2] == ID_SLIDER_HDL )
                      	dragStatus = DRAG_STATUS_SLDR;
                    else if ( ids[2] == ID_SLIDER_HDL_LFT )
                       	dragStatus = DRAG_STATUS_SLDR_LFT;
                    else if ( ids[2] == ID_SLIDER_HDL_RGT )
                       	dragStatus = DRAG_STATUS_SLDR_RGT;
                }
                else
                {
                   	dragStatus = DRAG_STATUS_SLDR;
                    handleHitSlider();
                }
            }
            else if ( mouseSide == MSE_SIDE_LFT && ids.size() > 2 &&
                      ( ids[1] == ID_ITEMS && attributes.size() > 0 ) )
            {
            	if ( mouseClick == MSE_CLICK_SINGLE )
                {
                	handleHitItems( ids[2] );
                	dragStatus = DRAG_STATUS_ITMS;
                }
                else if ( mouseClick == MSE_CLICK_DOUBLE )
                {
                	handleShowDiagram( ids[2] );
                }
                //mouseOverIdx = ids[2];
                //currIdxDgrm = -1;

                //mediator->handleMarkFrameClust( this );
                //mediator->handleUnshowFrame();
            }
            else if( mouseSide == MSE_SIDE_RGT && mouseClick == MSE_CLICK_SINGLE )
            {
               	if ( mouseButton == MSE_BUTTON_DOWN )
                {
                	/*
                    if ( ids[1] == ID_ITEMS )
                    *mediator << "show menu\n";
                    else
                    */
                    if ( ids[1] == ID_DIAGRAM )
                    	mediator->handleSendDgrm( this, false, false, false, true, false );
                }
            }
            else
            {
                if ( currIdxDgrm != -1 )
                {
                    currIdxDgrm = -1;
                    mediator->handleUnshowFrame();
                    mediator->handleMarkFrameClust( this );
                }

                if ( mouseOverIdx != -1 )
                    mouseOverIdx = -1;

                mediator->handleMarkFrameClust( this );
                mediator->handleUnshowFrame();
            }
        }
    }
    else
    {
        currIdxDgrm  = -1;
        mouseOverIdx = -1;
    }
}


// --------------------------
void TimeSeries::processHits(
    GLint hits,
    GLuint buffer[] )
// --------------------------
{
    GLuint *ptr;
    ptr = (GLuint*) buffer;

    if ( hits > 0 )
    {
        // if necassary, advance to closest hit
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
        int number = *ptr;
        ++ptr; // number
        ++ptr; // z1
        ++ptr; // z2

        vector< int > ids;
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


// ---------------------
void TimeSeries::clear()
// ---------------------
{
    VisUtils::clear( colClr );
}


// ----------------------------------------------------
void TimeSeries::drawSlider( const bool &inSelectMode )
// ----------------------------------------------------
{
    if ( inSelectMode == true )
    {
        double pix = canvas->getPixelSize();

        glPushName( ID_SLIDER );
        VisUtils::fillRect(
            posSliderTopLft.x, posSliderBotRgt.x,
            posSliderTopLft.y, posSliderBotRgt.y );

        glPushName( ID_SLIDER_HDL );
        VisUtils::fillRect(
            posSliderTopLft.x + wdwStartIdx*itvSliderPerNode,
            posSliderTopLft.x + (wdwStartIdx + nodesWdwScale)*itvSliderPerNode,
            posSliderTopLft.y - 2.0*pix,
            posSliderTopLft.y - 2.0*ySpacePxl*pix + 2.0*pix );
        glPopName();

        glPushName( ID_SLIDER_HDL_LFT );
        VisUtils::fillTriangle(
            posSliderTopLft.x + wdwStartIdx*itvSliderPerNode - 5*pix,
            posSliderTopLft.y - 2.0*ySpacePxl*pix - 10*pix,
            posSliderTopLft.x + wdwStartIdx*itvSliderPerNode,
            posSliderTopLft.y - 2.0*ySpacePxl*pix - 10*pix,
            posSliderTopLft.x + wdwStartIdx*itvSliderPerNode,
            posSliderTopLft.y - 2.0*ySpacePxl*pix );
        glPopName();

        glPushName( ID_SLIDER_HDL_RGT );
        VisUtils::fillTriangle(
            posSliderTopLft.x + (wdwStartIdx + nodesWdwScale)*itvSliderPerNode,
            posSliderTopLft.y - 2.0*ySpacePxl*pix - 10*pix,
            posSliderTopLft.x + (wdwStartIdx + nodesWdwScale)*itvSliderPerNode + 5*pix,
            posSliderTopLft.y - 2.0*ySpacePxl*pix - 10*pix,
            posSliderTopLft.x + (wdwStartIdx + nodesWdwScale)*itvSliderPerNode,
            posSliderTopLft.y - 2.0*ySpacePxl*pix );
        glPopName();

        glPopName();
    }
    else
    {
        double pix = canvas->getPixelSize();
        ColorRGB colFill, colFade;

        // draw marked items on slider
        VisUtils::setColor( colMrk );
        if ( itvSliderPerNode < pix )
        {
            set< int >::iterator it;
            for ( it = itemsMarked.begin(); it != itemsMarked.end(); ++it )
            {
                VisUtils::drawLine(
                    posSliderTopLft.x + (*it)*itvSliderPerNode,
                    posSliderTopLft.x + (*it)*itvSliderPerNode,
                    posSliderTopLft.y - 2.0*ySpacePxl*pix,
                    posSliderTopLft.y - 4.0*ySpacePxl*pix );
            }
        }
        else
        {
            set< int >::iterator it;
            for ( it = itemsMarked.begin(); it != itemsMarked.end(); ++it )
            {
                VisUtils::fillRect(
                    posSliderTopLft.x + (*it)*itvSliderPerNode,
                    posSliderTopLft.x + (*it + 1)*itvSliderPerNode,
                    posSliderTopLft.y - 2.0*ySpacePxl*pix,
                    posSliderTopLft.y - 4.0*ySpacePxl*pix );
            }
        }

        // draw positions of diagrams
        VisUtils::setColorCoolBlue();
        map< int, Position2D >::iterator it;
        for ( it = showDgrm.begin(); it != showDgrm.end(); ++it )
        {
            VisUtils::drawLine(
                posSliderTopLft.x + it->first*itvSliderPerNode,
                posSliderTopLft.x + it->first*itvSliderPerNode,
                posSliderTopLft.y - 2.0*ySpacePxl*pix,
                posSliderTopLft.y - 4.0*ySpacePxl*pix );
        }

        // draw slider outlines
        VisUtils::setColorMdGray();
        VisUtils::drawLine(
            posSliderTopLft.x, posSliderBotRgt.x,
            posSliderTopLft.y, posSliderTopLft.y );

        VisUtils::setColorWhite();
        VisUtils::drawLine(
            posSliderTopLft.x,
            posSliderBotRgt.x,
            posSliderBotRgt.y - 0.5*ySpacePxl*pix + 1.0*pix,
            posSliderBotRgt.y - 0.5*ySpacePxl*pix + 1.0*pix);
        VisUtils::setColorLtGray();
        VisUtils::drawLine(
            posSliderTopLft.x,
            posSliderBotRgt.x,
            posSliderBotRgt.y - 0.5*ySpacePxl*pix,
            posSliderBotRgt.y - 0.5*ySpacePxl*pix );
        VisUtils::setColorMdGray();
        VisUtils::drawLine(
            posSliderTopLft.x,
            posSliderBotRgt.x,
            posSliderBotRgt.y - 0.5*ySpacePxl*pix - 1.0*pix,
            posSliderBotRgt.y - 0.5*ySpacePxl*pix - 1.0*pix);

        // draw slider scale
        double pos = posSliderTopLft.x;
        int ctr = 0;
        while ( pos <= posSliderBotRgt.x )
        {
            if ( ctr%5 == 0 )
            {
                if ( ctr%10 == 0 )
                {
                    VisUtils::setColorMdGray();
                    VisUtils::drawLine(
                        pos,
                        pos,
                        posSliderTopLft.y - 2.0*ySpacePxl*pix,
                        posSliderTopLft.y - 4.0*ySpacePxl*pix );

                    VisUtils::setColor( colTxt );
                    VisUtils::drawLabelCenter(
                        texCharId,
                        pos,
                        posSliderTopLft.y - 5.0*ySpacePxl*pix,
                        szeTxt*pix/CHARHEIGHT,
                        Utils::intToStr( ctr*nodesItvSlider ) );
                }
                else
                {
                    VisUtils::setColorMdGray();
                    VisUtils::drawLine(
                        pos,
                        pos,
                        posSliderTopLft.y - 2.0*ySpacePxl*pix,
                        posSliderTopLft.y - 4.0*ySpacePxl*pix + 4.0*pix );
                }
            }
            else
            {
                VisUtils::setColorMdGray();
                VisUtils::drawLine(
                    pos,
                    pos,
                    posSliderTopLft.y - 2.0*ySpacePxl*pix,
                    posSliderTopLft.y - 4.0*ySpacePxl*pix + 8.0*pix );
            }

            pos += itvSlider;
            ctr += 1;
        }

        // draw slider
        VisUtils::mapColorLtCoolGreen( colFade );
        VisUtils::mapColorCoolGreen( colFill );
        VisUtils::fillRect(
            posSliderTopLft.x + wdwStartIdx*itvSliderPerNode,
            posSliderTopLft.x + (wdwStartIdx + nodesWdwScale)*itvSliderPerNode,
            posSliderTopLft.y - 2.0*pix,
            posSliderTopLft.y - 2.0*ySpacePxl*pix + 2.0*pix,
            colFade, colFade, colFill, colFill );

        // draw slider handles
        VisUtils::setColorDkGray();
        VisUtils::fillTriangle(
            posSliderTopLft.x + wdwStartIdx*itvSliderPerNode - 5*pix,
            posSliderTopLft.y - 2.0*ySpacePxl*pix - 10*pix,
            posSliderTopLft.x + wdwStartIdx*itvSliderPerNode,
            posSliderTopLft.y - 2.0*ySpacePxl*pix - 10*pix,
            posSliderTopLft.x + wdwStartIdx*itvSliderPerNode,
            posSliderTopLft.y - 2.0*ySpacePxl*pix );
        VisUtils::fillTriangle(
            posSliderTopLft.x + (wdwStartIdx + nodesWdwScale)*itvSliderPerNode,
            posSliderTopLft.y - 2.0*ySpacePxl*pix - 10*pix,
            posSliderTopLft.x + (wdwStartIdx + nodesWdwScale)*itvSliderPerNode + 5*pix,
            posSliderTopLft.y - 2.0*ySpacePxl*pix - 10*pix,
            posSliderTopLft.x + (wdwStartIdx + nodesWdwScale)*itvSliderPerNode,
            posSliderTopLft.y - 2.0*ySpacePxl*pix );

        VisUtils::setColorMdGray();
        VisUtils::enableLineAntiAlias();
        VisUtils::drawTriangle(
            posSliderTopLft.x + wdwStartIdx*itvSliderPerNode - 5*pix,
            posSliderTopLft.y - 2.0*ySpacePxl*pix - 10*pix,
            posSliderTopLft.x + wdwStartIdx*itvSliderPerNode,
            posSliderTopLft.y - 2.0*ySpacePxl*pix - 10*pix,
            posSliderTopLft.x + wdwStartIdx*itvSliderPerNode,
            posSliderTopLft.y - 2.0*ySpacePxl*pix );
        VisUtils::drawTriangle(
            posSliderTopLft.x + (wdwStartIdx + nodesWdwScale)*itvSliderPerNode,
            posSliderTopLft.y - 2.0*ySpacePxl*pix - 10*pix,
            posSliderTopLft.x + (wdwStartIdx + nodesWdwScale)*itvSliderPerNode + 5*pix,
            posSliderTopLft.y - 2.0*ySpacePxl*pix - 10*pix,
            posSliderTopLft.x + (wdwStartIdx + nodesWdwScale)*itvSliderPerNode,
            posSliderTopLft.y - 2.0*ySpacePxl*pix );
        VisUtils::disableLineAntiAlias();
    }
}


// ---------------------------------------------------
void TimeSeries::drawScale( const bool &inSelectMode )
// ---------------------------------------------------
{
    if ( inSelectMode == true )
    {}
    else
    {
        double pix = canvas->getPixelSize();
        int    beg = 0;
        for ( int i = 0; i < nodesWdwScale; ++i )
        {
            beg = wdwStartIdx+i;
            if ( beg%nodesItvScale == 0 )
                break;
        }

        for ( int i = beg; i <= wdwStartIdx+nodesWdwScale; i += nodesItvScale )
        {
            if ( (i/nodesItvScale)%5 == 0 )
            {
                if ( (i/nodesItvScale)%10 == 0 )
                {
                    VisUtils::setColorMdGray();
                    VisUtils::drawLine(
                        posScaleTopLft.x + (i-wdwStartIdx)*itvWdwPerNode,
                        posScaleTopLft.x + (i-wdwStartIdx)*itvWdwPerNode,
                        posScaleTopLft.y,
                        posScaleTopLft.y - 2.0*ySpacePxl*pix );
                    VisUtils::setColor( colTxt );
                    VisUtils::drawLabelCenter(
                        texCharId,
                        posScaleTopLft.x + (i-wdwStartIdx)*itvWdwPerNode,
                        posScaleTopLft.y - 3.0*ySpacePxl*pix,
                        szeTxt*pix/CHARHEIGHT,
                        Utils::intToStr( i ) );
                }
                else
                {
                    VisUtils::setColorMdGray();
                    VisUtils::drawLine(
                        posScaleTopLft.x + (i-wdwStartIdx)*itvWdwPerNode,
                        posScaleTopLft.x + (i-wdwStartIdx)*itvWdwPerNode,
                        posScaleTopLft.y,
                        posScaleTopLft.y - 2.0*ySpacePxl*pix + 4.0*pix );
                }
            }
            else
            {
                VisUtils::setColorMdGray();
                VisUtils::drawLine(
                    posScaleTopLft.x + (i-wdwStartIdx)*itvWdwPerNode,
                    posScaleTopLft.x + (i-wdwStartIdx)*itvWdwPerNode,
                    posScaleTopLft.y,
                    posScaleTopLft.y - 2.0*ySpacePxl*pix + 8.0*pix );
            }
        }
    }
}


// ---------------------------------------------------------
void TimeSeries::drawMarkedItems( const bool &inSelectMode )
// ---------------------------------------------------------
{
    if ( inSelectMode == true )
    {
        double pix = canvas->getPixelSize();

        glPushName( ID_ITEMS );
        for ( int i = 0; i < nodesWdwScale; ++i )
        {
            glPushName( wdwStartIdx + i );
            VisUtils::fillRect(
                posScaleTopLft.x + i*itvWdwPerNode,
                posScaleTopLft.x + (i+1)*itvWdwPerNode,
                posSliderBotRgt.y - 0.5*ySpacePxl*pix,
                posScaleTopLft.y - 2.0*ySpacePxl*pix );
            glPopName();
        }
        glPopName();
    }
    else
    {
        double pix = canvas->getPixelSize();

        // draw selected items
        VisUtils::setColor( colMrk );

        set< int >::iterator it;
        for ( it = itemsMarked.begin(); it != itemsMarked.end(); ++it )
        {
            if ( wdwStartIdx <= (*it) && (*it) <= (wdwStartIdx + nodesWdwScale) )
                VisUtils::fillRect(
                    posScaleTopLft.x - wdwStartIdx*itvWdwPerNode + (*it)*itvWdwPerNode,
                    posScaleTopLft.x - wdwStartIdx*itvWdwPerNode + (*it + 1)*itvWdwPerNode,
                    posSliderBotRgt.y - 0.5*ySpacePxl*pix,
                    posScaleTopLft.y - 2.0*ySpacePxl*pix );
        }
    }
}


// --------------------------------------------------
void TimeSeries::drawAxes( const bool &inSelectMode )
// --------------------------------------------------
{
    if ( inSelectMode == true )
    {}
    else
    {
        ColorRGB colFill;
        ColorRGB colFade;
        VisUtils::mapColorLtGray( colFill );
        VisUtils::mapColorLtLtGray( colFade );
        for ( size_t i = 0; i < posAxesTopLft.size(); ++i )
        {
            VisUtils::fillRect(
                posAxesTopLft[i].x,
                posAxesBotRgt[i].x,
                posAxesTopLft[i].y,
                posAxesBotRgt[i].y,
                colFill,
                colFill,
                colFade,
                colFade );
        }
    }
}


// ------------------------------------------------------
void TimeSeries::drawAttrVals( const bool &inSelectMode )
// ------------------------------------------------------
{
    if ( inSelectMode == true )
    {}
    else
    {
        double iter, numr;
        ColorRGB colFill, colFade;

        // draw bars
        for ( size_t i = 0; i < posValues.size(); ++i )
        {
            if ( attributes[i]->getSizeCurValues() == 0 )
            // unclustered attribute
            {
                double zero;
                if ( attributes[i]->getLowerBound() < 0 )
                    // cater for ranges that include negative values
                    zero = posAxesBotRgt[i].y + 0.5*( posAxesTopLft[i].y - posAxesBotRgt[i].y );
                else
                    zero = posAxesBotRgt[i].y;

                for ( int j = 0; j < nodesWdwScale; ++j )
                {
                    double value = graph->getNode( wdwStartIdx+j )->getTupleVal( attributes[i]->getIndex() );
                    if ( value >= 0 )
                    {
                        if ( useShading == true )
                        {
                            // positive values
                            if ( attributes[i]->getLowerBound() < 0 )
                            {
                                iter = value;
                                numr = Utils::maxx( Utils::abs( attributes[i]->getLowerBound() ), Utils::abs( attributes[i]->getUpperBound() ) );;
                            }
                            else
                            {
                                iter = value - attributes[i]->getLowerBound();
                                numr = attributes[i]->getUpperBound()-attributes[i]->getLowerBound();
                            }

                            VisUtils::mapColorSeqGreen( iter/numr, colFill );
                            colFade.r = colFill.r-0.25;
                            colFade.g = colFill.g-0.25;
                            colFade.b = colFill.b-0.25;
                            colFade.a = 1.0;

                            VisUtils::fillRect(
                                posValues[i][wdwStartIdx+j].x - wdwStartIdx*itvWdwPerNode,
                                posValues[i][wdwStartIdx+j].x - wdwStartIdx*itvWdwPerNode + itvWdwPerNode,
                                posValues[i][wdwStartIdx+j].y,
                                zero,
                                colFill,
                                colFill,
                                colFade,
                                colFade );
                        }
                        else
                        {
                            VisUtils::setColorCoolGreen();
                            VisUtils::fillRect(
                                posValues[i][wdwStartIdx+j].x - wdwStartIdx*itvWdwPerNode,
                                posValues[i][wdwStartIdx+j].x - wdwStartIdx*itvWdwPerNode + itvWdwPerNode,
                                posValues[i][wdwStartIdx+j].y,
                                zero );
                        }
                     }
                    else
                    {
                        if ( useShading == true )
                        {
                            // negative values
                            iter = value;
                            numr = Utils::maxx( Utils::abs( attributes[i]->getLowerBound() ), Utils::abs( attributes[i]->getUpperBound() ) );

                            VisUtils::mapColorSeqRed( Utils::abs( iter )/Utils::abs( numr ), colFill );
                            colFade.r = colFill.r-0.25;
                            colFade.g = colFill.g-0.25;
                            colFade.b = colFill.b-0.25;
                            colFade.a = 1.0;

                            VisUtils::fillRect(
                                posValues[i][wdwStartIdx+j].x - wdwStartIdx*itvWdwPerNode,
                                posValues[i][wdwStartIdx+j].x - wdwStartIdx*itvWdwPerNode + itvWdwPerNode,
                                posValues[i][wdwStartIdx+j].y,
                                zero,
                                colFade,
                                colFade,
                                colFill,
                                colFill );
                        }
                        else
                        {
                            VisUtils::setColorCoolRed();
                            VisUtils::fillRect(
                                posValues[i][wdwStartIdx+j].x - wdwStartIdx*itvWdwPerNode,
                                posValues[i][wdwStartIdx+j].x - wdwStartIdx*itvWdwPerNode + itvWdwPerNode,
                                posValues[i][wdwStartIdx+j].y,
                                zero );
                        }
                    }
                }
            }
            else
            // clustered attribute
            {
                for ( int j = 0; j < nodesWdwScale; ++j )
                {
                    if ( useShading == true )
                    {
                        iter = (double)attributes[i]->mapToValue(
                            graph->getNode(wdwStartIdx+j)->getTupleVal(
                                attributes[i]->getIndex() ) )->getIndex();
                        numr = (double)(attributes[i]->getSizeCurValues()-1);

                        VisUtils::mapColorSeqGreen( iter/numr, colFill );
                        colFade.r = colFill.r-0.25;
                        colFade.g = colFill.g-0.25;
                        colFade.b = colFill.b-0.25;
                        colFade.a = 1.0;

                        VisUtils::fillRect(
                            posValues[i][wdwStartIdx+j].x - wdwStartIdx*itvWdwPerNode,
                            posValues[i][wdwStartIdx+j].x - wdwStartIdx*itvWdwPerNode + itvWdwPerNode,
                            posValues[i][wdwStartIdx+j].y,
                            posAxesBotRgt[i].y,
                            colFill,
                            colFill,
                            colFade,
                            colFade );
                    }
                    else
                    {
                        VisUtils::setColorCoolGreen();
                        VisUtils::fillRect(
                            posValues[i][wdwStartIdx+j].x - wdwStartIdx*itvWdwPerNode,
                            posValues[i][wdwStartIdx+j].x - wdwStartIdx*itvWdwPerNode + itvWdwPerNode,
                            posValues[i][wdwStartIdx+j].y,
                            posAxesBotRgt[i].y );
                    }
                }
            }
        }

        // draw line above or below bars
        VisUtils::enableLineAntiAlias();
        for ( size_t i = 0; i < posValues.size(); ++i )
        {
            glBegin( GL_LINE_STRIP );
            for ( int j = 0; j < nodesWdwScale; ++j )
            {
                iter = graph->getNode( wdwStartIdx+j )->getTupleVal( attributes[i]->getIndex() );
                VisUtils::mapColorMdGray( colFill );

                glColor3f( colFill.r, colFill.g, colFill.b );
                glVertex2f(
                    posValues[i][wdwStartIdx + j].x - wdwStartIdx*itvWdwPerNode,
                    posValues[i][wdwStartIdx + j].y );
                glVertex2f(
                    posValues[i][wdwStartIdx + j].x - wdwStartIdx*itvWdwPerNode + itvWdwPerNode,
                    posValues[i][wdwStartIdx + j].y );
            }
            glEnd();
        }
        VisUtils::disableLineAntiAlias();
    }
}


// ------------------------------------------------------
void TimeSeries::drawDiagrams( const bool &inSelectMode )
// ------------------------------------------------------
{
    if ( inSelectMode == true )
    {
        glPushName( ID_DIAGRAM );
        if ( timerAnim->IsRunning() == true && animIdxDgrm >= 0 )
        {
            Position2D posDgrm;
            map< int, Position2D >::iterator it;
            vector< double >     vals;
            vector< Attribute* > attrs;

            it = showDgrm.find( animIdxDgrm );
            posDgrm = it->second;
            posDgrm.x -= wdwStartIdx*itvWdwPerNode;

            glPushMatrix();
            glTranslatef( posDgrm.x, posDgrm.y, 0.0 );
            glScalef( scaleDgrm, scaleDgrm, scaleDgrm );

            glPushName( it->first );
            VisUtils::fillRect( - 1.0, 1.0, 1.0, -1.0 );

            glPushName( ID_DIAGRAM_RWND );
            VisUtils::fillRect( 0.2, 0.36, -0.8, -0.98 );
            glPopName();

            glPushName( ID_DIAGRAM_PREV );
            VisUtils::fillRect( 0.4, 0.56, -0.8, -0.98 );
            glPopName();

            glPushName( ID_DIAGRAM_PLAY );
            VisUtils::fillRect( 0.6, 0.76, -0.8, -0.98 );
            glPopName();

            glPushName( ID_DIAGRAM_NEXT );
            VisUtils::fillRect( 0.8, 0.96, -0.8, -0.98 );
            glPopName();

            glPopName();
            glPopMatrix();
        }
        else
        {
            Position2D posDgrm;
            vector< double >     vals;
            vector< Attribute* > attrs;

            map< int, Position2D >::iterator it;
            for ( it = showDgrm.begin(); it != showDgrm.end(); ++it )
            {
                posDgrm = it->second;
                posDgrm.x -= wdwStartIdx*itvWdwPerNode;

                glPushMatrix();
                glTranslatef( posDgrm.x, posDgrm.y, 0.0 );
                glScalef( scaleDgrm, scaleDgrm, scaleDgrm );

                glPushName( it->first );
                VisUtils::fillRect( - 1.0, 1.0, 1.0, -1.0 );

                glPushName( ID_DIAGRAM_CLSE );
                VisUtils::fillRect( 0.8, 0.96, 0.96, 0.8 );
                glPopName();

                glPushName( ID_DIAGRAM_MORE );
                VisUtils::fillRect( -0.98, -0.8, -0.8, -0.98 );
                glPopName();

                if ( it->first == currIdxDgrm && itemsMarked.size() > 1 )
                {
                    glPushName( ID_DIAGRAM_RWND );
                    VisUtils::fillRect( 0.2, 0.36, -0.8, -0.98 );
                    glPopName();

                    glPushName( ID_DIAGRAM_PREV );
                    VisUtils::fillRect( 0.4, 0.56, -0.8, -0.98 );
                    glPopName();

                    glPushName( ID_DIAGRAM_PLAY );
                    VisUtils::fillRect( 0.6, 0.76, -0.8, -0.98 );
                    glPopName();

                    glPushName( ID_DIAGRAM_NEXT );
                    VisUtils::fillRect( 0.8, 0.96, -0.8, -0.98 );
                    glPopName();
                }

                glPopName();
                glPopMatrix();
            }
        }
        glPopName();
    }
    else
    {
        double     pix = canvas->getPixelSize();

        if ( timerAnim->IsRunning() == true && animIdxDgrm >= 0 )
        {
            Position2D posPvot, posDgrm;
            map< int, Position2D >::iterator it;
            vector< double >     vals;
            vector< Attribute* > attrs;

            it = showDgrm.find( animIdxDgrm );
            posDgrm = it->second;
            posDgrm.x -= wdwStartIdx*itvWdwPerNode;
            posPvot.x = posScaleTopLft.x + (*animFrame /*+ 0.5*/ - wdwStartIdx)*itvWdwPerNode;
            posPvot.y = posScaleTopLft.y /*+ 0.5*ySpacePxl*pix*/;

            double aglDeg = Utils::calcAngleDg( posDgrm.x-posPvot.x, posDgrm.y-posPvot.y );
            double dist   = Utils::dist( posPvot.x, posPvot.y, posDgrm.x, posDgrm.y );

            // draw vertical line
            VisUtils::setColorCoolBlue();
            VisUtils::drawLine(
                posPvot.x,
                posPvot.x,
                posSliderBotRgt.y - 0.5*ySpacePxl*pix,
                posScaleTopLft.y - 2.0*ySpacePxl*pix );

            // draw connector
            VisUtils::setColorCoolBlue();
            glPushMatrix();
            glTranslatef( posPvot.x, posPvot.y, 0.0 );
            glRotatef( aglDeg-90.0, 0.0, 0.0, 1.0 );
            VisUtils::enableLineAntiAlias();
            VisUtils::fillTriangle( 0.0, 0.0, -pix, dist, pix, dist );
            VisUtils::drawTriangle( 0.0, 0.0, -pix, dist, pix, dist );
            VisUtils::disableLineAntiAlias();
            glPopMatrix();

            glPushMatrix();
            glTranslatef( posDgrm.x, posDgrm.y, 0.0 );
            glScalef( scaleDgrm, scaleDgrm, scaleDgrm );

            // drop shadow
            VisUtils::setColorCoolBlue();
            VisUtils::fillRect(
                -1.0 + 4.0*pix/scaleDgrm,
                 1.0 + 4.0*pix/scaleDgrm,
                 1.0 - 4.0*pix/scaleDgrm,
                -1.0 - 4.0*pix/scaleDgrm );
            // diagram
            for ( int i = 0; i < graph->getSizeAttributes(); ++i )
            {
                Attribute* attr = graph->getAttribute( i );
                Node* node = graph->getNode( *animFrame );
                if ( attr->getSizeCurValues() > 0 )
                {
                    attrs.push_back( attr );
                    vals.push_back( attr->mapToValue( node->getTupleVal( attr->getIndex() ) )->getIndex() );
                }
                else
                {
                    double val = node->getTupleVal( attr->getIndex() );
                    attrs.push_back( attr );
                    vals.push_back( val );
                }
                attr = NULL;
                node = NULL;
            }
            diagram->visualize(
                inSelectMode,
                canvas,
                attrs,
                vals );
            attrs.clear();
            vals.clear();

            VisUtils::enableLineAntiAlias();

            VisUtils::setColorCoolBlue();
            VisUtils::fillRwndIcon( 0.2, 0.36, -0.8, -0.98 );
            VisUtils::setColorLtLtGray();
            VisUtils::drawRwndIcon( 0.2, 0.36, -0.8, -0.98 );

            VisUtils::setColorCoolBlue();
            VisUtils::fillPrevIcon( 0.4, 0.56, -0.8, -0.98 );
            VisUtils::setColorLtLtGray();
            VisUtils::drawPrevIcon( 0.4, 0.56, -0.8, -0.98 );

            VisUtils::setColorCoolBlue();
            VisUtils::fillPauseIcon( 0.6, 0.76, -0.8, -0.98 );
            VisUtils::setColorLtLtGray();
            VisUtils::drawPauseIcon( 0.6, 0.76, -0.8, -0.98 );

            VisUtils::setColorCoolBlue();
            VisUtils::fillNextIcon( 0.8, 0.96, -0.8, -0.98 );
            VisUtils::setColorLtLtGray();
            VisUtils::drawNextIcon( 0.8, 0.96, -0.8, -0.98 );

            VisUtils::disableLineAntiAlias();
            glPopMatrix();
        }
        else
        {
            Position2D posPvot, posDgrm;
            vector< double >     vals;
            vector< Attribute* > attrs;

            map< int, Position2D >::iterator it;
            for ( it = showDgrm.begin(); it != showDgrm.end(); ++it )
            {
                posPvot.x = posScaleTopLft.x + (it->first /*+ 0.5*/ - wdwStartIdx)*itvWdwPerNode;
                posPvot.y = posScaleTopLft.y /*+ 0.5*ySpacePxl*pix*/;
                posDgrm = it->second;
                posDgrm.x -= wdwStartIdx*itvWdwPerNode;

                double aglDeg = Utils::calcAngleDg( posDgrm.x-posPvot.x, posDgrm.y-posPvot.y );
                double dist   = Utils::dist( posPvot.x, posPvot.y, posDgrm.x, posDgrm.y );

                // draw vertical line
                VisUtils::setColorCoolBlue();
                VisUtils::drawLine(
                    posPvot.x,
                    posPvot.x,
                    posSliderBotRgt.y - 0.5*ySpacePxl*pix,
                    posScaleTopLft.y - 2.0*ySpacePxl*pix );

                // draw connector
                VisUtils::setColorCoolBlue();
                glPushMatrix();
                glTranslatef( posPvot.x, posPvot.y, 0.0 );
                glRotatef( aglDeg-90.0, 0.0, 0.0, 1.0 );
                VisUtils::enableLineAntiAlias();
                VisUtils::fillTriangle( 0.0, 0.0, -pix, dist, pix, dist );
                VisUtils::drawTriangle( 0.0, 0.0, -pix, dist, pix, dist );
                VisUtils::disableLineAntiAlias();
                glPopMatrix();

                glPushMatrix();
                glTranslatef( posDgrm.x, posDgrm.y, 0.0 );
                glScalef( scaleDgrm, scaleDgrm, scaleDgrm );

                // drop shadow
                VisUtils::setColorCoolBlue();
                VisUtils::fillRect(
                    -1.0 + 4.0*pix/scaleDgrm,
                     1.0 + 4.0*pix/scaleDgrm,
                     1.0 - 4.0*pix/scaleDgrm,
                    -1.0 - 4.0*pix/scaleDgrm );
                // diagram
                Attribute* attr;
                Node* node;
                for ( int i = 0; i < graph->getSizeAttributes(); ++i )
                {
                    attr = graph->getAttribute( i );
                    node = graph->getNode( it->first );
                    if ( attr->getSizeCurValues() > 0 )
                    {
                        attrs.push_back( attr );
                        vals.push_back( attr->mapToValue( node->getTupleVal( attr->getIndex() ) )->getIndex() );
                    }
                    else
                    {
                        double val = node->getTupleVal( attr->getIndex() );
                        attrs.push_back( attr );
                        vals.push_back( val );
                    }
                }
                attr = NULL;
                node = NULL;

                diagram->visualize(
                    inSelectMode,
                    canvas,
                    attrs,
                    vals );
                attrs.clear();
                vals.clear();

                VisUtils::enableLineAntiAlias();

                // close icon
                VisUtils::setColorCoolBlue();
                VisUtils::fillCloseIcon( 0.8, 0.96, 0.96, 0.8 );
                VisUtils::setColorLtLtGray();
                VisUtils::drawCloseIcon( 0.8, 0.96, 0.96, 0.8 );
                // more icon
                VisUtils::setColorCoolBlue();
                VisUtils::fillMoreIcon( -0.98, -0.8, -0.8, -0.98 );
                VisUtils::setColorLtLtGray();
                VisUtils::drawMoreIcon( -0.98, -0.8, -0.8, -0.98 );

                if ( it->first == currIdxDgrm && itemsMarked.size() > 1 )
                {
                    VisUtils::setColorCoolBlue();
                    VisUtils::fillRwndIcon( 0.2, 0.36, -0.8, -0.98 );
                    VisUtils::setColorLtLtGray();
                    VisUtils::drawRwndIcon( 0.2, 0.36, -0.8, -0.98 );

                    VisUtils::setColorCoolBlue();
                    VisUtils::fillPrevIcon( 0.4, 0.56, -0.8, -0.98 );
                    VisUtils::setColorLtLtGray();
                    VisUtils::drawPrevIcon( 0.4, 0.56, -0.8, -0.98 );

                    VisUtils::setColorCoolBlue();
                    VisUtils::fillPlayIcon( 0.6, 0.76, -0.8, -0.98 );
                    VisUtils::setColorLtLtGray();
                    VisUtils::drawPlayIcon( 0.6, 0.76, -0.8, -0.98 );

                    VisUtils::setColorCoolBlue();
                    VisUtils::fillNextIcon( 0.8, 0.96, -0.8, -0.98 );
                    VisUtils::setColorLtLtGray();
                    VisUtils::drawNextIcon( 0.8, 0.96, -0.8, -0.98 );
                }

                VisUtils::disableLineAntiAlias();
                glPopMatrix();
            }
        }
    }
}


// -------------------------------------------------------
void TimeSeries::drawMouseOver( const bool &inSelectMode )
// -------------------------------------------------------
{
    if ( inSelectMode != true )
    {
        if ( mouseOverIdx != -1 && attributes.size() > 0 )
        {
            double pix = canvas->getPixelSize();
            Position2D pos1, pos2;
            vector< string > lbls;
            vector< Position2D > posTopLft;
            vector< Position2D > posBotRgt;
            size_t maxLbl = 0;

            pos1.x = posScaleTopLft.x + (mouseOverIdx - wdwStartIdx)*itvWdwPerNode;
            pos1.y = posAxesTopLft[0].y;
            pos2.x = pos1.x;
            pos2.y = posAxesBotRgt[posAxesBotRgt.size()-1].y;

            double txtScaling = szeTxt*pix/CHARHEIGHT;

            VisUtils::setColorCoolBlue();
            VisUtils::drawLine( pos1.x, pos2.x, pos1.y, pos2.y );

            for ( size_t i = 0; i < attributes.size(); ++i )
            {
                string lbl;
                if ( attributes[i]->getSizeCurValues() == 0 )
                    lbl = Utils::dblToStr( graph->getNode( mouseOverIdx )->getTupleVal( attributes[i]->getIndex() ) );
                else
                {
                    Attribute* attr = attributes[i];
                    Node* node = graph->getNode( mouseOverIdx );

                    lbl = attr->mapToValue( node->getTupleVal( attr->getIndex() ) )->getValue();

                    attr = NULL;
                    node = NULL;
                }
                lbls.push_back( lbl );

                if ( lbl.size() > maxLbl )
                    maxLbl = lbl.size();

                pos1.x = posScaleTopLft.x + (mouseOverIdx - wdwStartIdx)*itvWdwPerNode;
                pos1.y = posAxesTopLft[i].y /*+ 0.5*txtScaling*CHARHEIGHT*/;
                pos2.x = pos1.x + (lbl.size()+1)*txtScaling*CHARWIDTH;
                pos2.y = pos1.y - 1.0*txtScaling*CHARHEIGHT;
                posTopLft.push_back( pos1 );
                posBotRgt.push_back( pos2 );
            }

            if ( pos1.x + maxLbl*txtScaling*CHARWIDTH > posScaleBotRgt.x )
            {
                for ( size_t i = 0; i < posTopLft.size(); ++i )
                {
                    double diff = posBotRgt[i].x-posTopLft[i].x;
                    posTopLft[i].x -= diff;
                    posBotRgt[i].x -= diff;
                }
            }

            for ( size_t i = 0; i < posTopLft.size(); ++i )
            {
                VisUtils::setColorWhite();
                VisUtils::fillRect(
                    posTopLft[i].x,
                    posBotRgt[i].x,
                    posTopLft[i].y,
                    posBotRgt[i].y );
                VisUtils::setColorCoolBlue();
                VisUtils::drawRect(
                    posTopLft[i].x,
                    posBotRgt[i].x,
                    posTopLft[i].y,
                    posBotRgt[i].y );

                VisUtils::setColor( colTxt );
                VisUtils::drawLabel(
                        texCharId,
                        posTopLft[i].x + 0.5*txtScaling*CHARWIDTH,
                        posTopLft[i].y - 0.5*txtScaling*CHARHEIGHT,
                        szeTxt*pix/CHARHEIGHT,
                        lbls[i] );
            }
        }
    }
}


// ----------------------------------------------------
void TimeSeries::drawLabels( const bool &inSelectMode )
// ----------------------------------------------------
{
if ( inSelectMode == true )
    {}
    else
    {
        double pix = canvas->getPixelSize();
        double txtScaling = szeTxt*pix/CHARHEIGHT;

        for ( size_t i = 0; i < posAxesTopLft.size(); ++i )
        {
            string lblTop, lblBot;
            if ( attributes[i]->getLowerBound() < 0 )
            {
                double tmp = Utils::maxx(
                    Utils::abs( attributes[i]->getLowerBound() ), Utils::abs( attributes[i]->getUpperBound() ) );
                lblTop = Utils::dblToStr( tmp );
                lblBot = Utils::dblToStr( -tmp );
            }
            else
            {
                lblTop = Utils::dblToStr( attributes[i]->getUpperBound() );
                lblBot = Utils::dblToStr( attributes[i]->getLowerBound() );
            }

            // min
            VisUtils::setColorWhite();
            VisUtils::drawLabel(
                texCharId,
                posAxesTopLft[i].x + 2.0*pix,
                posAxesBotRgt[i].y + 0.5*txtScaling*CHARHEIGHT + 1.0*pix,
                szeTxt*pix/CHARHEIGHT,
                lblBot );
            VisUtils::setColor( colTxt );
            VisUtils::drawLabel(
                texCharId,
                posAxesTopLft[i].x + 1.0*pix,
                posAxesBotRgt[i].y + 0.5*txtScaling*CHARHEIGHT + 2.0*pix,
                szeTxt*pix/CHARHEIGHT,
                lblBot );

            // max
            VisUtils::setColorWhite();
            VisUtils::drawLabel(
                texCharId,
                posAxesTopLft[i].x + 2.0*pix,
                posAxesTopLft[i].y - 0.5*txtScaling*CHARHEIGHT - 2.0*pix,
                szeTxt*pix/CHARHEIGHT,
                lblTop );
            VisUtils::setColor( colTxt );
            VisUtils::drawLabel(
                texCharId,
                posAxesTopLft[i].x + 1.0*pix,
                posAxesTopLft[i].y - 0.5*txtScaling*CHARHEIGHT - 1.0*pix,
                szeTxt*pix/CHARHEIGHT,
                lblTop );

            // attribute
            VisUtils::setColorWhite();
            VisUtils::drawLabelCenter(
                texCharId,
                posAxesTopLft[i].x + 0.5*(posAxesBotRgt[i].x - posAxesTopLft[i].x) + 1.0*pix,
                posAxesTopLft[i].y - 0.5*txtScaling*CHARHEIGHT - 2.0*pix,
                szeTxt*pix/CHARHEIGHT,
                attributes[i]->getName() );
            VisUtils::setColor( colTxt );
            VisUtils::drawLabelCenter(
                texCharId,
                posAxesTopLft[i].x + 0.5*(posAxesBotRgt[i].x - posAxesTopLft[i].x),
                posAxesTopLft[i].y - 0.5*txtScaling*CHARHEIGHT - 1.0*pix,
                szeTxt*pix/CHARHEIGHT,
                attributes[i]->getName() );
        }
    }
}


// -------------------------------
void TimeSeries::handleHitSlider()
// -------------------------------
{
    double x, y;
    canvas->getWorldCoords( xMouseCur, yMouseCur, x, y );
    double distWorld = x - ( posSliderTopLft.x + wdwStartIdx*itvSliderPerNode + 0.5*nodesWdwScale*itvSliderPerNode );

    dragDistNodes = distWorld/itvSliderPerNode;

    if ( distWorld < 0 )
    {
        // move to left
        if ( (wdwStartIdx + (int)dragDistNodes) < 0 )
            wdwStartIdx   = 0;
        else if ( (wdwStartIdx + (int)dragDistNodes) >= 0 )
            wdwStartIdx += (int)dragDistNodes;
    }
    else if ( distWorld > 0 )
    {
        // move to right
        if ( (wdwStartIdx + (int)dragDistNodes + nodesWdwScale ) <= (graph->getSizeNodes()/*-1*/) )
            wdwStartIdx += (int)dragDistNodes;
        else if ( (wdwStartIdx + (int)dragDistNodes + nodesWdwScale ) > (graph->getSizeNodes()/*-1*/) )
            wdwStartIdx = (graph->getSizeNodes()/*-1*/) - nodesWdwScale;
    }

    dragDistNodes = 0.0;
}


// -----------------------------------
void TimeSeries::handleDragSliderHdl()
// -----------------------------------
{
//    draggingSlider = true;
    dragStatus = DRAG_STATUS_SLDR;

    double x1, y1;
    double x2, y2;
    canvas->getWorldCoords( xMousePrev, yMousePrev, x1, y1 );
    canvas->getWorldCoords( xMouseCur, yMouseCur, x2, y2 );

    double distWorld = x2-x1;
    dragDistNodes += (distWorld/itvSliderPerNode);

    if ( dragDistNodes < -1 )
    {
        // move to left
        if ( (wdwStartIdx + (int)dragDistNodes) < 0 )
        {
            wdwStartIdx   = 0;
            dragDistNodes = 0.0;
        }
        else if ( (wdwStartIdx + (int)dragDistNodes) >= 0 )
        {
            wdwStartIdx += (int)dragDistNodes;
            dragDistNodes -= (int)dragDistNodes;
        }
    }
    else if ( dragDistNodes > 1 )
    {
        // move to right
        if ( (wdwStartIdx + (int)dragDistNodes + nodesWdwScale ) <= (graph->getSizeNodes()/*-1*/) )
        {
            wdwStartIdx += (int)dragDistNodes;
            dragDistNodes -= (int)dragDistNodes;
        }
        else if ( (wdwStartIdx + (int)dragDistNodes + nodesWdwScale ) > (graph->getSizeNodes()/*-1*/) )
        {
            wdwStartIdx = (graph->getSizeNodes()/*-1*/) - nodesWdwScale;
            dragDistNodes = 0.0;
        }
    }
}


// --------------------------------------
void TimeSeries::handleDragSliderHdlLft()
// --------------------------------------
{
    double pix  = canvas->getPixelSize();
    double xHdl = posSliderTopLft.x + wdwStartIdx*itvSliderPerNode;
    double xMse, yMse;
    canvas->getWorldCoords( xMouseCur, yMouseCur, xMse, yMse );

    if ( xMse < posSliderTopLft.x )
        xMse = posSliderTopLft.x;

    double distWorld  = xMse-xHdl;
    double distNodes  = Utils::rndToNearestMult( (distWorld/itvSliderPerNode), 1.0 );

    double distWindow = posSliderBotRgt.x - posSliderTopLft.x;
    double pixWindow  = distWindow/pix;

    double tempPixPerNode = pixWindow/(double)(nodesWdwScale-distNodes);
    if ( tempPixPerNode < minPixPerNode )
    {
        // update pixels per node
        actPixPerNode  = tempPixPerNode;
        wdwStartIdx   += int (distNodes);

        geomChanged    = true;
    }
    else
        dragStatus = -1;
}


// --------------------------------------
void TimeSeries::handleDragSliderHdlRgt()
// --------------------------------------
{
    double pix  = canvas->getPixelSize();
    double xHdl = posSliderTopLft.x + (wdwStartIdx + nodesWdwScale)*itvSliderPerNode;
    double xMse, yMse;
    canvas->getWorldCoords( xMouseCur, yMouseCur, xMse, yMse );

    if ( posSliderBotRgt.x < xMse )
        xMse = posSliderBotRgt.x;

    double distWorld  = xMse-xHdl;
    double distNodes  = distWorld/itvSliderPerNode;

    double distWindow = posSliderBotRgt.x - posSliderTopLft.x;
    double pixWindow  = distWindow/pix;

    double tempPixPerNode = pixWindow/(double)(nodesWdwScale+distNodes);
    if ( tempPixPerNode < minPixPerNode )
    {
        // update pixels per node
        actPixPerNode  = tempPixPerNode;
        geomChanged = true;
    }
    else
        dragStatus = -1;
}


// ----------------------------------------------
void TimeSeries::handleHitItems( const int &idx )
// ----------------------------------------------
{
    if ( shiftStartIdx < 0 )
        shiftStartIdx = idx;

    // calc index of animation frame
    int prevAnimIdx;
    if ( animFrame != itemsMarked.end() )
        prevAnimIdx = *animFrame;
    else
        prevAnimIdx = -1;

    // shift key
    if ( keyCodeDown == WXK_SHIFT )
    {
        int begIdx, endIdx;

        // calc range of marked items
        if ( shiftStartIdx <= idx )
        {
            begIdx = shiftStartIdx;
            endIdx = idx;
        }
        else if ( idx < shiftStartIdx )
        {
            begIdx = idx;
            endIdx = shiftStartIdx;
        }


        // update marked items
        itemsMarked.clear();
        for ( int i = begIdx; i <= endIdx; ++i )
            itemsMarked.insert( i );
    }
    // control key
    else if ( keyCodeDown == WXK_CONTROL )
    {
        // update marked items
        set< int >::iterator it;
        it = itemsMarked.find( idx );
        if ( it == itemsMarked.end() )
            itemsMarked.insert( idx );
        else
            itemsMarked.erase( it );
        shiftStartIdx = idx;
    }
    // no key
    else
    {
        // update marked items
        itemsMarked.clear();
        itemsMarked.insert( idx );
        shiftStartIdx = idx;
    }

    // update animation frame
    animFrame = itemsMarked.find( prevAnimIdx );
    if ( animFrame == itemsMarked.end() )
        animFrame = itemsMarked.begin();

    dragStartIdx = idx;

    mediator->handleMarkFrameClust( this );
}


// -----------------------------------------------
void TimeSeries::handleDragItems( const int &idx )
// -----------------------------------------------
{
    if ( dragStartIdx < 0 )
        dragStartIdx = idx;

    // calc index of animation frame
    int prevAnimIdx;
    if ( animFrame != itemsMarked.end() )
        prevAnimIdx = *animFrame;
    else
        prevAnimIdx = -1;

    dragStatus = DRAG_STATUS_ITMS;

    if ( dragStartIdx != idx )
    {
        // sort indices
        bool flag = false;

        // shift key
        if ( keyCodeDown == WXK_SHIFT )
        {
            /*
            for ( int i = begIdx; i <= endIdx; ++i )
                itemsMarked.insert( i );
            */
        }
        else if ( keyCodeDown == WXK_CONTROL )
        {
                int begIdx, endIdx;
                bool incr;

                if ( dragStartIdx <= idx )
                {
                    incr = true;
                    begIdx = dragStartIdx;
                    endIdx = idx;
                    if ( dragDir != DRAG_DIR_RGT )
                        flag = true;
                    dragDir = DRAG_DIR_RGT;
                }
                else
                {
                    incr = false;
                    begIdx = idx;
                    endIdx = dragStartIdx;
                    if ( dragDir != DRAG_DIR_LFT )
                        flag = true;
                    dragDir = DRAG_DIR_LFT;
                }

                set< int >::iterator it;
                if ( incr == true )
                {
                    if ( flag == true )
                    {
                        it = itemsMarked.find( begIdx );
                        if ( it != itemsMarked.end() )
                            itemsMarked.erase( begIdx );
                        flag = false;
                    }

                    for ( int i = begIdx+1; i <= endIdx; ++i )
                    {
                        it = itemsMarked.find( i );
                        if ( it == itemsMarked.end() )
                            itemsMarked.insert( i );
                        else
                            itemsMarked.erase( i );
                    }

                    dragStartIdx = endIdx;
                }
                else
                {
                    if ( flag == true )
                    {
                        it = itemsMarked.find( endIdx );
                        if ( it != itemsMarked.end() )
                            itemsMarked.erase( endIdx );
                        flag = false;
                    }

                    for( int i = begIdx; i < endIdx; ++i )
                    {
                        it = itemsMarked.find( i );
                        if ( it == itemsMarked.end() )
                            itemsMarked.insert( i );
                        else
                            itemsMarked.erase( i );
                    }

                    dragStartIdx = begIdx;
                }
        }
        else
        {
            int begIdx, endIdx;
            if ( dragStartIdx <= idx )
            {
                begIdx = dragStartIdx;
                endIdx = idx;
            }
            else
            {
                begIdx = idx;
                endIdx = dragStartIdx;
            }

            itemsMarked.clear();
            for ( int i = begIdx; i <= endIdx; ++i )
                itemsMarked.insert( i );
        }
    }

    // update animation frame
    animFrame = itemsMarked.find( prevAnimIdx );
    if ( animFrame == itemsMarked.end() )
        animFrame = itemsMarked.begin();

    mediator->handleMarkFrameClust( this );
}


// -----------------------------------------------------
void TimeSeries::handleShowDiagram( const int &dgrmIdx )
// -----------------------------------------------------
{
    map< int, Position2D >::iterator it;

    it = showDgrm.find( dgrmIdx );
    // diagram doesn't exist, add it
    if ( it == showDgrm.end() )
    {
        Position2D pos;
        pos.x = posScaleTopLft.x + (dgrmIdx+0.5)*itvWdwPerNode;
        pos.y = 0.0;
        showDgrm.insert( pair< int, Position2D >( dgrmIdx, pos ) );
    }
    // diagram exists, remove it
    else
    {
        showDgrm.erase( it );
    }
}


// -----------------------------------------------------
void TimeSeries::handleDragDiagram( const int &dgrmIdx )
// -----------------------------------------------------
{
    double x1, y1;
    double x2, y2;

    dragStatus = DRAG_STATUS_DGRM;

    canvas->getWorldCoords( xMousePrev, yMousePrev, x1, y1 );
    canvas->getWorldCoords( xMouseCur,  yMouseCur,  x2, y2 );

    map< int, Position2D >::iterator it;
    it = showDgrm.find( dgrmIdx );
    if ( it != showDgrm.end() )
    {
        it->second.x += (x2-x1);
        it->second.y += (y2-y1);
    }
}


// -- implement event table -----------------------------------------


BEGIN_EVENT_TABLE( TimeSeries, wxEvtHandler )
    // menu bar
    EVT_TIMER( ID_TIMER, TimeSeries::onTimer )
END_EVENT_TABLE()


// -- end -----------------------------------------------------------
