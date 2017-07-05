// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_LPSSYMBOLICBISIM_TOOL_MODE_H
#define MCRL2_LPSSYMBOLICBISIM_TOOL_MODE_H

#include <iostream>
#include <string>
#include "mcrl2/utilities/exception.h"

enum running_mode
{
  partition_refinement,
  exploration,
  exploration_and_refinement,
  simplify_lps
};

inline
running_mode parse_mode(const std::string& s)
{
  if(s == "refine" || s == "r")
  {
    return partition_refinement;
  }
  else if(s == "explore" || s == "e")
  {
    return exploration;
  }
  else if(s == "explorerefine" || s == "er")
  {
    return exploration_and_refinement;
  }
  else if(s == "simplify" || s == "s")
  {
    return simplify_lps;
  }
  else
  {
    throw mcrl2::runtime_error("Unknown running mode " + s);
  }
}

// \overload
inline
std::istream& operator>>(std::istream& is, running_mode& eq)
{
  try
  {
    std::string s;
    is >> s;
    eq = parse_mode(s);
  }
  catch(mcrl2::runtime_error&)
  {
    is.setstate(std::ios_base::failbit);
  }
  return is;
}

inline std::string print_equivalence(const running_mode r)
{
  switch(r)
  {
    case partition_refinement:
      return "refine";
    case exploration:
      return "explore";
    case exploration_and_refinement:
      return "explorerefine";
    case simplify_lps:
      return "simplify";
    default:
      throw mcrl2::runtime_error("Unknown running_mode.");
  }
}

// \overload
inline
std::ostream& operator<<(std::ostream& os, const running_mode eq)
{
  os << print_equivalence(eq);
  return os;
}

/** \brief Gives a description of an equivalence.
 * \param[in] eq The equivalence type.
 * \return A string describing the equivalence specified by \a
 * eq. */
inline std::string description(const running_mode r)
{
  switch(r)
  {
    case partition_refinement:
      return "partition refinement and generation of the quotient lts";
    case exploration:
      return "forward symbolic exploration of the state space";
    case exploration_and_refinement:
      return "exploration and refinement, where the reachable state space is used as invariant";
    case simplify_lps:
      return "simplify lps";
    default:
      throw mcrl2::runtime_error("Unknown running_mode.");
  }
}

#endif // MCRL2_LPSSYMBOLICBISIM_TOOL_MODE_H



