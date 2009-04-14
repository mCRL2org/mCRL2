// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./opacitychooser.h

#ifndef OPACITYCHOOSER_H
#define OPACITYCHOOSER_H

#include <cstddef>
#include <cstdlib>
#include <cmath>
#include <vector>
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
        const std::vector< double > &opa,
        const std::vector< double > &y );
    
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
    void handleHits( const std::vector< int > &ids );
    void handleDrag();

    // -- hit detection ---------------------------------------------
    void processHits( 
        GLint hits, 
        GLuint buffer[] );

    // -- data members ----------------------------------------------
    bool active;
    
    std::vector< double > positionsX;
    std::vector< double > positionsY;
    int dragIdx;

    // vis settings
    double hdlSize;
    /*
    std::vector< Position2D > positions;
    */
};

#endif

// -- end -----------------------------------------------------------
