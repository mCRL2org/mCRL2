// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/parse.h
/// \brief Parse function for process specifications

#ifndef MCRL2_PROCESS_PARSE_H
#define MCRL2_PROCESS_PARSE_H

#include <iostream>
#include <string>
#include <sstream>
#include "mcrl2/core/parser_utility.h"
#include "mcrl2/utilities/text_utility.h"
#include "mcrl2/utilities/detail/separate_keyword_section.h"
#include "mcrl2/process/action_parse.h"
#include "mcrl2/process/process_specification.h"
#include "mcrl2/process/typecheck.h"
#include "mcrl2/process/alphabet.h"

namespace mcrl2
{

namespace process
{

struct process_actions: public process::action_actions
{
  process_actions(const core::parser_table& table_)
    : process::action_actions(table_)
  {}

  core::identifier_string_list parse_ActIdSet(const core::parse_node& node)
  {
    return parse_IdList(node.child(1));
  }

  process::action_name_multiset parse_MultActId(const core::parse_node& node)
  {
    return action_name_multiset(parse_IdList(node));
  }

  process::action_name_multiset_list parse_MultActIdList(const core::parse_node& node)
  {
    return parse_list<process::action_name_multiset>(node, "MultActId", boost::bind(&process_actions::parse_MultActId, this, _1));
  }

  process::action_name_multiset_list parse_MultActIdSet(const core::parse_node& node)
  {
    return parse_MultActIdList(node.child(1));
  }

  process::communication_expression parse_CommExpr(const core::parse_node& node)
  {
    core::identifier_string id = parse_Id(node.child(0));
    core::identifier_string_list ids = parse_IdList(node.child(2));
    ids.push_front(id);
    action_name_multiset lhs(ids);
    core::identifier_string rhs = parse_Id(node.child(4));
    return process::communication_expression(lhs, rhs);
  }

  process::communication_expression_list parse_CommExprList(const core::parse_node& node)
  {
    return parse_list<process::communication_expression>(node, "CommExpr", boost::bind(&process_actions::parse_CommExpr, this, _1));
  }

  process::communication_expression_list parse_CommExprSet(const core::parse_node& node)
  {
    return parse_CommExprList(node.child(1));
  }

  process::rename_expression parse_RenExpr(const core::parse_node& node)
  {
    return process::rename_expression(parse_Id(node.child(0)), parse_Id(node.child(2)));
  }

  process::rename_expression_list parse_RenExprList(const core::parse_node& node)
  {
    return parse_list<process::rename_expression>(node, "RenExpr", boost::bind(&process_actions::parse_RenExpr, this, _1));
  }

  process::rename_expression_list parse_RenExprSet(const core::parse_node& node)
  {
    return parse_RenExprList(node.child(1));
  }

  process::process_expression parse_IfThen(const core::parse_node& node)
  {
    return parse_ProcExprNoIf(node.child(1));
  }

  bool is_proc_expr_sum(const core::parse_node& node) const
  {
    return (symbol_name(node).find("ProcExpr") == 0) && (node.child_count() == 3) && (symbol_name(node.child(0)) == "sum") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".");
  }

  bool is_proc_expr_if_then(const core::parse_node& node) const
  {
    return (symbol_name(node).find("ProcExpr") == 0) && (node.child_count() == 2) && (symbol_name(node.child(0)) == "DataExprUnit") && (symbol_name(node.child(1)) == "->");
  }

  bool is_proc_expr_if_then_else(const core::parse_node& node) const
  {
    return (symbol_name(node).find("ProcExpr") == 0) && (node.child_count() == 2) && (symbol_name(node.child(0)) == "DataExprUnit") && (symbol_name(node.child(1)) == "IfThen");
  }

  // override
  untyped_parameter_identifier parse_Action(const core::parse_node& node)
  {
    return untyped_parameter_identifier(parse_Id(node.child(0)), parse_DataExprList(node.child(1)));
  }

  process::process_expression parse_ProcExpr(const core::parse_node& node)
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Action")) { return parse_Action(node.child(0)); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "Id") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(3)) == ")")) { return untyped_process_assignment(parse_Id(node.child(0)), parse_AssignmentList(node.child(2))); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "delta")) { return delta(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "tau")) { return tau(); }
    else if ((node.child_count() == 6) && (symbol_name(node.child(0)) == "block") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "ActIdSet") && (symbol_name(node.child(3)) == ",") && (symbol_name(node.child(4)) == "ProcExpr") && (symbol_name(node.child(5)) == ")")) { return block(parse_ActIdSet(node.child(2)), parse_ProcExpr(node.child(4))); }
    else if ((node.child_count() == 6) && (symbol_name(node.child(0)) == "allow") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "MultActIdSet") && (symbol_name(node.child(3)) == ",") && (symbol_name(node.child(4)) == "ProcExpr") && (symbol_name(node.child(5)) == ")")) { return allow(parse_MultActIdSet(node.child(2)), parse_ProcExpr(node.child(4))); }
    else if ((node.child_count() == 6) && (symbol_name(node.child(0)) == "hide") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "ActIdSet") && (symbol_name(node.child(3)) == ",") && (symbol_name(node.child(4)) == "ProcExpr") && (symbol_name(node.child(5)) == ")")) { return hide(parse_ActIdSet(node.child(2)), parse_ProcExpr(node.child(4))); }
    else if ((node.child_count() == 6) && (symbol_name(node.child(0)) == "rename") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "RenExprSet") && (symbol_name(node.child(3)) == ",") && (symbol_name(node.child(4)) == "ProcExpr") && (symbol_name(node.child(5)) == ")")) { return rename(parse_RenExprSet(node.child(2)), parse_ProcExpr(node.child(4))); }
    else if ((node.child_count() == 6) && (symbol_name(node.child(0)) == "comm") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "CommExprSet") && (symbol_name(node.child(3)) == ",") && (symbol_name(node.child(4)) == "ProcExpr") && (symbol_name(node.child(5)) == ")")) { return comm(parse_CommExprSet(node.child(2)), parse_ProcExpr(node.child(4))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "(") && (symbol_name(node.child(1)) == "ProcExpr") && (symbol_name(node.child(2)) == ")")) { return parse_ProcExpr(node.child(1)); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ProcExpr") && (node.child(1).string() == "+") && (symbol_name(node.child(2)) == "ProcExpr")) { return choice(parse_ProcExpr(node.child(0)), parse_ProcExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ProcExpr") && (node.child(1).string() == "||") && (symbol_name(node.child(2)) == "ProcExpr")) { return merge(parse_ProcExpr(node.child(0)), parse_ProcExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ProcExpr") && (node.child(1).string() == "||_") && (symbol_name(node.child(2)) == "ProcExpr")) { return left_merge(parse_ProcExpr(node.child(0)), parse_ProcExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ProcExpr") && (node.child(1).string() == ".") && (symbol_name(node.child(2)) == "ProcExpr")) { return seq(parse_ProcExpr(node.child(0)), parse_ProcExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ProcExpr") && (node.child(1).string() == "<<") && (symbol_name(node.child(2)) == "ProcExpr")) { return bounded_init(parse_ProcExpr(node.child(0)), parse_ProcExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ProcExpr") && (node.child(1).string() == "@") && (symbol_name(node.child(2)) == "DataExprUnit")) { return at(parse_ProcExpr(node.child(0)), parse_DataExprUnit(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ProcExpr") && (node.child(1).string() == "|") && (symbol_name(node.child(2)) == "ProcExpr")) { return sync(parse_ProcExpr(node.child(0)), parse_ProcExpr(node.child(2))); }
    else if ((node.child_count() == 2) && is_proc_expr_sum(node.child(0)) && (symbol_name(node.child(1)) == "ProcExpr")) { return sum(parse_VarsDeclList(node.child(0).child(1)), parse_ProcExpr(node.child(1))); }
    else if ((node.child_count() == 2) && is_proc_expr_if_then(node.child(0)) && (symbol_name(node.child(1)) == "ProcExpr")) { return if_then(parse_DataExprUnit(node.child(0).child(0)), parse_ProcExpr(node.child(1))); }
    else if ((node.child_count() == 2) && is_proc_expr_if_then_else(node.child(0)) && (symbol_name(node.child(1)) == "ProcExpr")) { return if_then_else(parse_DataExprUnit(node.child(0).child(0)), parse_IfThen(node.child(0).child(1)), parse_ProcExpr(node.child(1))); }
    report_unexpected_node(node);
    return process::process_expression();
  }

  process::process_expression parse_ProcExprNoIf(const core::parse_node& node)
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Action")) { return parse_Action(node.child(0)); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "Id") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(3)) == ")")) { return untyped_process_assignment(parse_Id(node.child(0)), parse_AssignmentList(node.child(2))); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "delta")) { return delta(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "tau")) { return tau(); }
    else if ((node.child_count() == 6) && (symbol_name(node.child(0)) == "block") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "ActIdSet") && (symbol_name(node.child(3)) == ",") && (symbol_name(node.child(4)) == "ProcExpr") && (symbol_name(node.child(5)) == ")")) { return block(parse_ActIdSet(node.child(2)), parse_ProcExpr(node.child(4))); }
    else if ((node.child_count() == 6) && (symbol_name(node.child(0)) == "allow") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "MultActIdSet") && (symbol_name(node.child(3)) == ",") && (symbol_name(node.child(4)) == "ProcExpr") && (symbol_name(node.child(5)) == ")")) { return allow(parse_MultActIdSet(node.child(2)), parse_ProcExpr(node.child(4))); }
    else if ((node.child_count() == 6) && (symbol_name(node.child(0)) == "hide") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "ActIdSet") && (symbol_name(node.child(3)) == ",") && (symbol_name(node.child(4)) == "ProcExpr") && (symbol_name(node.child(5)) == ")")) { return hide(parse_ActIdSet(node.child(2)), parse_ProcExpr(node.child(4))); }
    else if ((node.child_count() == 6) && (symbol_name(node.child(0)) == "rename") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "RenExprSet") && (symbol_name(node.child(3)) == ",") && (symbol_name(node.child(4)) == "ProcExpr") && (symbol_name(node.child(5)) == ")")) { return rename(parse_RenExprSet(node.child(2)), parse_ProcExpr(node.child(4))); }
    else if ((node.child_count() == 6) && (symbol_name(node.child(0)) == "comm") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "CommExprSet") && (symbol_name(node.child(3)) == ",") && (symbol_name(node.child(4)) == "ProcExpr") && (symbol_name(node.child(5)) == ")")) { return comm(parse_CommExprSet(node.child(2)), parse_ProcExpr(node.child(4))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "(") && (symbol_name(node.child(1)) == "ProcExpr") && (symbol_name(node.child(2)) == ")")) { return parse_ProcExpr(node.child(1)); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ProcExprNoIf") && (node.child(1).string() == "+") && (symbol_name(node.child(2)) == "ProcExprNoIf")) { return choice(parse_ProcExprNoIf(node.child(0)), parse_ProcExprNoIf(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ProcExprNoIf") && (node.child(1).string() == "||") && (symbol_name(node.child(2)) == "ProcExprNoIf")) { return merge(parse_ProcExprNoIf(node.child(0)), parse_ProcExprNoIf(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ProcExprNoIf") && (node.child(1).string() == "||_") && (symbol_name(node.child(2)) == "ProcExprNoIf")) { return left_merge(parse_ProcExprNoIf(node.child(0)), parse_ProcExprNoIf(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ProcExprNoIf") && (node.child(1).string() == ".") && (symbol_name(node.child(2)) == "ProcExprNoIf")) { return seq(parse_ProcExprNoIf(node.child(0)), parse_ProcExprNoIf(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ProcExprNoIf") && (node.child(1).string() == "<<") && (symbol_name(node.child(2)) == "ProcExprNoIf")) { return bounded_init(parse_ProcExprNoIf(node.child(0)), parse_ProcExprNoIf(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ProcExprNoIf") && (node.child(1).string() == "@") && (symbol_name(node.child(2)) == "DataExprUnit")) { return at(parse_ProcExprNoIf(node.child(0)), parse_DataExprUnit(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ProcExprNoIf") && (node.child(1).string() == "|") && (symbol_name(node.child(2)) == "ProcExprNoIf")) { return sync(parse_ProcExprNoIf(node.child(0)), parse_ProcExprNoIf(node.child(2))); }
    else if ((node.child_count() == 2) && is_proc_expr_sum(node.child(0)) && (symbol_name(node.child(1)) == "ProcExprNoIf")) { return sum(parse_VarsDeclList(node.child(0).child(1)), parse_ProcExprNoIf(node.child(1))); }
    else if ((node.child_count() == 2) && is_proc_expr_if_then_else(node.child(0)) && (symbol_name(node.child(1)) == "ProcExprNoIf")) { return if_then_else(parse_DataExprUnit(node.child(0).child(0)), parse_IfThen(node.child(0).child(1)), parse_ProcExprNoIf(node.child(1))); }
    report_unexpected_node(node);
    return process::process_expression();
  }

  process::process_equation parse_ProcDecl(const core::parse_node& node)
  {
    core::identifier_string name = parse_Id(node.child(0));
    data::variable_list variables = parse_VarsDeclList(node.child(1));
    process_identifier id(name, variables);
    return process::process_equation(id, variables, parse_ProcExpr(node.child(3)));
  }

  std::vector<process::process_equation> parse_ProcDeclList(const core::parse_node& node)
  {
    return parse_vector<process::process_equation>(node, "ProcDecl", boost::bind(&process_actions::parse_ProcDecl, this, _1));
  }

  std::vector<process::process_equation> parse_ProcSpec(const core::parse_node& node)
  {
    return parse_ProcDeclList(node.child(1));
  }

  process::process_expression parse_Init(const core::parse_node& node)
  {
    return parse_ProcExpr(node.child(1));
  }

  bool callback_mCRL2Spec(const core::parse_node& node, process::process_specification& result)
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
    else if (symbol_name(node) == "GlobVarSpec")
    {
      data::variable_list variables = parse_GlobVarSpec(node);
      result.global_variables() = std::set<data::variable>(variables.begin(), variables.end());
      return true;
    }
    else if (symbol_name(node) == "ActSpec")
    {
      result.action_labels() = result.action_labels() + parse_ActSpec(node);
      return true;
    }
    else if (symbol_name(node) == "ProcSpec")
    {
      std::vector<process::process_equation> eqn = parse_ProcSpec(node);
      result.equations().insert(result.equations().end(), eqn.begin(), eqn.end());
      return true;
    }
    else if (symbol_name(node) == "Init")
    {
      result.init() = parse_Init(node);
    }
    return false;
  }

  process::process_specification parse_mCRL2Spec(const core::parse_node& node)
  {
    process::process_specification result;
    traverse(node, boost::bind(&process_actions::callback_mCRL2Spec, this, _1, boost::ref(result)));
    return result;
  }
};

inline
process_expression parse_process_expression_new(const std::string& text)
{
  core::parser p(parser_tables_mcrl2, core::detail::ambiguity_fn, core::detail::syntax_error_fn);
  unsigned int start_symbol_index = p.start_symbol_index("ProcExpr");
  bool partial_parses = false;
  core::parse_node node = p.parse(text, start_symbol_index, partial_parses);
  core::warn_and_or(node);
  core::warn_left_merge_merge(node);
  process_expression result = process_actions(parser_tables_mcrl2).parse_ProcExpr(node);
  p.destroy_parse_node(node);
  return result;
}

inline
process_specification parse_process_specification_new(const std::string& text)
{
  core::parser p(parser_tables_mcrl2, core::detail::ambiguity_fn, core::detail::syntax_error_fn);
  unsigned int start_symbol_index = p.start_symbol_index("mCRL2Spec");
  bool partial_parses = false;
  core::parse_node node = p.parse(text, start_symbol_index, partial_parses);
  core::warn_and_or(node);
  core::warn_left_merge_merge(node);
  process_specification result = process_actions(parser_tables_mcrl2).parse_mCRL2Spec(node);
  p.destroy_parse_node(node);
  return result;
}

inline
void complete_process_specification(process_specification& x, bool alpha_reduce = false)
{
  type_check(x);
  if (alpha_reduce)
  {
    alphabet_reduce(x);
  }
  process::translate_user_notation(x);
  process::normalize_sorts(x, x.data());
}

/// \brief Parses a process specification from an input stream
/// \param in An input stream
/// \param alpha_reduce Indicates whether alphabet reductions need to be performed
/// \return The parse result
inline
process_specification parse_process_specification(std::istream& in, bool alpha_reduce = false)
{
  std::string text = utilities::read_text(in);
  process_specification result = parse_process_specification_new(text);
  complete_process_specification(result, alpha_reduce);
  return result;
}

/// \brief Parses a process specification from a string
/// \param spec_string A string
/// \param alpha_reduce Indicates whether alphabet reductions needdto be performed
/// \return The parse result
inline
process_specification parse_process_specification(const std::string& spec_string, const bool alpha_reduce=false)
{
  std::istringstream in(spec_string);
  return parse_process_specification(in, alpha_reduce);
}

/// \brief Parses and type checks a process expression.
/// \param[in] text The input text containing a process expression.
/// \param[in] data_decl A declaration of data and actions ("glob m:Nat; act a:Nat;").
/// \param[in] proc_decl A process declaration ("proc P(n: Nat);").
inline
process_expression parse_process_expression(const std::string& text,
                                            const std::string& data_decl,
                                            const std::string& proc_decl
                                           )
{
  std::string proc_text = utilities::regex_replace(";", " = delta;", proc_decl);
  std::string init_text = "init\n     " + text + ";\n";
  std::string spec_text = data_decl + "\n" + proc_text + "\n" + init_text;
  process_specification spec = parse_process_specification(spec_text);
  return spec.init();
}

/// \brief Parses and type checks a process expression.
/// \param[in] text The input text containing a process expression.
/// \param[in] procspec_text A textual version of a process specification used as context
inline
process_expression parse_process_expression(const std::string& text, const std::string& procspec_text)
{
  std::vector<std::string> keywords;
  keywords.push_back("sort");
  keywords.push_back("var");
  keywords.push_back("eqn");
  keywords.push_back("map");
  keywords.push_back("cons");
  keywords.push_back("act");
  keywords.push_back("glob");
  keywords.push_back("proc");
  keywords.push_back("init");
  std::pair<std::string, std::string> result = utilities::detail::separate_keyword_section(procspec_text, "init", keywords);
  std::string init_text = "init\n     " + text + ";\n";
  std::string ptext = result.second + init_text;
  process_specification spec = parse_process_specification(ptext);
  return spec.init();
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_PARSE_H
