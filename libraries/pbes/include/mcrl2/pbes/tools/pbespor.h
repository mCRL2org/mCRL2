// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/tools/pbespor.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_TOOLS_PBESPOR_H
#define MCRL2_PBES_TOOLS_PBESPOR_H

#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/partial_order_reduction.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2 {

namespace pbes_system {

void pbespor(const std::string& input_filename,
             const std::string& output_filename,
             const utilities::file_format& input_format,
             const utilities::file_format& output_format,
             data::rewrite_strategy rewrite_strategy
            )
{
  pbes p;
  load_pbes(p, input_filename, input_format);
  algorithms::normalize(p);

  partial_order_reduction_algorithm algorithm(p, rewrite_strategy);
  algorithm.print();

  algorithm.explore(
    algorithm.initial_state(),

    // emit_node
    [&](const propositional_variable_instantiation& X)
    {
      std::cout << "emit node " << X << std::endl;
    },

    // emit_edge
    [&](const propositional_variable_instantiation& X, const propositional_variable_instantiation& Y)
    {
      std::cout << "emit edge " << X << " -> " << Y << std::endl;
    }
  );

//  save_pbes(p, output_filename, output_format, false);
//  if (!p.is_well_typed())
//  {
//    mCRL2log(log::error) << "pbespor error: not well typed!" << std::endl;
//    mCRL2log(log::error) << pp(p) << std::endl;
//  }
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_TOOLS_PBESPOR_H
