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
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/rewrite_strategy.h"
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
  bool remove_unused_rewrite_rules = false;
  bool cached = false;
  bool global_cache = false;
  bool confluence = false;
  bool detect_deadlock = false;
  bool detect_nondeterminism = false;
  bool detect_divergence = false;
  bool detect_action = false;
  bool save_error_trace = true;
  bool generate_traces = false;
  bool suppress_progress_messages = false;
  bool no_store = false;
  bool dfs_recursive = false;
  std::size_t max_states = std::numeric_limits<std::size_t>::max();
  std::size_t max_traces = 0;
  std::size_t todo_max = std::numeric_limits<std::size_t>::max();
  std::string trace_prefix;
  std::set<core::identifier_string> trace_actions;
  std::set<std::string> trace_multiaction_strings;
  std::set<lps::multi_action> trace_multiactions;
  std::set<core::identifier_string> actions_internal_for_divergencies;
  std::string confluence_action = "ctau";
};

inline
std::ostream& operator<<(std::ostream& out, const explorer_options& options)
{
  out << "rewrite-strategy = " << options.rewrite_strategy << std::endl;
  out << "search-strategy = " << options.search_strategy << std::endl;
  out << "cached = " << std::boolalpha << options.cached << std::endl;
  out << "global-cache = " << std::boolalpha << options.global_cache << std::endl;
  out << "confluence = " << std::boolalpha << options.confluence << std::endl;
  out << "confluence-action = " << options.confluence << std::endl;
  out << "one-point-rule-rewrite = " << std::boolalpha << options.one_point_rule_rewrite << std::endl;
  out << "resolve-summand-variable-name-clashes = " << std::boolalpha << options.resolve_summand_variable_name_clashes << std::endl;
  out << "replace-constants-by-variables = " << std::boolalpha << options.replace_constants_by_variables << std::endl;
  out << "remove-unused-rewrite-rules = " << std::boolalpha << options.remove_unused_rewrite_rules << std::endl;
  out << "detect-deadlock = " << std::boolalpha << options.detect_deadlock << std::endl;
  out << "detect-nondeterminism = " << std::boolalpha << options.detect_nondeterminism << std::endl;
  out << "detect-divergence = " << std::boolalpha << options.detect_divergence << std::endl;
  out << "detect-action = " << std::boolalpha << options.detect_action << std::endl;
  out << "save-error-trace = " << std::boolalpha << options.save_error_trace << std::endl;
  out << "generate-traces = " << std::boolalpha << options.generate_traces << std::endl;
  out << "suppress-progress-messages = " << std::boolalpha << options.suppress_progress_messages << std::endl;
  out << "no-store = " << std::boolalpha << options.no_store << std::endl;
  out << "dfs-recursive = " << std::boolalpha << options.dfs_recursive << std::endl;
  out << "max-states = " << options.max_states << std::endl;
  out << "max-traces = " << options.max_traces << std::endl;
  out << "todo-max = " << options.todo_max << std::endl;
  out << "trace-prefix = " << options.trace_prefix << std::endl;
  out << "trace-actions = " << core::detail::print_set(options.trace_actions) << std::endl;
  out << "trace-multiaction-strings = " << core::detail::print_set(options.trace_multiaction_strings) << std::endl;
  out << "trace-multiactions = " << core::detail::print_set(options.trace_multiactions) << std::endl;
  out << "actions-internal-for-divergencies = " << core::detail::print_set(options.actions_internal_for_divergencies) << std::endl;
  return out;
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_EXPLORER_OPTIONS_H
