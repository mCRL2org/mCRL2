// --- colleague.h --------------------------------------------------
// (c) 2006  -  A.J. Pretorius  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------

// ------------------------------------------------------------------
// This is the BASE CLASS from which all colleages inheret and 
// provides acces to a single Mediator. This allows for the 
// implementation of a MEDIATOR design pattern where an instance of
// Mediator serves as the mediator or controller.
// ------------------------------------------------------------------

#ifndef COLLEAGUE_H
#define COLLEAGUE_H

#include <cstddef>
#include <cstdlib>
#include "mediator.h"

class Colleague
{
public:
    // -- contstructors and destructors -----------------------------
    Colleague( Mediator* m );
    // -- !declaring destructor as virtual causes access violation! -
    ~Colleague();
    
protected:
    // -- data members ----------------------------------------------
    Mediator* mediator;     // association
};

#endif

// -- end -----------------------------------------------------------
