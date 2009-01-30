// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./timeseries.h

#ifndef TIMESERIES_H
#define TIMESERIES_H

#include <cstddef>
#include <cstdlib>
#include <cmath>
#include <map>
#include <set>
#include <string>
#include <vector>
//#include <wx/event.h>
#include "attribute.h"
#include "diagram.h"
#include "glcanvas.h"
#include "graph.h"
#include "visualizer.h"

class TimeSeries : public wxEvtHandler, public Visualizer
{
public:
    // -- constructors and destructor -------------------------------
    TimeSeries(
        Mediator* m,
        Graph* g,
        GLCanvas* c );
    virtual ~TimeSeries();

    // -- get functions ---------------------------------------------
    static bool getUseShading();
    void getIdcsClstMarked( std::set< int > &idcs );
    void getIdcsClstMarked(
        std::set< int > &idcs ,
        ColorRGB &col );
    void getIdxMseOver(
        int &idxLeaf,
        std::set< int > &idcsBndl,
        ColorRGB &colLeaf );
    void getCurrIdxDgrm(
        int &idxLeaf,
        std::set< int > &idcsBndl,
        ColorRGB &colLeaf );
    void getAnimIdxDgrm(
        int &idxLeaf,
        std::set< int > &idcsBndl,
        ColorRGB &colLeaf );
    void getAttrIdcs( std::vector< int > &idcs );

    // -- set functions ---------------------------------------------
    static void setUseShading( const bool &useShd );
    static void setColorClr( const ColorRGB &col );
    static void setColorTxt( const ColorRGB &col );
    static void setSizeTxt( const int &sze );
    
    void setDiagram( Diagram *dgrm );
    void initAttributes( const std::vector< int > attrIdcs );
    void clearData();

    void markItems( Cluster* frame );
    void markItems( const std::vector< Cluster* > frames );

    void handleSendDgrmSglToExnr();
    
    // -- visualization functions  ----------------------------------
    void visualize( const bool &inSelectMode );
        
    // -- event handlers --------------------------------------------
    void handleMouseLftDownEvent( 
        const int &x, 
        const int &y );
    void handleMouseLftUpEvent( 
        const int &x, 
        const int &y );
    void handleMouseLftDClickEvent( 
        const int &x, 
        const int &y );
    void handleMouseRgtDownEvent( 
        const int &x, 
        const int &y );
    void handleMouseRgtUpEvent( 
        const int &x, 
        const int &y );
    void handleMouseMotionEvent( 
        const int &x, 
        const int &y );
    void handleMouseWheelIncEvent( 
        const int &x, 
        const int &y );
    void handleMouseWheelDecEvent( 
        const int &x, 
        const int &y );
    void handleMouseLeaveEvent();
    void handleKeyDownEvent( const int &keyCode );
    void handleKeyUpEvent( const int &keyCode );
    
protected:
    // -- utility functions -----------------------------------------
    void calcSettingsGeomBased();
    void calcSettingsDataBased();
    void calcPositions();

    void clearDiagram();
    void clearAttributes();

    // -- utility event handlers ------------------------------------
    void onTimer( wxTimerEvent &e );
    void handleRwndDiagram( const int &dgrmIdx );
    void handlePrevDiagram( const int &dgrmIdx );
    void handlePlayDiagram( const int &dgrmIdx );
    void handleNextDiagram( const int &dgrmIdx );
    
    // -- hit detection ---------------------------------------------
    void handleHits( const std::vector< int > &ids );
    void processHits( 
        GLint hits, 
        GLuint buffer[] );
    
    // -- utility drawing functions ---------------------------------
    void clear();
    void drawSlider( const bool &inSelectMode );
    void drawScale( const bool &inSelectMode );
    void drawMarkedItems( const bool &inSelectMode );
    void drawAxes( const bool &inSelectMode );
    void drawAttrVals( const bool &inSelectMode );
    void drawDiagrams( const bool &inSelectMode );
    void drawMouseOver( const bool &inSelectMode );
    void drawLabels( const bool &inSelectMode );

    void handleHitSlider();
    void handleDragSliderHdl();
    void handleDragSliderHdlLft();
    void handleDragSliderHdlRgt();

    void handleHitItems( const int &idx );
    void handleDragItems( const int &idx );

    void handleShowDiagram( const int &dgrmIdx );
    void handleDragDiagram( const int &dgrmIdx );
    
    // -- static variables ------------------------------------------
    static ColorRGB colClr;
    static ColorRGB colTxt;
    static int      szeTxt;
    static ColorRGB colMrk;
    enum
    {
        ID_TIMER,
        ID_CANVAS,
        ID_SLIDER,
        ID_SLIDER_HDL,
        ID_SLIDER_HDL_LFT,
        ID_SLIDER_HDL_RGT,
        ID_ITEMS,
        ID_DIAGRAM,
        ID_DIAGRAM_CLSE,
        ID_DIAGRAM_MORE,
        ID_DIAGRAM_RWND,
        ID_DIAGRAM_PREV,
        ID_DIAGRAM_PLAY,
        ID_DIAGRAM_NEXT,
        DRAG_DIR_NULL,
        DRAG_DIR_LFT,
        DRAG_DIR_RGT,
        DRAG_STATUS_NONE,
        DRAG_STATUS_SLDR,
        DRAG_STATUS_SLDR_LFT,
        DRAG_STATUS_SLDR_RGT,
        DRAG_STATUS_ITMS,
        DRAG_STATUS_DGRM
    };
    
    // -- data members ----------------------------------------------
    Diagram* diagram;                // association
    std::vector< Attribute* > attributes; // association
    
    static bool useShading;

    Position2D posSliderTopLft;
    Position2D posSliderBotRgt;
    double itvSlider;
    int    nodesItvSlider;
    double itvSliderPerNode;
    
    Position2D posScaleTopLft;
    Position2D posScaleBotRgt;
    double minPixPerNode;      // min number of pixels per item
    double actPixPerNode;      // actual number of pixels per item
    
    int    nodesWdwScale;
    double itvWdwPerNode;
    int    wdwStartIdx;
    int    nodesItvScale;      // number of nodes per marked interval

    std::vector< Position2D > posAxesTopLft;
    std::vector< Position2D > posAxesBotRgt;
    
    std::vector< std::vector< Position2D> > posValues;

    // interaction
    int dragStatus;
    double dragDistNodes;

    // selection
    std::set< int > itemsMarked;
    int  shiftStartIdx;
    int  dragStartIdx;
    int  dragDir;

    // mouse overs
    int mouseOverIdx;

    // diagrams
    double scaleDgrm;
    std::map< int, Position2D > showDgrm;
    int dragIdxDgrm; // diagram currently being dragged
    int currIdxDgrm; // currently selected diagram
    int animIdxDgrm; // diagram being animated

    double ySpacePxl;
    bool critSect;

    // animation
    wxTimer* timerAnim;
    static int itvAnim;
    std::set< int >::iterator animFrame;
    
    // -- declare event table ---------------------------------------
    DECLARE_EVENT_TABLE()
};

#endif

// -- end -----------------------------------------------------------
