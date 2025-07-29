// Author(s): Jan Friso Groote. Based on pbes/detail/has_propositional_variables.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/detail/has_propositional_variables.h
/// \brief add your file description here.

#ifndef MCRL2_PRES_DETAIL_HAS_PROPOSITIONAL_VARIABLES_H
#define MCRL2_PRES_DETAIL_HAS_PROPOSITIONAL_VARIABLES_H

#include "mcrl2/pres/traverser.h"





namespace mcrl2::pres_system::detail {

struct has_propositional_variables_traverser: public pres_expression_traverser<has_propositional_variables_traverser>
{
  using super = pres_expression_traverser<has_propositional_variables_traverser>;
  using super::enter;
  using super::leave;
  using super::apply;

  bool result = false;

  void enter(const propositional_variable_instantiation&)
  {
    result = true;
  }
};

inline
bool has_propositional_variables(const pres_expression& x)
{
  has_propositional_variables_traverser f;
  f.apply(x);
  return f.result;
}

} // namespace mcrl2::pres_system::detail





#endif // MCRL2_PRES_DETAIL_HAS_PROPOSITIONAL_VARIABLES_H
