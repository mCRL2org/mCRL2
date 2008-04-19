// Author(s): Muck van Weerdenburg
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts_eq.cpp

#include <vector>
#include <set>
#include <stack>
#include <string>
#include "mcrl2/core/messaging.h"
#include "mcrl2/lts/liblts.h"
#include "mcrl2/lts/detail/liblts_bisim.h"

using namespace std;
using namespace mcrl2::core;

namespace mcrl2
{
namespace lts
{

lts_eq_options::lts_eq_options()
{
  reduce.add_class_to_state = false;
  reduce.tau_actions.clear();
}

lts_eq_options lts_eq_no_options = lts_eq_options();

void lts_reduce_add_tau_actions(lts_eq_options& opts, std::string const& act_names)
{
  string::size_type lastpos = 0, pos;
  while ( (pos = act_names.find(',',lastpos)) != string::npos )
  {
    opts.reduce.tau_actions.push_back(act_names.substr(lastpos,pos-lastpos));
    lastpos = pos+1;
  }
  opts.reduce.tau_actions.push_back(act_names.substr(lastpos));
}

bool lts::reduce(lts_equivalence eq, lts_eq_options const&opts)
{
  switch ( eq )
  {
    case lts_eq_none:
    case lts_eq_isomorph:
      return true;
    case lts_eq_strong:
      return bisimulation_reduce(*this,false,opts.reduce.add_class_to_state,&opts.reduce.tau_actions);
    case lts_eq_branch:
      return bisimulation_reduce(*this,true,opts.reduce.add_class_to_state,&opts.reduce.tau_actions);
    case lts_eq_trace:
      bisimulation_reduce(*this,false);
      determinise();
      return bisimulation_reduce(*this,false);
    case lts_eq_weak_trace:
      bisimulation_reduce(*this,true,false,&opts.reduce.tau_actions);
      tau_star_reduce();
      bisimulation_reduce(*this,false);
      determinise();
      return bisimulation_reduce(*this,false);
    default:
      return false;
  }
}
 
bool lts::compare(lts &l, lts_equivalence eq, lts_eq_options const&opts)
{
  switch ( eq )
  {
    case lts_eq_none:
      return false;
    case lts_eq_strong:
      return bisimulation_compare(*this,l,false,&opts.reduce.tau_actions);
    case lts_eq_branch:
      return bisimulation_compare(*this,l,true,&opts.reduce.tau_actions);
    case lts_eq_trace:
    case lts_eq_weak_trace:
    case lts_eq_isomorph:
    default:
      gsErrorMsg("comparision for this equivalence is not yet implemented\n");
      return false;
  }
}
 
}
}
