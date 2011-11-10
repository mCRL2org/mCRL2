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

#include <map>
#include <set>
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/pbes/pbes.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

inline
std::string print_removed_equations(const atermpp::vector<propositional_variable>& removed)
{
  std::ostringstream out;
  out << "\nremoved the following equations:" << std::endl;
  for (atermpp::vector<propositional_variable>::const_iterator i = removed.begin(); i != removed.end(); ++i)
  {
    out << "  " << pbes_system::pp(*i) << std::endl;
  }
  return out.str();
}

} // namespace detail

template <typename Container>
std::set<propositional_variable> reachable_variables(const pbes<Container>& p)
{
  typedef typename Container::const_iterator iterator;

  // create a mapping from variable names to iterators
  std::map<core::identifier_string, iterator> index;
  for (iterator i = p.equations().begin(); i != p.equations().end(); ++i)
  {
    index[i->variable().name()] = i;
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
    for (std::set<propositional_variable_instantiation>::iterator i = iocc.begin(); i != iocc.end(); ++i)
    {
      if (explored.find(i->name()) == explored.end())
      {
        visited.insert(i->name());
      }
    }
  }

  std::set<propositional_variable> result;
  for (std::set<core::identifier_string>::iterator i = explored.begin(); i != explored.end(); ++i)
  {
    result.insert(index[*i]->variable());
  }
  return result;
}

/// \brief Removes equations that are not (syntactically) reachable from the initial state of a PBES.
/// \return The removed variables
template <typename Container>
atermpp::vector<propositional_variable> remove_unreachable_variables(pbes<Container>& p)
{
  atermpp::vector<propositional_variable> result;

  std::set<propositional_variable> V = reachable_variables(p);
  Container eqn;
  for (typename Container::iterator i = p.equations().begin(); i != p.equations().end(); ++i)
  {
    if (V.find(i->variable()) != V.end())
    {
      eqn.push_back(*i);
    }
    else
    {
      result.push_back(i->variable());
    }
  }
  p.equations() = eqn;
  return result;
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_REMOVE_EQUATIONS_H
