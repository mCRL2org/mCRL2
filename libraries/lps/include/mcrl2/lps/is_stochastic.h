// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/is_stochastic.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_IS_STOCHASTIC_H
#define MCRL2_LPS_IS_STOCHASTIC_H

#include "mcrl2/lps/traverser.h"

namespace mcrl2::lps
{

namespace detail {

struct is_stochastic_traverser: public lps::data_expression_traverser<is_stochastic_traverser>
{
  using super = lps::data_expression_traverser<is_stochastic_traverser>;
  using super::enter;
  using super::leave;
  using super::apply;

  bool result = false;
  
  void apply(const lps::stochastic_distribution& x)
  {
    if (x.is_defined())
    {
      result = true;
    }
  }

};

} // namespace detail

/// \brief Returns true if the LPS object x contains a stochastic distribution in one of its attributes.
template <typename T>
bool is_stochastic(const T& x)
{
  detail::is_stochastic_traverser f;
  f.apply(x);
  return f.result;
}

} // namespace mcrl2::lps

#endif // MCRL2_LPS_IS_STOCHASTIC_H
