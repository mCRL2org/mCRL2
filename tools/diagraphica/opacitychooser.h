//  Author(s): A.J. (Hannes) Pretorius
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  LICENSE_1_0.txt)
//
/// \file ./opacitychooser.h

// --- opacitychooser.h ---------------------------------------------
// (c) 2007  -  A.J. Pretorius  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------

#ifndef OPACITYCHOOSER_H
#define OPACITYCHOOSER_H

#include <cstddef>
#include <cstdlib>
#include <cmath>
#include <vector>
using namespace std;
#include "graph.h"
#include "utils.h"
#include "visualizer.h"

class OpacityChooser : public Visualizer
{
public:
    // -- constructors and destructor -------------------------------
    OpacityChooser(
        Mediator* m,
        Graph* g,
        GLCanvas* c );
    virtual ~OpacityChooser();

    // -- set functions ---------------------------------------------
    void setActive( const bool &flag );
    void setPoints(
        const vector< double > &opa,
        const vector< double > &y );
    
    // -- visualization functions  ----------------------------------
    void visualize( const bool &inSelectMode );
    
    // -- event handlers --------------------------------------------
    void handleMouseLftDownEvent( 
        const int &x, 
        const int &y );
    void handleMouseLftUpEvent( 
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

    // -- public constants ------------------------------------------
    static double hdlSzeHnt;
    
protected:
    // -- utility drawing functions ---------------------------------
    // ***
    //void clear();
    void drawGrayScale();
    void drawPath( const bool &inSelectMode );
    void drawPoints( const bool &inSelectMode );

    // -- utility event handlers ------------------------------------
    void handleHits( const vector< int > &ids );
    void handleDrag();

    // -- hit detection ---------------------------------------------
    void processHits( 
        GLint hits, 
        GLuint buffer[] );

    // -- data members ----------------------------------------------
    bool active;
    
    vector< double > positionsX;
    vector< double > positionsY;
    int dragIdx;

    // vis settings
    double hdlSize;
    /*
    vector< Position2D > positions;
    */
};

#endif

// -- end -----------------------------------------------------------
