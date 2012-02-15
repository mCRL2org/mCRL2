// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbesinst_strategy.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_PBESINST_STRATEGY_H
#define MCRL2_PBES_PBESINST_STRATEGY_H

#include <string>
#include "mcrl2/exception.h"

namespace mcrl2 {

namespace pbes_system {

/// \brief pbesinst transformation strategies
enum pbesinst_strategy
{
  pbesinst_lazy,
  pbesinst_finite
};

/// \brief Parse a pbesinst transformation strategy.
inline
pbesinst_strategy parse_pbesinst_strategy(const std::string& s)
{
  if (s == "finite")
  {
    return pbesinst_finite;
  }
  else if (s == "lazy")
  {
    return pbesinst_lazy;
  }
  else
  {
    throw mcrl2::runtime_error("unknown pbesinst strategy specified (got `" + s + "')");
  }
}

/// \brief Returns a string representation of a pbesinst transformation strategy.
inline
std::string print_pbesinst_strategy(pbesinst_strategy strategy)
{
  if (strategy == pbesinst_finite)
  {
    return "finite";
  }
  else if (strategy == pbesinst_lazy)
  {
    return "lazy";
  }
  return "unknown";
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBESINST_STRATEGY_H
