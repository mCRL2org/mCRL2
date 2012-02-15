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
#include "mcrl2/exception.h"

namespace mcrl2 {

namespace pbes_system {

/// \brief An enumerated type for the available bisimulation types
enum bisimulation_type
{
  strong_bisim,
  weak_bisim,
  branching_bisim,
  branching_sim
};

/// \brief Returns the string corresponding to a bisimulation type
inline
bisimulation_type parse_bisimulation_type(const std::string& type)
{
  if (type == "strong-bisim"        ) return strong_bisim;
  else if (type == "weak-bisim"     ) return weak_bisim;
  else if (type == "branching-bisim") return branching_bisim;
  else if (type == "branching-sim"  ) return branching_sim;
  throw mcrl2::runtime_error(std::string("unknown bisimulation type ") + type + "!");
  return strong_bisim;
}

/// \brief Returns a description of a bisimulation type
inline
std::string print_bisimulation_type(int type)
{
  switch (type)
  {
    case strong_bisim:
      return "strong bisimulation";
    case weak_bisim:
      return "weak bisimulation";
    case branching_bisim:
      return "branching bisimulation";
    case branching_sim:
      return "branching simulation equivalence";
  }
  return "unknown type";
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_BISIMULATION_TYPE_H
