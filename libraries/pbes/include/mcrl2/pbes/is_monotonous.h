// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/is_monotonous.h
/// \brief Functions for computing monotonicity of pbes data types.

#ifndef MCRL2_PBES_IS_MONOTONOUS_H
#define MCRL2_PBES_IS_MONOTONOUS_H

#include "mcrl2/pbes/pbes.h"

namespace mcrl2
{

namespace pbes_system
{

/// \brief Returns true if the pbes expression is monotonous.
/// \param f A pbes expression.
/// \return True if the pbes expression is monotonous.
inline
bool is_monotonous(pbes_expression f)
{
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
      return is_monotonous(atermpp::down_cast<not_>(f).operand());
    }
    else if (is_and(f))
    {
      const auto& left = atermpp::down_cast<and_>(f).left();
      const auto& right = atermpp::down_cast<and_>(f).right();
      return is_monotonous(not_(left)) && is_monotonous(not_(right));
    }
    else if (is_or(f))
    {
      const auto& left = atermpp::down_cast<or_>(f).left();
      const auto& right = atermpp::down_cast<or_>(f).right();
      return is_monotonous(not_(left)) && is_monotonous(not_(right));
    }
    else if (is_imp(f))
    {
      const auto& left = atermpp::down_cast<imp>(f).left();
      const auto& right = atermpp::down_cast<imp>(f).right();
      return is_monotonous(left) && is_monotonous(not_(right));
    }
    else if (is_forall(f))
    {
      const auto& body = atermpp::down_cast<forall>(f).body();
      return is_monotonous(not_(body));
    }
    else if (is_exists(f))
    {
      const auto& body = atermpp::down_cast<exists>(f).body();
      return is_monotonous(not_(body));
    }
    else if (is_propositional_variable_instantiation(f))
    {
      return false;
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
    const auto& left = atermpp::down_cast<and_>(f).left();
    const auto& right = atermpp::down_cast<and_>(f).right();
    return is_monotonous(left) && is_monotonous(right);
  }
  else if (is_or(f))
  {
    const auto& left = atermpp::down_cast<or_>(f).left();
    const auto& right = atermpp::down_cast<or_>(f).right();
    return is_monotonous(left) && is_monotonous(right);
  }
  else if (is_imp(f))
  {
    const auto& left = atermpp::down_cast<imp>(f).left();
    const auto& right = atermpp::down_cast<imp>(f).right();
    return is_monotonous(not_(left)) && is_monotonous(right);
  }
  else if (is_forall(f))
  {
    const auto& body = atermpp::down_cast<forall>(f).body();
    return is_monotonous(body);
  }
  else if (is_exists(f))
  {
    const auto& body = atermpp::down_cast<exists>(f).body();
    return is_monotonous(body);
  }
  else if (is_propositional_variable_instantiation(f))
  {
    return true;
  }

  throw mcrl2::runtime_error(std::string("is_monotonous(pbes_expression) error: unknown argument ") + pp(f));
  return false;
}

/// \brief Returns true if the pbes equation is monotonous.
inline
bool is_monotonous(const pbes_equation& e)
{
  return is_monotonous(e.formula());
}

/// \brief Returns true if the pbes is monotonous.
inline
bool is_monotonous(const pbes& p)
{
  for (const pbes_equation& eqn: p.equations())
  {
    if (!is_monotonous(eqn))
    {
      return false;
    }
  }
  return true;
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_IS_MONOTONOUS_H
