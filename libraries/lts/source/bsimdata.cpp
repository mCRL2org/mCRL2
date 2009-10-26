// Author(s): Muck van Weerdenburg, Bert Lisser
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file bsimdata.cpp

#include <vector>
#include <string>
#include "mcrl2/core/aterm_ext.h"
/* #include <algorithm>
#include <boost/scoped_array.hpp>
#include "mcrl2/core/print.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/lts/detail/bsim.h"

#define ATisAppl(t) (ATgetType(t) == AT_APPL)

using namespace std;
using namespace mcrl2::lts;
using namespace mcrl2::core;
using namespace mcrl2::core::detail; */

/* Data definition */

unsigned int nlabel=0; 
ATerm *label_name; 
int label_tau = -1;

static std::vector<std::string> const*tau_actions = NULL;

void set_tau_actions(std::vector<std::string> const*actions)
{ tau_actions = actions;
}

