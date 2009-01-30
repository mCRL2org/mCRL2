// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./dof.h

#ifndef DOF_H
#define DOF_H

#include <cstddef>
#include <string>
#include <vector>
#include "attribute.h"

class DOF
{
public:
	// -- constructors and destructor -------------------------------
    DOF( 
        const int &idx,
        const std::string &lbl );
    DOF( const DOF &dof );
	virtual ~DOF();
	
    // -- std::set functions ---------------------------------------------
    void setIndex( const int &idx );
    void setLabel( const std::string &lbl );
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
    void setTextStatus( const int &status );
    
    // -- get functions ---------------------------------------------
    int getIndex();
    std::string getLabel();
    double getMin();
    double getMax();
    int getSizeValues();
    double getValue( const int &idx );
    void getValues( std::vector< double > &vals );
    int getDir();
    Attribute* getAttribute();
    int getTextStatus();

    // -- public constants ------------------------------------------
    enum
    {
        ID_TEXT_NONE,
        ID_TEXT_ALL,
        ID_TEXT_ATTR,
        ID_TEXT_VAL
    };
    
protected:
    // -- data members ----------------------------------------------
	int    index;    // index in attribute
    std::string label;
    /*
    double min;
    double max;
    */
    std::vector< double > values;
    int    dir;
    Attribute* attr; // association
    int    textStatus;
};

#endif

// -- end -----------------------------------------------------------
