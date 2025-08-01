// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/has_propositional_variables.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_HAS_PROPOSITIONAL_VARIABLES_H
#define MCRL2_PBES_DETAIL_HAS_PROPOSITIONAL_VARIABLES_H

#include "mcrl2/pbes/traverser.h"

namespace mcrl2::pbes_system::detail
{

struct has_propositional_variables_traverser: public pbes_expression_traverser<has_propositional_variables_traverser>
{
  using super = pbes_expression_traverser<has_propositional_variables_traverser>;
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
bool has_propositional_variables(const pbes_expression& x)
{
  has_propositional_variables_traverser f;
  f.apply(x);
  return f.result;
}

} // namespace mcrl2::pbes_system::detail

#endif // MCRL2_PBES_DETAIL_HAS_PROPOSITIONAL_VARIABLES_H
