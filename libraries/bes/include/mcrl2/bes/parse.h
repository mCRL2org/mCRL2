// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/parse.h
/// \brief add your file description here.

#ifndef MCRL2_BES_PARSE_H
#define MCRL2_BES_PARSE_H

#include "mcrl2/core/parser_utility.h"
#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/bes/detail/pbes_expression2boolean_expression_visitor.h"
#include "mcrl2/pbes/is_bes.h"
#include "mcrl2/pbes/parse.h"

namespace mcrl2
{

namespace bes
{

struct bes_actions: public core::default_parser_actions
{
  bes_actions(const core::parser_table& table_)
    : core::default_parser_actions(table_)
  {}

  bes::boolean_expression parse_BesExpr(const core::parse_node& node)
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "true")) { return bes::true_(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "false")) { return bes::false_(); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "!") && (symbol_name(node.child(1)) == "BesExpr")) { return bes::not_(parse_BesExpr(node.child(1))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "BesExpr") && (node.child(1).string() == "=>") && (symbol_name(node.child(2)) == "BesExpr")) { return bes::imp(parse_BesExpr(node.child(0)), parse_BesExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "BesExpr") && (node.child(1).string() == "&&") && (symbol_name(node.child(2)) == "BesExpr")) { return bes::and_(parse_BesExpr(node.child(0)), parse_BesExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "BesExpr") && (node.child(1).string() == "||") && (symbol_name(node.child(2)) == "BesExpr")) { return bes::or_(parse_BesExpr(node.child(0)), parse_BesExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "(") && (symbol_name(node.child(1)) == "BesExpr") && (symbol_name(node.child(2)) == ")")) { return parse_BesExpr(node.child(1)); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "BesVar")) { return parse_BesVar(node.child(0)); }
    report_unexpected_node(node);
    return bes::boolean_expression();
  }

  bes::boolean_variable parse_BesVar(const core::parse_node& node)
  {
    return bes::boolean_variable(parse_Id(node.child(0)));
  }

  fixpoint_symbol parse_FixedPointOperator(const core::parse_node& node)
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "mu")) { return fixpoint_symbol::mu(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "nu")) { return fixpoint_symbol::nu(); }
    report_unexpected_node(node);
    return pbes_system::fixpoint_symbol();
  }

  bes::boolean_equation parse_BesEqnDecl(const core::parse_node& node)
  {
    return bes::boolean_equation(parse_FixedPointOperator(node.child(0)), parse_BesVar(node.child(1)), parse_BesExpr(node.child(3)));
  }

  std::vector<boolean_equation> parse_BesEqnSpec(const core::parse_node& node)
  {
    return parse_BesEqnDeclList(node.child(1));
  }

  bes::boolean_variable parse_BesInit(const core::parse_node& node)
  {
    return parse_BesVar(node.child(1));
  }

  bes::boolean_equation_system parse_BesSpec(const core::parse_node& node)
  {
    return bes::boolean_equation_system(parse_BesEqnSpec(node.child(0)), parse_BesInit(node.child(1)));
  }

  std::vector<boolean_equation> parse_BesEqnDeclList(const core::parse_node& node)
  {
    return parse_vector<bes::boolean_equation>(node, "BesEqnDecl", boost::bind(&bes_actions::parse_BesEqnDecl, this, _1));
  }
};

inline
boolean_expression parse_boolean_expression_new(const std::string& text)
{
  core::parser p(parser_tables_mcrl2, core::detail::ambiguity_fn, core::detail::syntax_error_fn);
  unsigned int start_symbol_index = p.start_symbol_index("BesExpr");
  bool partial_parses = false;
  core::parse_node node = p.parse(text, start_symbol_index, partial_parses);
  core::warn_and_or(node);
  boolean_expression result = bes_actions(parser_tables_mcrl2).parse_BesExpr(node);
  p.destroy_parse_node(node);
  return result;
}

inline
boolean_equation_system parse_boolean_equation_system_new(const std::string& text)
{
  core::parser p(parser_tables_mcrl2, core::detail::ambiguity_fn, core::detail::syntax_error_fn);
  unsigned int start_symbol_index = p.start_symbol_index("BesSpec");
  bool partial_parses = false;
  core::parse_node node = p.parse(text, start_symbol_index, partial_parses);
  core::warn_and_or(node);
  boolean_equation_system result = bes_actions(parser_tables_mcrl2).parse_BesSpec(node);
  p.destroy_parse_node(node);
  return result;
}

/// \brief Converts a pbes expression to a boolean expression.
/// Throws an mcrl2_error if it is not a valid boolean expression.
inline
boolean_expression pbes_expression2boolean_expression(const pbes_system::pbes_expression& x)
{
  bes::detail::pbes_expression2boolean_expression_visitor<pbes_system::pbes_expression> visitor;
  visitor.visit(x);
  return visitor.result();
}

/// \brief Reads a boolean equation system from an input stream.
/// \param from An input stream
/// \param b A boolean equation system
/// \return The input stream
inline
std::istream& operator>>(std::istream& from, boolean_equation_system& b)
{
  pbes_system::pbes p;
  from >> p;
  if (!is_bes(p))
  {
    throw mcrl2::runtime_error("parsing of boolean equation system failed: it is not a BES!");
  }

  std::vector<boolean_equation> equations;
  for (std::vector<pbes_system::pbes_equation>::const_iterator i = p.equations().begin(); i != p.equations().end(); ++i)
  {
    boolean_variable v(i->variable().name());
    boolean_expression rhs = pbes_expression2boolean_expression(i->formula());
    equations.push_back(boolean_equation(i->symbol(), v, rhs));
  }

  boolean_expression init = pbes_expression2boolean_expression(p.initial_state());
  b = boolean_equation_system(equations, init);
  return from;
}

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_PARSE_H
