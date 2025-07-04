// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/parse_impl.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_PARSE_IMPL_H
#define MCRL2_PROCESS_PARSE_IMPL_H

#include "mcrl2/data/parse_impl.h"
#include "mcrl2/process/typecheck.h"
#include "mcrl2/utilities/detail/separate_keyword_section.h"

namespace mcrl2::process
{

struct untyped_process_specification: public data::untyped_data_specification
{
  data::variable_list global_variables;
  action_label_list action_labels;
  std::vector<process::process_equation> equations;
  process_expression init;

  process_specification construct_process_specification()
  {
    process_specification result;
    result.data() = construct_data_specification();
    result.global_variables() = std::set<data::variable>(global_variables.begin(), global_variables.end());
    result.action_labels() = action_labels;
    result.equations() = equations;
    result.init() = init;
    return result;
  }
};

namespace detail
{

struct action_actions: public data::detail::data_specification_actions
{
  action_actions(const core::parser& parser_)
    : data::detail::data_specification_actions(parser_)
  {}

  data::untyped_data_parameter parse_Action(const core::parse_node& node) const
  {
    return data::untyped_data_parameter(parse_Id(node.child(0)), parse_DataExprList(node.child(1)));
  }

  data::untyped_data_parameter_list parse_ActionList(const core::parse_node& node) const
  {
    return parse_list<data::untyped_data_parameter>(node, "Action", [&](const core::parse_node& node) { return parse_Action(node); });
  }

  bool callback_ActDecl(const core::parse_node& node, action_label_vector& result) const
  {
    if (symbol_name(node) == "ActDecl")
    {
      core::identifier_string_list ids = parse_IdList(node.child(0));
      data::sort_expression_list sorts;
      if (node.child(1).child(0))
      {
        sorts = parse_SortProduct(node.child(1).child(0).child(1));
      }
      for (const core::identifier_string& id: ids)
      {
        result.emplace_back(id, sorts);
      }
      return true;
    }
    return false;
  };

  action_label_list parse_ActDeclList(const core::parse_node& node) const
  {
    action_label_vector result;
    traverse(node, [&](const core::parse_node& node) { return callback_ActDecl(node, result); });
    return process::action_label_list(result.begin(), result.end());
  }

  action_label_list parse_ActSpec(const core::parse_node& node) const
  {
    return parse_ActDeclList(node.child(1));
  }
};

struct process_actions: public process::detail::action_actions
{
  explicit process_actions(const core::parser& parser_)
    : process::detail::action_actions(parser_)
  {}

  core::identifier_string_list parse_ActIdSet(const core::parse_node& node) const
  {
    return parse_IdList(node.child(1));
  }

  process::action_name_multiset parse_MultActId(const core::parse_node& node) const
  {
    return action_name_multiset(parse_IdList(node));
  }

  process::action_name_multiset_list parse_MultActIdList(const core::parse_node& node) const
  {
    return parse_list<process::action_name_multiset>(node, "MultActId", [&](const core::parse_node& node) { return parse_MultActId(node); });
  }

  process::action_name_multiset_list parse_MultActIdSet(const core::parse_node& node) const
  {
    return parse_MultActIdList(node.child(1));
  }

  process::communication_expression parse_CommExpr(const core::parse_node& node) const
  {
    core::identifier_string id = parse_Id(node.child(0));
    core::identifier_string_list ids = parse_IdList(node.child(2));
    ids.push_front(id);
    action_name_multiset lhs(ids);
    core::identifier_string rhs = parse_Id(node.child(4));
    return process::communication_expression(lhs, rhs);
  }

  process::communication_expression_list parse_CommExprList(const core::parse_node& node) const
  {
    return parse_list<process::communication_expression>(node, "CommExpr", [&](const core::parse_node& node) { return parse_CommExpr(node); });
  }

  process::communication_expression_list parse_CommExprSet(const core::parse_node& node) const
  {
    return parse_CommExprList(node.child(1));
  }

  process::rename_expression parse_RenExpr(const core::parse_node& node) const
  {
    return process::rename_expression(parse_Id(node.child(0)), parse_Id(node.child(2)));
  }

  process::rename_expression_list parse_RenExprList(const core::parse_node& node) const
  {
    return parse_list<process::rename_expression>(node, "RenExpr", [&](const core::parse_node& node) { return parse_RenExpr(node); });
  }

  process::rename_expression_list parse_RenExprSet(const core::parse_node& node) const
  {
    return parse_RenExprList(node.child(1));
  }

  bool is_process_expression(const std::string& s) const
  {
    return s == "ProcExpr" || s == "ProcExprNoIf";
  }

  bool is_proc_expr_stochastic_operator(const core::parse_node& node) const
  {
    return is_process_expression(symbol_name(node)) && (node.child_count() == 7) && (symbol_name(node.child(0)) == "dist") && (symbol_name(node.child(1)) == "VarsDeclList") &&
           (symbol_name(node.child(2)) == "[") && (symbol_name(node.child(3)) == "DataExpr") && (symbol_name(node.child(4)) == "]") && (symbol_name(node.child(5)) == ".") && is_process_expression(symbol_name(node.child(6)));
  }

  // override
  data::untyped_data_parameter parse_Action(const core::parse_node& node) const
  {
    return data::untyped_data_parameter(parse_Id(node.child(0)), parse_DataExprList(node.child(1)));
  }

  process::process_expression parse_ProcExpr(const core::parse_node& node) const
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Action")) { return parse_Action(node.child(0)); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "Id") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(3)) == ")")) { return untyped_process_assignment(parse_Id(node.child(0)), parse_AssignmentList(node.child(2))); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "delta")) { return delta(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "tau")) { return tau(); }
    else if ((node.child_count() == 6) && (symbol_name(node.child(0)) == "block") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "ActIdSet") && (symbol_name(node.child(3)) == ",") && is_process_expression(symbol_name(node.child(4))) && (symbol_name(node.child(5)) == ")")) { return process::block(parse_ActIdSet(node.child(2)), parse_ProcExpr(node.child(4))); }
    else if ((node.child_count() == 6) && (symbol_name(node.child(0)) == "allow") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "MultActIdSet") && (symbol_name(node.child(3)) == ",") && is_process_expression(symbol_name(node.child(4))) && (symbol_name(node.child(5)) == ")")) { return process::allow(parse_MultActIdSet(node.child(2)), parse_ProcExpr(node.child(4))); }
    else if ((node.child_count() == 6) && (symbol_name(node.child(0)) == "hide") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "ActIdSet") && (symbol_name(node.child(3)) == ",") && is_process_expression(symbol_name(node.child(4))) && (symbol_name(node.child(5)) == ")")) { return process::hide(parse_ActIdSet(node.child(2)), parse_ProcExpr(node.child(4))); }
    else if ((node.child_count() == 6) && (symbol_name(node.child(0)) == "rename") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "RenExprSet") && (symbol_name(node.child(3)) == ",") && is_process_expression(symbol_name(node.child(4))) && (symbol_name(node.child(5)) == ")")) { return process::rename(parse_RenExprSet(node.child(2)), parse_ProcExpr(node.child(4))); }
    else if ((node.child_count() == 6) && (symbol_name(node.child(0)) == "comm") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "CommExprSet") && (symbol_name(node.child(3)) == ",") && is_process_expression(symbol_name(node.child(4))) && (symbol_name(node.child(5)) == ")")) { return process::comm(parse_CommExprSet(node.child(2)), parse_ProcExpr(node.child(4))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "(") && is_process_expression(symbol_name(node.child(1))) && (symbol_name(node.child(2)) == ")")) { return parse_ProcExpr(node.child(1)); }
    else if ((node.child_count() == 3) && is_process_expression(symbol_name(node.child(0))) && (node.child(1).string() == "+") && is_process_expression(symbol_name(node.child(2)))) { return choice(parse_ProcExpr(node.child(0)), parse_ProcExpr(node.child(2))); }
    else if ((node.child_count() == 3) && is_process_expression(symbol_name(node.child(0))) && (node.child(1).string() == "||") && is_process_expression(symbol_name(node.child(2)))) { return merge(parse_ProcExpr(node.child(0)), parse_ProcExpr(node.child(2))); }
    else if ((node.child_count() == 3) && is_process_expression(symbol_name(node.child(0))) && (node.child(1).string() == "||_") && is_process_expression(symbol_name(node.child(2)))) { return left_merge(parse_ProcExpr(node.child(0)), parse_ProcExpr(node.child(2))); }
    else if ((node.child_count() == 3) && is_process_expression(symbol_name(node.child(0))) && (node.child(1).string() == ".") && is_process_expression(symbol_name(node.child(2)))) { return seq(parse_ProcExpr(node.child(0)), parse_ProcExpr(node.child(2))); }
    else if ((node.child_count() == 3) && is_process_expression(symbol_name(node.child(0))) && (node.child(1).string() == "<<") && is_process_expression(symbol_name(node.child(2)))) { return bounded_init(parse_ProcExpr(node.child(0)), parse_ProcExpr(node.child(2))); }
    else if ((node.child_count() == 3) && is_process_expression(symbol_name(node.child(0))) && (node.child(1).string() == "@") && (symbol_name(node.child(2)) == "DataExprUnit")) { return at(parse_ProcExpr(node.child(0)), parse_DataExprUnit(node.child(2))); }
    else if ((node.child_count() == 3) && is_process_expression(symbol_name(node.child(0))) && (node.child(1).string() == "|") && is_process_expression(symbol_name(node.child(2)))) { return sync(parse_ProcExpr(node.child(0)), parse_ProcExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExprUnit") && (node.child(1).string() == "->") && is_process_expression(symbol_name(node.child(2)))) { return if_then(parse_DataExprUnit(node.child(0)), parse_ProcExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExprUnit") && (symbol_name(node.child(1)) == "IfThen") && node.child(1).child(0).string() == "->" && is_process_expression(symbol_name(node.child(1).child(1))) && (node.child(1).child(2).string() == "<>") && is_process_expression(symbol_name(node.child(2)))) { return if_then_else(parse_DataExprUnit(node.child(0)), parse_ProcExpr(node.child(1).child(1)), parse_ProcExpr(node.child(2))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "sum") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && is_process_expression(symbol_name(node.child(3)))) { return sum(parse_VarsDeclList(node.child(1)), parse_ProcExpr(node.child(3))); }
    else if ((node.child_count() == 7) && is_proc_expr_stochastic_operator(node)) { return stochastic_operator(parse_VarsDeclList(node.child(1)), parse_DataExpr(node.child(3)), parse_ProcExpr(node.child(6))); }
    throw core::parse_node_unexpected_exception(m_parser, node);
  }

  process::process_equation parse_ProcDecl(const core::parse_node& node) const
  {
    core::identifier_string name = parse_Id(node.child(0));
    data::variable_list variables = parse_VarsDeclList(node.child(1));
    process_identifier id(name, variables);
    return process::process_equation(id, variables, parse_ProcExpr(node.child(3)));
  }

  std::vector<process::process_equation> parse_ProcDeclList(const core::parse_node& node) const
  {
    return parse_vector<process::process_equation>(node, "ProcDecl", [&](const core::parse_node& node) { return parse_ProcDecl(node); });
  }

  std::vector<process::process_equation> parse_ProcSpec(const core::parse_node& node) const
  {
    return parse_ProcDeclList(node.child(1));
  }

  process::process_expression parse_Init(const core::parse_node& node) const
  {
    return parse_ProcExpr(node.child(1));
  }

  bool callback_mCRL2Spec(const core::parse_node& node, untyped_process_specification& result) const
  {
    if (symbol_name(node) == "SortSpec")
    {
      return callback_DataSpecElement(node, result);
    }
    else if (symbol_name(node) == "ConsSpec")
    {
      return callback_DataSpecElement(node, result);
    }
    else if (symbol_name(node) == "MapSpec")
    {
      return callback_DataSpecElement(node, result);
    }
    else if (symbol_name(node) == "EqnSpec")
    {
      return callback_DataSpecElement(node, result);
    }
    else if (symbol_name(node) == "GlobVarSpec")
    {
      result.global_variables = parse_GlobVarSpec(node);
      return true;
    }
    else if (symbol_name(node) == "ActSpec")
    {
      result.action_labels = result.action_labels + parse_ActSpec(node);
      return true;
    }
    else if (symbol_name(node) == "ProcSpec")
    {
      std::vector<process::process_equation> eqn = parse_ProcSpec(node);
      result.equations.insert(result.equations.end(), eqn.begin(), eqn.end());
      return true;
    }
    else if (symbol_name(node) == "Init")
    {
      result.init = parse_Init(node);
    }
    return false;
  }

  untyped_process_specification parse_mCRL2Spec(const core::parse_node& node) const
  {
    untyped_process_specification result;
    traverse(node, [&](const core::parse_node& node) { return callback_mCRL2Spec(node, result); });
    return result;
  }
};

} // namespace detail

} // namespace mcrl2::process

#endif // MCRL2_PROCESS_PARSE_IMPL_H
