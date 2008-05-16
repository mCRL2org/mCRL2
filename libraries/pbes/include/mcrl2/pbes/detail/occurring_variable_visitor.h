// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING.
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
#include "mcrl2/pbes/pbes_expression_visitor.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

/// Visitor for collecting the propositional variables that occur in a pbes expression.
struct occurring_variable_visitor: public pbes_expression_visitor
{
  std::set<propositional_variable_instantiation> variables;

  bool visit_propositional_variable(const pbes_expression& e, const propositional_variable_instantiation& v)
  {
    variables.insert(v);
    return stop_recursion;
  }
};  

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_OCCURRING_VARIABLE_VISITOR_H
