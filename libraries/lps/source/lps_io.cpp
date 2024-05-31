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
  return atermpp::aterm(atermpp::function_symbol("linear_process_specification", 0));
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

atermpp::aterm_ostream& operator<<(atermpp::aterm_ostream& stream, const stochastic_action_summand& summand)
{
  stream << summand.distribution();
  stream << summand.summation_variables();
  stream << summand.condition();
  stream << summand.multi_action();
  stream << summand.assignments();
  return stream;
}

atermpp::aterm_istream& operator>>(atermpp::aterm_istream& stream, stochastic_action_summand& summand)
{
  stochastic_distribution distribution;
  data::variable_list summation_variables;
  data::data_expression condition;
  lps::multi_action multi_action;
  data::assignment_list assignments;

  stream >> distribution;
  stream >> summation_variables;
  stream >> condition;
  stream >> multi_action;
  stream >> assignments;

  summand = stochastic_action_summand(summation_variables,
    condition,
    multi_action,
    assignments,
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
  stream >> action_summands;
  stream >> deadlock_summands;

  lps = linear_process_base<ActionSummand>(process_parameters, deadlock_summands, action_summands);
  return stream;
}

static
void write_spec(atermpp::aterm_ostream& stream, const stochastic_specification& spec)
{
  atermpp::aterm_stream_state state(stream);
  stream << data::detail::remove_index_impl;

  stream << linear_process_specification_marker();
  stream << spec.data();
  stream << spec.action_labels();
  stream << spec.global_variables();
  stream << spec.process();
  stream << spec.initial_process();
}

static
void read_spec(atermpp::aterm_istream& stream, stochastic_specification& spec)
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
    stochastic_linear_process process;
    stochastic_process_initializer initial_process;

    stream >> data;
    stream >> action_labels;
    stream >> global_variables;
    stream >> process;
    stream >> initial_process;
    spec = stochastic_specification(data, action_labels, global_variables, process, initial_process);
  }
  catch (std::exception& ex)
  {
    mCRL2log(log::error) << ex.what() << "\n";
    throw mcrl2::runtime_error(std::string("Error reading linear process specification (LPS)."));
  }
}

atermpp::aterm_ostream& operator<<(atermpp::aterm_ostream& stream, const specification& spec)
{
  write_spec(stream, stochastic_specification(spec));
  return stream;
}

atermpp::aterm_ostream& operator<<(atermpp::aterm_ostream& stream, const stochastic_specification& spec)
{
  write_spec(stream, spec);
  return stream;
}

atermpp::aterm_istream& operator>>(atermpp::aterm_istream& stream, lps::specification& spec)
{
  stochastic_specification stochastic_spec;
  read_spec(stream, stochastic_spec);
  spec = remove_stochastic_operators(stochastic_spec);
  return stream;
}

atermpp::aterm_istream& operator>>(atermpp::aterm_istream& stream, lps::stochastic_specification& spec)
{
  read_spec(stream, spec);
  return stream;
}

} // namespace mcrl2::lps
