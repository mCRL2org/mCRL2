// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/absinthe_strategy.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_ABSINTHE_STRATEGY_H
#define MCRL2_PBES_ABSINTHE_STRATEGY_H

#include <iostream>
#include <string>
#include "mcrl2/utilities/exception.h"

namespace mcrl2 {

namespace pbes_system {

/// The approximation strategies of the absinthe tool.
enum absinthe_strategy
{
  absinthe_over,
  absinthe_under
};

/// \brief Parses an absinthe strategy
inline
absinthe_strategy parse_absinthe_strategy(const std::string& strategy)
{
  if (strategy == "over")
  {
    return absinthe_over;
  }
  else if (strategy == "under")
  {
    return absinthe_under;
  }
  else
  {
    throw mcrl2::runtime_error("unknown approximation strategy specified (got `" + strategy + "')");
  }
}

/// \brief Prints an absinthe strategy
inline
std::string print_absinthe_strategy(const absinthe_strategy strategy)
{
  switch (strategy)
  {
    case absinthe_over:
      return "over";
    case absinthe_under:
      return "under";
  }
  throw mcrl2::runtime_error("unknown absinthe strategy");
}

inline
std::istream& operator>>(std::istream& is, absinthe_strategy& strategy)
{
  try
  {
    std::string s;
    is >> s;
    strategy = parse_absinthe_strategy(s);
  }
  catch(mcrl2::runtime_error&)
  {
    is.setstate(std::ios_base::failbit);
  }
  return is;
}

inline
std::ostream& operator <<(std::ostream& os, const absinthe_strategy strategy)
{
  os << print_absinthe_strategy(strategy);
  return os;
}

/// \brief Prints an absinthe strategy
inline
std::string description(const absinthe_strategy strategy)
{
  switch (strategy)
  {
    case absinthe_over:
      return "an over-approximation";
    case absinthe_under:
      return "an under-approximation";
  }
  throw mcrl2::runtime_error("unknown absinthe strategy");
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_ABSINTHE_STRATEGY_H
