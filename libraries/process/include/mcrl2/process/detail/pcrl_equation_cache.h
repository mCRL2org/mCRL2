// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/detail/pcrl_equation_cache.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_DETAIL_PCRL_EQUATION_CACHE_H
#define MCRL2_PROCESS_DETAIL_PCRL_EQUATION_CACHE_H

#include "mcrl2/process/alphabet_efficient.h"
#include "mcrl2/process/process_variable_strongly_connected_components.h" // find_process_identifiers

namespace mcrl2::process::detail
{

// Returns a mapping P -> alphabet(P) for all pCRL equations P.
// TODO: This mapping can probably be computed more efficiently using the SCC decomposition of the equations.
inline
std::map<process_identifier, multi_action_name_set> compute_pcrl_equation_cache(const std::vector<process_equation>& equations)
{
  std::map<process_identifier, multi_action_name_set> result;
  for (const process_equation& eqn: equations)
  {
    if (is_pcrl(eqn.expression()))
    {
      result[eqn.identifier()] = alphabet_efficient(eqn.expression(), equations);
    }
  }
  return result;
}

// for traversal of reachable process equations starting in the initial state
struct process_traversal_algorithm
{
  const std::vector<process_equation>& equations;
  const process_expression& init;

  std::set<process_identifier> todo;
  std::set<process_identifier> done;

  process_traversal_algorithm(const std::vector<process_equation>& equations_, const process_expression& init_)
    : equations(equations_), init(init_)
  {
    todo = find_process_identifiers(init);
  }

  // returns true if there is another reachable process equation
  bool has_next()
  {
    return !todo.empty();
  }

  const process_equation& next()
  {
    assert(has_next());

    // pick an element from todo
    process_identifier id = *todo.begin();
    todo.erase(todo.begin());
    done.insert(id);

    const process_equation& eqn = find_equation(equations, id);

    // search for new identifiers in the rhs of the equation
    for (const process_identifier& id: find_process_identifiers(eqn.expression()))
    {
      if (done.find(id) == done.end())
      {
        todo.insert(id);
      }
    }

    return eqn;
  }
};

// Returns a mapping P -> alphabet(P) for all pCRL equations P that are reachable from init.
inline
std::map<process_identifier, multi_action_name_set> compute_pcrl_equation_cache(const std::vector<process_equation>& equations, const process_expression& init)
{
  // use the scc decomposition to ensure that the alphabet for equations in the same scc is computed only once
  std::vector<std::set<process_identifier> > sccs = process_variable_strongly_connected_components(equations, init);
  std::map<process_identifier, std::size_t> scc_index;
  for (std::size_t i = 0; i < sccs.size(); i++)
  {
    for (const process_identifier& id: sccs[i])
    {
      scc_index[id] = i;
    }
  }

  std::map<process_identifier, multi_action_name_set> result;
  process_traversal_algorithm algorithm(equations, init);
  while (algorithm.has_next())
  {
    const process_equation& eqn = algorithm.next();
    if (is_pcrl(eqn.expression()) && result.find(eqn.identifier()) == result.end())
    {
      multi_action_name_set A = alphabet_efficient(eqn.expression(), equations);
      const std::set<process_identifier>& scc = sccs[scc_index[eqn.identifier()]];
      for (const process_identifier& id: scc)
      {
        result[id] = A;
      }
    }
  }
  return result;
}

} // namespace mcrl2::process::detail

#endif // MCRL2_PROCESS_DETAIL_PCRL_EQUATION_CACHE_H
