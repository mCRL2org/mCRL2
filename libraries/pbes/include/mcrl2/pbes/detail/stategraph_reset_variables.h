// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/stategraph_reset_variables.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_STATEGRAPH_RESET_VARIABLES_H
#define MCRL2_PBES_DETAIL_STATEGRAPH_RESET_VARIABLES_H

#include "mcrl2/data/representative_generator.h"
#include "mcrl2/pbes/builder.h"





namespace mcrl2::pbes_system::detail {

// Applies the reset variables expansion to a pbes expression
struct reset_variable_builder: public pbes_expression_builder<reset_variable_builder>
{
  using super = pbes_expression_builder<reset_variable_builder>;
  using super::apply;

  std::vector<pbes_expression>::const_iterator i;

  reset_variable_builder(std::vector<pbes_expression>::const_iterator i_)
    : i(i_)
  {}

  template <class T>
  void apply(T& result, const propositional_variable_instantiation& x)
  {
    mCRL2log(log::debug) << "<reset>" << pbes_system::pp(x) << " -> " << pbes_system::pp(*i) << std::endl;
    result = *i++;
  }
};

} // namespace mcrl2::pbes_system::detail





#endif // MCRL2_PBES_DETAIL_STATEGRAPH_RESET_VARIABLES_H
