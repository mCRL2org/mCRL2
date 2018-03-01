// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/remove_equations.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_REMOVE_EQUATIONS_H
#define MCRL2_PBES_REMOVE_EQUATIONS_H

#include "mcrl2/pbes/pbes.h"
#include <map>
#include <set>

namespace mcrl2 {

namespace pbes_system {

namespace detail {

inline
std::string print_removed_equations(const std::vector<propositional_variable>& removed)
{
  std::ostringstream out;
  out << "\nremoved the following equations:" << std::endl;
  for (const propositional_variable& v: removed)
  {
    out << "  " << pbes_system::pp(v) << std::endl;
  }
  return out.str();
}

} // namespace detail

inline
std::set<propositional_variable> reachable_variables(const pbes& p)
{
  typedef std::vector<pbes_equation>::const_iterator iterator;

  // create a mapping from variable names to iterators
  std::map<core::identifier_string, iterator> index;
  for (auto i = p.equations().begin(); i != p.equations().end(); ++i)
  {
    // index[i->variable().name()] = i;  <-- This leads to a attempt to copy-
    //                                       construct an iterator from a singular iterator when the toolset
    //                                       is compiled in maintainer mode.
    index.insert(std::pair<core::identifier_string, iterator>(i->variable().name(),i));
  }

  std::set<core::identifier_string> visited;
  std::set<core::identifier_string> explored;
  visited.insert(p.initial_state().name());
  while (!visited.empty())
  {
    core::identifier_string X = *visited.begin();
    visited.erase(visited.begin());
    explored.insert(X);
    pbes_expression phi = index[X]->formula();
    std::set<propositional_variable_instantiation> iocc = pbes_system::find_propositional_variable_instantiations(phi);
    for (const propositional_variable_instantiation& i: iocc)
    {
      if (explored.find(i.name()) == explored.end())
      {
        visited.insert(i.name());
      }
    }
  }

  std::set<propositional_variable> result;
  for (const core::identifier_string& i: explored)
  {
    result.insert(index[i]->variable());
  }
  return result;
}

/// \brief Removes equations that are not (syntactically) reachable from the initial state of a PBES.
/// \return The removed variables
inline
std::vector<propositional_variable> remove_unreachable_variables(pbes& p)
{
  std::vector<propositional_variable> result;

  std::set<propositional_variable> V = reachable_variables(p);
  std::vector<pbes_equation> equations;
  for (pbes_equation& eqn: p.equations())
  {
    if (V.find(eqn.variable()) != V.end())
    {
      equations.push_back(eqn);
    }
    else
    {
      result.push_back(eqn.variable());
    }
  }
  p.equations() = equations;
  return result;
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_REMOVE_EQUATIONS_H
