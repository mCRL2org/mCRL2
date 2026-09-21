// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/bisimulation_type.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_BISIMULATION_TYPE_H
#define MCRL2_PBES_BISIMULATION_TYPE_H

#include "mcrl2/utilities/exception.h"
#include <string>

namespace mcrl2::pbes_system {

/// \brief An enumerated type for the available bisimulation types
enum class bisimulation_type
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
  if (type == "strong-bisim")
  {
    return bisimulation_type::strong_bisim;
  }
  else if (type == "weak-bisim")
  {
    return bisimulation_type::weak_bisim;
  }
  else if (type == "branching-bisim")
  {
    return bisimulation_type::branching_bisim;
  }
  else if (type == "branching-sim")
  {
    return bisimulation_type::branching_sim;
  }
  throw mcrl2::runtime_error(std::string("unknown bisimulation type ") + type + "!");
  return bisimulation_type::strong_bisim;
}

/// \brief Returns a description of a bisimulation type
inline
std::string print_bisimulation_type(const bisimulation_type t)
{
  switch (t)
  {
    case bisimulation_type::strong_bisim:
      return "strong-bisim";
    case bisimulation_type::weak_bisim:
      return "weak-bisim";
    case bisimulation_type::branching_bisim:
      return "branching-bisim";
    case bisimulation_type::branching_sim:
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
    case bisimulation_type::strong_bisim:
      return "strong bisimulation";
    case bisimulation_type::weak_bisim:
      return "weak bisimulation";
    case bisimulation_type::branching_bisim:
      return "branching bisimulation";
    case bisimulation_type::branching_sim:
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


} // namespace mcrl2::pbes_system



#endif // MCRL2_PBES_BISIMULATION_TYPE_H
