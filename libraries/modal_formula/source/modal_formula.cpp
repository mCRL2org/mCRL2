// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file modal_formula.cpp
/// \brief

#include "mcrl2/modal_formula/algorithms.h"
#include "mcrl2/modal_formula/find.h"
#include "mcrl2/modal_formula/monotonicity.h"
#include "mcrl2/modal_formula/normalize_sorts.h"
#include "mcrl2/modal_formula/parse.h"
#include "mcrl2/modal_formula/preprocess_state_formula.h"
#include "mcrl2/modal_formula/print.h"
#include "mcrl2/modal_formula/replace.h"
#include "mcrl2/modal_formula/state_formula_normalize.h"
#include "mcrl2/modal_formula/translate_user_notation.h"

namespace mcrl2
{

namespace action_formulas
{

//--- start generated action_formulas overloads ---//
std::string pp(const action_formulas::action_formula& x) { return action_formulas::pp< action_formulas::action_formula >(x); }
std::set<data::variable> find_all_variables(const action_formulas::action_formula& x) { return action_formulas::find_all_variables< action_formulas::action_formula >(x); }
//--- end generated action_formulas overloads ---//

} // namespace action_formulas

namespace regular_formulas
{

//--- start generated regular_formulas overloads ---//
std::string pp(const regular_formulas::regular_formula& x) { return regular_formulas::pp< regular_formulas::regular_formula >(x); }
//--- end generated regular_formulas overloads ---//

} // namespace regular_formulas

namespace state_formulas
{

//--- start generated state_formulas overloads ---//
std::string pp(const state_formulas::state_formula& x) { return state_formulas::pp< state_formulas::state_formula >(x); }
state_formulas::state_formula normalize_sorts(const state_formulas::state_formula& x, const data::data_specification& dataspec) { return state_formulas::normalize_sorts< state_formulas::state_formula >(x, dataspec); }
state_formulas::state_formula translate_user_notation(const state_formulas::state_formula& x) { return state_formulas::translate_user_notation< state_formulas::state_formula >(x); }
std::set<data::sort_expression> find_sort_expressions(const state_formulas::state_formula& x) { return state_formulas::find_sort_expressions< state_formulas::state_formula >(x); }
std::set<data::variable> find_all_variables(const state_formulas::state_formula& x) { return state_formulas::find_all_variables< state_formulas::state_formula >(x); }
std::set<data::variable> find_free_variables(const state_formulas::state_formula& x) { return state_formulas::find_free_variables< state_formulas::state_formula >(x); }
std::set<core::identifier_string> find_identifiers(const state_formulas::state_formula& x) { return state_formulas::find_identifiers< state_formulas::state_formula >(x); }
bool find_nil(const state_formulas::state_formula& x) { return state_formulas::find_nil< state_formulas::state_formula >(x); }
//--- end generated state_formulas overloads ---//

namespace algorithms {

state_formula parse_state_formula(std::istream& in, lps::specification& spec)
{
  return state_formulas::parse_state_formula(in, spec, true);
}

state_formula parse_state_formula(const std::string& formula_text, lps::specification& spec)
{
  return state_formulas::parse_state_formula(formula_text, spec);
}

state_formulas::state_formula preprocess_state_formula(const state_formulas::state_formula& formula, const lps::specification& spec)
{
  return state_formulas::preprocess_state_formula(formula, spec);
}

bool is_monotonous(state_formula f)
{
  return state_formulas::is_monotonous(f);
}

state_formula normalize(const state_formula& x)
{
  return state_formulas::normalize(x);
}

bool is_normalized(const state_formula& x)
{
  return state_formulas::is_normalized(x);
}

std::set<core::identifier_string> find_state_variable_names(const state_formula& x)
{
  return state_formulas::find_state_variable_names(x);
}

} // algorithms

} // namespace state_formulas

} // namespace mcrl2
