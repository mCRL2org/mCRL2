// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/bes2pbes.h
/// \brief Contains functions to transfrom a BES into a PBES.

#ifndef MCRL2_BES_BES2PBES_H
#define MCRL2_BES_BES2PBES_H

#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/bes/detail/boolean_expression2pbes_expression_traverser.h"

namespace mcrl2
{

namespace bes
{

/// \brief Converts a boolean variable into a propositional variable
/// \param v A boolean variable
inline
pbes_system::propositional_variable_instantiation bes2pbes(const boolean_variable& v)
{
  return pbes_system::propositional_variable_instantiation(v.name(), data::data_expression_list());
}

/// \brief Converts a boolean expression into a PBES expression
/// \param x A boolean expression
inline
pbes_system::pbes_expression bes2pbes(const boolean_expression& x)
{
  bes::detail::boolean_expression2pbes_expression_traverser t;
  t.apply(x);
  return t.result();
}

/// \brief Converts a boolean equation into a PBES equation
/// \param eq A boolean equation
inline
pbes_system::pbes_equation bes2pbes(const boolean_equation& eq)
{
  return pbes_system::pbes_equation(eq.symbol(), pbes_system::propositional_variable(eq.variable().name(), data::variable_list()), bes2pbes(eq.formula()));
}

/// \brief Converts a BES into a PBES
/// \param x A boolean expression
inline
pbes_system::pbes bes2pbes(const boolean_equation_system& x)
{
  data::data_specification data_spec;
  std::vector<pbes_system::pbes_equation> equations;
  for (const boolean_equation& eqn: x.equations())
  {
    equations.push_back(bes2pbes(eqn));
  }
  pbes_system::propositional_variable_instantiation initial_state = atermpp::down_cast<pbes_system::propositional_variable_instantiation>(bes2pbes(x.initial_state()));

  return pbes_system::pbes(data_spec, equations, initial_state);
}

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_BES2PBES_H
