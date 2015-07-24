// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/parse.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_PARSE_H
#define MCRL2_MODAL_FORMULA_PARSE_H

#include <iostream>
#include "mcrl2/core/parser_utility.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/modal_formula/typecheck.h"
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/modal_formula/translate_regular_formulas.h"
#include "mcrl2/modal_formula/has_name_clashes.h"
#include "mcrl2/modal_formula/resolve_name_clashes.h"

namespace mcrl2
{

namespace action_formulas
{

namespace detail
{

struct action_formula_actions: public lps::detail::multi_action_actions
{
  action_formula_actions(const core::parser& parser_)
    : lps::detail::multi_action_actions(parser_)
  {}

  action_formulas::action_formula parse_ActFrm(const core::parse_node& node) const
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "MultAct")) { return action_formulas::untyped_multi_action(parse_ActionList(node.child(0))); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "DataValExpr")) { return parse_DataValExpr(node.child(0)); }
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

inline
action_formula parse_action_formula_new(const std::string& text)
{
  core::parser p(parser_tables_mcrl2, core::detail::ambiguity_fn, core::detail::syntax_error_fn);
  unsigned int start_symbol_index = p.start_symbol_index("ActFrm");
  bool partial_parses = false;
  core::parse_node node = p.parse(text, start_symbol_index, partial_parses);
  core::warn_and_or(node);
  action_formula result = action_formula_actions(p).parse_ActFrm(node);
  p.destroy_parse_node(node);
  return result;
}

} // namespace detail

} // namespace action_formulas

namespace regular_formulas
{

namespace detail
{

struct regular_formula_actions: public action_formulas::detail::action_formula_actions
{
  regular_formula_actions(const core::parser& parser_)
    : action_formulas::detail::action_formula_actions(parser_)
  {}

  regular_formulas::regular_formula parse_RegFrm(const core::parse_node& node) const
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "ActFrm")) { return parse_ActFrm(node.child(0)); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "nil")) { return regular_formulas::nil(); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "(") && (symbol_name(node.child(1)) == "RegFrm") && (symbol_name(node.child(2)) == ")")) { return parse_RegFrm(node.child(1)); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "RegFrm") && (symbol_name(node.child(1)) == "*")) { return trans_or_nil(parse_RegFrm(node.child(0))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "RegFrm") && (symbol_name(node.child(1)) == "+")) { return trans(parse_RegFrm(node.child(0))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "RegFrm") && (node.child(1).string() == ".") && (symbol_name(node.child(2)) == "RegFrm")) { return seq(parse_RegFrm(node.child(0)), parse_RegFrm(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "RegFrm") && (node.child(1).string() == "+") && (symbol_name(node.child(2)) == "RegFrm")) { return alt(parse_RegFrm(node.child(0)), parse_RegFrm(node.child(2))); }
    throw core::parse_node_unexpected_exception(m_parser, node);
  }
};

inline
regular_formula parse_regular_formula_new(const std::string& text)
{
  core::parser p(parser_tables_mcrl2, core::detail::ambiguity_fn, core::detail::syntax_error_fn);
  unsigned int start_symbol_index = p.start_symbol_index("RegFrm");
  bool partial_parses = false;
  core::parse_node node = p.parse(text, start_symbol_index, partial_parses);
  regular_formula result = regular_formula_actions(p).parse_RegFrm(node);
  p.destroy_parse_node(node);
  return result;
}

} // namespace detail

} // namespace regular_formulas

namespace state_formulas
{

namespace detail
{

struct state_formula_actions: public regular_formulas::detail::regular_formula_actions
{
  state_formula_actions(const core::parser& parser_)
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
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "true")) { return state_formulas::true_(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "false")) { return state_formulas::false_(); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "!") && (symbol_name(node.child(1)) == "StateFrm")) { return state_formulas::not_(parse_StateFrm(node.child(1))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "StateFrm") && (node.child(1).string() == "=>") && (symbol_name(node.child(2)) == "StateFrm")) { return state_formulas::imp(parse_StateFrm(node.child(0)), parse_StateFrm(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "StateFrm") && (node.child(1).string() == "&&") && (symbol_name(node.child(2)) == "StateFrm")) { return state_formulas::and_(parse_StateFrm(node.child(0)), parse_StateFrm(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "StateFrm") && (node.child(1).string() == "||") && (symbol_name(node.child(2)) == "StateFrm")) { return state_formulas::or_(parse_StateFrm(node.child(0)), parse_StateFrm(node.child(2))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "forall") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "StateFrm")) { return state_formulas::forall(parse_VarsDeclList(node.child(1)), parse_StateFrm(node.child(3))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "exists") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "StateFrm")) { return state_formulas::exists(parse_VarsDeclList(node.child(1)), parse_StateFrm(node.child(3))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "[") && (symbol_name(node.child(1)) == "RegFrm") && (symbol_name(node.child(2)) == "]") && (symbol_name(node.child(3)) == "StateFrm")) { return state_formulas::must(parse_RegFrm(node.child(1)), parse_StateFrm(node.child(3))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "<") && (symbol_name(node.child(1)) == "RegFrm") && (symbol_name(node.child(2)) == ">") && (symbol_name(node.child(3)) == "StateFrm")) { return state_formulas::may(parse_RegFrm(node.child(1)), parse_StateFrm(node.child(3))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "mu") && (symbol_name(node.child(1)) == "StateVarDecl") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "StateFrm")) { return state_formulas::mu(parse_Id(node.child(1).child(0)), parse_StateVarAssignmentList(node.child(1).child(1)), parse_StateFrm(node.child(3))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "nu") && (symbol_name(node.child(1)) == "StateVarDecl") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "StateFrm")) { return state_formulas::nu(parse_Id(node.child(1).child(0)), parse_StateVarAssignmentList(node.child(1).child(1)), parse_StateFrm(node.child(3))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "Id")) { return state_formulas::variable(parse_Id(node.child(0)), parse_DataExprList(node.child(1))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "delay")) { return make_delay(node.child(1)); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "yaled")) { return make_yaled(node.child(1)); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "(") && (symbol_name(node.child(1)) == "StateFrm") && (symbol_name(node.child(2)) == ")")) { return parse_StateFrm(node.child(1)); }
    throw core::parse_node_unexpected_exception(m_parser, node);
  }
};

inline
state_formula parse_state_formula_new(const std::string& text)
{
  core::parser p(parser_tables_mcrl2, core::detail::ambiguity_fn, core::detail::syntax_error_fn);
  unsigned int start_symbol_index = p.start_symbol_index("StateFrm");
  bool partial_parses = false;
  core::parse_node node = p.parse(text, start_symbol_index, partial_parses);
  core::warn_and_or(node);
  state_formula result = state_formula_actions(p).parse_StateFrm(node);
  p.destroy_parse_node(node);
  return result;
}

} // namespace detail

/// \brief Parses a state formula from an input stream
// spec may be updated as the data implementation of the state formula
// may cause internal names to change.
/// \param formula_stream A stream from which can be read
/// \param spec A linear process specification
/// \param check_monotonicity If true, an exception will be thrown if the formula is not monotonous. Furthermore, name clashes are resolved.
/// \return The converted modal formula
inline
state_formula parse_state_formula(std::istream& in, lps::specification& spec, bool check_monotonicity = true, bool translate_regular = true,
                                  bool type_check = true, bool translate_user_notation = true, bool normalize_sorts = true)
{
  std::string text = utilities::read_text(in);
  state_formula x = detail::parse_state_formula_new(text);
  if (find_nil(x))
  {
    throw mcrl2::runtime_error("regular formulas containing nil are unsupported!");
  }
  if (type_check)
  {
    x = state_formulas::type_check(x, spec, check_monotonicity);
  }
  if (translate_regular)
  {
    mCRL2log(log::debug) << "formula before translating regular formulas: " << x << std::endl;
    x = translate_regular_formulas(x);
    mCRL2log(log::debug) << "formula after translating regular formulas: " << x << std::endl;
  }
  spec.data().add_context_sorts(state_formulas::find_sort_expressions(x));
  if (translate_user_notation)
  {
    x = state_formulas::translate_user_notation(x);
  }
  if (normalize_sorts)
  {
    x = state_formulas::normalize_sorts(x, spec.data());
  }
  if (check_monotonicity && state_formulas::has_name_clashes(x))
  {
    mCRL2log(log::debug) << "formula before resolving name clashes: " << x << std::endl;
    x = state_formulas::resolve_name_clashes(x);
    mCRL2log(log::debug) << "formula after resolving name clashes: " << x << std::endl;
  }
  return x;
}

/// \brief Parses a state formula from text
// spec may be updated as the data implementation of the state formula
// may cause internal names to change.
/// \param formula_text A string
/// \param spec A linear process specification
/// \return The converted modal formula
inline
state_formula parse_state_formula(const std::string& formula_text, lps::specification& spec, bool check_monotonicity = true, bool translate_regular = true,
                                  bool type_check = true, bool translate_user_notation = true, bool normalize_sorts = true)
{
  std::stringstream formula_stream(formula_text);
  return parse_state_formula(formula_stream, spec, check_monotonicity, translate_regular, type_check, translate_user_notation, normalize_sorts);
}

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_PARSE_H
