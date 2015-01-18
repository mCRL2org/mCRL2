// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/is_monotonous.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_IS_MONOTONOUS_H
#define MCRL2_MODAL_FORMULA_IS_MONOTONOUS_H

#include <set>
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/utilities/detail/container_utility.h"

namespace mcrl2
{

namespace state_formulas
{

/// \brief Returns true if the state formula is monotonous.
/// \param f A modal formula
/// \return True if the state formula is monotonous.
inline
bool is_monotonous(state_formula f, const std::set<core::identifier_string>& negated_variables)
{
  using utilities::detail::contains;

  //--- handle negations ---//
  if (is_not(f))
  {
    f = atermpp::down_cast<not_>(f).operand(); // remove the not
    if (data::is_data_expression(f))
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
    else if (is_not(f))
    {
      const not_& g = atermpp::down_cast<not_>(f);
      return is_monotonous(g.operand(), negated_variables);
    }
    else if (is_and(f))
    {
      const and_& g = atermpp::down_cast<and_>(f);
      return is_monotonous(not_(g.left()), negated_variables) && is_monotonous(not_(g.right()), negated_variables);
    }
    else if (is_or(f))
    {
      const or_& g = atermpp::down_cast<or_>(f);
      return is_monotonous(not_(g.left()), negated_variables) && is_monotonous(not_(g.right()), negated_variables);
    }
    else if (is_imp(f))
    {
      const imp& g = atermpp::down_cast<imp>(f);
      return is_monotonous(g.left(), negated_variables) && is_monotonous(not_(g.right()), negated_variables);
    }
    else if (is_forall(f))
    {
      const forall& g = atermpp::down_cast<forall>(f);
      return is_monotonous(not_(g.body()), negated_variables);
    }
    else if (is_exists(f))
    {
      const exists& g = atermpp::down_cast<exists>(f);
      return is_monotonous(not_(g.body()), negated_variables);
    }
    else if (is_may(f))
    {
      const may& g = atermpp::down_cast<may>(f);
      return is_monotonous(not_(g.operand()), negated_variables);
    }
    else if (is_must(f))
    {
      const must& g = atermpp::down_cast<must>(f);
      return is_monotonous(not_(g.operand()), negated_variables);
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
      return contains(negated_variables, g.name());
    }
    else if (is_mu(f))
    {
      const mu& g = atermpp::down_cast<mu>(f);
      std::set<core::identifier_string> neg = negated_variables;
      core::identifier_string X = g.name();
      neg.insert(X);
      return is_monotonous(not_(g.operand()), neg);
    }
    else if (is_nu(f))
    {
      const nu& g = atermpp::down_cast<nu>(f);
      std::set<core::identifier_string> neg = negated_variables;
      core::identifier_string X = g.name();
      neg.insert(X);
      return is_monotonous(not_(g.operand()), neg);
    }
  }

  //--- handle everything except negations ---//
  if (data::is_data_expression(f))
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
    return is_monotonous(g.left(), negated_variables) && is_monotonous(g.right(), negated_variables);
  }
  else if (is_or(f))
  {
    const or_& g = atermpp::down_cast<or_>(f);
    return is_monotonous(g.left(), negated_variables) && is_monotonous(g.right(), negated_variables);
  }
  else if (is_imp(f))
  {
    const imp& g = atermpp::down_cast<imp>(f);
    return is_monotonous(not_(g.left()), negated_variables) && is_monotonous(g.right(), negated_variables);
  }
  else if (is_forall(f))
  {
    const forall& g = atermpp::down_cast<forall>(f);
    return is_monotonous(g.body(), negated_variables);
  }
  else if (is_exists(f))
  {
    const exists& g = atermpp::down_cast<exists>(f);
    return is_monotonous(g.body(), negated_variables);
  }
  else if (is_may(f))
  {
    const may& g = atermpp::down_cast<may>(f);
    return is_monotonous(g.operand(), negated_variables);
  }
  else if (is_must(f))
  {
    const must& g = atermpp::down_cast<must>(f);
    return is_monotonous(g.operand(), negated_variables);
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
    return is_monotonous(g.operand(), negated_variables);
  }
  else if (is_nu(f))
  {
    const nu& g = atermpp::down_cast<nu>(f);
    return is_monotonous(g.operand(), negated_variables);
  }

  throw mcrl2::runtime_error(std::string("is_monotonous(state_formula) error: unknown argument ") + to_string(f));
  return false;
}

/// \brief Returns true if the state formula is monotonous.
/// \param f A modal formula
/// \return True if the state formula is monotonous.
inline
bool is_monotonous(const state_formula& f)
{
  std::set<core::identifier_string> negated_variables;
  return is_monotonous(f, negated_variables);
}

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_IS_MONOTONOUS_H
