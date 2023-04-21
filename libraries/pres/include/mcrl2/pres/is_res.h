// Author(s): Jan Friso Groote. Based on pbes/is_bes.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pres/include/mcrl2/pres/is_bes.h
/// \brief add your file description here.

#ifndef MCRL2_PRES_IS_BES_H
#define MCRL2_PRES_IS_BES_H

#include "mcrl2/data/consistency.h"
#include "mcrl2/pres/traverser.h"

namespace mcrl2 {

namespace pres_system {

/// \cond INTERNAL_DOCS
/// \brief Visitor for checking if a pres object is in BES form.
struct is_bes_traverser: public pres_expression_traverser<is_bes_traverser>
{
  typedef pres_expression_traverser<is_bes_traverser> super;
  using super::enter;
  using super::leave;
  using super::apply;

  bool result;

  is_bes_traverser()
    : result(true)
  {}

  void enter(const minall& /* x */)
  {
    result = false;
  }

  void enter(const maxall& /* x */)
  {
    result = false;
  }

  void enter(const data::data_expression& x)
  {
    if (x != data::true_() && x != data::false_())
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

  void enter(const pres_equation& x)
  {
    if (result)
    {
      result = x.variable().parameters().empty();
    }
  }
};
/// \endcond

/// \brief Returns true if a PRES object is in BES form.
/// \param x a PRES object
template <typename T>
bool is_bes(const T& x)
{
  is_bes_traverser f;
  f.apply(x);
  return f.result;
}

} // namespace pres_system

} // namespace mcrl2

#endif // MCRL2_PRES_IS_BES_H
