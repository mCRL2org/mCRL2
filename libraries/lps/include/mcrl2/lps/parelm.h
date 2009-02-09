// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/parelm.h
/// \brief The parelm algorithm.

#ifndef MCRL2_LPS_PARELM_H
#define MCRL2_LPS_PARELM_H

#include <iterator>
#include <algorithm>
#include <map>
#include <set>
#include <vector>
#include <boost/graph/adjacency_list.hpp>
#include <boost/integer.hpp>
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/core/reachable_nodes.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/detail/iota.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/detail/data_assignment_functional.h"
#include "mcrl2/data/detail/sorted_sequence_algorithm.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/detail/remove_parameters.h"

namespace mcrl2 {

namespace lps {

/// \brief Returns the data variables that are considered in the parelm algorithm.
/// \param first Start of a sequence of summands
/// \param last End of a sequence of summands
/// \return The data variables that appear in the condition, action or time of the summands in the sequence [first, last).
template <typename Iterator>
std::set<data::data_variable> transition_variables(Iterator first, Iterator last)
{
  std::set<data::data_variable> result;
  for (Iterator i = first; i != last; ++i)
  {
    if (i->is_delta())
    {
      continue;
    }
    atermpp::find_all_if(i->condition(), data::is_data_variable, std::inserter(result, result.end()));
    atermpp::find_all_if(i->actions(), data::is_data_variable, std::inserter(result, result.end()));
    if (i->has_time())
    {
      atermpp::find_all_if(i->time(), data::is_data_variable, std::inserter(result, result.end()));
    }
  }
  return result;
}

/// \brief Returns a set of insignificant process parameters that may be eliminated from p.
/// \param p A linear process
/// \return A set of insignificant process parameters
inline
std::set<data::data_variable> compute_insignificant_parameters(const linear_process& p)
{
  std::set<data::data_variable> process_parameters(p.process_parameters().begin(), p.process_parameters().end());

  // significant variables may not be removed by parelm
  std::set<data::data_variable> significant_variables = transition_variables(p.summands().begin(), p.summands().end());

  // recursively extend the set of significant variables
  std::set<data::data_variable> todo = significant_variables;
  while (!todo.empty())
  {
    data::data_variable x = *todo.begin();
    todo.erase(todo.begin());

    for (summand_list::iterator i = p.summands().begin(); i != p.summands().end(); ++i)
    {
      if (!i->is_delta())
      {
        data::data_assignment_list::iterator j = std::find_if(i->assignments().begin(), i->assignments().end(), data::detail::has_left_hand_side(x));
        if (j != i->assignments().end())
        {
          std::set<data::data_variable> new_variables = data::detail::set_difference(data::find_all_data_variables(j->rhs()), significant_variables);
          todo.insert(new_variables.begin(), new_variables.end());
          significant_variables.insert(new_variables.begin(), new_variables.end());
        }
      }
    }
  }
  return data::detail::set_difference(process_parameters, significant_variables);
}

/// \brief Removes zero or more insignificant parameters from the specification spec.
/// \param spec A linear process specification
/// \return The transformed specification
inline
specification parelm(const specification& spec)
{
  std::set<data::data_variable> to_be_removed = compute_insignificant_parameters(spec.process());

  // logging statements
  mcrl2::core::gsVerboseMsg("parelm removed %d process parameters: ", to_be_removed.size());
  for (std::set<data::data_variable>::iterator i = to_be_removed.begin(); i != to_be_removed.end(); ++i)
  {
    mcrl2::core::gsVerboseMsg("%s:%s ", mcrl2::core::pp(*i).c_str(), mcrl2::core::pp(i->sort()).c_str());
  }
  mcrl2::core::gsVerboseMsg("\n");

  specification result = detail::remove_parameters(spec, to_be_removed);
  assert(result.is_well_typed());
  return result;
}

/// \brief Removes zero or more insignificant parameters from the specification spec.
/// \param spec A linear process specification
/// \return The transformed specification
inline
specification parelm2(const specification& spec)
{
  std::vector<data::data_variable> process_parameters(spec.process().process_parameters().begin(), spec.process().process_parameters().end());

  // create a mapping m from process parameters to integers
  std::map<data::data_variable, int> m;
  int index = 0;
  for (std::vector<data::data_variable>::iterator i = process_parameters.begin(); i != process_parameters.end(); ++i)
  {
    m[*i] = index++;
  }

  // compute the initial set v of significant variables
  std::set<data::data_variable> tvars = transition_variables(spec.process().summands().begin(), spec.process().summands().end());
  std::vector<int> v;
  for (std::set<data::data_variable>::iterator i = tvars.begin(); i != tvars.end(); ++i)
  {
    v.push_back(m[*i]);
  }

  // compute the dependency graph G
  typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS> graph;
  typedef boost::graph_traits<graph>::vertex_descriptor vertex_descriptor;
  graph G(process_parameters.size());
  for (summand_list::iterator i = spec.process().summands().begin(); i != spec.process().summands().end(); ++i)
  {
    if (!i->is_delta())
    {
      for (data::data_assignment_list::iterator j = i->assignments().begin(); j != i->assignments().end(); ++j)
      {
        int j0 = m[j->lhs()];
        std::set<data::data_variable> vars = data::find_all_data_variables(j->rhs());
        for (std::set<data::data_variable>::iterator k = vars.begin(); k != vars.end(); ++k)
        {
          int k0 = m[*k];
          boost::add_edge(j0, k0, G);
        }
      }
    }
  }

  // compute the reachable nodes (i.e. the significant parameters)
  std::vector<int> r = mcrl2::core::reachable_nodes(G, v.begin(), v.end());
  std::sort(r.begin(), r.end());
  std::vector<int> q(process_parameters.size());
  core::detail::iota(q.begin(), q.end(), 0);
  std::vector<int> s;
  std::set_difference(q.begin(), q.end(), r.begin(), r.end(), std::back_inserter(s));
  std::set<data::data_variable> to_be_removed;
  for (std::vector<int>::iterator i = s.begin(); i != s.end(); ++i)
  {
    to_be_removed.insert(process_parameters[*i]);
  }

  // logging statements
  mcrl2::core::gsVerboseMsg("parelm removed %d process parameters: ", to_be_removed.size());
  for (std::set<data::data_variable>::iterator i = to_be_removed.begin(); i != to_be_removed.end(); ++i)
  {
    mcrl2::core::gsVerboseMsg("%s:%s ", mcrl2::core::pp(*i).c_str(), mcrl2::core::pp(i->sort()).c_str());
  }
  mcrl2::core::gsVerboseMsg("\n");

  specification result = detail::remove_parameters(spec, to_be_removed);
  assert(result.is_well_typed());
  return result;
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_PARELM_H
