// --- colleague.cpp ------------------------------------------------
// (c) 2007  -  A.J. Pretorius  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------


#include "colleague.h"


// -- contstructors and destructors ---------------------------------


// --------------------------------
Colleague::Colleague( Mediator* m )
// --------------------------------
{
    mediator = m;
}


// -----------------------------------------------
Colleague::Colleague( const Colleague &colleague )
// -----------------------------------------------
{
    mediator = colleague.mediator;
}


// --------------------
Colleague::~Colleague()
// --------------------
{
    mediator = NULL;
}


// -- end -----------------------------------------------------------
