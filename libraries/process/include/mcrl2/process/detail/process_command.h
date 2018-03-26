// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/detail/process_command.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_DETAIL_PROCESS_COMMAND_H
#define MCRL2_PROCESS_DETAIL_PROCESS_COMMAND_H

#include "mcrl2/data/rewriter.h"
#include "mcrl2/process/parse.h"
#include "mcrl2/process/detail/process_io.h"
#include "mcrl2/utilities/detail/command.h"

namespace mcrl2 {

namespace process {

namespace detail {

struct process_command: public utilities::detail::command
{
  process::process_specification procspec;

  process_command(const std::string& name,
                  const std::string& input_filename,
                  const std::string& output_filename,
                  const std::vector<std::string>& options
                 )
    : utilities::detail::command(name, input_filename, output_filename, options)
  {}

  void execute()
  {
    procspec = detail::parse_process_specification(input_filename);
  }
};

/// \brief Process command that uses a rewrite strategy
struct process_rewriter_command: public process_command
{
  data::rewrite_strategy strategy;

  process_rewriter_command(const std::string& name,
                           const std::string& input_filename,
                           const std::string& output_filename,
                           const std::vector<std::string>& options,
                           data::rewrite_strategy strategy_
                          )
    : process_command(name, input_filename, output_filename, options),
      strategy(strategy_)
  {}
};

} // namespace detail

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_DETAIL_PROCESS_COMMAND_H
