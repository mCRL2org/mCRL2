// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbes2besconversion.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_PBES2BESCONVERSION_H
#define MCRL2_PBES_PBES2BESCONVERSION_H

#include <cassert>
#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/detail/pbes_expression2boolean_expression_visitor.h"

namespace mcrl2 {

namespace pbes_system {

  /// \brief Converts a propositional variable into a boolean variable
  /// \param v A propositional variable
  inline
  bes::boolean_variable pbes2besconversion(const propositional_variable_instantiation& v)
  {
    return bes::boolean_variable(v.name());
  }

  /// \brief Converts a PBES expression into a boolean expression
  /// \param x A PBES expression
  inline
  bes::boolean_expression pbes2besconversion(const pbes_expression& x)
  {
    pbes_system::detail::pbes_expression2boolean_expression_visitor<pbes_system::pbes_expression> visitor;
    visitor.visit(x);
    return visitor.result();
  }

  /// \brief Converts a PBES equation into a boolean equation
  /// \param x A PBES equation
  inline
  bes::boolean_equation pbes2besconversion(const pbes_equation& eq)
  {
    return bes::boolean_equation(eq.symbol(), bes::boolean_variable(eq.variable().name()), pbes2besconversion(eq.formula()));
  }

  /// \brief Converts a PBES into a BES
  /// \param p A PBES
  /// \pre The PBES must be a BES
  inline
  bes::boolean_equation_system<> pbes2besconversion(const pbes<>& p)
  {
    assert(p.is_bes());

    atermpp::vector<bes::boolean_equation> equations;
    for (atermpp::vector<pbes_equation>::const_iterator i = p.equations().begin(); i != p.equations().end(); ++i)
    {
      equations.push_back(pbes2besconversion(*i));
    }
    bes::boolean_expression initial_state = pbes2besconversion(p.initial_state());

    return bes::boolean_equation_system<>(equations, initial_state);
  }

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBES2BESCONVERSION_H
