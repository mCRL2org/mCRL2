// Author(s): Jan Friso Groote, Xiao Qi
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file search_strategy.h
/// \brief Search strategy when generating a BES from a PBES.

#ifndef MCRL2_PBES_SEARCH_STRATEGY_H
#define MCRL2_PBES_SEARCH_STRATEGY_H

#include "mcrl2/utilities/exception.h"
#include <string>

namespace mcrl2::pbes_system
{

/// \brief Search strategy when generating a BES from a PBES.
enum class search_strategy
{
  breadth_first, // Generate the rhs of the last generated BES variable last.
  depth_first,   // Generate the rhs of the last generated BES variable first.
  breadth_first_short,
  depth_first_short
};

inline
search_strategy parse_search_strategy(const std::string& s)
{
  if (s == "breadth-first")
  {
    return search_strategy::breadth_first;
  }
  else if (s == "b")
  {
    return search_strategy::breadth_first_short;
  }
  else if (s == "depth-first")
  {
    return search_strategy::depth_first;
  }
  else if (s == "d")
  {
    return search_strategy::depth_first_short;
  }
  else
  {
    throw mcrl2::runtime_error("unknown search strategy " + s);
  }
}

inline
std::string print_search_strategy(const search_strategy s)
{
  switch(s)
  {
    case search_strategy::breadth_first: return "breadth-first";
    case search_strategy::depth_first: return "depth-first";
    case search_strategy::breadth_first_short: return "b";
    case search_strategy::depth_first_short: return "d";
  }
  throw mcrl2::runtime_error("unknown search strategy");
}

inline
std::istream& operator>>(std::istream& is, search_strategy& strategy)
{
  try
  {
    std::string s;
    is >> s;
    strategy = parse_search_strategy(s);
  }
  catch(mcrl2::runtime_error&)
  {
    is.setstate(std::ios_base::failbit);
  }
  return is;
}

inline
std::ostream& operator<<(std::ostream& os, const search_strategy s)
{
  os << print_search_strategy(s);
  return os;
}

inline
std::string description(const search_strategy s)
{
  switch(s)
  {
    case search_strategy::breadth_first: return "Compute the right hand side of the boolean variables"
        " in a first come first served basis. This is comparable with a breadth-first search."
        " This is good for generating counter examples. ";
    case search_strategy::depth_first: return "Compute the right hand side of a boolean variables where "
        " the last generated variable is investigated first. This corresponds to a depth-first "
        " search. This can substantially outperform breadth-first search when the validity of a"
        " formula is determined at a larger depth. ";
    case search_strategy::breadth_first_short: return "Shorthand for breadth-first.";
    case search_strategy::depth_first_short: return "Shorthand for depth-first.";
  }
  throw mcrl2::runtime_error("unknown search strategy");
}

} // namespace mcrl2::pbes_system



#endif // MCRL2_PBES_SEARCH_STRATEGY_H
