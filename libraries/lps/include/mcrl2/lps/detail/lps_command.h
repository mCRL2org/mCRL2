// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/lps_command.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_DETAIL_LPS_COMMAND_H
#define MCRL2_LPS_DETAIL_LPS_COMMAND_H

#include "mcrl2/lps/detail/lps_io.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/utilities/detail/command.h"

namespace mcrl2 {

namespace lps {

namespace detail {

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

  void execute()
  {
    lpsspec = lps::detail::load_lps(input_filename);
  }
};

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_LPS_COMMAND_H
