// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lts/detail/lts_load.h
/// \brief add your file description here.

#ifndef MCRL2_LTS_DETAIL_LTS_LOAD_H
#define MCRL2_LTS_DETAIL_LTS_LOAD_H

#include "mcrl2/lts/lts_io.h"
#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/utilities/tool.h"

namespace mcrl2 {

namespace lts {

namespace detail {

inline
void add_options(utilities::interface_description& desc)
{
  desc.add_option("data", utilities::make_file_argument("FILE"),
                  "use FILE as the data and action specification. "
                  "FILE must be a .mcrl2 file which does not contain an init clause. ", 'D');

  desc.add_option("lps", utilities::make_file_argument("FILE"),
                  "use FILE for the data and action specification. "
                  "FILE must be a .lps file. ", 'l');

  desc.add_option("mcrl2", utilities::make_file_argument("FILE"),
                  "use FILE as the data and action specification for the LTS. "
                  "FILE must be a .mcrl2 file. ", 'm');
}

// Load an LTS from ltsfilename, with additional files specified in the command line parser.
// TODO: this incomprehensible interface for loading LTSs needs improvement.
inline
void load_lts(const utilities::command_line_parser& parser, const std::string& ltsfilename, lts_lts_t& result)
{
  data_file_type_t data_file_type = data_file_type_t::none_e;
  std::string data_file;

  if (parser.options.count("data"))
  {
    if (1 < parser.options.count("data"))
    {
      mCRL2log(log::warning) << "multiple data specification files are specified; can only use one.\n";
    }
    data_file_type = data_file_type_t::data_e;
    data_file = parser.option_argument("data");
  }

  if (parser.options.count("lps"))
  {
    if (1 < parser.options.count("lps") || data_file_type != data_file_type_t::none_e)
    {
      mCRL2log(log::warning) << "multiple data specification files are specified; can only use one.\n";
    }

    data_file_type = data_file_type_t::lps_e;
    data_file = parser.option_argument("lps");
  }

  if (parser.options.count("mcrl2"))
  {
    if (1 < parser.options.count("mcrl2") || data_file_type != data_file_type_t::none_e)
    {
      mCRL2log(log::warning) << "multiple data specification files are specified; can only use one.\n";
    }

    data_file_type = data_file_type_t::mcrl2_e;
    data_file = parser.option_argument("mcrl2");
  }

  lts_type input_type = detail::guess_format(ltsfilename);
  load_lts(result, ltsfilename, input_type, data_file_type, data_file);
}

// extracts a specification from an LTS
inline
lps::specification extract_specification(const lts_lts_t& l)
{
  lps::action_summand_vector action_summands;
  data::variable process_parameter("x", data::sort_pos::pos());
  data::variable_list process_parameters({ process_parameter });
  std::set<data::variable> global_variables;
  // Add a single delta.
  lps::deadlock_summand_vector deadlock_summands(1, lps::deadlock_summand(data::variable_list(), data::sort_bool::true_(), lps::deadlock()));
  lps::linear_process lps(process_parameters, deadlock_summands, lps::action_summand_vector());
  lps::process_initializer initial_process(data::assignment_list({ data::assignment(process_parameter, data::sort_pos::pos(l.initial_state() + 1)) }));
  return lps::specification(l.data(), l.action_label_declarations(), global_variables, lps, initial_process);
}

} // namespace detail

} // namespace lts

} // namespace mcrl2

#endif // MCRL2_LTS_DETAIL_LTS_LOAD_H
