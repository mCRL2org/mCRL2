// Author(s): Jan Friso Groote. Based on pbes/parse_impl.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/parse_impl.h
/// \brief add your file description here.

#ifndef MCRL2_PRES_PARSE_IMPL_H
#define MCRL2_PRES_PARSE_IMPL_H

#include "mcrl2/data/detail/parse_substitution.h"
#include "mcrl2/data/parse_impl.h"
#include "mcrl2/pres/typecheck.h"
#include "mcrl2/pres/untyped_pres.h"





namespace mcrl2::pres_system::detail
{

struct pres_actions: public data::detail::data_specification_actions
{
  explicit pres_actions(const core::parser& parser_)
    : data::detail::data_specification_actions(parser_)
  {}

  pres_system::pres_expression parse_PresExpr(const core::parse_node& node) const
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "DataValExpr")) { return parse_DataValExpr(node.child(0)); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "DataExpr")) { return parse_DataExpr(node.child(0)); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "true")) { return pres_system::true_(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "false")) { return pres_system::false_(); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "inf") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "PresExpr")) { return pres_system::infimum(parse_VarsDeclList(node.child(1)), parse_PresExpr(node.child(3))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "sup") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "PresExpr")) { return pres_system::supremum(parse_VarsDeclList(node.child(1)), parse_PresExpr(node.child(3))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "sum") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "PresExpr")) { return pres_system::sum(parse_VarsDeclList(node.child(1)), parse_PresExpr(node.child(3))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "-") && (symbol_name(node.child(1)) == "PresExpr")) { return pres_system::minus(parse_PresExpr(node.child(1))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "PresExpr") && (node.child(1).string() == "=>") && (symbol_name(node.child(2)) == "PresExpr")) { return pres_system::imp(parse_PresExpr(node.child(0)), parse_PresExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "PresExpr") && (node.child(1).string() == "&&") && (symbol_name(node.child(2)) == "PresExpr")) { return pres_system::and_(parse_PresExpr(node.child(0)), parse_PresExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "PresExpr") && (node.child(1).string() == "||") && (symbol_name(node.child(2)) == "PresExpr")) { return pres_system::or_(parse_PresExpr(node.child(0)), parse_PresExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "PresExpr") && (node.child(1).string() == "+") && (symbol_name(node.child(2)) == "PresExpr")) { return pres_system::plus(parse_PresExpr(node.child(0)), parse_PresExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataValExpr") && (node.child(1).string() == "*") && (symbol_name(node.child(2)) == "PresExpr")) { return pres_system::const_multiply(parse_DataValExpr(node.child(0)), parse_PresExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "PresExpr") && (node.child(1).string() == "*") && (symbol_name(node.child(2)) == "DataValExpr")) { return pres_system::const_multiply_alt(parse_PresExpr(node.child(0)), parse_DataValExpr(node.child(2))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "eqinf") && (node.child(1).string() == "(") && (symbol_name(node.child(2)) == "PresExpr") && (node.child(1).string() == ")")) { return pres_system::const_multiply_alt(parse_PresExpr(node.child(0)), parse_DataValExpr(node.child(2))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "eqninf") && (node.child(1).string() == "(") && (symbol_name(node.child(2)) == "PresExpr") && (node.child(1).string() == ")")) { return pres_system::const_multiply_alt(parse_PresExpr(node.child(0)), parse_DataValExpr(node.child(2))); }
    else if ((node.child_count() == 8) && (symbol_name(node.child(0)) == "condsm") && (node.child(1).string() == "(") && (symbol_name(node.child(2)) == "PresExpr") && (node.child(3).string() == ",") && (symbol_name(node.child(4)) == "PresExpr") && (node.child(5).string() == ",") && (symbol_name(node.child(6)) == "PresExpr") && (node.child(7).string() == ")")) { return pres_system::condsm(parse_PresExpr(node.child(2)), parse_PresExpr(node.child(4)), parse_PresExpr(node.child(6))); }
    else if ((node.child_count() == 8) && (symbol_name(node.child(0)) == "condeq") && (node.child(1).string() == "(") && (symbol_name(node.child(2)) == "PresExpr") && (node.child(3).string() == ",") && (symbol_name(node.child(4)) == "PresExpr") && (node.child(5).string() == ",") && (symbol_name(node.child(6)) == "PresExpr") && (node.child(7).string() == ")")) { return pres_system::condeq(parse_PresExpr(node.child(2)), parse_PresExpr(node.child(4)), parse_PresExpr(node.child(6))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "(") && (symbol_name(node.child(1)) == "PresExpr") && (symbol_name(node.child(2)) == ")")) { return parse_PresExpr(node.child(1)); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "PropVarInst")) { return parse_PropVarInst(node.child(0)); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "Id")) { return data::untyped_data_parameter(parse_Id(node.child(0)), parse_DataExprList(node.child(1))); }
    throw core::parse_node_unexpected_exception(m_parser, node);
  }

  pres_system::propositional_variable parse_PropVarDecl(const core::parse_node& node) const
  {
    return pres_system::propositional_variable(parse_Id(node.child(0)), parse_VarsDeclList(node.child(1)));
  }

  pres_system::propositional_variable_instantiation parse_PropVarInst(const core::parse_node& node) const
  {
    return pres_system::propositional_variable_instantiation(parse_Id(node.child(0)), parse_DataExprList(node.child(1)));
  }

  pres_system::propositional_variable_instantiation parse_PresInit(const core::parse_node& node) const
  {
    return parse_PropVarInst(node.child(1));
  }

  pres_system::fixpoint_symbol parse_FixedPointOperator(const core::parse_node& node) const
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "mu")) { return fixpoint_symbol::mu(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "nu")) { return fixpoint_symbol::nu(); }
    throw core::parse_node_unexpected_exception(m_parser, node);
  }

  pres_equation parse_PresEqnDecl(const core::parse_node& node) const
  {
    return pres_equation(parse_FixedPointOperator(node.child(0)), parse_PropVarDecl(node.child(1)), parse_PresExpr(node.child(3)));
  }

  std::vector<pres_equation> parse_PresEqnDeclList(const core::parse_node& node) const
  {
    return parse_vector<pres_equation>(node, "PresEqnDecl", [&](const core::parse_node& node) { return parse_PresEqnDecl(node); });
  }

  std::vector<pres_equation> parse_PresEqnSpec(const core::parse_node& node) const
  {
    return parse_PresEqnDeclList(node.child(1));
  }

  untyped_pres parse_PresSpec(const core::parse_node& node) const
  {
    untyped_pres result;
    result.dataspec = parse_DataSpec(node.child(0));
    result.global_variables = parse_GlobVarSpec(node.child(1));
    result.equations = parse_PresEqnSpec(node.child(2));
    result.initial_state = parse_PresInit(node.child(3));
    return result;
  }
};

} // namespace mcrl2::pres_system::detail





#endif // MCRL2_PRES_PARSE_IMPL_H
