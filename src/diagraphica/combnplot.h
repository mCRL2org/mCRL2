// --- combnplot.h --------------------------------------------------
// (c) 2006  -  A.J. Pretorius  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------

#ifndef COMBNPLOT_H
#define COMBNPLOT_H

#include <cstddef>
#include <cstdlib>
#include <cmath>
#include <vector>
using namespace std;
#include "diagram.h"
#include "graph.h"
#include "utils.h"
#include "visualizer.h"

class CombnPlot : public Visualizer
{
public:
    // -- constructors and destructor -------------------------------
    CombnPlot(
        Mediator* m,
        Graph* g,
        GLCanvas* c );
    virtual ~CombnPlot();

    // -- set data functions ----------------------------------------
    void setValues(
        const vector< int > &attrIndcs,
        vector< vector< int > > &combs,
        vector< int > &number );
    void clearValues();

  	void setDiagram( Diagram* dgrm );
        
    // -- set vis settings functions --------------------------------

    // -- visualization functions  ----------------------------------
    void visualize( const bool &inSelectMode );
    void drawAxes( const bool &inSelectMode );
    void drawAxesBC( const bool &inSelectMode );
    void drawAxesCP( const bool &inSelectMode );
    void drawLabels( const bool &inSelectMode );
    void drawLabelsBC( const bool &inSelectMode );
    void drawLabelsCP( const bool &inSelectMode );
    void drawPlot( const bool &inSelectMode );
    void drawPlotBC( const bool &inSelectMode );
    void drawPlotCP( const bool &inSelectMode );
    void drawMousePos( const bool &inSelectMode );
    void drawDiagram( const bool &inSelectMode );
    
    // -- input event handlers --------------------------------------
    void handleMouseMotionEvent(
		const int &x,
		const int &y );
    /*
    void handleMouseEnterEvent();
    void handleMouseLeaveEvent();
    */
    
protected:
    // -- utility data functions ------------------------------------
    void initLabels();
    void calcMaxAttrCard();
    void calcMaxNumberPerComb();

    // -- utility drawing functions ---------------------------------
    // ***
    //void clear();
    void setScalingTransf();
    void displTooltip( const int &posIdx );

    void calcPositions();
    void calcPosBC();
    void calcPosCP();
    void clearPositions();

    // -- hit detection ---------------------------------------------
    void processHits(  
        GLint hits, 
        GLuint buffer[] );

    // -- data members ----------------------------------------------
    enum
    {
    };
    
    // data
    vector< string >        attributeLabels;
    vector< int >           attributeIndcs;
    int                     maxAttrCard;
    vector< vector< int > > combinations;
    vector< int >           numberPerComb;
    int                     maxNumberPerComb;

    // bar chart
    int    minHgtHintPixBC;     // bar height cannot be less
    int    maxWthHintPixBC;     // bar width cannot be more
    double widthBC;             // actual width calculated & used for every bar
    vector< Position2D > posBC; // top, center

    // combination plot
    vector< vector< Position2D > > posLftTop;
    vector< vector< Position2D > > posRgtBot;

    bool   mouseInside;
    int    mouseCombnIdx;

  	// diagram
	Diagram*      diagram;         // association, user-defined diagram
    double        scaleDgrm;       // scale factor for diagram
    Position2D    posDgrm;         // positions of diagram
    bool          showDgrm;        // show or hide diagram
    vector< int > attrValIdcsDgrm; // value idx of attribute associated with diagram
    string        msgDgrm;         // message to show with diagram
};

#endif

// -- end -----------------------------------------------------------
