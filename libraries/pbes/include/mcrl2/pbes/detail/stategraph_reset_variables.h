// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/stategraph_reset_variables.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_STATEGRAPH_RESET_VARIABLES_H
#define MCRL2_PBES_DETAIL_STATEGRAPH_RESET_VARIABLES_H

#include <functional>
#include "mcrl2/data/representative_generator.h"
#include "mcrl2/pbes/builder.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

// Applies the reset variables expansion to a pbes expression
struct reset_variable_builder: public pbes_expression_builder<reset_variable_builder>
{
  typedef pbes_expression_builder<reset_variable_builder> super;
  using super::apply;

  std::vector<pbes_expression>::const_iterator i;

  reset_variable_builder(std::vector<pbes_expression>::const_iterator i_)
    : i(i_)
  {}

  pbes_expression apply(const propositional_variable_instantiation& x)
  {
    mCRL2log(log::debug, "stategraph") << "<reset>" << pbes_system::pp(x) << " -> " << pbes_system::pp(*i) << std::endl;
    return *i++;
  }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_STATEGRAPH_RESET_VARIABLES_H
