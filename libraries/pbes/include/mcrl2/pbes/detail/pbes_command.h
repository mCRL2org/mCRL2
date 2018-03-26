// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/pbes_command.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_PBES_COMMAND_H
#define MCRL2_PBES_DETAIL_PBES_COMMAND_H

#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/parse.h"
#include "mcrl2/pbes/detail/pbes_io.h"
#include "mcrl2/utilities/detail/command.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

/// \brief Command that operates on a PBES
struct pbes_command: public utilities::detail::command
{
  pbes_system::pbes pbesspec;

  pbes_command(const std::string& name,
               const std::string& input_filename,
               const std::string& output_filename,
               const std::vector<std::string>& options
              )
    : utilities::detail::command(name, input_filename, output_filename, options)
  {}

  void execute()
  {
    pbesspec = pbes_system::detail::load_pbes(input_filename);
  }
};

/// \brief PBES command that uses a rewrite strategy
struct pbes_rewriter_command: public pbes_command
{
  data::rewrite_strategy strategy;

  pbes_rewriter_command(const std::string& name,
                        const std::string& input_filename,
                        const std::string& output_filename,
                        const std::vector<std::string>& options,
                        data::rewrite_strategy strategy_
                       )
    : pbes_command(name, input_filename, output_filename, options),
      strategy(strategy_)
  {}
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PBES_COMMAND_H
