// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/parse_impl.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_PARSE_IMPL_H
#define MCRL2_PBES_PARSE_IMPL_H

#include "mcrl2/data/detail/parse_substitution.h"
#include "mcrl2/data/parse_impl.h"
#include "mcrl2/pbes/typecheck.h"
#include "mcrl2/pbes/untyped_pbes.h"

namespace mcrl2::pbes_system::detail
{

struct pbes_actions: public data::detail::data_specification_actions
{
  explicit pbes_actions(const core::parser& parser_)
    : data::detail::data_specification_actions(parser_)
  {}

  pbes_system::pbes_expression parse_PbesExpr(const core::parse_node& node) const
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "DataValExpr")) { return parse_DataValExpr(node.child(0)); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "DataExpr")) { return parse_DataExpr(node.child(0)); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "true")) { return pbes_system::true_(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "false")) { return pbes_system::false_(); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "forall") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "PbesExpr")) { return pbes_system::forall(parse_VarsDeclList(node.child(1)), parse_PbesExpr(node.child(3))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "exists") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "PbesExpr")) { return pbes_system::exists(parse_VarsDeclList(node.child(1)), parse_PbesExpr(node.child(3))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "!") && (symbol_name(node.child(1)) == "PbesExpr")) { return pbes_system::not_(parse_PbesExpr(node.child(1))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "PbesExpr") && (node.child(1).string() == "=>") && (symbol_name(node.child(2)) == "PbesExpr")) { return pbes_system::imp(parse_PbesExpr(node.child(0)), parse_PbesExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "PbesExpr") && (node.child(1).string() == "&&") && (symbol_name(node.child(2)) == "PbesExpr")) { return pbes_system::and_(parse_PbesExpr(node.child(0)), parse_PbesExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "PbesExpr") && (node.child(1).string() == "||") && (symbol_name(node.child(2)) == "PbesExpr")) { return pbes_system::or_(parse_PbesExpr(node.child(0)), parse_PbesExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "(") && (symbol_name(node.child(1)) == "PbesExpr") && (symbol_name(node.child(2)) == ")")) { return parse_PbesExpr(node.child(1)); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "PropVarInst")) { return parse_PropVarInst(node.child(0)); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "Id")) { return data::untyped_data_parameter(parse_Id(node.child(0)), parse_DataExprList(node.child(1))); }
    throw core::parse_node_unexpected_exception(m_parser, node);
  }

  pbes_system::propositional_variable parse_PropVarDecl(const core::parse_node& node) const
  {
    return pbes_system::propositional_variable(parse_Id(node.child(0)), parse_VarsDeclList(node.child(1)));
  }

  pbes_system::propositional_variable_instantiation parse_PropVarInst(const core::parse_node& node) const
  {
    return pbes_system::propositional_variable_instantiation(parse_Id(node.child(0)), parse_DataExprList(node.child(1)));
  }

  pbes_system::propositional_variable_instantiation parse_PbesInit(const core::parse_node& node) const
  {
    return parse_PropVarInst(node.child(1));
  }

  pbes_system::fixpoint_symbol parse_FixedPointOperator(const core::parse_node& node) const
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "mu")) { return fixpoint_symbol::mu(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "nu")) { return fixpoint_symbol::nu(); }
    throw core::parse_node_unexpected_exception(m_parser, node);
  }

  pbes_equation parse_PbesEqnDecl(const core::parse_node& node) const
  {
    return pbes_equation(parse_FixedPointOperator(node.child(0)), parse_PropVarDecl(node.child(1)), parse_PbesExpr(node.child(3)));
  }

  std::vector<pbes_equation> parse_PbesEqnDeclList(const core::parse_node& node) const
  {
    return parse_vector<pbes_equation>(node, "PbesEqnDecl", [&](const core::parse_node& node) { return parse_PbesEqnDecl(node); });
  }

  std::vector<pbes_equation> parse_PbesEqnSpec(const core::parse_node& node) const
  {
    return parse_PbesEqnDeclList(node.child(1));
  }

  untyped_pbes parse_PbesSpec(const core::parse_node& node) const
  {
    untyped_pbes result;
    result.dataspec = parse_DataSpec(node.child(0));
    result.global_variables = parse_GlobVarSpec(node.child(1));
    result.equations = parse_PbesEqnSpec(node.child(2));
    result.initial_state = parse_PbesInit(node.child(3));
    return result;
  }
};

} // namespace mcrl2::pbes_system::detail

#endif // MCRL2_PBES_PARSE_IMPL_H
