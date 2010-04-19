// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/bes_parse.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_BES_PARSE_H
#define MCRL2_PBES_BES_PARSE_H

#include "mcrl2/pbes/parse.h"
#include "mcrl2/pbes/bes.h"
#include "mcrl2/pbes/detail/pbes_expression2boolean_expression_visitor.h"

namespace mcrl2 {

namespace bes {

  /// \brief Converts a pbes expression to a boolean expression.
  /// Throws an mcrl2_error if it is not a valid boolean expression.
  inline
  boolean_expression pbes_expression2boolean_expression(const pbes_system::pbes_expression& x)
  {
    pbes_system::detail::pbes_expression2boolean_expression_visitor<pbes_system::pbes_expression> visitor;
    visitor.visit(x);
    return visitor.result();
  }

  /// \brief Reads a boolean equation system from an input stream.
  /// \param from An input stream
  /// \param b A boolean equation system
  /// \return The input stream
  template <typename Container>
  std::istream& operator>>(std::istream& from, boolean_equation_system<Container>& b)
  {
    pbes_system::pbes<> p;
    from >> p;
    if (!p.is_bes())
    {
      throw mcrl2::runtime_error("parsing of boolean equation system failed: it is not a BES!");
    }

    atermpp::vector<boolean_equation> equations;
    for (typename atermpp::vector<pbes_system::pbes_equation>::const_iterator i = p.equations().begin(); i != p.equations().end(); ++i)
    {
      boolean_variable v(i->variable().name());
      boolean_expression rhs = pbes_expression2boolean_expression(i->formula());
      equations.push_back(boolean_equation(i->symbol(), v, rhs));
    }
    
    boolean_expression init = pbes_expression2boolean_expression(p.initial_state());
    b = boolean_equation_system<Container>(Container(equations.begin(), equations.end()), init);
    return from;
  }

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_PBES_BES_PARSE_H
