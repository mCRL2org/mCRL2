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
#include "mcrl2/new_data/utility.h"
#include "mcrl2/new_data/detail/assignment_functional.h"
#include "mcrl2/new_data/detail/sorted_sequence_algorithm.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/detail/remove_parameters.h"

namespace mcrl2 {

namespace lps {

/// \brief Returns the data variables that are considered in the parelm algorithm.
/// \param first Start of a sequence of summands
/// \param last End of a sequence of summands
/// \return The data variables that appear in the condition, action or time of the summands in the sequence [first, last).
template <typename Iterator>
std::set<new_data::variable> transition_variables(Iterator first, Iterator last)
{
  struct local {
    static bool is_variable(atermpp::aterm p) {
      return new_data::data_expression(p).is_variable();
    }
  };

  std::set<new_data::variable> result;
  for (Iterator i = first; i != last; ++i)
  {
    //if (i->is_delta())
    //{
    //  continue;
    //}
    atermpp::find_all_if(i->condition(), local::is_variable, std::inserter(result, result.end()));
    atermpp::find_all_if(i->actions(), local::is_variable, std::inserter(result, result.end()));
    if (i->has_time())
    {
      atermpp::find_all_if(i->time(), local::is_variable, std::inserter(result, result.end()));
    }
  }
  return result;
}

/// \brief Returns a set of insignificant process parameters that may be eliminated from p.
/// \param p A linear process
/// \return A set of insignificant process parameters
inline                                                              
std::set<new_data::variable> compute_insignificant_parameters(const linear_process& p)
{
  new_data::variable_list      process_parameter_list(p.process_parameters());

  std::set<new_data::variable> process_parameters(process_parameter_list.begin(), process_parameter_list.end());

  summand_list summands(p.summands());

  // significant variables may not be removed by parelm
  std::set<new_data::variable> significant_variables = transition_variables(summands.begin(), summands.end());

#ifdef MCRL2_LPS_PARELM_DEBUG
  std::clog << "<todo list>";
  for (std::set<data::data_variable>::iterator i = significant_variables.begin(); i != significant_variables.end(); ++i)
  {
    std::clog << core::pp(*i) << " ";
  }
  std::clog << std::endl;
#endif

  // recursively extend the set of significant variables
  std::set<new_data::variable> todo = significant_variables;
  while (!todo.empty())
  {
    new_data::variable x = *todo.begin();
    todo.erase(todo.begin());

#ifdef MCRL2_LPS_PARELM_DEBUG
    std::clog << "<handling todo element>" << core::pp(x) << std::endl;
#endif

    for (summand_list::iterator i = summands.begin(); i != summands.end(); ++i)
    {
      if (!i->is_delta())
      {
        new_data::assignment_list assignments(i->assignments());
        new_data::assignment_list::iterator j = std::find_if(assignments.begin(), assignments.end(), new_data::detail::has_left_hand_side(x));
        if (j != assignments.end())
        {
          std::set<new_data::variable> new_variables = new_data::detail::set_difference(new_data::find_all_variables(j->rhs()), significant_variables);
          todo.insert(new_variables.begin(), new_variables.end());
          significant_variables.insert(new_variables.begin(), new_variables.end());
        }
      }
    }
  }
  return new_data::detail::set_difference(process_parameters, significant_variables);
}

/// \brief Removes zero or more insignificant parameters from the specification spec.
/// \param spec A linear process specification
/// \return The transformed specification
inline
specification parelm(const specification& spec)
{
  std::set<new_data::variable> to_be_removed = compute_insignificant_parameters(spec.process());

  // logging statements
  mcrl2::core::gsVerboseMsg("parelm removed %d process parameters: ", to_be_removed.size());
  for (std::set<new_data::variable>::iterator i = to_be_removed.begin(); i != to_be_removed.end(); ++i)
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
  new_data::variable_vector process_parameters(new_data::make_variable_vector(spec.process().process_parameters()));

  // create a mapping m from process parameters to integers
  std::map<new_data::variable, int> m;
  int index = 0;
  for (new_data::variable_vector::const_iterator i = process_parameters.begin(); i != process_parameters.end(); ++i)
  {
    m[*i] = index++;
  }

  summand_list summands(spec.process().summands());

  // compute the initial set v of significant variables
  std::set<new_data::variable> tvars = transition_variables(summands.begin(), summands.end());
  std::vector<int> v;
  for (std::set<new_data::variable>::iterator i = tvars.begin(); i != tvars.end(); ++i)
  {
    v.push_back(m[*i]);
  }

  // compute the dependency graph G
  typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS> graph;
  typedef boost::graph_traits<graph>::vertex_descriptor vertex_descriptor;
  graph G(process_parameters.size());
  for (summand_list::iterator i = summands.begin(); i != summands.end(); ++i)
  {
    if (!i->is_delta())
    {
      new_data::assignment_list assignments(i->assignments());

      for (new_data::assignment_list::iterator j = assignments.begin(); j != assignments.end(); ++j)
      {
        int j0 = m[j->lhs()];
        std::set<new_data::variable> vars = new_data::find_all_variables(j->rhs());
        for (std::set<new_data::variable>::iterator k = vars.begin(); k != vars.end(); ++k)
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
  std::set<new_data::variable> to_be_removed;
  for (std::vector<int>::iterator i = s.begin(); i != s.end(); ++i)
  {
    to_be_removed.insert(process_parameters[*i]);
  }

  // logging statements
  mcrl2::core::gsVerboseMsg("parelm removed %d process parameters: ", to_be_removed.size());
  for (std::set<new_data::variable>::iterator i = to_be_removed.begin(); i != to_be_removed.end(); ++i)
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
