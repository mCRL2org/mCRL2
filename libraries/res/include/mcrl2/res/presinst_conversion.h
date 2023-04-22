// Author(s): Jan Friso Groote. Based on bes/pbesinst_conversion.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/res/presinst_conversion.h
/// \brief add your file description here.

#ifndef MCRL2_RES_PRESINST_CONVERSION_H
#define MCRL2_RES_PRESINST_CONVERSION_H

#include "mcrl2/res/res_equation_system.h"
#include "mcrl2/res/detail/pres_expression2res_expression_traverser.h"
#include "mcrl2/pres/algorithms.h"

namespace mcrl2
{

namespace res
{

/// \brief Converts a propositional variable into a res variable
/// \param v A propositional variable
inline
res::res_variable presinst_conversion(const pres_system::propositional_variable_instantiation& v)
{
  return res::res_variable(v.name());
}

/// \brief Converts a PRES expression into a res expression
/// \param x A PRES expression
inline
res::res_expression presinst_conversion(const pres_system::pres_expression& x)
{
  return res::pres_expression2res_expression(x);
}

/// \brief Converts a PRES equation into a res equation
/// \param eq A PRES equation
inline
res::res_equation presinst_conversion(const pres_system::pres_equation& eq)
{
  return res::res_equation(eq.symbol(), res::res_variable(eq.variable().name()), presinst_conversion(eq.formula()));
}

/// \brief Converts a PRES into a RES
/// \param p A PRES
/// \pre The PRES must be a RES
inline
res::res_equation_system presinst_conversion(const pres_system::pres& p)
{
  assert(pres_system::algorithms::is_res(p));

  std::vector<res::res_equation> equations;
  for (const pres_system::pres_equation& eqn: p.equations())
  {
    equations.push_back(presinst_conversion(eqn));
  }
  res::res_expression initial_state = presinst_conversion(p.initial_state());

  return res::res_equation_system(equations, initial_state);
}

} // namespace res

} // namespace mcrl2

#endif // MCRL2_RES_PRESINST_CONVERSION_H
