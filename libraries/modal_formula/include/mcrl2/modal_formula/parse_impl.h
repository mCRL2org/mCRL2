// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/parse_impl.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_PARSE_IMPL_H
#define MCRL2_MODAL_FORMULA_PARSE_IMPL_H

#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/parse_impl.h"
#include "mcrl2/modal_formula/typecheck.h"

namespace mcrl2
{

namespace action_formulas::detail
{

struct action_formula_actions: public lps::detail::multi_action_actions
{
  explicit action_formula_actions(const core::parser& parser_)
    : lps::detail::multi_action_actions(parser_)
  {}

  action_formulas::action_formula parse_ActFrm(const core::parse_node& node) const
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "MultAct")) { return process::untyped_multi_action(parse_ActionList(node.child(0))); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "DataValExpr")) { return parse_DataValExpr(node.child(0)); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "DataExpr")) { return parse_DataExpr(node.child(0)); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "true")) { return action_formulas::true_(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "false")) { return action_formulas::false_(); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "!") && (symbol_name(node.child(1)) == "ActFrm")) { return action_formulas::not_(parse_ActFrm(node.child(1))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ActFrm") && (node.child(1).string() == "=>") && (symbol_name(node.child(2)) == "ActFrm")) { return action_formulas::imp(parse_ActFrm(node.child(0)), parse_ActFrm(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ActFrm") && (node.child(1).string() == "&&") && (symbol_name(node.child(2)) == "ActFrm")) { return action_formulas::and_(parse_ActFrm(node.child(0)), parse_ActFrm(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ActFrm") && (node.child(1).string() == "||") && (symbol_name(node.child(2)) == "ActFrm")) { return action_formulas::or_(parse_ActFrm(node.child(0)), parse_ActFrm(node.child(2))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "forall") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "ActFrm")) { return action_formulas::forall(parse_VarsDeclList(node.child(1)), parse_ActFrm(node.child(3))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "exists") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "ActFrm")) { return action_formulas::exists(parse_VarsDeclList(node.child(1)), parse_ActFrm(node.child(3))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ActFrm") && (node.child(1).string() == "@") && (symbol_name(node.child(2)) == "DataExpr")) { return action_formulas::at(parse_ActFrm(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "(") && (symbol_name(node.child(1)) == "ActFrm") && (symbol_name(node.child(2)) == ")")) { return parse_ActFrm(node.child(1)); }
    throw core::parse_node_unexpected_exception(m_parser, node);
  }
};

} // namespace action_formulas::detail

namespace regular_formulas::detail
{

struct regular_formula_actions: public action_formulas::detail::action_formula_actions
{
  explicit regular_formula_actions(const core::parser& parser_)
    : action_formulas::detail::action_formula_actions(parser_)
  {}

  regular_formulas::regular_formula parse_RegFrm(const core::parse_node& node) const
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "ActFrm")) { return parse_ActFrm(node.child(0)); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "(") && (symbol_name(node.child(1)) == "RegFrm") && (symbol_name(node.child(2)) == ")")) { return parse_RegFrm(node.child(1)); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "RegFrm") && (symbol_name(node.child(1)) == "*")) { return trans_or_nil(parse_RegFrm(node.child(0))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "RegFrm") && (symbol_name(node.child(1)) == "+")) { return trans(parse_RegFrm(node.child(0))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "RegFrm") && (node.child(1).string() == ".") && (symbol_name(node.child(2)) == "RegFrm")) { return untyped_regular_formula(core::identifier_string("."), parse_RegFrm(node.child(0)), parse_RegFrm(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "RegFrm") && (node.child(1).string() == "+") && (symbol_name(node.child(2)) == "RegFrm")) { return untyped_regular_formula(core::identifier_string("+"), parse_RegFrm(node.child(0)), parse_RegFrm(node.child(2))); }
    throw core::parse_node_unexpected_exception(m_parser, node);
  }
};

} // namespace regular_formulas::detail

namespace state_formulas::detail
{

struct untyped_state_formula_specification: public data::untyped_data_specification
{
  process::action_label_list action_labels;
  state_formula formula;

  state_formula_specification construct_state_formula_specification()
  {
    state_formula_specification result;
    result.data() = construct_data_specification();
    result.action_labels() = action_labels;
    result.formula() = formula;
    return result;
  }
};

struct state_formula_actions: public regular_formulas::detail::regular_formula_actions
{
  explicit state_formula_actions(const core::parser& parser_)
    : regular_formulas::detail::regular_formula_actions(parser_)
  {}

  state_formula make_delay(const core::parse_node& node) const
  {
    if (node.child(0))
    {
      return delay_timed(parse_DataExpr(node.child(0).child(1)));
    }
    else
    {
      return delay();
    }
  }

  state_formula make_yaled(const core::parse_node& node) const
  {
    if (node.child(0))
    {
      return yaled_timed(parse_DataExpr(node.child(0).child(1)));
    }
    else
    {
      return yaled();
    }
  }

  data::assignment parse_StateVarAssignment(const core::parse_node& node) const
  {
    return data::assignment(data::variable(parse_Id(node.child(0)), parse_SortExpr(node.child(2))), parse_DataExpr(node.child(4)));
  }

  data::assignment_list parse_StateVarAssignmentList(const core::parse_node& node) const
  {
    return parse_list<data::assignment>(node, "StateVarAssignment", [&](const core::parse_node& node) { return parse_StateVarAssignment(node); });
  }

  state_formulas::state_formula parse_StateFrm(const core::parse_node& node) const
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "DataValExpr")) { return parse_DataValExpr(node.child(0)); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "DataExpr")) { return parse_DataExpr(node.child(0)); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "true")) { return state_formulas::true_(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "false")) { return state_formulas::false_(); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "!") && (symbol_name(node.child(1)) == "StateFrm")) { return state_formulas::not_(parse_StateFrm(node.child(1))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "-") && (symbol_name(node.child(1)) == "StateFrm")) { return state_formulas::minus(parse_StateFrm(node.child(1))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "StateFrm") && (node.child(1).string() == "=>") && (symbol_name(node.child(2)) == "StateFrm")) { return state_formulas::imp(parse_StateFrm(node.child(0)), parse_StateFrm(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "StateFrm") && (node.child(1).string() == "&&") && (symbol_name(node.child(2)) == "StateFrm")) { return state_formulas::and_(parse_StateFrm(node.child(0)), parse_StateFrm(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "StateFrm") && (node.child(1).string() == "||") && (symbol_name(node.child(2)) == "StateFrm")) { return state_formulas::or_(parse_StateFrm(node.child(0)), parse_StateFrm(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "StateFrm") && (node.child(1).string() == "+") && (symbol_name(node.child(2)) == "StateFrm")) { return state_formulas::plus(parse_StateFrm(node.child(0)), parse_StateFrm(node.child(2))); } 
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataValExpr") && (node.child(1).string() == "*") && (symbol_name(node.child(2)) == "StateFrm")) { return state_formulas::const_multiply(parse_DataValExpr(node.child(0)), parse_StateFrm(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "StateFrm") && (node.child(1).string() == "*") && (symbol_name(node.child(2)) == "DataValExpr")) { return state_formulas::const_multiply_alt(parse_StateFrm(node.child(0)), parse_DataValExpr(node.child(2))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "forall") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "StateFrm")) { return state_formulas::forall(parse_VarsDeclList(node.child(1)), parse_StateFrm(node.child(3))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "exists") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "StateFrm")) { return state_formulas::exists(parse_VarsDeclList(node.child(1)), parse_StateFrm(node.child(3))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "inf") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "StateFrm")) { return state_formulas::infimum(parse_VarsDeclList(node.child(1)), parse_StateFrm(node.child(3))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "sup") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "StateFrm")) { return state_formulas::supremum(parse_VarsDeclList(node.child(1)), parse_StateFrm(node.child(3))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "sum") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "StateFrm")) { return state_formulas::sum(parse_VarsDeclList(node.child(1)), parse_StateFrm(node.child(3))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "[") && (symbol_name(node.child(1)) == "RegFrm") && (symbol_name(node.child(2)) == "]") && (symbol_name(node.child(3)) == "StateFrm")) { return state_formulas::must(parse_RegFrm(node.child(1)), parse_StateFrm(node.child(3))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "<") && (symbol_name(node.child(1)) == "RegFrm") && (symbol_name(node.child(2)) == ">") && (symbol_name(node.child(3)) == "StateFrm")) { return state_formulas::may(parse_RegFrm(node.child(1)), parse_StateFrm(node.child(3))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "mu") && (symbol_name(node.child(1)) == "StateVarDecl") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "StateFrm")) { return state_formulas::mu(parse_Id(node.child(1).child(0)), parse_StateVarAssignmentList(node.child(1).child(1)), parse_StateFrm(node.child(3))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "nu") && (symbol_name(node.child(1)) == "StateVarDecl") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "StateFrm")) { return state_formulas::nu(parse_Id(node.child(1).child(0)), parse_StateVarAssignmentList(node.child(1).child(1)), parse_StateFrm(node.child(3))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "Id")) { return data::untyped_data_parameter(parse_Id(node.child(0)), parse_DataExprList(node.child(1))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "delay")) { return make_delay(node.child(1)); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "yaled")) { return make_yaled(node.child(1)); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "(") && (symbol_name(node.child(1)) == "StateFrm") && (symbol_name(node.child(2)) == ")")) { return parse_StateFrm(node.child(1)); }
    throw core::parse_node_unexpected_exception(m_parser, node);
  }

  state_formula parse_FormSpec(const core::parse_node& node) const
  {
    return parse_StateFrm(node.child(1));
  }

  bool callback_StateFrmSpec(const core::parse_node& node, untyped_state_formula_specification& result) const
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
    else if (symbol_name(node) == "ActSpec")
    {
      result.action_labels = result.action_labels + parse_ActSpec(node);
      return true;
    }
    else if (symbol_name(node) == "FormSpec")
    {
      result.formula = parse_FormSpec(node);
      return true;
    }
    else if (symbol_name(node) == "StateFrm")
    {
      result.formula = parse_StateFrm(node);
      return true;
    }
    return false;
  }

  untyped_state_formula_specification parse_StateFrmSpec(const core::parse_node& node) const
  {
    untyped_state_formula_specification result;
    traverse(node, [&](const core::parse_node& node) { return callback_StateFrmSpec(node, result); });
    return result;
  }
};

} // namespace state_formulas::detail

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_PARSE_IMPL_H
