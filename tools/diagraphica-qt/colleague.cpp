// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./colleague.cpp

#include "wx.hpp" // precompiled headers

#include "colleague.h"


// -- contstructors and destructors ---------------------------------


Colleague::Colleague(Mediator* m)
{
  mediator = m;
}


Colleague::Colleague(const Colleague& colleague)
{
  mediator = colleague.mediator;
}


Colleague::~Colleague()
{
  mediator = NULL;
}


// -- end -----------------------------------------------------------
