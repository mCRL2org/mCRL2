// Author(s): Jan Friso Groote. Based on bes/bes2pbes.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/res/res2pbes.h
/// \brief Contains functions to transfrom a RES into a PRES.

#ifndef MCRL2_RES_RES2PRES_H
#define MCRL2_RES_RES2PRES_H

#include "mcrl2/res/detail/res_expression2pres_expression_traverser.h"
#include "mcrl2/pres/pres.h"

namespace mcrl2
{

namespace res
{

/// \brief Converts a res variable into a propositional variable
/// \param v A res variable
inline
pres_system::propositional_variable_instantiation res2pres(const res_variable& v)
{
  return pres_system::propositional_variable_instantiation(v.name(), data::data_expression_list());
}

/// \brief Converts a res expression into a PRES expression
/// \param x A res expression
inline
pres_system::pres_expression res2pres(const res_expression& x)
{
  res::detail::res_expression2pres_expression_traverser t;
  t.apply(x);
  return t.result();
}

/// \brief Converts a res equation into a PRES equation
/// \param eq A res equation
inline
pres_system::pres_equation res2pres(const res_equation& eq)
{
  return pres_system::pres_equation(eq.symbol(), pres_system::propositional_variable(eq.variable().name(), data::variable_list()), res2pres(eq.formula()));
}

/// \brief Converts a RES into a PRES
/// \param x A res expression
inline
pres_system::pres res2pres(const res_equation_system& x)
{
  data::data_specification data_spec;
  std::vector<pres_system::pres_equation> equations;
  for (const res_equation& eqn: x.equations())
  {
    equations.push_back(res2pres(eqn));
  }
  pres_system::propositional_variable_instantiation initial_state = atermpp::down_cast<pres_system::propositional_variable_instantiation>(res2pres(x.initial_state()));

  return pres_system::pres(data_spec, equations, initial_state);
}

} // namespace res

} // namespace mcrl2

#endif // MCRL2_RES_RES2PRES_H
