// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/alphabet_reduce.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_ALPHABET_REDUCE_H
#define MCRL2_PROCESS_ALPHABET_REDUCE_H

#include "mcrl2/process/alphabet.h"
#include "mcrl2/process/alphabet_efficient.h"
#include "mcrl2/process/builder.h"
#include "mcrl2/process/detail/alphabet_push_allow.h"
#include "mcrl2/process/detail/alphabet_push_block.h"
#include "mcrl2/process/detail/pcrl_equation_cache.h"
#include "mcrl2/process/remove_equations.h"
#include "mcrl2/process/traverser.h"
#include "mcrl2/process/utility.h"
#include "mcrl2/utilities/logger.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <limits>
#include <sstream>

namespace mcrl2 {

namespace process {

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

  // cache the alphabet of pcrl equations and apply alphabet reduction to block({}, init)
  std::vector<process_equation>& equations = procspec.equations();
  std::map<process_identifier, multi_action_name_set> pcrl_equation_cache;
  data::set_identifier_generator id_generator;
  for (process_equation& equation: equations)
  {
    id_generator.add_identifier(equation.identifier().name());
  }
  pcrl_equation_cache = detail::compute_pcrl_equation_cache(equations, init);
  core::identifier_string_list empty_blockset;
  procspec.init() = push_block(empty_blockset, init, equations, id_generator, pcrl_equation_cache);

  // remove duplicate equations
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
