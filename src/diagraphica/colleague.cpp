// --- colleague.cpp ------------------------------------------------
// (c) 2006  -  A.J. Pretorius  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------


#include "colleague.h"


// -- contstructors and destructors ---------------------------------


// --------------------------------
Colleague::Colleague( Mediator* m )
// --------------------------------
{
    mediator = m;
}


// --------------------
Colleague::~Colleague()
// --------------------
{
    mediator = NULL;
}


// -- end -----------------------------------------------------------
