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
#include "mcrl2/utilities/exception.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/process/action_parse.h"
#include "mcrl2/lps/detail/linear_process_conversion_traverser.h"
#include "mcrl2/lps/action_rename.h"
#include "mcrl2/lps/stochastic_specification.h"
#include "mcrl2/lps/typecheck.h"
#include "mcrl2/process/is_linear.h"
#include "mcrl2/process/parse.h"

namespace mcrl2
{

namespace lps
{

namespace detail
{

struct multi_action_actions: public process::detail::action_actions
{
  multi_action_actions(const core::parser& parser_)
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

inline
process::untyped_multi_action parse_multi_action_new(const std::string& text)
{
  core::parser p(parser_tables_mcrl2, core::detail::ambiguity_fn, core::detail::syntax_error_fn);
  unsigned int start_symbol_index = p.start_symbol_index("MultAct");
  bool partial_parses = false;
  core::parse_node node = p.parse(text, start_symbol_index, partial_parses);
  process::untyped_multi_action result = multi_action_actions(p).parse_MultAct(node);
  p.destroy_parse_node(node);
  return result;
}

inline
multi_action complete_multi_action(process::untyped_multi_action& x, const process::action_label_list& action_decls, const data::data_specification& data_spec = data::detail::default_specification())
{
  multi_action result = lps::type_check_multi_action(x, data_spec, action_decls);
  lps::translate_user_notation(result);
  lps::normalize_sorts(result, data_spec);
  return result;
}

struct action_rename_actions: public process::detail::action_actions
{
  action_rename_actions(const core::parser& parser_)
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
    for (std::vector<lps::action_rename_rule>::iterator i = rules.begin(); i != rules.end(); ++i)
    {
      i->variables() = variables;
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
      result.action_labels() = parse_ActSpec(node);
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

inline
action_rename_specification parse_action_rename_specification_new(const std::string& text)
{
  core::parser p(parser_tables_mcrl2, core::detail::ambiguity_fn, core::detail::syntax_error_fn);
  unsigned int start_symbol_index = p.start_symbol_index("ActionRenameSpec");
  bool partial_parses = false;
  core::parse_node node = p.parse(text, start_symbol_index, partial_parses);
  action_rename_specification result = action_rename_actions(p).parse_ActionRenameSpec(node);
  p.destroy_parse_node(node);
  return result;
}

inline
void complete_action_rename_specification(action_rename_specification& x, const lps::specification& spec)
{
  using namespace mcrl2::data;
  x = lps::type_check_action_rename_specification(x, spec);
  x.data().declare_data_specification_to_be_type_checked();
  x = action_rename_specification(x.data() + spec.data(), x.action_labels(), x.rules());
  detail::translate_user_notation(x);
}

} // namespace detail

/// \brief Parses a multi_action from an input stream
/// \param in An input stream containing a multi_action
/// \param[in] action_decls A list of allowed action labels that is used for type checking.
/// \param[in] data_spec The data specification that is used for type checking.
/// \return The parsed multi_action
/// \exception mcrl2::runtime_error when the input does not match the syntax of a multi action.
inline
multi_action parse_multi_action(std::stringstream& in, const process::action_label_list& action_decls, const data::data_specification& data_spec = data::detail::default_specification())
{
  std::string text = utilities::read_text(in);
  process::untyped_multi_action u = detail::parse_multi_action_new(text);
  return detail::complete_multi_action(u, action_decls, data_spec);
}

/// \brief Parses a multi_action from a string
/// \param text A string containing a multi_action
/// \param[in] action_decls A list of allowed action labels that is used for type checking.
/// \param[in] data_spec The data specification that is used for type checking.
/// \return The parsed multi_action
/// \exception mcrl2::runtime_error when the input does not match the syntax of a multi action.
inline
multi_action parse_multi_action(const std::string& text, const process::action_label_list& action_decls, const data::data_specification& data_spec = data::detail::default_specification())
{
  std::stringstream ma_stream(text);
  return parse_multi_action(ma_stream, action_decls, data_spec);
}

/// \brief Parses a process specification from an input stream
/// \param in An input stream
/// \param spec A linear process specification.
/// \return The parse result
inline
action_rename_specification parse_action_rename_specification(std::istream& in, const lps::specification& spec)
{
  std::string text = utilities::read_text(in);
  action_rename_specification result = detail::parse_action_rename_specification_new(text);
  detail::complete_action_rename_specification(result, spec);
  return result;
}

/// \brief Parses an action rename specification.
/// Parses an acion rename specification.
/// If the action rename specification contains data types that are not
/// present in the data specification of \p spec they are added to it.
/// \param spec_string A string containing an action rename specification.
/// \param spec A linear process specification
/// \return An action rename specification
inline
action_rename_specification parse_action_rename_specification(const std::string& spec_string, const lps::specification& spec)
{
  std::istringstream in(spec_string);
  return parse_action_rename_specification(in, spec);
}

/// \brief Parses a linear process specification from an input stream
/// \param spec_stream An input stream containing a linear process specification
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

template <typename Specification>
void parse_lps(std::istream&, Specification&)
{
  throw mcrl2::runtime_error("parse_lps not implemented yet!");
}

template <>
inline
void parse_lps<specification>(std::istream& from, specification& result)
{
  result = parse_linear_process_specification(from);
}

/// \brief Parses a stochastic linear process specification from an input stream
/// \param spec_stream An input stream containing a linear process specification
/// \return The parsed specification
/// \exception non_linear_process if a non-linear sub-expression is encountered.
/// \exception mcrl2::runtime_error in the following cases:
/// \li The number of equations is not equal to one
/// \li The initial process is not a process instance, or it does not match with the equation
/// \li A sequential process is found with a right hand side that is not a process instance,
/// or it doesn't match the equation
template <>
inline
void parse_lps<stochastic_specification>(std::istream& from, stochastic_specification& result)
{
  process::process_specification pspec = mcrl2::process::parse_process_specification(from);
  if (!process::is_linear(pspec, true))
  {
    throw mcrl2::runtime_error("the process specification is not linear!");
  }
  process::detail::stochastic_linear_process_conversion_traverser visitor;
  result = visitor.convert(pspec);
  complete_data_specification(result);
}

template <typename Specification>
void parse_lps(const std::string& text, Specification& result)
{
  std::istringstream stream(text);
  parse_lps(stream, result);
}

/// \brief Parses an action from a string
/// \param text A string containing an action
/// \param action_decls An action declaration
/// \param[in] data_spec A data specification used for sort normalization
/// \return An action
/// \exception mcrl2::runtime_error when the input does not match the syntax of an action.
// TODO: implement this function in the Process Library
inline
process::action parse_action(const std::string& text, const process::action_label_list& action_decls, const data::data_specification& data_spec = data::detail::default_specification())
{
  multi_action result = parse_multi_action(text, action_decls, data_spec);
  if (result.actions().size() != 1)
  {
    throw mcrl2::runtime_error("cannot parse '" + text + " as an action!");
  }
  return result.actions().front();
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_PARSE_H
