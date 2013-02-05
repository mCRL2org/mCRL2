// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/control_flow_utility.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_CONTROL_FLOW_UTILITY_H
#define MCRL2_PBES_DETAIL_CONTROL_FLOW_UTILITY_H

#include "mcrl2/data/detail/simplify_rewrite_builder.h"
#include "mcrl2/pbes/detail/simplify_quantifier_builder.h"
#include "mcrl2/pbes/detail/pfnf_pbes.h"
#include "mcrl2/pbes/detail/stategraph_utility.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

inline
std::vector<pfnf_equation>::const_iterator find_equation(const pfnf_pbes& p, const core::identifier_string& X)
{
  const std::vector<pfnf_equation>& equations = p.equations();
  for (std::vector<pfnf_equation>::const_iterator i = equations.begin(); i != equations.end(); ++i)
  {
    if (i->variable().name() == X)
    {
      return i;
    }
  }
  return equations.end();
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_CONTROL_FLOW_UTILITY_H
