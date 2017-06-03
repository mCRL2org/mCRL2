// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/tools/pbesabsinthe.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_TOOLS_PBESABSINTHE_H
#define MCRL2_PBES_TOOLS_PBESABSINTHE_H

#include "mcrl2/pbes/absinthe.h"
#include "mcrl2/pbes/absinthe_strategy.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/utilities/text_utility.h"

namespace mcrl2 {

namespace pbes_system {

void pbesabsinthe(const std::string& input_filename,
                  const std::string& output_filename,
                  const utilities::file_format& input_format,
                  const utilities::file_format& output_format,
                  const std::string& abstraction_file,
                  absinthe_strategy strategy,
                  bool print_used_function_symbols,
                  bool enable_logging
                 )
{
  // load the pbes
  pbes p;
  load_pbes(p, input_filename, input_format);

  if (print_used_function_symbols)
  {
    detail::print_used_function_symbols(p);
  }

  std::string abstraction_text;
  if (!abstraction_file.empty())
  {
    abstraction_text = utilities::read_text(abstraction_file);
  }

  bool over_approximation = (strategy == absinthe_over);

  absinthe_algorithm algorithm;
  if (enable_logging)
  {
    algorithm.enable_logging();
  }
  algorithm.run(p, abstraction_text, over_approximation);

  // save the result
  save_pbes(p, output_filename, output_format);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_TOOLS_PBESABSINTHE_H
