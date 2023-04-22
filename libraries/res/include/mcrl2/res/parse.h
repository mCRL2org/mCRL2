// Author(s): Jan Friso Groote. Based on bes/parse.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/res/parse.h
/// \brief add your file description here.

#ifndef MCRL2_RES_PARSE_H
#define MCRL2_RES_PARSE_H

#include "mcrl2/res/res_equation_system.h"
#include "mcrl2/res/detail/pres_expression2res_expression_traverser.h"
#include "mcrl2/core/parser_utility.h"
#include "mcrl2/pres/is_res.h"
#include "mcrl2/pres/parse.h"

namespace mcrl2
{

namespace res
{

namespace detail {

struct res_actions: public core::default_parser_actions
{
  res_actions(const core::parser& parser_)
    : core::default_parser_actions(parser_)
  {}

  res::res_expression parse_BesExpr(const core::parse_node& node) const
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "true")) { return res::true_(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "false")) { return res::false_(); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "!") && (symbol_name(node.child(1)) == "BesExpr")) { return res::minus(parse_BesExpr(node.child(1))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "BesExpr") && (node.child(1).string() == "=>") && (symbol_name(node.child(2)) == "BesExpr")) { return res::imp(parse_BesExpr(node.child(0)), parse_BesExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "BesExpr") && (node.child(1).string() == "&&") && (symbol_name(node.child(2)) == "BesExpr")) { return res::and_(parse_BesExpr(node.child(0)), parse_BesExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "BesExpr") && (node.child(1).string() == "||") && (symbol_name(node.child(2)) == "BesExpr")) { return res::or_(parse_BesExpr(node.child(0)), parse_BesExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "(") && (symbol_name(node.child(1)) == "BesExpr") && (symbol_name(node.child(2)) == ")")) { return parse_BesExpr(node.child(1)); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "BesVar")) { return parse_BesVar(node.child(0)); }
    throw core::parse_node_unexpected_exception(m_parser, node);
  }

  res::res_variable parse_BesVar(const core::parse_node& node) const
  {
    return res::res_variable(parse_Id(node.child(0)));
  }

  fixpoint_symbol parse_FixedPointOperator(const core::parse_node& node) const
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "mu")) { return fixpoint_symbol::mu(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "nu")) { return fixpoint_symbol::nu(); }
    throw core::parse_node_unexpected_exception(m_parser, node);
  }

  res::res_equation parse_BesEqnDecl(const core::parse_node& node) const
  {
    return res::res_equation(parse_FixedPointOperator(node.child(0)), parse_BesVar(node.child(1)), parse_BesExpr(node.child(3)));
  }

  std::vector<res_equation> parse_BesEqnSpec(const core::parse_node& node) const
  {
    return parse_BesEqnDeclList(node.child(1));
  }

  res::res_variable parse_BesInit(const core::parse_node& node) const
  {
    return parse_BesVar(node.child(1));
  }

  res::res_equation_system parse_BesSpec(const core::parse_node& node) const
  {
    return res::res_equation_system(parse_BesEqnSpec(node.child(0)), parse_BesInit(node.child(1)));
  }

  std::vector<res_equation> parse_BesEqnDeclList(const core::parse_node& node) const
  {
    return parse_vector<res::res_equation>(node, "BesEqnDecl", [&](const core::parse_node& node) { return parse_BesEqnDecl(node); });
  }
};

inline
res_expression parse_res_expression(const std::string& text)
{
  core::parser p(parser_tables_mcrl2, core::detail::ambiguity_fn, core::detail::syntax_error_fn);
  unsigned int start_symbol_index = p.start_symbol_index("BesExpr");
  bool partial_parses = false;
  core::parse_node node = p.parse(text, start_symbol_index, partial_parses);
  core::warn_and_or(node);
  res_expression result = detail::res_actions(p).parse_BesExpr(node);
  return result;
}

inline
res_equation_system parse_res_equation_system(const std::string& text)
{
  core::parser p(parser_tables_mcrl2, core::detail::ambiguity_fn, core::detail::syntax_error_fn);
  unsigned int start_symbol_index = p.start_symbol_index("BesSpec");
  bool partial_parses = false;
  core::parse_node node = p.parse(text, start_symbol_index, partial_parses);
  core::warn_and_or(node);
  res_equation_system result = detail::res_actions(p).parse_BesSpec(node);
  return result;
}

} // namespace detail

/// \brief Reads a res equation system from an input stream.
/// \param from An input stream
/// \param b A res equation system
/// \return The input stream
inline
std::istream& operator>>(std::istream& from, res_equation_system& b)
{
  pres_system::pres p;
  from >> p;
  if (!is_res(p))
  {
    throw mcrl2::runtime_error("parsing of res equation system failed: it is not a RES!");
  }

  std::vector<res_equation> equations;
  for (const pres_system::pres_equation& eqn: p.equations())
  {
    res_variable v(eqn.variable().name());
    res_expression rhs = res::pres_expression2res_expression(eqn.formula());
    equations.emplace_back(eqn.symbol(), v, rhs);
  }

  res_expression init = res::pres_expression2res_expression(p.initial_state());
  b = res_equation_system(equations, init);
  return from;
}

} // namespace res

} // namespace mcrl2

#endif // MCRL2_RES_PARSE_H
