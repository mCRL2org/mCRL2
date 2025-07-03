// Author(s): Jan Friso Groote. Based on pbes/is_monotonous.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/is_monotonous.h
/// \brief Functions for computing monotonicity of pres data types.

#ifndef MCRL2_PRES_IS_MONOTONOUS_H
#define MCRL2_PRES_IS_MONOTONOUS_H

#include "mcrl2/pres/pres.h"



namespace mcrl2::pres_system
{

/// \brief Returns true if the pres expression is monotonous.
/// \param f A pres expression.
/// \return True if the pres expression is monotonous.
inline
bool is_monotonous(pres_expression f)
{
  //--- handle negations ---//
  if (is_minus(f))
  {
    f = atermpp::down_cast<minus>(f).operand(); // remove the not
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
    else if (is_minus(f))
    {
      return is_monotonous(atermpp::down_cast<minus>(f).operand());
    }
    else if (is_and(f))
    {
      const pres_expression& left = atermpp::down_cast<and_>(f).left();
      const pres_expression& right = atermpp::down_cast<and_>(f).right();
      return is_monotonous(minus(left)) && is_monotonous(minus(right));
    }
    else if (is_or(f))
    {
      const pres_expression& left = atermpp::down_cast<or_>(f).left();
      const pres_expression& right = atermpp::down_cast<or_>(f).right();
      return is_monotonous(minus(left)) && is_monotonous(minus(right));
    }
    else if (is_imp(f))
    {
      const pres_expression& left = atermpp::down_cast<imp>(f).left();
      const pres_expression& right = atermpp::down_cast<imp>(f).right();
      return is_monotonous(left) && is_monotonous(minus(right));
    }
    else if (is_plus(f))
    {
      const pres_expression& left = atermpp::down_cast<plus>(f).left();
      const pres_expression& right = atermpp::down_cast<plus>(f).right();
      return is_monotonous(minus(left)) && is_monotonous(minus(right));
    }
    else if (is_const_multiply(f))
    {
      const pres_expression& right = atermpp::down_cast<const_multiply>(f).right();
      return is_monotonous(minus(right));
    }
    else if (is_const_multiply_alt(f))
    {
      const pres_expression& left = atermpp::down_cast<const_multiply_alt>(f).left();
      return is_monotonous(minus(left));
    }
    else if (is_infimum(f))
    {
      const pres_expression& body = atermpp::down_cast<infimum>(f).body();
      return is_monotonous(minus(body));
    }
    else if (is_supremum(f))
    {
      const pres_expression& body = atermpp::down_cast<supremum>(f).body();
      return is_monotonous(minus(body));
    }
    else if (is_sum(f))
    {
      const pres_expression& body = atermpp::down_cast<sum>(f).body();
      return is_monotonous(minus(body));
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
    const pres_expression& left = atermpp::down_cast<and_>(f).left();
    const pres_expression& right = atermpp::down_cast<and_>(f).right();
    return is_monotonous(left) && is_monotonous(right);
  }
  else if (is_or(f))
  {
    const pres_expression& left = atermpp::down_cast<or_>(f).left();
    const pres_expression& right = atermpp::down_cast<or_>(f).right();
    return is_monotonous(left) && is_monotonous(right);
  }
  else if (is_imp(f))
  {
    const pres_expression& left = atermpp::down_cast<imp>(f).left();
    const pres_expression& right = atermpp::down_cast<imp>(f).right();
    return is_monotonous(minus(left)) && is_monotonous(right);
  }
  else if (is_plus(f))
  {
    const pres_expression& left = atermpp::down_cast<plus>(f).left();
    const pres_expression& right = atermpp::down_cast<plus>(f).right();
    return is_monotonous(left) && is_monotonous(right);
  }
  else if (is_const_multiply(f))
  {
    const pres_expression& right = atermpp::down_cast<const_multiply>(f).right();
    return is_monotonous(right);
  }
  else if (is_const_multiply_alt(f))
  {
    const pres_expression& left = atermpp::down_cast<const_multiply_alt>(f).left();
    return is_monotonous(left);
  }
  else if (is_infimum(f))
  {
    const pres_expression& body = atermpp::down_cast<infimum>(f).body();
    return is_monotonous(body);
  }
  else if (is_supremum(f))
  {
    const pres_expression& body = atermpp::down_cast<supremum>(f).body();
    return is_monotonous(body);
  }
  else if (is_sum(f))
  {
    const pres_expression& body = atermpp::down_cast<sum>(f).body();
    return is_monotonous(body);
  }
  else if (is_propositional_variable_instantiation(f))
  {
    return true;
  }

  throw mcrl2::runtime_error(std::string("is_monotonous(pres_expression) error: unknown argument ") + pp(f));
  return false;
}

/// \brief Returns true if the pres equation is monotonous.
inline
bool is_monotonous(const pres_equation& e)
{
  return is_monotonous(e.formula());
}

/// \brief Returns true if the pres is monotonous.
inline
bool is_monotonous(const pres& p)
{
  for (const pres_equation& eqn: p.equations())
  {
    if (!is_monotonous(eqn))
    {
      return false;
    }
  }
  return true;
}

} // namespace mcrl2::pres_system



#endif // MCRL2_PRES_IS_MONOTONOUS_H
