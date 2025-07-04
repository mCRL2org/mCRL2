// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/is_monotonous.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_IS_MONOTONOUS_H
#define MCRL2_MODAL_FORMULA_IS_MONOTONOUS_H

#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/modal_formula/state_formula.h"

namespace mcrl2::state_formulas
{

/// \brief Returns true if the state formula is monotonous.
/// \param f A modal formula.
/// \param non_negated_variables Names of state variables that occur positively in the current scope.
/// \param negated_variables Names of state variables that occur negatively in the current scope.
/// \return True if the state formula is monotonous.
inline
bool is_monotonous(const state_formula& f, 
                   const std::set<core::identifier_string>& non_negated_variables,
                   const std::set<core::identifier_string>& negated_variables)
{
  using utilities::detail::contains;

  if (is_not(f))
  {
    return is_monotonous(atermpp::down_cast<not_>(f).operand(), negated_variables,  non_negated_variables);
  }
  else if (is_minus(f))
  {
    return is_monotonous(atermpp::down_cast<minus>(f).operand(), negated_variables,  non_negated_variables);
  }
  else if (data::is_data_expression(f))
  {
    return true;
  }
  else if (is_true(f))
  {
    return true;
  }
  else if (is_false(f))
  {
    return true;
  }
  else if (is_and(f))
  {
    const and_& g = atermpp::down_cast<and_>(f);
    return is_monotonous(g.left(), non_negated_variables, negated_variables) && 
           is_monotonous(g.right(), non_negated_variables, negated_variables);
  }
  else if (is_or(f))
  {
    const or_& g = atermpp::down_cast<or_>(f);
    return is_monotonous(g.left(), non_negated_variables, negated_variables) && 
           is_monotonous(g.right(), non_negated_variables, negated_variables);
  }
  else if (is_imp(f))
  {
    const imp& g = atermpp::down_cast<imp>(f);
    return is_monotonous(g.left(), negated_variables, non_negated_variables) && 
           is_monotonous(g.right(), non_negated_variables, negated_variables);
  }
  else if (is_plus(f))
  {
    const plus& g = atermpp::down_cast<plus>(f);
    return is_monotonous(g.left(), non_negated_variables, negated_variables) && 
           is_monotonous(g.right(), non_negated_variables, negated_variables);
  }
  else if (is_const_multiply(f))
  {
    const const_multiply& g = atermpp::down_cast<const_multiply>(f);
    return is_monotonous(g.right(), non_negated_variables, negated_variables);
  }
  else if (is_const_multiply_alt(f))
  {
    const const_multiply_alt& g = atermpp::down_cast<const_multiply_alt>(f);
    return is_monotonous(g.left(), non_negated_variables, negated_variables);
  }
  else if (is_forall(f))
  {
    const forall& g = atermpp::down_cast<forall>(f);
    return is_monotonous(g.body(), non_negated_variables, negated_variables);
  }
  else if (is_exists(f))
  {
    const exists& g = atermpp::down_cast<exists>(f);
    return is_monotonous(g.body(), non_negated_variables, negated_variables);
  }
  else if (is_infimum(f))
  {
    const infimum& g = atermpp::down_cast<infimum>(f);
    return is_monotonous(g.body(), non_negated_variables, negated_variables);
  }
  else if (is_supremum(f))
  {
    const supremum& g = atermpp::down_cast<supremum>(f);
    return is_monotonous(g.body(), non_negated_variables, negated_variables);
  }
  else if (is_sum(f))
  {
    const sum& g = atermpp::down_cast<sum>(f);
    return is_monotonous(g.body(), non_negated_variables, negated_variables);
  }
  else if (is_may(f))
  {
    const may& g = atermpp::down_cast<may>(f);
    return is_monotonous(g.operand(), non_negated_variables, negated_variables);
  }
  else if (is_must(f))
  {
    const must& g = atermpp::down_cast<must>(f);
    return is_monotonous(g.operand(), non_negated_variables, negated_variables);
  }
  else if (is_yaled_timed(f))
  {
    return true;
  }
  else if (is_yaled(f))
  {
    return true;
  }
  else if (is_delay_timed(f))
  {
    return true;
  }
  else if (is_delay(f))
  {
    return true;
  }
  else if (is_variable(f))
  {
    const variable& g = atermpp::down_cast<variable>(f);
    return !contains(negated_variables, g.name());
  }
  else if (is_mu(f))
  {
    const mu& g = atermpp::down_cast<mu>(f);
    std::set<core::identifier_string> non_neg = non_negated_variables;
    non_neg.insert(g.name());
    return is_monotonous(g.operand(), non_neg, negated_variables);
  }
  else if (is_nu(f))
  {
    const nu& g = atermpp::down_cast<nu>(f);
    std::set<core::identifier_string> non_neg = non_negated_variables;
    non_neg.insert(g.name());
    return is_monotonous(g.operand(), non_neg, negated_variables);
  }

  throw mcrl2::runtime_error(std::string("is_monotonous(state_formula) error: unknown argument ") + pp(f));
  return false;
}

/// \brief Returns true if the state formula is monotonous.
/// \param f A modal formula
/// \return True if the state formula is monotonous.
inline
bool is_monotonous(const state_formula& f)
{
  std::set<core::identifier_string> non_negated_variables;
  std::set<core::identifier_string> negated_variables;
  return is_monotonous(f, non_negated_variables, negated_variables);
}

} // namespace mcrl2::state_formulas



#endif // MCRL2_MODAL_FORMULA_IS_MONOTONOUS_H
