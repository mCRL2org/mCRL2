// Author(s): Jan Friso Groote. Based on pbes/detail/occurring_variable_visitor.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/detail/occurring_variable_visitor.h
/// \brief Add your file description here.

#ifndef MCRL2_PRES_DETAIL_OCCURRING_VARIABLE_VISITOR_H
#define MCRL2_PRES_DETAIL_OCCURRING_VARIABLE_VISITOR_H

#include "mcrl2/pres/traverser.h"





namespace mcrl2::pres_system::detail
{

struct occurring_variable_visitor: public pres_expression_traverser<occurring_variable_visitor>
{
  using super = pres_expression_traverser<occurring_variable_visitor>;
  using super::enter;
  using super::leave;
  using super::apply;

  std::set<propositional_variable_instantiation> variables;

  void apply(const propositional_variable_instantiation& x)
  {
    variables.insert(x);
  }
};

} // namespace mcrl2::pres_system::detail





#endif // MCRL2_PRES_DETAIL_OCCURRING_VARIABLE_VISITOR_H
