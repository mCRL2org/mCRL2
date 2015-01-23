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

#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/traverser.h"

namespace mcrl2
{

namespace pbes_system
{

namespace detail
{

struct occurring_variable_visitor: public pbes_expression_traverser<occurring_variable_visitor>
{
  typedef pbes_expression_traverser<occurring_variable_visitor> super;
  using super::enter;
  using super::leave;
  using super::apply;

  std::set<propositional_variable_instantiation> variables;

  void apply(const propositional_variable_instantiation& x)
  {
    variables.insert(x);
  }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_OCCURRING_VARIABLE_VISITOR_H
