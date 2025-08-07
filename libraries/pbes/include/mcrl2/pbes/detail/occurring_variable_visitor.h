// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/occurring_variable_visitor.h
/// \brief Add your file description here.

#ifndef MCRL2_PBES_DETAIL_OCCURRING_VARIABLE_VISITOR_H
#define MCRL2_PBES_DETAIL_OCCURRING_VARIABLE_VISITOR_H

#include "mcrl2/pbes/traverser.h"

namespace mcrl2::pbes_system::detail
{

struct occurring_variable_visitor: public pbes_expression_traverser<occurring_variable_visitor>
{
  using super = pbes_expression_traverser<occurring_variable_visitor>;
  using super::enter;
  using super::leave;
  using super::apply;

  std::set<propositional_variable_instantiation> variables;

  void apply(const propositional_variable_instantiation& x)
  {
    variables.insert(x);
  }
};

} // namespace mcrl2::pbes_system::detail

#endif // MCRL2_PBES_DETAIL_OCCURRING_VARIABLE_VISITOR_H
