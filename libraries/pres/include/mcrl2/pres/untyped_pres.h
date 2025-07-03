// Author(s): Jan Friso Groote. Based on pbes/untyped_pbes.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/untyped_pres.h
/// \brief add your file description here.

#ifndef MCRL2_PRES_UNTYPED_PRES_H
#define MCRL2_PRES_UNTYPED_PRES_H

#include "mcrl2/data/untyped_data_specification.h"
#include "mcrl2/pres/pres.h"



namespace mcrl2::pres_system {

struct untyped_pres
{
  data::untyped_data_specification dataspec;
  data::variable_list global_variables;
  std::vector<pres_equation> equations;
  propositional_variable_instantiation initial_state;

  pres construct_pres() const
  {
    pres result;
    data::data_specification d=dataspec.construct_data_specification();
    d.add_context_sort(data::sort_real::real_());
    result.set_data(d);
    result.global_variables() = std::set<data::variable>(global_variables.begin(), global_variables.end());
    result.equations() = equations;
    result.initial_state() = initial_state;
    return result;
  }
};

} // namespace mcrl2::pres_system



#endif // MCRL2_PRES_UNTYPED_PRES_H
