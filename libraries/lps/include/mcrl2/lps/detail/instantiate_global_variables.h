// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/instantiate_global_variables.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_DETAIL_INSTANTIATE_GLOBAL_VARIABLES_H
#define MCRL2_LPS_DETAIL_INSTANTIATE_GLOBAL_VARIABLES_H

#include "mcrl2/data/representative_generator.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/lps/remove.h"
#include "mcrl2/lps/replace.h"
#include "mcrl2/lps/specification.h"

namespace mcrl2
{

namespace lps
{

namespace detail
{

template <typename Specification>
void instantiate_global_variables(Specification& spec)
{
  mCRL2log(log::verbose) << "Replacing free variables with dummy values." << std::endl;
  data::mutable_map_substitution<> sigma;
  data::representative_generator default_expression_generator(spec.data());
  for (const data::variable& v : spec.global_variables())
  {
    data::data_expression d = default_expression_generator(v.sort());
    if (!d.defined())
    {
      throw mcrl2::runtime_error("Error in lps::instantiate_global_variables: could not instantiate " + data::pp(v) + ". ");
    }
    sigma[v] = d;
  }
  lps::replace_free_variables(spec.process(), sigma);
  spec.initial_process() = lps::replace_free_variables(spec.initial_process(), sigma);
  spec.global_variables().clear();
}

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_INSTANTIATE_GLOBAL_VARIABLES_H
