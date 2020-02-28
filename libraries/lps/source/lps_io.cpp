// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/lps/io.h"

#include "mcrl2/lps/stochastic_specification.h"
#include "mcrl2/lps/specification.h"

namespace mcrl2::lps
{

atermpp::aterm linear_process_specification_marker()
{
  return atermpp::aterm_appl(atermpp::function_symbol("linear_process_specification", 0));
}

atermpp::aterm_ostream& operator<<(atermpp::aterm_ostream& stream, const multi_action& action)
{
  stream << action.actions();
  stream << action.time();
  return stream;
}

atermpp::aterm_istream& operator>>(atermpp::aterm_istream& stream, multi_action& action)
{
  process::action_list actions;
  data::data_expression time;

  stream >> actions;
  stream >> time;

  action = multi_action(actions, time);
  return stream;
}

atermpp::aterm_ostream& operator<<(atermpp::aterm_ostream& stream, const deadlock_summand& summand)
{
  stream << summand.summation_variables();
  stream << summand.condition();
  stream << summand.deadlock().time();
  return stream;
}

atermpp::aterm_istream& operator>>(atermpp::aterm_istream& stream, deadlock_summand& summand)
{
  data::variable_list summation_variables;
  data::data_expression condition;
  data::data_expression time;

  stream >> summation_variables;
  stream >> condition;
  stream >> time;

  summand = deadlock_summand(summation_variables, condition, lps::deadlock(time));

  return stream;
}

atermpp::aterm_ostream& operator<<(atermpp::aterm_ostream& stream, const action_summand& summand)
{
  stream << summand.summation_variables();
  stream << summand.condition();
  stream << summand.multi_action();
  stream << summand.assignments();
  return stream;
}

atermpp::aterm_istream& operator>>(atermpp::aterm_istream& stream, action_summand& summand)
{
  data::variable_list summation_variables;
  data::data_expression condition;
  lps::multi_action multi_action;
  data::assignment_list assignments;

  stream >> summation_variables;
  stream >> condition;
  stream >> multi_action;
  stream >> assignments;

  summand = action_summand(summation_variables, condition, multi_action, assignments);
  return stream;
}

atermpp::aterm_ostream& operator<<(atermpp::aterm_ostream& stream, const stochastic_action_summand& summand)
{
  stream << summand.distribution();
  stream << static_cast<action_summand>(summand);
  return stream;
}

atermpp::aterm_istream& operator>>(atermpp::aterm_istream& stream, stochastic_action_summand& summand)
{
  stochastic_distribution distribution;
  action_summand action_summand;

  stream >> distribution;
  stream >> action_summand;

  summand = stochastic_action_summand(action_summand.summation_variables(),
    action_summand.condition(),
    action_summand.multi_action(),
    action_summand.assignments(),
    distribution);
  return stream;
}

template <typename ActionSummand>
atermpp::aterm_ostream& operator<<(atermpp::aterm_ostream& stream, const linear_process_base<ActionSummand>& lps)
{
  stream << lps.process_parameters();
  stream << lps.action_summands();
  stream << lps.deadlock_summands();

  return stream;
}

template <typename ActionSummand>
atermpp::aterm_istream& operator>>(atermpp::aterm_istream& stream, linear_process_base<ActionSummand>& lps)
{
  data::variable_list process_parameters;
  deadlock_summand_vector deadlock_summands;
  std::vector<ActionSummand> action_summands;

  stream >> process_parameters;
  stream >> deadlock_summands;
  stream >> action_summands;

  lps = linear_process_base<ActionSummand>(process_parameters, deadlock_summands, action_summands);
  return stream;
}

template <typename LinearProcess, typename InitialProcessExpression>
atermpp::aterm_ostream& operator<<(atermpp::aterm_ostream& stream, const specification_base<LinearProcess, InitialProcessExpression>& spec)
{
  atermpp::aterm_stream_state state(stream);
  stream << data::detail::remove_index_impl;

  stream << linear_process_specification_marker();
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

  try
  {
    atermpp::aterm marker;
    stream >> marker;

    if (marker != linear_process_specification_marker())
    {
      throw mcrl2::runtime_error("Stream does not contain a linear process specification (LPS).");
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
  }
  catch (std::exception& ex)
  {
    mCRL2log(log::error) << ex.what() << "\n";
    throw mcrl2::runtime_error(std::string("Error reading linear process specification (LPS)."));
  }

  return stream;
}

template atermpp::aterm_ostream& operator<<(atermpp::aterm_ostream& stream,
  const specification_base<mcrl2::lps::linear_process, mcrl2::lps::process_initializer>& spec);

template atermpp::aterm_ostream& operator<<(atermpp::aterm_ostream& stream,
  const specification_base<mcrl2::lps::stochastic_linear_process, mcrl2::lps::stochastic_process_initializer>& spec);

template atermpp::aterm_istream& operator>>(atermpp::aterm_istream& stream,
  specification_base<mcrl2::lps::stochastic_linear_process, mcrl2::lps::stochastic_process_initializer>& spec);

template atermpp::aterm_istream& operator>>(atermpp::aterm_istream& stream,
  specification_base<mcrl2::lps::linear_process, mcrl2::lps::process_initializer>& spec);

} // namespace mcrl2::lps
