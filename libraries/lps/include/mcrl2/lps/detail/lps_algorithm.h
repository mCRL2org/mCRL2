// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/lps_algorithm.h
/// \brief Add your file description here.

#ifndef MCRL2_LPS_DETAIL_LPS_ALGORITHM_H
#define MCRL2_LPS_DETAIL_LPS_ALGORITHM_H

#include "mcrl2/data/rewriter.h"
#include "mcrl2/lps/detail/instantiate_global_variables.h"
#include "mcrl2/lps/rewrite.h"

namespace mcrl2
{

namespace lps
{

namespace detail
{

/// \brief Algorithm class for algorithms on linear process specifications.
/// It can be instantiated with lps::specification and lps::stochastic_specification.
template <typename Specification = specification>
class lps_algorithm
{
  protected:
    /// \brief The specification that is processed by the algorithm
    Specification& m_spec;

    void sumelm_find_variables(const action_summand& s, std::set<data::variable>& result) const
    {
      std::set<data::variable> tmp;

      tmp = data::find_free_variables(s.condition());
      result.insert(tmp.begin(), tmp.end());

      tmp = lps::find_free_variables(s.multi_action());
      result.insert(tmp.begin(), tmp.end());

      tmp = data::find_free_variables(s.assignments());
      result.insert(tmp.begin(), tmp.end());
    }

    void sumelm_find_variables(const deadlock_summand& s, std::set<data::variable>& result) const
    {
      std::set<data::variable> tmp;

      tmp = data::find_free_variables(s.condition());
      result.insert(tmp.begin(), tmp.end());

      tmp = lps::find_free_variables(s.deadlock());
      result.insert(tmp.begin(), tmp.end());
    }

    template <typename SummandType>
    void summand_remove_unused_summand_variables(SummandType& summand_)
    {
      data::variable_vector new_summation_variables;

      std::set<data::variable> occurring_vars;
      sumelm_find_variables(summand_, occurring_vars);

      std::set<data::variable> summation_variables(summand_.summation_variables().begin(),summand_.summation_variables().end());
      std::set_intersection(summation_variables.begin(), summation_variables.end(),
                            occurring_vars.begin(), occurring_vars.end(),
                            std::inserter(new_summation_variables, new_summation_variables.end()));

      summand_.summation_variables() = data::variable_list(new_summation_variables.begin(),new_summation_variables.end());
    }

  public:
    /// \brief Constructor
    lps_algorithm(Specification& spec)
      : m_spec(spec)
    {}

    /// \brief Flag for verbose output
    bool verbose() const
    {
      return mCRL2logEnabled(log::verbose);
    }

    /// \brief Applies the next state substitution to the variable v.
    data::data_expression next_state(const action_summand& s, const data::variable& v) const
    {
      for (const data::assignment& a: s.assignments())
      {
        if (a.lhs() == v)
        {
          return a.rhs();
        }
      }
      return v; // no assignment to v found, so return v itself
    }

    /// \brief Attempts to eliminate the free variables of the specification, by substituting
    /// a constant value for them. If no constant value is found for one of the variables,
    /// an exception is thrown.
    void instantiate_free_variables()
    {
      lps::detail::instantiate_global_variables(m_spec);
    }

    /// \brief Removes formal parameters from the specification
    void remove_parameters(const std::set<data::variable>& to_be_removed)
    {
      lps::remove_parameters(m_spec, to_be_removed);
    }

    /// \brief Removes parameters with a singleton sort
    void remove_singleton_sorts()
    {
      lps::remove_singleton_sorts(m_spec);
    }

    /// \brief Removes summands with condition equal to false
    void remove_trivial_summands()
    {
      lps::remove_trivial_summands(m_spec);
    }

    /// \brief Removes unused summand variables.
    void remove_unused_summand_variables()
    {
      auto& v = m_spec.process().action_summands();
      std::for_each(v.begin(), 
                    v.end(), 
                    [this](action_summand& s){lps_algorithm::summand_remove_unused_summand_variables<typename Specification::process_type::action_summand_type>(s); }); 

      auto& w = m_spec.process().deadlock_summands();
      std::for_each(w.begin(), 
                    w.end(), 
                    [this](deadlock_summand& s){lps_algorithm::summand_remove_unused_summand_variables<deadlock_summand>(s); }); 
    }
};

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_LPS_ALGORITHM_H
