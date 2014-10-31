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

#include <algorithm>
#include <map>
#include <set>
#include <vector>
#include <boost/graph/adjacency_list.hpp>
#include <boost/integer.hpp>
#include "mcrl2/utilities/reachable_nodes.h"
#include "mcrl2/utilities/detail/iota.h"
#include "mcrl2/data/print.h"
#include "mcrl2/data/detail/assignment_functional.h"
#include "mcrl2/data/detail/sorted_sequence_algorithm.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/stochastic_specification.h"
#include "mcrl2/lps/detail/lps_algorithm.h"

namespace mcrl2
{

namespace lps
{

namespace detail
{
  inline
  void collect_transition_variables(const action_summand& s, std::set<data::variable>& result)
  {
    std::set<data::variable> tmp;

    tmp = data::find_all_variables(s.condition());
    result.insert(tmp.begin(), tmp.end());

    tmp = lps::find_all_variables(s.multi_action());
    result.insert(tmp.begin(), tmp.end());
  }

  inline
  void collect_transition_variables(const stochastic_action_summand& s, std::set<data::variable>& result)
  {
    collect_transition_variables(static_cast<const action_summand&>(s), result);
    std::set<data::variable> tmp = lps::find_all_variables(s.distribution());
    result.insert(tmp.begin(), tmp.end());
  }

  inline
  void collect_transition_variables(const deadlock_summand& s, std::set<data::variable>& result)
  {
    std::set<data::variable> tmp;

    tmp = data::find_all_variables(s.condition());
    result.insert(tmp.begin(), tmp.end());

    tmp = lps::find_all_variables(s.deadlock());
    result.insert(tmp.begin(), tmp.end());
  }

} // namespace detail

/// \brief Algorithm class for elimination of unused parameters from a linear
/// process specification
template <typename Specification>
class parelm_algorithm: public lps::detail::lps_algorithm<Specification>
{
  typedef typename lps::detail::lps_algorithm<Specification> super;
  using super::m_spec;
  using super::verbose;

  protected:

    /// \brief Returns the data variables that are considered in the parelm algorithm.
    /// \return The data variables that appear in the condition, action or time of the summands
    std::set<data::variable> transition_variables()
    {
      std::set<data::variable> result;

      auto const& action_summands = m_spec.process().action_summands();
      for (auto i = action_summands.begin(); i != action_summands.end(); ++i)
      {
        detail::collect_transition_variables(*i, result);
      }

      auto const& deadlock_summands = m_spec.process().deadlock_summands();
      for (auto i = deadlock_summands.begin(); i != deadlock_summands.end(); ++i)
      {
        detail::collect_transition_variables(*i, result);
      }
      return result;
    }

    void report_results(const std::set<data::variable>& to_be_removed) const
    {
      if (verbose())
      {
        std::clog << "parelm removed " << to_be_removed.size() << " process parameters: " <<std::endl;
        for (auto i = to_be_removed.begin(); i != to_be_removed.end(); ++i)
        {
          std::clog << *i << ":" << i->sort() << std::endl;
        }
      }
    }

    /// \brief First version of parelm1
    void parelm1()
    {
#ifdef MCRL2_LPS_PARELM_DEBUG
      std::clog << "--- parelm 1 ---" << std::endl;
#endif
      const data::variable_list& pars = m_spec.process().process_parameters();
      std::set<data::variable> process_parameters(pars.begin(),pars.end());

      // significant variables may not be removed by parelm
      std::set<data::variable> significant_variables = transition_variables();

#ifdef MCRL2_LPS_PARELM_DEBUG
      std::clog << "initial significant variables: ";
      for (auto i = significant_variables.begin(); i != significant_variables.end(); ++i)
      {
        std::clog << *i << " ";
      }
      std::clog << std::endl;
#endif

      // recursively extend the set of significant variables
      std::set<data::variable> todo = significant_variables;
      while (!todo.empty())
      {
        data::variable x = *todo.begin();
        todo.erase(todo.begin());

        for (auto i = m_spec.process().action_summands().begin(); i != m_spec.process().action_summands().end(); ++i)
        {
          data::assignment_list assignments(i->assignments());
          auto j = std::find_if(assignments.begin(), assignments.end(), data::detail::has_left_hand_side(x));
          if (j != assignments.end())
          {
            std::set<data::variable> vars;
            data::find_all_variables(j->rhs(), std::inserter(vars, vars.end()));
            std::set<data::variable> new_variables = data::detail::set_difference(vars, significant_variables);
            todo.insert(new_variables.begin(), new_variables.end());
            significant_variables.insert(new_variables.begin(), new_variables.end());
#ifdef MCRL2_LPS_PARELM_DEBUG
            for (auto k = new_variables.begin(); k != new_variables.end(); ++k)
            {
              std::clog << "found dependency " << x << " -> " << *k << std::endl;
            }
#endif
          }
        }
      }
      std::set<data::variable> to_be_removed = data::detail::set_difference(process_parameters, significant_variables);
#ifdef MCRL2_LPS_PARELM_DEBUG
      std::clog << "to be removed: " << data::pp(data::variable_list(to_be_removed.begin(), to_be_removed.end())) << std::endl;
#endif
      report_results(to_be_removed);
      lps::remove_parameters(m_spec, to_be_removed);
      assert(is_well_typed(m_spec));
    }

    /// \brief Second version of parelm that builds a dependency graph
    void parelm2()
    {
#ifdef MCRL2_LPS_PARELM_DEBUG
      std::clog << "--- parelm 2 ---" << std::endl;
#endif
      const data::variable_list& pars = m_spec.process().process_parameters();
      std::set<data::variable> process_parameters(pars.begin(),pars.end());

      // create a mapping m from process parameters to integers
      std::map<data::variable, size_t> m;
      int index = 0;
      for (auto i = process_parameters.begin(); i != process_parameters.end(); ++i)
      {
#ifdef MCRL2_LPS_PARELM_DEBUG
        std::clog << "vertex " << index << " = " << data::pp(*i) << std::endl;
#endif
        m[*i] = index++;
      }

      // compute the initial set v of significant variables
      std::set<data::variable> significant_variables = transition_variables();
      std::vector<size_t> v;
      for (auto i = significant_variables.begin(); i != significant_variables.end(); ++i)
      {
        v.push_back(m[*i]);
      }

      // compute the dependency graph G
      typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS> graph;
      graph G(process_parameters.size());
      for (auto i = m_spec.process().action_summands().begin(); i != m_spec.process().action_summands().end(); ++i)
      {
        data::assignment_list assignments(i->assignments());
        for (auto j = assignments.begin(); j != assignments.end(); ++j)
        {
          size_t j0 = m[j->lhs()];
          std::set<data::variable> vars;
          data::find_all_variables(j->rhs(), std::inserter(vars, vars.end()));
          for (auto k = vars.begin(); k != vars.end(); ++k)
          {
            size_t k0 = m[*k];
#ifdef MCRL2_LPS_PARELM_DEBUG
            std::clog << "edge " << j0 << " -> " << k0 << std::endl;
#endif
            boost::add_edge(j0, k0, G);
          }
        }
      }

#ifdef MCRL2_LPS_PARELM_DEBUG
      std::clog << "initial significant variables: ";
      for (auto k = v.begin(); k != v.end(); ++k)
      {
        std::clog << *k << " ";
      }
      std::clog << std::endl;
#endif

      // compute the reachable nodes (i.e. the significant parameters)
      std::vector<size_t> r1 = mcrl2::utilities::reachable_nodes(G, v.begin(), v.end());
#ifdef MCRL2_LPS_PARELM_DEBUG
      std::clog << "reachable nodes: ";
      for (auto k = r1.begin(); k != r1.end(); ++k)
      {
        std::clog << *k << " ";
      }
      std::clog << std::endl;
#endif
      std::set<size_t> r2(r1.begin(), r1.end());
      std::set<data::variable> to_be_removed;
      for (auto i = process_parameters.begin(); i != process_parameters.end(); ++i)
      {
        if (r2.find(m[*i]) == r2.end())
        {
          to_be_removed.insert(*i);
        }
      }
#ifdef MCRL2_LPS_PARELM_DEBUG
      std::clog << "to be removed: " << data::pp(data::variable_list(to_be_removed.begin(), to_be_removed.end())) << std::endl;
#endif
      report_results(to_be_removed);
      lps::remove_parameters(m_spec, to_be_removed);
      assert(is_well_typed(m_spec));
    }

  public:

    /// \brief Constructor
    parelm_algorithm(Specification& spec )
      : lps::detail::lps_algorithm<Specification>(spec)
    {}

    /// \brief Runs the parelm algorithm
    void run(bool variant1 = true)
    {
      if (variant1)
      {
        parelm1();
      }
      else
      {
        parelm2();
      }
    }
};

/// \brief Removes unused parameters from a linear process specification.
/// \param spec A linear process specification
/// \param variant1 If true the default variant of parelm is used, otherwise an
///        alternative implementation is chosen.
template <typename Specification>
void parelm(Specification& spec, bool variant1 = true)
{
  parelm_algorithm<Specification> algorithm(spec);
  algorithm.run(variant1);
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_PARELM_H
