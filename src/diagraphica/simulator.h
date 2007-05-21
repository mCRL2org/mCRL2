// --- simulator.h --------------------------------------------------
// (c) 2007  -  A.J. Pretorius  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <cstddef>
#include <cstdlib>
#include <cmath>
#include <set>
#include <string>
#include <vector>
using namespace std;
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

class Simulator : public wxEvtHandler, public Visualizer
{
public:
    // -- constructors and destructor -------------------------------
    Simulator(
        Mediator* m,
        Graph* g,
        GLCanvas* c );
    virtual ~Simulator();

    // -- get functions ---------------------------------------------
    static ColorRGB getColorClr();
    static ColorRGB getColorTxt();
    static int getSizeTxt();
    static ColorRGB getColorBdl();

    static int getBlendType();
    
    ColorRGB getColorSel();
    int getIdxClstSel();

    // -- set functions ---------------------------------------------
    static void setColorClr( const ColorRGB &col );
    static void setColorTxt( const ColorRGB &col );
    static void setSizeTxt( const int &sze );
    static void setColorBdl( const ColorRGB &col );

    static void setBlendType( const int &type );
    
    void setDiagram( Diagram *dgrm );
    void initFrameCurr( 
        Cluster* frame,
        const vector< Attribute* > &attrs );
    void updateFrameCurr( 
        Cluster* frame,
        const Position2D &pos );
    void clearData();

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
    void handleMouseLeaveEvent();

    void handleKeyDownEvent( const int &keyCode );
    /*
    void handleMarkFrameClust(
        DiagramChooser* dc,
        const int &idx );
    */

protected:
    // -- utility functions -----------------------------------------
    void initAttributes( const vector< Attribute* > &attrs );
    void initFramesPrevNext();
    void initBundles();
    void sortFramesPrevNext();
    
    void calcSettingsGeomBased();
    void calcSettingsDataBased();
    void calcIntervals();
    void calcPositions();
    void calcPosFrames();
    void calcPosBundles();
    
    void handleKeyUp();
    void handleKeyRgt();
    void handleKeyDwn();
    void handleKeyLft();
    void markFrameClusts();
    
    void clearAttributes();
    void clearDiagram();
    void clearFrames();
    void clearBundles();

    // -- hit detection ---------------------------------------------
    void handleHits( const vector< int > &ids );
    void processHits( 
        GLint hits, 
        GLuint buffer[] );

    // -- utility drawing functions ---------------------------------
    void clear();
    void calcColor( 
        const int &iter,
        const int &numr,
        ColorRGB &col );
    
    void drawFrameCurr( const bool &inSelectMode );
    void drawFramesPrev( const bool &inSelectMode );
    void drawFramesNext( const bool &inSelectMode );
    void drawBdlLblGridPrev( const bool &inSelectMode );
    void drawBdlLblGridNext( const bool &inSelectMode );
    void drawBundlesPrev( const bool &inSelectMode );
    void drawBundlesNext( const bool &inSelectMode );
    void drawControls( const bool &inSelectMode );
    void animate();
    
    // -- utility event handlers ------------------------------------
    void onTimer( wxTimerEvent &e );
    
    // -- static variables ------------------------------------------
    
    static ColorRGB colClr;
    static ColorRGB colTxt;
    static int      szeTxt;
    static ColorRGB colBdl;

    enum
    {
        ID_CANVAS,
        ID_ICON_CLEAR,
        ID_ICON_UP,
        ID_ICON_NEXT,
        ID_ICON_DOWN,
        ID_ICON_PREV,
        ID_FRAME_CURR,
        ID_FRAME_PREV,
        ID_FRAME_NEXT,
        ID_BUNDLE_PREV,
        ID_BUNDLE_NEXT,
        ID_BUNDLE_LBL,
        ID_TIMER,
        ANIM_NONE,
        ANIM_POS,
        ANIM_BLEND,
        ID_CLEAR,
        ID_DIAGRAM_MORE,
    };
    
    // -- data members ----------------------------------------------
    Diagram* diagram;                // association
    vector< Attribute* > attributes; // association

    Cluster* frameCurr;            // composition
    vector< Cluster* > framesPrev; // composition
    vector< Cluster* > framesNext; // composition
    vector< Bundle* >  bundles;    // composition
    
    vector< Bundle* >  bundlesByLbl;
    vector< Bundle* >  bundlesPrevByLbl;
    vector< Bundle* >  bundlesNextByLbl;
    
    static int itvLblPixVert;
    double scaleDgrmHori;
    double scaleDgrmVert;

    int focusDepthIdx;
    int focusFrameIdx;
    int focusDepthIdxLast;
    int focusFrameIdxPrevLast;
    int focusFrameIdxNextLast;

    int fcsLblPrevIdx;
    int fcsLblNextIdx;

    Position2D posFrameCurr;
    vector< Position2D > posFramesPrev;
    vector< Position2D > posFramesNext;
    
    vector< Position2D > posBdlLblGridPrevTopLft;
    vector< Position2D > posBdlLblGridPrevBotRgt;
    vector< Position2D > posBdlLblGridNextTopLft;
    vector< Position2D > posBdlLblGridNextBotRgt;

    vector< vector< Position2D > > posBundlesPrevTopLft;
    vector< vector< Position2D > > posBundlesPrevBotRgt;
    vector< vector< Position2D > > posBundlesNextTopLft;
    vector< vector< Position2D > > posBundlesNextBotRgt;
    
    // animation
    static int itvTmrMS;
    static double pixPerMS;
    double timeTotalMS;
    double timeAlphaMS;
    int animPhase;
    
    wxTimer* timerAnim;
    bool animating;  
    
    Cluster* keyFrameFr;
    Cluster* keyFrameTo;
    Position2D posKeyFrameFr;
    Position2D posKeyFrameTo;
    Position2D posTweenFrame;

    static int blendType;
    double opacityKeyFrameFr;
    double opacityKeyFrameTo;

    // -- declare event table ---------------------------------------
    DECLARE_EVENT_TABLE();
};

#endif

// -- end -----------------------------------------------------------
