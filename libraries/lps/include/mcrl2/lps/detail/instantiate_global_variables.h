// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
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
#include "mcrl2/lps/remove.h"

namespace mcrl2::lps::detail
{

/// \brief Applies a global variable substitution to an LPS.
template <typename Specification>
void replace_global_variables(Specification& lpsspec, const data::mutable_map_substitution<>& sigma)
{
  lps::replace_free_variables(lpsspec.process(), sigma);
  lpsspec.initial_process() = lps::replace_free_variables(lpsspec.initial_process(), sigma);
  lpsspec.global_variables().clear();
}

/// \brief Eliminates the global variables of an LPS, by substituting
/// a constant value for them. If no constant value is found for one of the variables,
/// an exception is thrown.
template <typename Specification>
data::mutable_map_substitution<> instantiate_global_variables(Specification& lpsspec)
{
  data::mutable_map_substitution<> sigma;

  mCRL2log(log::verbose) << "Replacing global variables with dummy values." << std::endl;
  data::representative_generator default_expression_generator(lpsspec.data());
  for (const data::variable& v : lpsspec.global_variables())
  {
    data::data_expression d = default_expression_generator(v.sort());
    if (!d.defined())
    {
      throw mcrl2::runtime_error("Error in lps::instantiate_global_variables: could not instantiate " + data::pp(v) + ". ");
    }
    sigma[v] = d;
  }

  mCRL2log(log::debug) << "instantiating global LPS variables " << sigma << std::endl;
  replace_global_variables(lpsspec, sigma);

  return sigma;
}

} // namespace mcrl2::lps::detail





#endif // MCRL2_LPS_DETAIL_INSTANTIATE_GLOBAL_VARIABLES_H
