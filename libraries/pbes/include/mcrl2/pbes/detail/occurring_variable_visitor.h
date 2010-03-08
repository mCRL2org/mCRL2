// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/occurring_variable_visitor.h
/// \brief Add your file description here.

#ifndef MCRL2_PBES_DETAIL_OCCURRING_VARIABLE_VISITOR_H
#define MCRL2_PBES_DETAIL_OCCURRING_VARIABLE_VISITOR_H

#include "mcrl2/data/detail/data_functional.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/pbes_expression_visitor.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

/// Visitor for collecting the propositional variables that occur in a pbes expression.
struct occurring_variable_visitor: public pbes_expression_visitor<pbes_expression>
{
  std::set<propositional_variable_instantiation> variables;

  /// \brief Visit propositional_variable node
  /// \param e A PBES expression
  /// \param v A propositional variable instantiation
  /// \return The result of visiting the node
  bool visit_propositional_variable(const pbes_expression& /* e */, const propositional_variable_instantiation& v)
  {
    variables.insert(v);
    return stop_recursion;
  }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_OCCURRING_VARIABLE_VISITOR_H
