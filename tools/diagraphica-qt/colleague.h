// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./colleague.h

// ------------------------------------------------------------------
// This is the BASE CLASS from which all colleagues inherit and
// provides access to a single Mediator. This allows for the
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
    // -- constructors and destructors -----------------------------
    Colleague(Mediator* m);
    Colleague(const Colleague& colleague);
    virtual ~Colleague();

  protected:
    // -- data members ----------------------------------------------
    Mediator* mediator;     // association
};

#endif

// -- end -----------------------------------------------------------
