// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./examiner.h

#ifndef EXAMINER_H
#define EXAMINER_H

#include <cstddef>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <wx/event.h>
#include <wx/timer.h>
#include "bundle.h"
#include "diagram.h"
#include "edge.h"
#include "glcanvas.h"
#include "graph.h"
#include "utils.h"
#include "visualizer.h"
#include "visutils.h"

class Examiner : public wxEvtHandler, public Visualizer
{
public:
    // -- constructors and destructor -------------------------------
    Examiner(
        Mediator* m,
        Graph* g,
        GLCanvas* c );
    virtual ~Examiner();

    // -- get functions ---------------------------------------------
    static ColorRGB getColorClr();
    static ColorRGB getColorTxt();
    static int getSizeTxt();
    static ColorRGB getColorBdl();

    ColorRGB getColorSel();
    int getIdxClstSel();

    // -- set functions ---------------------------------------------
    static void setColorClr( const ColorRGB &col );
    static void setColorTxt( const ColorRGB &col );
    static void setSizeTxt( const int &sze );
    static void setColorBdl( const ColorRGB &col );

    static void setBlendType( const int &type );
    
    void setDiagram( Diagram *dgrm );
    void setFrame( 
        Cluster* frme,
        const std::vector< Attribute* > &attrs,
        ColorRGB col );
    void clrFrame();

    void addFrameHist(
        Cluster* frme,
        const std::vector< Attribute* > &attrs );
    void clrFrameHist();
    void clrFrameHistCur();
    int getSizeFramesHist();

    void clearData();

    void handleSendDgrmSglToSiml();
    void handleSendDgrmSglToTrace();
    void handleSendDgrmSetToTrace();

    // -- visualization functions  ----------------------------------
    void visualize( const bool &inSelectMode );
        
    // -- event handlers --------------------------------------------
    void handleSizeEvent();
    
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
    void handleKeyDownEvent( const int &keyCode );

protected:
    // -- utility functions -----------------------------------------
/*
    void initAttributes( const std::vector< Attribute* > &attrs );
    void initFrames();
    void initBundles();
*/    
    void calcSettingsGeomBased();
    void calcSettingsDataBased();
    void calcPosFrame();
    void calcPosFramesHist();
    
    void clearAttributes();
    void clearDiagram();
    void clearFrames();

    // -- hit detection ---------------------------------------------
    void handleHits( const std::vector< int > &ids );
    void handleIconRwnd();
    void handleIconLft();
    /*
    void handleIconPlay();
    */
    void handleIconRgt();
    void processHits( 
        GLint hits, 
        GLuint buffer[] );

    // -- utility drawing functions ---------------------------------
    void clear();

    void drawFrame( const bool &inSelectMode );
    void drawFramesHist( const bool &inSelectMode );
    void drawControls( const bool &inSelectMode );

    // -- utility event handlers ------------------------------------
/*
    void onTimer( wxTimerEvent &e );
*/  
    // -- static variables ------------------------------------------
    
    static ColorRGB colClr;
    static ColorRGB colTxt;
    static int      szeTxt;
    static ColorRGB colBdl;
    static int      hgtHstPix;

    enum
    {
        ID_ICON_CLR,
        ID_ICON_MORE,
        ID_ICON_RWND,
        ID_ICON_LFT,
        ID_ICON_RGT,
        ID_ICON_PLAY,
        ID_FRAME,
        ID_FRAME_HIST
    };
    
    // -- data members ----------------------------------------------
    Diagram* diagram;                // association
    std::vector< Attribute* > attributes; // association

    Cluster* frame;                  // composition
    ColorRGB colFrm;

    std::vector< Cluster* > framesHist;            // composition
    std::vector< std::vector< Attribute* > > attrsHist; // association

    Position2D posFrame;
    std::vector< Position2D > posFramesHist;

    size_t focusFrameIdx;

    double scaleFrame;
    double scaleFramesHist;

    double offset;
    size_t vsblHistIdxLft;
    size_t vsblHistIdxRgt;

    // -- declare event table ---------------------------------------
    DECLARE_EVENT_TABLE()
};

#endif

// -- end -----------------------------------------------------------
