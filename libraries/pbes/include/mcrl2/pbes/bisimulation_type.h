// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/bisimulation_type.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_BISIMULATION_TYPE_H
#define MCRL2_PBES_BISIMULATION_TYPE_H

#include <iostream>
#include <string>
#include "mcrl2/utilities/exception.h"

namespace mcrl2 {

namespace pbes_system {

/// \brief An enumerated type for the available bisimulation types
enum bisimulation_type
{
  strong_bisim,
  weak_bisim,
  branching_bisim,
  branching_bisim_gw,
  branching_bisim_gjkw,
  branching_sim
};

/// \brief Returns the string corresponding to a bisimulation type
inline
bisimulation_type parse_bisimulation_type(const std::string& type)
{
  if (type == "strong-bisim"        ) return strong_bisim;
  else if (type == "weak-bisim"     ) return weak_bisim;
  else if (type == "branching-bisim") return branching_bisim;
  else if (type == "branching-bisim-gw")    return branching_bisim_gw;
  else if (type == "branching-bisim-gjkw")  return branching_bisim_gjkw;
  else if (type == "branching-sim"  ) return branching_sim;
  throw mcrl2::runtime_error(std::string("unknown bisimulation type ") + type + "!");
  return strong_bisim;
}

/// \brief Returns a description of a bisimulation type
inline
std::string print_bisimulation_type(const bisimulation_type t)
{
  switch (t)
  {
    case strong_bisim:
      return "strong-bisim";
    case weak_bisim:
      return "weak-bisim";
    case branching_bisim:
      return "branching-bisim";
    case branching_bisim_gw:
      return "branching-bisim-gw";
    case branching_bisim_gjkw:
      return "branching-bisim-gjkw";
    case branching_sim:
      return "branching-sim";
  }
  throw mcrl2::runtime_error("unknown type");
}

/// \brief Returns a description of a bisimulation type
inline
std::string description(const bisimulation_type t)
{
  switch (t)
  {
    case strong_bisim:
      return "strong bisimulation";
    case weak_bisim:
      return "weak bisimulation";
    case branching_bisim:
      return "branching bisimulation";
    case branching_bisim_gw:
      return "almost-O(m log n) branching bisimulation [GW]";
    case branching_bisim_gjkw:
      return "O(m log n) branching bisimulation [GJKW]";
    case branching_sim:
      return "branching simulation equivalence";
  }
  throw mcrl2::runtime_error("unknown type");
}

inline
std::istream& operator>>(std::istream& is, bisimulation_type& t)
{
  try
  {
    std::string s;
    is >> s;
    t = parse_bisimulation_type(s);
  }
  catch (mcrl2::runtime_error&)
  {
    is.setstate(std::ios_base::failbit);
  }
  return is;
}

inline
std::ostream& operator<<(std::ostream& os, const bisimulation_type t)
{
  os << print_bisimulation_type(t);
  return os;
}


} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_BISIMULATION_TYPE_H
