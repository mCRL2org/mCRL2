// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./corrlplot.h

// --- corrlplot.h --------------------------------------------------
// (c) 2007  -  A.J. Pretorius  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------

#ifndef CORRLPLOT_H
#define CORRLPLOT_H

#include <cstddef>
#include <cstdlib>
#include <cmath>
#include <vector>
using namespace std;
#include "diagram.h"
#include "graph.h"
#include "utils.h"
#include "visualizer.h"

class CorrlPlot : public Visualizer
{
public:
    // -- constructors and destructor -------------------------------
    CorrlPlot(
        Mediator* m,
        Graph* g,
        GLCanvas* c );
    virtual ~CorrlPlot();

    // -- set data functions ----------------------------------------
    void setValues(
        const int &idx1,
        const int &idx2,
        const vector< vector< int > > &mapXY,
        const vector< vector< int > > &num );
    void clearValues();

  	void setDiagram( Diagram* dgrm );
        
    // -- visualization functions  ----------------------------------
    void visualize( const bool &inSelectMode );
    void drawAxes(
        const bool &inSelectMode,
        const string &xLbl,
        const string &yLbl );
    void drawLabels( const bool &inSelectMode );
    void drawPlot( const bool &inSelectMode );
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
    void calcMaxNumber();

    // -- utility drawing functions ---------------------------------
    // ***
    //void clear();
    void setScalingTransf();
    void displTooltip(
        const int &xIdx,
        const int &yIdx );

    void calcPositions();
    void clearPositions();

    // -- hit detection ---------------------------------------------
    void processHits( 
        GLint hits, 
        GLuint buffer[] );

    // -- data members ----------------------------------------------
    
    // data
    int attrIdx1;
    int attrIdx2;
    string xLabel;
    string yLabel;
    vector< vector< int > > mapXToY; // for each x idx, 1 or more y indices
    vector< vector< int > > number;  // number associated with every entry in mapXToY
    vector< int > maxNumX;   // for every column, the largest number 
    int sumMaxNumX;          // sum of entries in maxNumX
    vector< int > maxNumY;   // for every row, the largest number 
    int sumMaxNumY;          // sum of entries in maxNumY
    int maxNumber;           // overall largest number
    
    // vis settings
    int minRadHintPx; // min radius cannot be smaller than this
    int maxRadHintPx; // max radius cannot be smaller than this
    vector< vector< double > > radii;
    vector< vector< Position2D > > positions;

    // diagram
    Diagram*   diagram;         // association, user-defined diagram
    double     scaleDgrm;       // scale factor for diagram
    Position2D posDgrm;         // positions of diagram
    bool       showDgrm;        // show or hide diagram
    int        attrValIdx1Dgrm; // value idx of 1st attribute associated with diagram
    int        attrValIdx2Dgrm; // value idx of 2nd attribute associated with diagram
    string     msgDgrm;        // message to show with diagram
};

#endif

// -- end -----------------------------------------------------------
