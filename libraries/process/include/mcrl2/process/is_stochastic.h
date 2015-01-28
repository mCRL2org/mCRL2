// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/is_stochastic.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_IS_STOCHASTIC_H
#define MCRL2_PROCESS_IS_STOCHASTIC_H

#include "mcrl2/process/traverser.h"

namespace mcrl2 {

namespace process {

namespace detail {

struct is_stochastic_traverser: public process::data_expression_traverser<is_stochastic_traverser>
{
  typedef process::data_expression_traverser<is_stochastic_traverser> super;
  using super::enter;
  using super::leave;
  using super::apply;

  bool result;

  is_stochastic_traverser()
    : result(false)
  {}

  void apply(const process::stochastic_operator& x)
  {
    if (!x.variables().empty())
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
  f(x);
  return f.result;
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_IS_STOCHASTIC_H
