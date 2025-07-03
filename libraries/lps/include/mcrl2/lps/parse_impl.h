// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/parse_impl.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_PARSE_IMPL_H
#define MCRL2_LPS_PARSE_IMPL_H

#include "mcrl2/lps/detail/linear_process_conversion_traverser.h"
#include "mcrl2/lps/typecheck.h"
#include "mcrl2/process/parse.h"
#include "mcrl2/process/parse_impl.h"

namespace mcrl2::lps::detail
{

struct multi_action_actions: public process::detail::action_actions
{
  explicit multi_action_actions(const core::parser& parser_)
    : process::detail::action_actions(parser_)
  {}

  process::untyped_multi_action parse_MultAct(const core::parse_node& node) const
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "tau")) { return process::untyped_multi_action(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "ActionList"))
    {
      return process::untyped_multi_action(parse_ActionList(node.child(0)));
    }
    throw core::parse_node_unexpected_exception(m_parser, node);
  }
};

struct action_rename_actions: public process::detail::action_actions
{
  explicit action_rename_actions(const core::parser& parser_)
    : process::detail::action_actions(parser_)
  {}

  // create an action with an incomplete action label
  process::action parse_Action_as_action(const core::parse_node& node) const
  {
    process::action_label label(parse_Id(node.child(0)), {});
    return process::action(label, parse_DataExprList(node.child(1)));
  }

  process::process_expression parse_ActionRenameRuleRHS(const core::parse_node& node) const
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Action")) { return parse_Action_as_action(node.child(0)); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "tau")) { return process::tau(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "delta")) { return process::delta(); }
    throw core::parse_node_unexpected_exception(m_parser, node);
  }

  lps::action_rename_rule parse_ActionRenameRule(const core::parse_node& node) const
  {
    data::data_expression condition = data::sort_bool::true_();
    if (node.child(0).child(0))
    {
      condition = parse_DataExpr(node.child(0).child(0).child(0));
    }
    return action_rename_rule(data::variable_list(), condition, parse_Action_as_action(node.child(1)), parse_ActionRenameRuleRHS(node.child(3)));
  }

  std::vector<lps::action_rename_rule> parse_ActionRenameRuleList(const core::parse_node& node) const
  {
    return parse_vector<lps::action_rename_rule>(node, "ActionRenameRule", [&](const core::parse_node& node) { return parse_ActionRenameRule(node); });
  }

  std::vector<lps::action_rename_rule> parse_ActionRenameRuleSpec(const core::parse_node& node) const
  {
    data::variable_list variables = parse_VarSpec(node.child(0));
    std::vector<lps::action_rename_rule> rules = parse_ActionRenameRuleList(node.child(2));
    for (lps::action_rename_rule& rule: rules)
    {
      rule.variables() = variables;
    }
    return rules;
  }

  bool callback_ActionRenameSpec(const core::parse_node& node, data::untyped_data_specification& dataspec_result, lps::action_rename_specification& result) const
  {
    if (symbol_name(node) == "SortSpec")
    {
      return callback_DataSpecElement(node, dataspec_result);
    }
    else if (symbol_name(node) == "ConsSpec")
    {
      return callback_DataSpecElement(node, dataspec_result);
    }
    else if (symbol_name(node) == "MapSpec")
    {
      return callback_DataSpecElement(node, dataspec_result);
    }
    else if (symbol_name(node) == "EqnSpec")
    {
      return callback_DataSpecElement(node, dataspec_result);
    }
    else if (symbol_name(node) == "ActSpec")
    {
      result.action_labels() = result.action_labels() + parse_ActSpec(node);
      return true;
    }
    else if (symbol_name(node) == "ActionRenameRuleSpec")
    {
      std::vector<lps::action_rename_rule> rules = parse_ActionRenameRuleSpec(node);
      result.rules().insert(result.rules().end(), rules.begin(), rules.end());
      return true;
    }
    return false;
  }

  lps::action_rename_specification parse_ActionRenameSpec(const core::parse_node& node) const
  {
    data::untyped_data_specification dataspec_result;
    lps::action_rename_specification result;
    traverse(node, [&](const core::parse_node& node) { return callback_ActionRenameSpec(node, dataspec_result, result); });
    result.data() = dataspec_result.construct_data_specification();
    return result;
  }
};

} // namespace mcrl2::lps::detail

#endif // MCRL2_LPS_PARSE_IMPL_H
