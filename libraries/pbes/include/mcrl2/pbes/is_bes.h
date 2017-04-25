// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes/include/mcrl2/pbes/is_bes.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_IS_BES_H
#define MCRL2_PBES_IS_BES_H

#include "mcrl2/pbes/traverser.h"

namespace mcrl2 {

namespace pbes_system {

/// \cond INTERNAL_DOCS
/// \brief Visitor for checking if a pbes object is in BES form.
struct is_bes_traverser: public pbes_expression_traverser<is_bes_traverser>
{
  typedef pbes_expression_traverser<is_bes_traverser> super;
  using super::enter;
  using super::leave;
  using super::apply;

  bool result;

  is_bes_traverser()
    : result(true)
  {}

  void enter(const forall& /* x */)
  {
    result = false;
  }

  void enter(const exists& /* x */)
  {
    result = false;
  }

  void enter(const data::data_expression& x)
  {
    if (x != data::sort_bool::true_() && x != data::sort_bool::false_())
    {
      result = false;
    }
  }

  void enter(const propositional_variable_instantiation& x)
  {
    if (result)
    {
      result = x.parameters().empty();
    }
  }

  void enter(const pbes_equation& x)
  {
    if (result)
    {
      result = x.variable().parameters().empty();
    }
  }
};
/// \endcond

/// \brief Returns true if a PBES object is in BES form.
/// \param x a PBES object
template <typename T>
bool is_bes(const T& x)
{
  is_bes_traverser f;
  f.apply(x);
  return f.result;
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_IS_BES_H
