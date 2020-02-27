// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/lps/io.h"

namespace mcrl2::lps
{

atermpp::aterm linear_process_marker()
{
  return atermpp::aterm_appl(atermpp::function_symbol("linear_process_specification", 0));
}

atermpp::aterm_int linear_process_version()
{
  return atermpp::aterm_int(1);
}

template <typename LinearProcess, typename InitialProcessExpression>
atermpp::aterm_ostream& operator<<(atermpp::aterm_ostream& stream, const specification_base<LinearProcess, InitialProcessExpression>& spec)
{
  atermpp::aterm_stream_state state(stream);
  stream << data::detail::remove_index_impl;

  stream << linear_process_marker();
  stream << spec.data();
  stream << spec.action_labels();
  stream << spec.global_variables();
  stream << spec.process();
  stream << spec.initial_process();

  return stream;
}

template <typename LinearProcess, typename InitialProcessExpression>
atermpp::aterm_istream& operator>>(atermpp::aterm_istream& stream, specification_base<LinearProcess, InitialProcessExpression>& spec)
{
  atermpp::aterm_stream_state state(stream);
  stream >> data::detail::add_index_impl;

  atermpp::aterm marker;
  atermpp::aterm_int version;
  stream >> marker;

  if (marker != linear_process_marker())
  {
    throw mcrl2::runtime_error(std::string("Could not read a valid linear process specification."));
  }

  stream >> version;
  if (version != linear_process_version())
  {
    throw mcrl2::runtime_error(std::string("Unsupported linear process specification format version."));
  }

  data::data_specification data;
  process::action_label_list action_labels;
  std::set<data::variable> global_variables;
  LinearProcess process;
  InitialProcessExpression initial_process;

  stream >> data;
  stream >> action_labels;
  stream >> global_variables;
  stream >> process;
  stream >> initial_process;
  spec = specification_base<LinearProcess, InitialProcessExpression>(data, action_labels, global_variables, process, initial_process);

  return stream;
}

} // namespace mcrl2::lps
