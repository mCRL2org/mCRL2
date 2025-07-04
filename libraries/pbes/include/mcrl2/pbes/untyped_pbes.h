// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/untyped_pbes.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_UNTYPED_PBES_H
#define MCRL2_PBES_UNTYPED_PBES_H

#include "mcrl2/data/untyped_data_specification.h"
#include "mcrl2/pbes/pbes.h"

namespace mcrl2::pbes_system {

struct untyped_pbes
{
  data::untyped_data_specification dataspec;
  data::variable_list global_variables;
  std::vector<pbes_equation> equations;
  propositional_variable_instantiation initial_state;

  pbes construct_pbes() const
  {
    pbes result;
    result.data() = dataspec.construct_data_specification();
    result.global_variables() = std::set<data::variable>(global_variables.begin(), global_variables.end());
    result.equations() = equations;
    result.initial_state() = initial_state;
    return result;
  }
};

} // namespace mcrl2::pbes_system



#endif // MCRL2_PBES_UNTYPED_PBES_H
