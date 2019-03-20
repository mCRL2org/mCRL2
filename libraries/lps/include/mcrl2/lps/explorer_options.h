// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/explorer_options.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_EXPLORER_OPTIONS_H
#define MCRL2_LPS_EXPLORER_OPTIONS_H

#include <iomanip>
#include "mcrl2/lps/multi_action.h"
#include "mcrl2/lps/exploration_strategy.h"

namespace mcrl2 {

namespace lps {

struct explorer_options
{
  data::rewrite_strategy rewrite_strategy = data::jitty;
  exploration_strategy search_strategy;
  bool one_point_rule_rewrite = false;
  bool replace_constants_by_variables = false;
  bool resolve_summand_variable_name_clashes = false;
  bool store_states_as_trees = true;
  bool cached = false;
  bool global_cache = false;
  bool confluence = false;
  bool detect_deadlock = false;
  bool detect_nondeterminism = false;
  bool detect_divergence = false;
  bool detect_action = false;
  bool generate_traces = false;
  std::size_t max_states = std::numeric_limits<std::size_t>::max();
  std::size_t max_traces = 0;
  std::string priority_action;
  std::string trace_prefix;
  std::set<core::identifier_string> trace_actions;
  std::set<std::string> trace_multiaction_strings;
  std::set<lps::multi_action> trace_multiactions;
  std::set<core::identifier_string> actions_internal_for_divergencies;
};

inline
std::ostream& operator<<(std::ostream& out, const explorer_options& options)
{
  out << "rewrite_strategy = " << options.rewrite_strategy << std::endl;
  out << "search_strategy = " << options.search_strategy << std::endl;
  out << "cached = " << std::boolalpha << options.cached << std::endl;
  out << "global-cache = " << std::boolalpha << options.global_cache << std::endl;
  out << "confluence = " << std::boolalpha << options.confluence << std::endl;
  out << "one_point_rule_rewrite = " << std::boolalpha << options.one_point_rule_rewrite << std::endl;
  out << "resolve_summand_variable_name_clashes = " << std::boolalpha << options.resolve_summand_variable_name_clashes << std::endl;
  out << "replace_constants_by_variables = " << std::boolalpha << options.replace_constants_by_variables << std::endl;
  out << "store_states_as_trees = " << std::boolalpha << options.store_states_as_trees << std::endl;
  out << "detect_deadlock = " << std::boolalpha << options.detect_deadlock << std::endl;
  out << "detect_nondeterminism = " << std::boolalpha << options.detect_nondeterminism << std::endl;
  out << "detect_divergence = " << std::boolalpha << options.detect_divergence << std::endl;
  out << "detect_action = " << std::boolalpha << options.detect_action << std::endl;
  out << "generate_traces = " << std::boolalpha << options.generate_traces << std::endl;
  out << "max_states = " << options.max_states << std::endl;
  out << "max_traces = " << options.max_traces << std::endl;
  out << "priority_action = " << options.priority_action << std::endl;
  out << "trace_prefix = " << options.trace_prefix << std::endl;
  out << "trace_actions = " << core::detail::print_set(options.trace_actions) << std::endl;
  out << "trace_multiaction_strings = " << core::detail::print_set(options.trace_multiaction_strings) << std::endl;
  out << "trace_multiactions = " << core::detail::print_set(options.trace_multiactions) << std::endl;
  out << "actions_internal_for_divergencies = " << core::detail::print_set(options.actions_internal_for_divergencies) << std::endl;
  return out;
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_EXPLORER_OPTIONS_H
