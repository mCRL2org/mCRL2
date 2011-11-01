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

/// Sets the transformation strategy.
/// \param s A transformation strategy.
pbesinst_strategy parse_transformation_strategy(const std::string& s)
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
    throw mcrl2::runtime_error("unknown output strategy specified (got `" + s + "')");
  }
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBESINST_STRATEGY_H
