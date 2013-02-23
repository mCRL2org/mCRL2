// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/alphabet.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_ALPHABET_H
#define MCRL2_PROCESS_ALPHABET_H

#include <algorithm>
#include <iterator>
#include <iostream>
#include <sstream>
#include "mcrl2/data/substitutions.h"
#include "mcrl2/process/detail/alphabet_push_allow.h"
#include "mcrl2/process/detail/alphabet_push_block.h"
#include "mcrl2/process/detail/alphabet_traverser.h"
#include "mcrl2/process/detail/expand_process_instance_assignments.h"
#include "mcrl2/process/builder.h"
#include "mcrl2/process/traverser.h"
#include "mcrl2/process/utility.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2 {

namespace process {

namespace detail {

struct alphabet_reduce_builder: public process_expression_builder<alphabet_reduce_builder>
{
  typedef process_expression_builder<alphabet_reduce_builder> super;
  using super::enter;
  using super::leave;
  using super::operator();

#if BOOST_MSVC
#include "mcrl2/core/detail/builder_msvc.inc.h"
#endif

  const std::vector<process_equation>& equations;

  alphabet_reduce_builder(const std::vector<process_equation>& equations_)
    : equations(equations_)
  {}

  process_expression operator()(const process::allow& x)
  {
    return push_allow(x.operand(), x.allow_set(), equations);
  }

  process_expression operator()(const process::block& x)
  {
    return push_block(x.block_set(), x.operand(), equations);
  }
};

inline
process_expression alphabet_reduce(const process_expression& x, const std::vector<process_equation>& equations)
{
  alphabet_reduce_builder f(equations);
  return f(x);
}

} // detail

inline
void alphabet_reduce(process_specification& procspec)
{
  expand_process_instance_assignments(procspec);
  procspec.init() = detail::alphabet_reduce(procspec.init(), procspec.equations());
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_ALPHABET_H
