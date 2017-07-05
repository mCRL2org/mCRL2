// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/instantiate_global_variables.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_INSTANTIATE_GLOBAL_VARIABLES_H
#define MCRL2_PBES_DETAIL_INSTANTIATE_GLOBAL_VARIABLES_H

#include "mcrl2/data/replace.h"
#include "mcrl2/data/representative_generator.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/replace.h"

namespace mcrl2
{

namespace pbes_system
{

namespace detail
{

/// \brief Attempts to eliminate the free variables of a PBES, by substituting
/// a constant value for them. If no constant value is found for one of the variables,
/// an exception is thrown.
inline
void instantiate_global_variables(pbes& p)
{
  if (p.global_variables().empty())
  {
    return;
  }
  data::mutable_map_substitution<> sigma;
  data::representative_generator default_expression_generator(p.data());
  std::set<data::variable> to_be_removed;
  for (const data::variable& v: p.global_variables())
  {
    data::data_expression d = default_expression_generator(v.sort());
    if (!d.defined())
    {
      throw mcrl2::runtime_error("Error in pbes::instantiate_global_variables: could not instantiate " + data::pp(v));
    }
    sigma[v] = d;
    to_be_removed.insert(v);
  }
  pbes_system::replace_free_variables(p.equations(), sigma);
  p.initial_state() = pbes_system::replace_free_variables(p.initial_state(), sigma);
  p.global_variables().clear();
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_INSTANTIATE_GLOBAL_VARIABLES_H
