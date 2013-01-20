// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/monotonicity.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_MONOTONICITY_H
#define MCRL2_MODAL_FORMULA_MONOTONICITY_H

#include <set>
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/modal_formula/detail/state_formula_accessors.h"

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
  using namespace state_formulas::detail::accessors;

  //--- handle negations ---//
  if (is_not(f))
  {
    f = arg(f); // remove the not
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
      return is_monotonous(arg(f), negated_variables);
    }
    else if (is_and(f))
    {
      return is_monotonous(not_(left(f)), negated_variables) && is_monotonous(not_(right(f)), negated_variables);
    }
    else if (is_or(f))
    {
      return is_monotonous(not_(left(f)), negated_variables) && is_monotonous(not_(right(f)), negated_variables);
    }
    else if (is_imp(f))
    {
      return is_monotonous(left(f), negated_variables) && is_monotonous(not_(right(f)), negated_variables);
    }
    else if (is_forall(f))
    {
      return is_monotonous(not_(arg(f)), negated_variables);
    }
    else if (is_exists(f))
    {
      return is_monotonous(not_(arg(f)), negated_variables);
    }
    else if (is_may(f))
    {
      return is_monotonous(not_(arg(f)), negated_variables);
    }
    else if (is_must(f))
    {
      return is_monotonous(not_(arg(f)), negated_variables);
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
      return negated_variables.find(name(f)) != negated_variables.end();
    }
    else if (is_mu(f))
    {
      std::set<core::identifier_string> neg = negated_variables;
      core::identifier_string X = name(f);
      std::set<core::identifier_string>::iterator i = neg.find(X);
      if (i != neg.end())
      {
        neg.erase(i);
      }
      else
      {
        neg.insert(X);
      }
      return is_monotonous(not_(arg(f)), neg);
    }
    else if (is_nu(f))
    {
      std::set<core::identifier_string> neg = negated_variables;
      core::identifier_string X = name(f);
      std::set<core::identifier_string>::iterator i = neg.find(X);
      if (i != neg.end())
      {
        neg.erase(i);
      }
      else
      {
        neg.insert(X);
      }
      return is_monotonous(not_(arg(f)), neg);
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
    return is_monotonous(left(f), negated_variables) && is_monotonous(right(f), negated_variables);
  }
  else if (is_or(f))
  {
    return is_monotonous(left(f), negated_variables) && is_monotonous(right(f), negated_variables);
  }
  else if (is_imp(f))
  {
    return is_monotonous(not_(left(f)), negated_variables) && is_monotonous(right(f), negated_variables);
  }
  else if (is_forall(f))
  {
    return is_monotonous(arg(f), negated_variables);
  }
  else if (is_exists(f))
  {
    return is_monotonous(arg(f), negated_variables);
  }
  else if (is_may(f))
  {
    return is_monotonous(arg(f), negated_variables);
  }
  else if (is_must(f))
  {
    return is_monotonous(arg(f), negated_variables);
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
    return negated_variables.find(name(f)) == negated_variables.end();
  }
  else if (is_mu(f))
  {
    return is_monotonous(arg(f), negated_variables);
  }
  else if (is_nu(f))
  {
    return is_monotonous(arg(f), negated_variables);
  }

  throw mcrl2::runtime_error(std::string("is_monotonous(state_formula) error: unknown argument ") + to_string(f));
  return false;
}

/// \brief Returns true if the state formula is monotonous.
/// \param f A modal formula
/// \return True if the state formula is monotonous.
inline
bool is_monotonous(state_formula f)
{
  std::set<core::identifier_string> negated_variables;
  return is_monotonous(f, negated_variables);
}

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_MONOTONICITY_H
