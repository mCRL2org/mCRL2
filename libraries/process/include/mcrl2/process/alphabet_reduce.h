// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/alphabet_reduce.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_ALPHABET_REDUCE_H
#define MCRL2_PROCESS_ALPHABET_REDUCE_H

#include <algorithm>
#include <iterator>
#include <iostream>
#include <limits>
#include <sstream>
#include "mcrl2/process/alphabet.h"
#include "mcrl2/process/expand_process_instance_assignments.h"
#include "mcrl2/process/builder.h"
#include "mcrl2/process/eliminate_unused_equations.h"
#include "mcrl2/process/remove_equations.h"
#include "mcrl2/process/traverser.h"
#include "mcrl2/process/utility.h"
#include "mcrl2/process/alphabet_efficient.h"
#include "mcrl2/process/detail/alphabet_push_allow.h"
#include "mcrl2/process/detail/alphabet_push_block.h"
#include "mcrl2/process/detail/pcrl_equation_cache.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2 {

namespace process {

namespace detail {

struct alphabet_push_builder: public process_expression_builder<alphabet_push_builder>
{
  typedef process_expression_builder<alphabet_push_builder> super;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::update;

  std::vector<process_equation>& equations;
  process_expression init;
  data::set_identifier_generator id_generator;
  std::map<process_identifier, multi_action_name_set> pcrl_equation_cache;

  alphabet_push_builder(std::vector<process_equation>& equations_, const process_expression& init_)
    : equations(equations_), init(init_)
  {
    for (process_equation& equation: equations_)
    {
      id_generator.add_identifier(equation.identifier().name());
    }
    eliminate_unused_equations(equations, init);
    pcrl_equation_cache = detail::compute_pcrl_equation_cache(equations);
  }

  process_expression apply(const process::allow& x)
  {
    return push_allow(x.operand(), x.allow_set(), equations, id_generator, pcrl_equation_cache);
  }

  process_expression apply(const process::block& x)
  {
    return push_block(x.block_set(), x.operand(), equations, id_generator, pcrl_equation_cache);
  }
};

inline
process_expression alphabet_reduce(const process_expression& x, std::vector<process_equation>& equations)
{
  alphabet_push_builder f(equations, x);
  return f.apply(x);
}

} // detail

/// \brief Applies alphabet reduction to a process specification.
/// \param procspec A process specification
/// \param duplicate_equation_limit If the number of equations is less than
/// duplicate_equation_limit, the remove duplicate equations procedure is applied.
/// Note that this procedure is not efficient, so it should not be used if the number
/// of equations is big.
inline
void alphabet_reduce(process_specification& procspec, std::size_t duplicate_equation_limit = (std::numeric_limits<size_t>::max)())
{
  mCRL2log(log::verbose) << "applying alphabet reduction..." << std::endl;
  process_expression init = procspec.init();
  if (is_pcrl(init))
  {
    init = expand_process_instance_assignments(init, procspec.equations());
  }
  process_expression init_reduced = detail::alphabet_reduce(init, procspec.equations());
  if (init != init_reduced)
  {
    procspec.init() = init_reduced;
  }
  if (procspec.equations().size() < duplicate_equation_limit)
  {
    mCRL2log(log::debug) << "removing duplicate equations..." << std::endl;
    remove_duplicate_equations(procspec);
    mCRL2log(log::debug) << "removing duplicate equations finished" << std::endl;
  }
  mCRL2log(log::debug) << "alphabet reduction finished" << std::endl;
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_ALPHABET_REDUCE_H
