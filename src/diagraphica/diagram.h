// --- diagram.h ----------------------------------------------------
// (c) 2007  -  A.J. Pretorius  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------

#ifndef DIAGRAM_H
#define DIAGRAM_H

#include <cstddef>
#include <string>
#include <vector>
using namespace std;
#include "colleague.h"
#include "glcanvas.h"
#include "visutils.h"
#include "shape.h"

class Diagram : public Colleague
{
public:
	// -- constructors and destructor -------------------------------
    Diagram( Mediator* m );
	virtual ~Diagram();
	
    // -- set functions ---------------------------------------------
    void addShape( Shape *s );
    void moveShapeToFront( const int &idx );
    void moveShapeToBack( const int &idx );
    void moveShapeForward( const int &idx );
    void moveShapeBackward( const int &idx );

    void setShowGrid( const bool &flag );
    void setSnapGrid( const bool &flag );
    
    // -- get functions ---------------------------------------------
    int getSizeShapes();
    Shape* getShape( const int &idx );

    bool getSnapGrid();
    double getGridInterval( GLCanvas* canvas );
    double getAngleInterval();

    // -- clear functions -------------------------------------------
    void deleteShape( const int &idx );

    // -- vis functions ---------------------------------------------
    void visualize( 
        const bool &inSelectMode,
        GLCanvas* canvas );
    void visualize(
        const bool &inSelectMode,
        GLCanvas* canvas,
        const vector< Attribute* > attrs,
        const vector< double > attrValIdcs );
    void visualize(
        const bool &inSelectMode,
        GLCanvas* canvas,
        const double &opacity,
        const vector< Attribute* > attrs,
        const vector< double > attrValIdcs );

protected:
    // -- private utility functions ---------------------------------
    void initGridSettings();
    
    void drawAxes( 
        const bool &inSelectMode, 
        GLCanvas* canvas );
    void drawBorder( 
        const bool &inSelectMode, 
        GLCanvas* canvas );
    void drawBorderFlush( 
        const bool &inSelectMode, 
        GLCanvas* canvas );
    void drawBorderFlush( 
        const bool &inSelectMode, 
        GLCanvas* canvas,
        const double &opacity );
    void drawGrid( 
        const bool &inSelectMode, 
        GLCanvas* canvas );
    void drawShapes( 
        const bool &inSelectMode,
        GLCanvas* canvas );
    
    // -- data members ----------------------------------------------
    vector< Shape* > shapes; // composition

    bool   showGrid;
    bool   snapGrid;
    
    // constants
    int SIZE_BORDER;
    int GRID_NUM_INTERV_HINT;
    int ANGL_NUM_INTERV_HINT;
};

#endif

// -- end -----------------------------------------------------------
