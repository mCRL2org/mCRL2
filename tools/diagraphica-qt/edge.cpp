// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./edge.cpp

#include "wx.hpp" // precompiled headers

#include "edge.h"


// -- constructors and destructor -----------------------------------

using namespace std;

Edge::Edge(
  const size_t& idx,
  const string& lbl)
{
  index   = idx;
  label   = lbl;
  inNode  = NULL;
  outNode = NULL;
  bundle  = NULL;
}


Edge::~Edge()
{
  clearInNode();
  clearOutNode();
  clearBundle();
}

// -- end -----------------------------------------------------------
