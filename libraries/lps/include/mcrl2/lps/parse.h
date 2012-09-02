// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/parse.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_PARSE_H
#define MCRL2_LPS_PARSE_H

#include <sstream>
#include "mcrl2/atermpp/convert.h"
#include "mcrl2/utilities/exception.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/lps/action_parse.h"
#include "mcrl2/lps/detail/linear_process_conversion_traverser.h"
#include "mcrl2/lps/action_rename.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/typecheck.h"
#include "mcrl2/process/is_linear.h"
#include "mcrl2/process/parse.h"

namespace mcrl2
{

namespace lps
{

struct action_rename_actions: public lps::action_actions
{
  action_rename_actions(const core::parser_table& table_)
    : lps::action_actions(table_)
  {}

  lps::action_rename_rule_rhs parse_ActionRenameRuleRHS(const core::parse_node& node)
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Action")) { return action_rename_rule_rhs(parse_Action(node.child(0))); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "tau")) { return action_rename_rule_rhs(core::detail::gsMakeTau()); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "delta")) { return action_rename_rule_rhs(core::detail::gsMakeDelta()); }
    report_unexpected_node(node);
    return lps::action_rename_rule_rhs();
  }

  lps::action_rename_rule parse_ActionRenameRule(const core::parse_node& node)
  {
    data::data_expression condition = data::sort_bool::true_();
    if (node.child(0).child(0))
    {
      condition = parse_DataExpr(node.child(0).child(0).child(0));
    }
    return action_rename_rule(core::detail::gsMakeActionRenameRule(data::variable_list(), condition, parse_Action(node.child(1)), parse_ActionRenameRuleRHS(node.child(3))));
  }

  atermpp::vector<lps::action_rename_rule> parse_ActionRenameRuleList(const core::parse_node& node)
  {
    return parse_vector<lps::action_rename_rule>(node, "ActionRenameRule", boost::bind(&action_rename_actions::parse_ActionRenameRule, this, _1));
  }

  atermpp::vector<lps::action_rename_rule> parse_ActionRenameRuleSpec(const core::parse_node& node)
  {
    data::variable_list variables = parse_VarSpec(node.child(0));
    atermpp::vector<lps::action_rename_rule> rules = parse_ActionRenameRuleList(node.child(2));
    for (atermpp::vector<lps::action_rename_rule>::iterator i = rules.begin(); i != rules.end(); ++i)
    {
      i->variables() = variables;
    }
    return rules;
  }

  bool callback_ActionRenameSpec(const core::parse_node& node, lps::action_rename_specification& result)
  {
    if (symbol_name(node) == "SortSpec")
    {
      return callback_DataSpecElement(node, result.data());
    }
    else if (symbol_name(node) == "ConsSpec")
    {
      return callback_DataSpecElement(node, result.data());
    }
    else if (symbol_name(node) == "MapSpec")
    {
      return callback_DataSpecElement(node, result.data());
    }
    else if (symbol_name(node) == "EqnSpec")
    {
      return callback_DataSpecElement(node, result.data());
    }
    else if (symbol_name(node) == "ActSpec")
    {
      result.action_labels() = parse_ActSpec(node);
      return true;
    }
    else if (symbol_name(node) == "ActionRenameRuleSpec")
    {
      atermpp::vector<lps::action_rename_rule> rules = parse_ActionRenameRuleSpec(node);
      result.rules().insert(result.rules().end(), rules.begin(), rules.end());
      return true;
    }
    return false;
  }

  lps::action_rename_specification parse_ActionRenameSpec(const core::parse_node& node)
  {
    lps::action_rename_specification result;
    traverse(node, boost::bind(&action_rename_actions::callback_ActionRenameSpec, this, _1, boost::ref(result)));
    return result;
  }
};

inline
action_rename_specification parse_action_rename_specification_new(const std::string& text)
{
  core::parser p(parser_tables_mcrl2, core::detail::ambiguity_fn, core::detail::syntax_error_fn);
  unsigned int start_symbol_index = p.start_symbol_index("ActionRenameSpec");
  bool partial_parses = false;
  core::parse_node node = p.parse(text, start_symbol_index, partial_parses);
  action_rename_specification result = action_rename_actions(parser_tables_mcrl2).parse_ActionRenameSpec(node);
  p.destroy_parse_node(node);
  return result;
}

inline
void complete_action_rename_specification(action_rename_specification& x, const lps::specification& spec)
{
  using namespace mcrl2::data;
  atermpp::aterm_appl result = lps::action_rename_specification_to_aterm(x);
  atermpp::aterm_appl lps_spec = lps::specification_to_aterm(spec);
  result = lps::detail::type_check_action_rename_specification(result, lps_spec);
  x = action_rename_specification(result);
  x.data().declare_data_specification_to_be_type_checked();
  x = action_rename_specification(x.data()+spec.data(),x.action_labels(),x.rules());
  x = detail::translate_user_notation_and_normalise_sorts_action_rename_spec(x);
}

/// \brief Parses a process specification from an input stream
/// \param in An input stream
/// \param alpha_reduce Indicates whether alphabet reductions need to be performed
/// \return The parse result
inline
action_rename_specification parse_action_rename_specification(std::istream& in, const lps::specification& spec)
{
  std::string text = utilities::read_text(in);
  action_rename_specification result = parse_action_rename_specification_new(text);
  complete_action_rename_specification(result, spec);
  return result;
}

/// \brief Parses an action rename specification.
/// Parses an acion rename specification.
/// If the action rename specification contains data types that are not
/// present in the data specification of \p spec they are added to it.
/// \param in An input stream
/// \param spec A linear process specification
/// \return An action rename specification
inline
action_rename_specification parse_action_rename_specification(const std::string& spec_string, const lps::specification& spec)
{
  std::istringstream in(spec_string);
  return parse_action_rename_specification(in, spec);
}

/// \brief Parses a linear process specification from an input stream
/// \param text An input stream containing a linear process specification
/// \return The parsed specification
/// \exception non_linear_process if a non-linear sub-expression is encountered.
/// \exception mcrl2::runtime_error in the following cases:
/// \li The number of equations is not equal to one
/// \li The initial process is not a process instance, or it does not match with the equation
/// \li A sequential process is found with a right hand side that is not a process instance,
/// or it doesn't match the equation
inline
specification parse_linear_process_specification(std::istream& spec_stream)
{
  process::process_specification pspec = mcrl2::process::parse_process_specification(spec_stream);
  if (!process::is_linear(pspec, true))
  {
    throw mcrl2::runtime_error("the process specification is not linear!");
  }
  process::detail::linear_process_conversion_traverser visitor;
  specification result = visitor.convert(pspec);
  complete_data_specification(result);
  return result;
}
/// \brief Parses a linear process specification from a string
/// \param text A string containing a linear process specification
/// \return The parsed specification
/// \exception non_linear_process if a non-linear sub-expression is encountered.
/// \exception mcrl2::runtime_error in the following cases:
/// \li The number of equations is not equal to one
/// \li The initial process is not a process instance, or it does not match with the equation
/// \li A sequential process is found with a right hand side that is not a process instance,
/// or it doesn't match the equation
inline
specification parse_linear_process_specification(const std::string& text)
{
  std::istringstream stream(text);
  return parse_linear_process_specification(stream);
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_PARSE_H
