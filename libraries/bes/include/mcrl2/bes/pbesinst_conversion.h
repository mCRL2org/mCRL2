// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/pbesinst_conversion.h
/// \brief add your file description here.

#ifndef MCRL2_BES_PBESINST_CONVERSION_H
#define MCRL2_BES_PBESINST_CONVERSION_H

#include <cassert>
#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/bes/detail/pbes_expression2boolean_expression_traverser.h"
#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/pbes.h"

namespace mcrl2
{

namespace bes
{

/// \brief Converts a propositional variable into a boolean variable
/// \param v A propositional variable
inline
bes::boolean_variable pbesinst_conversion(const pbes_system::propositional_variable_instantiation& v)
{
  return bes::boolean_variable(v.name());
}

/// \brief Converts a PBES expression into a boolean expression
/// \param x A PBES expression
inline
bes::boolean_expression pbesinst_conversion(const pbes_system::pbes_expression& x)
{
  return bes::pbes_expression2boolean_expression(x);
}

/// \brief Converts a PBES equation into a boolean equation
/// \param eq A PBES equation
inline
bes::boolean_equation pbesinst_conversion(const pbes_system::pbes_equation& eq)
{
  return bes::boolean_equation(eq.symbol(), bes::boolean_variable(eq.variable().name()), pbesinst_conversion(eq.formula()));
}

/// \brief Converts a PBES into a BES
/// \param p A PBES
/// \pre The PBES must be a BES
inline
bes::boolean_equation_system pbesinst_conversion(const pbes_system::pbes& p)
{
  assert(pbes_system::algorithms::is_bes(p));

  std::vector<bes::boolean_equation> equations;
  for (const pbes_system::pbes_equation& eqn: p.equations())
  {
    equations.push_back(pbesinst_conversion(eqn));
  }
  bes::boolean_expression initial_state = pbesinst_conversion(p.initial_state());

  return bes::boolean_equation_system(equations, initial_state);
}

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_PBESINST_CONVERSION_H
