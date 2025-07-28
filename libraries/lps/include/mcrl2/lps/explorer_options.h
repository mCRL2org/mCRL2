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

namespace mcrl2::lps
{

struct explorer_options
{
  data::rewrite_strategy rewrite_strategy = data::jitty;
  exploration_strategy search_strategy;
  bool one_point_rule_rewrite = false;
  bool replace_constants_by_variables = false;
  bool remove_unused_rewrite_rules = false;
  bool cached = false;
  bool global_cache = false;
  bool confluence = false;
  bool detect_deadlock = false;
  bool detect_nondeterminism = false;
  bool detect_divergence = false;
  bool detect_action = false;
  bool check_probabilities = false;
  bool save_error_trace = false;
  bool generate_traces = false;
  bool suppress_progress_messages = false;
  bool save_at_end = false;
  bool dfs_recursive = false;
  bool discard_lts_state_labels = false;
  bool rewrite_actions = true;    // If false, this option prevents rewriting actions.
                                  // Rewriting actions is only needed if they occur in the
                                  // generated lts, or in traces. 
  std::size_t max_states = std::numeric_limits<std::size_t>::max();
  std::size_t max_traces = 0;
  std::size_t highway_todo_max = std::numeric_limits<std::size_t>::max();
  std::size_t number_of_threads = 1;
  std::string trace_prefix;
  std::set<core::identifier_string> trace_actions;
  std::set<lps::multi_action> trace_multiactions;
  std::set<core::identifier_string> actions_internal_for_divergencies;
  std::string confluence_action = "ctau";

  // Constructor.
  explorer_options() = default;

  // Constructor.
  explorer_options(data::rewrite_strategy rewr_strat)
   : rewrite_strategy(rewr_strat)
  {}

  //
  explorer_options(const explorer_options& other) = default;
  explorer_options& operator=(const explorer_options& other) = default;
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
  out << "replace-constants-by-variables = " << std::boolalpha << options.replace_constants_by_variables << std::endl;
  out << "remove-unused-rewrite-rules = " << std::boolalpha << options.remove_unused_rewrite_rules << std::endl;
  out << "check-probabilities = " << std::boolalpha << options.check_probabilities << std::endl;
  out << "detect-deadlock = " << std::boolalpha << options.detect_deadlock << std::endl;
  out << "detect-nondeterminism = " << std::boolalpha << options.detect_nondeterminism << std::endl;
  out << "detect-divergence = " << std::boolalpha << options.detect_divergence << std::endl;
  out << "detect-action = " << std::boolalpha << options.detect_action << std::endl;
  out << "discard-lts-state-labels = " << std::boolalpha << options.discard_lts_state_labels << std::endl;
  out << "save-error-trace = " << std::boolalpha << options.save_error_trace << std::endl;
  out << "generate-traces = " << std::boolalpha << options.generate_traces << std::endl;
  out << "suppress-progress-messages = " << std::boolalpha << options.suppress_progress_messages << std::endl;
  out << "save-aut-at-end = " << std::boolalpha << options.save_at_end << std::endl;
  out << "dfs-recursive = " << std::boolalpha << options.dfs_recursive << std::endl;
  out << "max-states = " << options.max_states << std::endl;
  out << "max-traces = " << options.max_traces << std::endl;
  out << "todo-max = " << options.highway_todo_max << std::endl;
  out << "threads = " << options.number_of_threads << std::endl;
  out << "trace-prefix = " << options.trace_prefix << std::endl;
  out << "trace-actions = " << core::detail::print_set(options.trace_actions) << std::endl;
  out << "trace-multiactions = " << core::detail::print_set(options.trace_multiactions) << std::endl;
  out << "actions-internal-for-divergencies = " << core::detail::print_set(options.actions_internal_for_divergencies) << std::endl;
  return out;
}

} // namespace mcrl2::lps

#endif // MCRL2_LPS_EXPLORER_OPTIONS_H
