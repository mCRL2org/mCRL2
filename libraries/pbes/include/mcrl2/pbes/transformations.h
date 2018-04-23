// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/transformations.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_TRANSFORMATIONS_H
#define MCRL2_PBES_TRANSFORMATIONS_H

#include "mcrl2/pbes/builder.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

struct order_quantified_variables_builder: public pbes_expression_builder<order_quantified_variables_builder>
{
  typedef pbes_expression_builder<order_quantified_variables_builder> super;
  using super::apply;

  const data::data_specification& dataspec;

  order_quantified_variables_builder(const data::data_specification& dataspec_)
    : dataspec(dataspec_)
  {}

  pbes_expression apply(const forall& x)
  {
    return make_forall(data::order_variables_to_optimise_enumeration(x.variables(), dataspec), apply(x.body()));
  }

  pbes_expression apply(const exists& x)
  {
    return make_exists(data::order_variables_to_optimise_enumeration(x.variables(), dataspec), apply(x.body()));
  }
};

} // namespace detail

inline
pbes_expression order_quantified_variables(const pbes_expression& x, const data::data_specification& dataspec)
{
  detail::order_quantified_variables_builder f(dataspec);
  return f.apply(x);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_TRANSFORMATIONS_H
