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
#include "mcrl2/lps/parse.h"
#include "mcrl2/modal_formula/find.h"
#include "mcrl2/modal_formula/typecheck.h"
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/modal_formula/translate_user_notation.h"
#include "mcrl2/modal_formula/normalize_sorts.h"
#include "mcrl2/modal_formula/detail/regfrmtrans.h"

namespace mcrl2
{

namespace action_formulas
{

struct action_formula_actions: public lps::action_actions
{
  action_formulas::action_formula parse_ActFrm(const core::parse_node& node)
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "MultAct")) { return parse_MultAct(node.child(0)); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "DataValExpr")) { return parse_DataValExpr(node.child(0)); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "true")) { return action_formulas::true_(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "false")) { return action_formulas::false_(); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "!") && (symbol_name(node.child(1)) == "ActFrm")) { return action_formulas::not_(parse_ActFrm(node.child(1))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ActFrm") && (symbol_name(node.child(1)) == "=>") && (symbol_name(node.child(2)) == "ActFrm")) { return action_formulas::imp(parse_ActFrm(node.child(0)), parse_ActFrm(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ActFrm") && (symbol_name(node.child(1)) == "&&") && (symbol_name(node.child(2)) == "ActFrm")) { return action_formulas::and_(parse_ActFrm(node.child(0)), parse_ActFrm(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ActFrm") && (symbol_name(node.child(1)) == "||") && (symbol_name(node.child(2)) == "ActFrm")) { return action_formulas::or_(parse_ActFrm(node.child(0)), parse_ActFrm(node.child(2))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "forall") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "ActFrm")) { return action_formulas::forall(parse_VarsDeclList(node.child(1)), parse_ActFrm(node.child(3))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "exists") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "ActFrm")) { return action_formulas::exists(parse_VarsDeclList(node.child(1)), parse_ActFrm(node.child(3))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ActFrm") && (symbol_name(node.child(1)) == "@") && (symbol_name(node.child(2)) == "DataExpr")) { return action_formulas::at(parse_ActFrm(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "(") && (symbol_name(node.child(1)) == "ActFrm") && (symbol_name(node.child(2)) == ")")) { return parse_ActFrm(node.child(1)); }
    report_unexpected_node(node);
    return action_formulas::action_formula();
  }
};

inline
action_formula parse_action_formula_new(const std::string& text)
{
  core::parser p(parser_tables_mcrl2);
  unsigned int start_symbol_index = p.start_symbol_index("ActFrm");
  bool partial_parses = false;
  core::parse_node node = p.parse(text, start_symbol_index, partial_parses);
  return action_formula_actions().parse_ActFrm(node);
}

} // namespace action_formulas

namespace regular_formulas
{

struct regular_formula_actions: public action_formulas::action_formula_actions
{
  regular_formulas::regular_formula parse_RegFrm(const core::parse_node& node)
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "ActFrm")) { return parse_ActFrm(node.child(0)); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "nil")) { return regular_formulas::nil(); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "(") && (symbol_name(node.child(1)) == "RegFrm") && (symbol_name(node.child(2)) == ")")) { return parse_RegFrm(node.child(1)); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "*") && (symbol_name(node.child(1)) == "RegFrm")) { return trans_or_nil(parse_RegFrm(node.child(1))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "+") && (symbol_name(node.child(1)) == "RegFrm")) { return trans(parse_RegFrm(node.child(1))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "RegFrm") && (symbol_name(node.child(1)) == ".") && (symbol_name(node.child(2)) == "RegFrm")) { return seq(parse_RegFrm(node.child(0)), parse_RegFrm(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "RegFrm") && (symbol_name(node.child(1)) == "+") && (symbol_name(node.child(2)) == "RegFrm")) { return alt(parse_RegFrm(node.child(0)), parse_RegFrm(node.child(2))); }
    report_unexpected_node(node);
    return regular_formulas::regular_formula();
  }
};

inline
regular_formula parse_regular_formula_new(const std::string& text)
{
  core::parser p(parser_tables_mcrl2);
  unsigned int start_symbol_index = p.start_symbol_index("RegFrm");
  bool partial_parses = false;
  core::parse_node node = p.parse(text, start_symbol_index, partial_parses);
  return regular_formula_actions().parse_RegFrm(node);
}

} // namespace regular_formulas

namespace state_formulas
{

struct state_formula_actions: public regular_formulas::regular_formula_actions
{
  data::data_expression parse_Time(const core::parse_node& node)
  {
    if (node.child(1))
    {
      return parse_DataExpr(node.child(1));
    }
    else
    {
      return data::data_expression(core::detail::gsMakeNil());
    }
  }

  state_formulas::state_formula parse_StateFrm(const core::parse_node& node)
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "DataValExpr")) { return parse_DataValExpr(node.child(0)); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "true")) { return state_formulas::true_(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "false")) { return state_formulas::false_(); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "!") && (symbol_name(node.child(1)) == "StateFrm")) { return state_formulas::not_(parse_StateFrm(node.child(1))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "StateFrm") && (symbol_name(node.child(1)) == "=>") && (symbol_name(node.child(2)) == "StateFrm")) { return state_formulas::imp(parse_StateFrm(node.child(0)), parse_StateFrm(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "StateFrm") && (symbol_name(node.child(1)) == "&&") && (symbol_name(node.child(2)) == "StateFrm")) { return state_formulas::and_(parse_StateFrm(node.child(0)), parse_StateFrm(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "StateFrm") && (symbol_name(node.child(1)) == "||") && (symbol_name(node.child(2)) == "StateFrm")) { return state_formulas::or_(parse_StateFrm(node.child(0)), parse_StateFrm(node.child(2))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "forall") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "StateFrm")) { return state_formulas::forall(parse_VarsDeclList(node.child(1)), parse_StateFrm(node.child(3))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "exists") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "StateFrm")) { return state_formulas::exists(parse_VarsDeclList(node.child(1)), parse_StateFrm(node.child(3))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "[") && (symbol_name(node.child(1)) == "RegFrm") && (symbol_name(node.child(2)) == "]")) { return state_formulas::may(parse_RegFrm(node.child(1))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "<") && (symbol_name(node.child(1)) == "RegFrm") && (symbol_name(node.child(2)) == ">")) { return state_formulas::must(parse_RegFrm(node.child(1))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "mu") && (symbol_name(node.child(1)) == "StateVarDecl") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "StateFrm")) { return state_formulas::mu(parse_Id(node.child(1).child(0)), parse_DataExprList(node.child(1).child(1)), parse_StateFrm(node.child(3))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "nu") && (symbol_name(node.child(1)) == "StateVarDecl") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "StateFrm")) { return state_formulas::nu(parse_Id(node.child(1).child(0)), parse_DataExprList(node.child(1).child(1)), parse_StateFrm(node.child(3))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "Id")) { return state_formulas::variable(parse_Id(node.child(0)), parse_DataExprList(node.child(1))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "delay")) { return state_formulas::delay(parse_Time(node.child(1))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "yaled")) { return state_formulas::yaled(parse_Time(node.child(1))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "(") && (symbol_name(node.child(1)) == "StateFrm") && (symbol_name(node.child(2)) == ")")) { return parse_StateFrm(node.child(1)); }
    report_unexpected_node(node);
    return state_formulas::state_formula();
  }
};

inline
state_formula parse_state_formula_new(const std::string& text)
{
  core::parser p(parser_tables_mcrl2);
  unsigned int start_symbol_index = p.start_symbol_index("StateFrm");
  bool partial_parses = false;
  core::parse_node node = p.parse(text, start_symbol_index, partial_parses);
  return state_formula_actions().parse_StateFrm(node);
}

/// \brief Translates regular formulas appearing in f into action formulas.
/// \param f A state formula
inline
void translate_regular_formula(state_formula& f)
{
  ATermAppl result = regular_formulas::detail::translate_reg_frms(f);
  if (result == NULL)
  {
    throw mcrl2::runtime_error("formula translation error");
  }
  f = state_formula(result);
}

/// \brief Parses a state formula from an input stream
// spec may be updated as the data implementation of the state formula
// may cause internal names to change.
/// \param formula_stream A stream from which can be read
/// \param spec A linear process specification
/// \param check_monotonicity If true, an exception will be thrown if the formula is not monotonous
/// \return The converted modal formula
inline
state_formula parse_state_formula(std::istream& from, lps::specification& spec, bool check_monotonicity = true)
{
  ATermAppl result = core::parse_state_frm(from);
  if (result == NULL)
  {
    throw mcrl2::runtime_error("parse error in parse_state_frm()");
  }
  state_formula f = atermpp::aterm_appl(result);
  type_check(f, spec, check_monotonicity);
  translate_regular_formula(f);

  spec.data().add_context_sorts(state_formulas::find_sort_expressions((f)));
  f = state_formulas::translate_user_notation(f);
  f = state_formulas::normalize_sorts(f, spec.data());

  return f;
}

/// \brief Parses a state formula from text
// spec may be updated as the data implementation of the state formula
// may cause internal names to change.
/// \param formula_text A string
/// \param spec A linear process specification
/// \return The converted modal formula
inline
state_formula parse_state_formula(const std::string& formula_text, lps::specification& spec, bool check_monotonicity = true)
{
  std::stringstream formula_stream(formula_text);
  return parse_state_formula(formula_stream, spec, check_monotonicity);
}

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_PARSE_H
