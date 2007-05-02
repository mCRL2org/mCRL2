// --- dof.h --------------------------------------------------------
// (c) 2006  -  A.J. Pretorius  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------

#ifndef DOF_H
#define DOF_H

#include <cstddef>
#include <string>
#include <set>
#include <vector>
using namespace std;
#include "attribute.h"

class DOF
{
public:
	// -- constructors and destructor -------------------------------
    DOF( 
        const int &idx,
        const string &lbl );
    DOF( const DOF &dof );
	virtual ~DOF();
	
    // -- set functions ---------------------------------------------
    void setIndex( const int &idx );
    void setLabel( const string &lbl );
    void setMin( const double &m );
    void setMax( const double &m );
    void setMinMax( const double &mn, const double &mx );
    void setValue( 
        const int &idx,
        const double &val );
    void addValue( const double &val );
    void clearValue( const int &idx );
    void setDir( const int &dr );
    void setAttribute( Attribute* a );
    
    // -- get functions ---------------------------------------------
    int getIndex();
    string getLabel();
    double getMin();
    double getMax();
    int getSizeValues();
    double getValue( const int &idx );
    void getValues( vector< double > &vals );
    int getDir();
    Attribute* getAttribute();
    
protected:
    // -- data members ----------------------------------------------
	int    index;    // index in attribute
    string label;
    /*
    double min;
    double max;
    */
    vector< double > values;
    int    dir;
    Attribute* attr; // association
};

#endif

// -- end -----------------------------------------------------------
