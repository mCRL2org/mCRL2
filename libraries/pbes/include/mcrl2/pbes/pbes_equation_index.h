// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbes_equation_index.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_PBES_EQUATION_INDEX_H
#define MCRL2_PBES_PBES_EQUATION_INDEX_H

#include <map>
#include "mcrl2/pbes/pbes.h"

namespace mcrl2 {

namespace pbes_system {

struct pbes_equation_index
{
  // maps the name of an equation to the pair (i, k) with i the corresponding index of the equation, and k the rank
  std::unordered_map<core::identifier_string, std::pair<std::size_t, std::size_t> > equation_index;

  pbes_equation_index()
  { }

  pbes_equation_index(const pbes& p)
  {
    auto const& equations = p.equations();
    std::size_t rank;
    for (std::size_t i = 0; i < equations.size(); i++)
    {
      const auto& eqn = equations[i];
      if (i == 0)
      {
        rank = equations.front().symbol().is_mu() ? 1 : 0;
      }
      else
      {
        if (equations[i - 1].symbol() != equations[i].symbol())
        {
          rank++;
        }
      }
      equation_index.insert({eqn.variable().name(), std::make_pair(i, rank)});
    }
  }

  /// \brief Returns the index of the equation of the variable with the given name
  std::size_t index(const core::identifier_string& name) const
  {
    auto i = equation_index.find(name);
    assert (i != equation_index.end());
    return i->second.first;
  }

  /// \brief Returns the rank of the equation of the variable with the given name
  std::size_t rank(const core::identifier_string& name) const
  {
    auto i = equation_index.find(name);
    assert (i != equation_index.end());
    return i->second.second;
  }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBES_EQUATION_INDEX_H
