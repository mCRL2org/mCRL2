// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/lps_command.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_DETAIL_LPS_COMMAND_H
#define MCRL2_LPS_DETAIL_LPS_COMMAND_H

#include "mcrl2/data/rewriter.h"
#include "mcrl2/lps/io.h" 
#include "mcrl2/utilities/detail/command.h"

namespace mcrl2::lps::detail
{

struct lps_command: public utilities::detail::command
{
  lps::specification lpsspec;

  lps_command(const std::string& name,
              const std::string& input_filename,
              const std::string& output_filename,
              const std::vector<std::string>& options
             )
    : utilities::detail::command(name, input_filename, output_filename, options)
  {}

  void execute() override { lps::load_lps(lpsspec, input_filename); }
};

/// \brief PBES command that uses a rewrite strategy
struct lps_rewriter_command: public lps_command
{
  data::rewrite_strategy strategy;

  lps_rewriter_command(const std::string& name,
                       const std::string& input_filename,
                       const std::string& output_filename,
                       const std::vector<std::string>& options,
                       data::rewrite_strategy strategy_)
      : lps_command(name, input_filename, output_filename, options),
        strategy(strategy_)
  {}
};

} // namespace mcrl2::lps::detail

#endif // MCRL2_LPS_DETAIL_LPS_COMMAND_H
