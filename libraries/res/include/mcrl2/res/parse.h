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

#include "mcrl2/core/parser_utility.h"
#include "mcrl2/data/parse_impl.h"
#include "mcrl2/res/res_equation_system.h"
#include "mcrl2/res/detail/pres_expression2res_expression_traverser.h"
#include "mcrl2/pres/is_res.h"
#include "mcrl2/pres/parse.h"

namespace mcrl2
{

namespace res
{

namespace detail {

struct res_actions: public data::detail::data_expression_actions
{
  res_actions(const core::parser& parser_)
    : data::detail::data_expression_actions(parser_)
    // : core::default_parser_actions(parser_)
  {}

  res::res_expression parse_ResExpr(const core::parse_node& node) const
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "true")) { return res::true_(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "false")) { return res::false_(); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "!") && (symbol_name(node.child(1)) == "ResExpr")) { return res::minus(parse_ResExpr(node.child(1))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ResExpr") && (node.child(1).string() == "=>") && (symbol_name(node.child(2)) == "ResExpr")) { return res::imp(parse_ResExpr(node.child(0)), parse_ResExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ResExpr") && (node.child(1).string() == "&&") && (symbol_name(node.child(2)) == "ResExpr")) { return res::and_(parse_ResExpr(node.child(0)), parse_ResExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ResExpr") && (node.child(1).string() == "||") && (symbol_name(node.child(2)) == "ResExpr")) { return res::or_(parse_ResExpr(node.child(0)), parse_ResExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ResExpr") && (node.child(1).string() == "+") && (symbol_name(node.child(2)) == "ResExpr")) { return res::plus(parse_ResExpr(node.child(0)), parse_ResExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataValExpr") && (node.child(1).string() == "*") && (symbol_name(node.child(2)) == "ResExpr")) { return res::const_multiply(parse_DataValExpr(node.child(0)), parse_ResExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ResExpr") && (node.child(1).string() == "*") && (symbol_name(node.child(2)) == "DataValExpr")) { return res::const_multiply_alt(parse_ResExpr(node.child(0)), parse_DataValExpr(node.child(2))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "eqinf") && (node.child(1).string() == "(") && (symbol_name(node.child(2)) == "ResExpr") && (node.child(1).string() == ")")) { return res::const_multiply_alt(parse_ResExpr(node.child(0)), parse_DataValExpr(node.child(2))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "eqninf") && (node.child(1).string() == "(") && (symbol_name(node.child(2)) == "ResExpr") && (node.child(1).string() == ")")) { return res::const_multiply_alt(parse_ResExpr(node.child(0)), parse_DataValExpr(node.child(2))); }
    else if ((node.child_count() == 8) && (symbol_name(node.child(0)) == "condsm") && (node.child(1).string() == "(") && (symbol_name(node.child(2)) == "ResExpr") && (node.child(3).string() == ",") && (symbol_name(node.child(4)) == "ResExpr") && (node.child(5).string() == ",") && (symbol_name(node.child(6)) == "ResExpr") && (node.child(7).string() == ")")) { return res::condsm(parse_ResExpr(node.child(2)), parse_ResExpr(node.child(4)), parse_ResExpr(node.child(6))); }
    else if ((node.child_count() == 8) && (symbol_name(node.child(0)) == "condeq") && (node.child(1).string() == "(") && (symbol_name(node.child(2)) == "ResExpr") && (node.child(3).string() == ",") && (symbol_name(node.child(4)) == "ResExpr") && (node.child(5).string() == ",") && (symbol_name(node.child(6)) == "ResExpr") && (node.child(7).string() == ")")) { return res::condeq(parse_ResExpr(node.child(2)), parse_ResExpr(node.child(4)), parse_ResExpr(node.child(6))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "(") && (symbol_name(node.child(1)) == "ResExpr") && (symbol_name(node.child(2)) == ")")) { return parse_ResExpr(node.child(1)); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "ResVar")) { return parse_ResVar(node.child(0)); }
    throw core::parse_node_unexpected_exception(m_parser, node);
  }

  res::res_variable parse_ResVar(const core::parse_node& node) const
  {
    return res::res_variable(parse_Id(node.child(0)));
  }

  fixpoint_symbol parse_FixedPointOperator(const core::parse_node& node) const
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "mu")) { return fixpoint_symbol::mu(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "nu")) { return fixpoint_symbol::nu(); }
    throw core::parse_node_unexpected_exception(m_parser, node);
  }

  res::res_equation parse_ResEqnDecl(const core::parse_node& node) const
  {
    return res::res_equation(parse_FixedPointOperator(node.child(0)), parse_ResVar(node.child(1)), parse_ResExpr(node.child(3)));
  }

  std::vector<res_equation> parse_ResEqnSpec(const core::parse_node& node) const
  {
    return parse_ResEqnDeclList(node.child(1));
  }

  res::res_variable parse_ResInit(const core::parse_node& node) const
  {
    return parse_ResVar(node.child(1));
  }

  res::res_equation_system parse_ResSpec(const core::parse_node& node) const
  {
    return res::res_equation_system(parse_ResEqnSpec(node.child(0)), parse_ResInit(node.child(1)));
  }

  std::vector<res_equation> parse_ResEqnDeclList(const core::parse_node& node) const
  {
    return parse_vector<res::res_equation>(node, "ResEqnDecl", [&](const core::parse_node& node) { return parse_ResEqnDecl(node); });
  }
};

inline
res_expression parse_res_expression(const std::string& text)
{
  core::parser p(parser_tables_mcrl2, core::detail::ambiguity_fn, core::detail::syntax_error_fn);
  unsigned int start_symbol_index = p.start_symbol_index("ResExpr");
  bool partial_parses = false;
  core::parse_node node = p.parse(text, start_symbol_index, partial_parses);
  core::warn_and_or(node);
  res_expression result = detail::res_actions(p).parse_ResExpr(node);
  return result;
}

inline
res_equation_system parse_res_equation_system(const std::string& text)
{
  core::parser p(parser_tables_mcrl2, core::detail::ambiguity_fn, core::detail::syntax_error_fn);
  unsigned int start_symbol_index = p.start_symbol_index("ResSpec");
  bool partial_parses = false;
  core::parse_node node = p.parse(text, start_symbol_index, partial_parses);
  core::warn_and_or(node);
  res_equation_system result = detail::res_actions(p).parse_ResSpec(node);
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
