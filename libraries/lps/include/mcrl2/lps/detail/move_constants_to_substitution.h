// Author(s): Jan Friso Groote 
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/move_constants_to_substitution.h
/// \brief This file contains a function that replaces constant
///        non trivial terms in an lps by variables, and assigns
///        the normal forms corresponding to the variables to a
///        substitution. The purpose of this is to avoid rewriting
///        these non trivial constant iteratively, when for instance
///        generating a state space. 

#ifndef MCRL2_LPS_MOVE_CONSTANTS_TO_SUBSTITUTION_H
#define MCRL2_LPS_MOVE_CONSTANTS_TO_SUBSTITUTION_H

#include "mcrl2/data/detail/move_constants_to_substitution.h"
#include "mcrl2/lps/stochastic_specification.h"

namespace mcrl2
{

namespace lps
{

namespace detail
{


// For an explanation see move_constants_to_substitution below.
template <class SUBSTITUTION>
void move_constants_to_substitution_helper(lps::stochastic_specification& spec,
                                           data::rewriter& r,
                                           SUBSTITUTION& sigma,
                                           std::unordered_map<data::data_expression, data::variable>& expression_to_variable_map,
                                           data::set_identifier_generator& identifier_generator
                                          )
{
  for(stochastic_action_summand& s: spec.process().action_summands())
  {
    process::action_vector new_actions;
    for(const process::action& a: s.multi_action().actions())
    {
      data::data_expression_vector new_parameters;
      for(const data::data_expression& e: a.arguments())
      {
        new_parameters.emplace_back(data::detail::move_constants_to_substitution(e,r,sigma,expression_to_variable_map,identifier_generator));
      }
      new_actions.emplace_back(a.label(),data::data_expression_list(new_parameters.begin(),new_parameters.end()));
    }
    lps::multi_action new_multi_action(process::action_list(new_actions.begin(), new_actions.end()),
                                       data::detail::move_constants_to_substitution(s.multi_action().time(),
                                                                                    r,
                                                                                    sigma,
                                                                                    expression_to_variable_map,
                                                                                    identifier_generator));

    data::assignment_vector new_assignments;
    for(const data::assignment&a: s.assignments())
    {
      new_assignments.emplace_back(a.lhs(),
                                   data::detail::move_constants_to_substitution(a.rhs(),r,sigma,expression_to_variable_map,identifier_generator));
    }

    s=stochastic_action_summand(s.summation_variables(), 
                                data::detail::move_constants_to_substitution(s.condition(),r,sigma,expression_to_variable_map,identifier_generator),
                                new_multi_action,
                                data::assignment_list(new_assignments.begin(),new_assignments.end()),
                                stochastic_distribution(s.distribution().variables(),
                                                        data::detail::move_constants_to_substitution(s.distribution().distribution(),
                                                                                                     r,
                                                                                                     sigma,
                                                                                                     expression_to_variable_map,
                                                                                                     identifier_generator)));
  }
}


} // namespace detail


/// \brief Replace non trivial constants c in the lps by variables and add assignments x:=normal_form(c) to the substitution.
/// \details This operation is useful when terms in the lps are very often rewritten. Constant expressions in the lps are 
///          brought to normal form whenever they are encountered. This happens very often when generating a state space.
///          In the substitution they are considered a normal form, and not rewritten. This can save more than 50% of the time
///          to generate a state space.
/// \param lpsspec A linear process specification.
/// \param r The rewriter used to calculate a normal form.
/// \param sigma The substitution into which it is stored which variables represent which terms. 
template <class SUBSTITUTION>
void move_constants_to_substitution(lps::stochastic_specification& spec,
                                    data::rewriter& r,
                                    SUBSTITUTION& sigma)
{
  std::unordered_map<data::data_expression, data::variable> expression_to_variable_map;
  data::set_identifier_generator identifier_generator;
  detail::move_constants_to_substitution_helper(spec,r,sigma,expression_to_variable_map,identifier_generator);
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_MOVE_CONSTANTS_TO_SUBSTITUTION_H
