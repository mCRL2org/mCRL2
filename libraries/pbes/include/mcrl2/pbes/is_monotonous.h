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
/// \param f A pbes expression
/// \param negated_variables The set of variables that occurs under an odd number of negations.
/// \return True if the pbes expression is monotonous.
inline
bool is_monotonous(pbes_expression f)
{
  namespace p = pbes_expr;
  using namespace accessors;

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
      return is_monotonous(arg(f));
    }
    else if (is_and(f))
    {
      return is_monotonous(p::not_(left(f))) && is_monotonous(p::not_(right(f)));
    }
    else if (is_or(f))
    {
      return is_monotonous(p::not_(left(f))) && is_monotonous(p::not_(right(f)));
    }
    else if (is_imp(f))
    {
      return is_monotonous(left(f)) && is_monotonous(p::not_(right(f)));
    }
    else if (is_forall(f))
    {
      return is_monotonous(p::not_(arg(f)));
    }
    else if (is_exists(f))
    {
      return is_monotonous(p::not_(arg(f)));
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
    return is_monotonous(left(f)) && is_monotonous(right(f));
  }
  else if (is_or(f))
  {
    return is_monotonous(left(f)) && is_monotonous(right(f));
  }
  else if (is_imp(f))
  {
    return is_monotonous(p::not_(left(f))) && is_monotonous(right(f));
  }
  else if (is_forall(f))
  {
    return is_monotonous(arg(f));
  }
  else if (is_exists(f))
  {
    return is_monotonous(arg(f));
  }
  else if (is_propositional_variable_instantiation(f))
  {
    return true;
  }

  throw mcrl2::runtime_error(std::string("is_monotonous(pbes_expression) error: unknown argument ") + to_string(f));
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
  for (auto i = p.equations().begin(); i != p.equations().end(); ++i)
  {
    if (!is_monotonous(*i))
    {
      return false;
    }
  }
  return true;
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_IS_MONOTONOUS_H
