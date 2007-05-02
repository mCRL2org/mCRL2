// --- arcdiagram.h -------------------------------------------------
// (c) 2006  -  A.J. Pretorius  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------

#ifndef ARCDIAGRAM_H
#define ARCDIAGRAM_H

#include <cstddef>
#include <cstdlib>
#include <cmath>
#include <map>
#include <vector>

#include <wx/timer.h>
#include "diagram.h"
#include "graph.h"
#include "utils.h"
#include "visualizer.h"
#include "visutils.h"

class ArcDiagram : public wxEvtHandler, public Visualizer
{
public:
    // -- constructors and destructor -------------------------------
    ArcDiagram(
        Mediator* m,
        Graph* g,
        GLCanvas* c );
    virtual ~ArcDiagram();

    // -- get functions ---------------------------------------------
    static ColorRGB getColorClr();
    static ColorRGB getColorTxt();
    static int getSizeTxt();
    static double getIntervAnim();
    static bool getShowTree();
    static bool getAnnotateTree();
    static int getColorMap();
    static bool getShowBarTree();
    static double getMagnBarTree();
    static bool getShowLeaves();
    static bool getShowBundles();
    static ColorRGB getColorBundles();
    static double getTrspBundles();

    // -- set functions ---------------------------------------------
    static void setColorClr( const ColorRGB &col );
    static void setColorTxt( const ColorRGB &col );
    static void setSizeTxt( const int &sze );
    static void setIntervAnim( const int &itv );
    static void setShowTree( const bool &shw );
    static void setAnnotateTree( const bool &shw );
    static void setColorMap( const int &colMap );
    static void setShowBarTree( const bool &shw );
    static void setMagnBarTree( const double &val );
    static void setShowLeaves( const bool &shw );
    static void setShowBundles( const bool &shw );
    static void setColorBundles( const ColorRGB &col );
    static void setTrspBundles( const double &trsp );

    // -*- //
    void setAttrsTree( const vector< int > idcs );

    void setDiagram( Diagram *dgrm );
    void hideAllDiagrams();
    
    // -*- //
    void markLeaf(
        const int &leafIdx,
        ColorRGB &col );
    void unmarkLeaves();
    // -*- //

    void handleSendDgrmSglToSiml();
    void handleSendDgrmSglToExnr();
    void handleSendDgrmSetToExnr();
    
    // -- visualization functions  ----------------------------------
    void visualize( const bool &inSelectMode );
    void drawBundles( const bool &inSelectMode );
    void drawLeaves( const bool &inSelectMode );
    void drawTree( const bool &inSelectMode );
    void drawTreeLvls( const bool &inSelectMode );
    void drawBarTree( const bool &inSelectMode );
    void drawMarkedLeaves( const bool &inSelectMode );
    void drawDiagrams( const bool &inSelectMode );
        
    // -- input event handlers --------------------------------------
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

    void updateDiagramData();

protected:
    // -- utility drawing functions ---------------------------------
    void clear();
    void calcColor( 
        const int &iter,
        const int &numr,
        ColorRGB &col );
    void calcSettingsGeomBased();
    void calcSettingsDataBased();
    
    void calcSettingsLeaves();
    void calcSettingsBundles();
    void calcSettingsTree();
    void calcPositionsTree( 
        Cluster* c,
        const int &maxLvl,
        const double &itvHgt );
    void calcSettingsBarTree();
    void calcPositionsBarTree(
        Cluster* c,
        const double &yBot,
        const double &height );
    void calcSettingsDiagram();
    void updateMarkBundles();
    
    void clearSettings();
    void clearSettingsLeaves();
    void clearSettingsBundles();
    void clearSettingsTree();
    void clearSettingsBarTree();
    void clearSettingsDiagram();
    
    // -- utility event handlers ------------------------------------
    void onTimer( wxTimerEvent &e );

    void handleHits( const vector< int > &ids );
    
    void handleHoverCluster( 
        const int &i,
        const int &j );
    void handleHoverBundle( const int &bndlIdx );
    void handleHoverBarTree(
        const int &i,
        const int &j );

    void handleShowDiagram( const int &dgrmIdx );
    void handleDragDiagram();
    void handleRwndDiagram( const int &dgrmIdx );
    void handlePrevDiagram( const int &dgrmIdx );
    void handlePlayDiagram( const int &dgrmIdx );
    void handleNextDiagram( const int &dgrmIdx );

    void showDiagram( const int &dgrmIdx );
    void hideDiagram( const int &dgrmIdx );
    
    // -- hit detection ---------------------------------------------
    void processHits(  
        GLint hits, 
        GLuint buffer[] );

    // -- static variables ------------------------------------------
    
    // general
    static ColorRGB colClr;
    static ColorRGB colTxt;
    static int      szeTxt;
    // cluster tree
    static bool showTree;
    static bool annotateTree;
    static int colorMap;
    // bar tree
    static bool showBarTree;
    static double magnBarTree;
    // arc diagram
    static bool showLeaves;
    static bool showBundles;
    static ColorRGB colBundles;
    
    // -- data members ----------------------------------------------
    
    // vis settings bundles
    vector< Position2D > posBundles;
    vector< double >     radiusBundles;
    vector< double >     widthBundles;
    vector< int >        orientBundles;
    vector< bool >       markBundles;
    
    // vis settings leaves
    vector< Position2D > posLeaves;
    double               radLeaves;
    int                  idxInitStLeaves;
    
    // vis settings hierarchy
    vector< Attribute* >           attrsTree;
    vector< vector< Position2D > > posTreeTopLft;
    vector< vector< Position2D > > posTreeBotRgt;
    vector< vector< Cluster* > >   mapPosToClust;
    
    // vis settings bar tree
    vector< vector< Position2D > > posBarTreeTopLft;
    vector< vector< Position2D > > posBarTreeBotRgt;
    
    // diagrams
    Diagram*                       diagram;         // association, user-defined diagram   
    vector< bool >                 showDgrm;        // show/hide diagram for every leaf node
    vector< vector< Attribute* > > attrsDgrm;       // association, attributes linked to shown diagrams
    vector< vector< Cluster* > >   framesDgrm;      // composition, clusters of identical states for shown diagrams
    vector< int >                  frameIdxDgrm;    // current index into framesDgrm
    vector< Position2D >           posDgrm;         // positions of diagrams
    int                            dragIdxDgrm;     // diagram currently being dragged
    int                            animIdxDgrm;     // diagram currently being animated
    int                            currIdxDgrm;
    
    // simulator
    // -*- //
    int prevFrameIdxClust;
    int currFrameIdxClust;
    int nextFrameIdxClust;
    // -*- //
    map< int, vector< ColorRGB > > markLeaves;
    
    // animation
    wxTimer* timerAnim;
    static int itvAnim;

    // -- constants -------------------------------------------------
    enum
    {
        ID_TIMER,
        ID_CANVAS,
        ID_TREE_NODE,
        ID_LEAF_NODE,
        ID_BAR_TREE,
        ID_BUNDLES,
        ID_DIAGRAM,
        ID_DIAGRAM_CLSE,
        ID_DIAGRAM_MORE,
        ID_DIAGRAM_RWND,
        ID_DIAGRAM_PREV,
        ID_DIAGRAM_PLAY,
        ID_DIAGRAM_NEXT,
    };

    static int MIN_RAD_HINT_PX; // radius cannot be smaller than this
    static int MAX_RAD_HINT_PX; // radius cannot be larger than this
    static int SEGM_HINT_HQ;
    static int SEGM_HINT_LQ;
    
    // -- declare event table ---------------------------------------
    DECLARE_EVENT_TABLE();
};

#endif

// -- end -----------------------------------------------------------
