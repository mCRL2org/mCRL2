// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lps.cpp
/// \brief

#include "mcrl2/lps/find.h"
#include "mcrl2/lps/normalize_sorts.h"
#include "mcrl2/lps/print.h"
#include "mcrl2/lps/replace.h"
#include "mcrl2/lps/state.h"
#include "mcrl2/lps/translate_user_notation.h"

#include "mcrl2/modal_formula/find.h"
#include "mcrl2/modal_formula/normalize_sorts.h"
#include "mcrl2/modal_formula/print.h"
#include "mcrl2/modal_formula/replace.h"
#include "mcrl2/modal_formula/translate_user_notation.h"

namespace mcrl2
{

namespace lps
{

//--- start generated lps overloads ---//
std::string pp(const lps::specification& x) { return lps::pp< lps::specification >(x); }
std::string pp(const lps::linear_process& x) { return lps::pp< lps::linear_process >(x); }
std::string pp(const lps::action& x) { return lps::pp< lps::action >(x); }
std::string pp(const lps::action_list& x) { return lps::pp< lps::action_list >(x); }
std::string pp(const lps::action_vector& x) { return lps::pp< lps::action_vector >(x); }
std::string pp(const lps::action_label& x) { return lps::pp< lps::action_label >(x); }
std::string pp(const lps::action_label_list& x) { return lps::pp< lps::action_label_list >(x); }
std::string pp(const lps::action_label_vector& x) { return lps::pp< lps::action_label_vector >(x); }
std::string pp(const lps::multi_action& x) { return lps::pp< lps::multi_action >(x); }
std::string pp(const lps::process_initializer& x) { return lps::pp< lps::process_initializer >(x); }
std::string pp(const lps::state& x) { return lps::pp< lps::state >(x); }
lps::action normalize_sorts(const lps::action& x, const data::data_specification& dataspec) { return lps::normalize_sorts< lps::action >(x, dataspec); }
lps::action_label_list normalize_sorts(const lps::action_label_list& x, const data::data_specification& dataspec) { return lps::normalize_sorts< lps::action_label_list >(x, dataspec); }
void normalize_sorts(lps::multi_action& x, const data::data_specification& dataspec) { lps::normalize_sorts< lps::multi_action >(x, dataspec); }
lps::action translate_user_notation(const lps::action& x) { return lps::translate_user_notation< lps::action >(x); }
void translate_user_notation(lps::multi_action& x) { lps::translate_user_notation< lps::multi_action >(x); }
std::set<data::sort_expression> find_sort_expressions(const lps::action_label_list& x) { return lps::find_sort_expressions< lps::action_label_list >(x); }
std::set<data::sort_expression> find_sort_expressions(const lps::specification& x) { return lps::find_sort_expressions< lps::specification >(x); }
std::set<data::variable> find_variables(const lps::linear_process& x) { return lps::find_variables< lps::linear_process >(x); }
std::set<data::variable> find_variables(const lps::specification& x) { return lps::find_variables< lps::specification >(x); }
std::set<data::variable> find_variables(const lps::deadlock& x) { return lps::find_variables< lps::deadlock >(x); }
std::set<data::variable> find_variables(const lps::multi_action& x) { return lps::find_variables< lps::multi_action >(x); }
std::set<data::variable> find_free_variables(const lps::action& x) { return lps::find_free_variables< lps::action >(x); }
std::set<data::variable> find_free_variables(const lps::linear_process& x) { return lps::find_free_variables< lps::linear_process >(x); }
std::set<data::variable> find_free_variables(const lps::specification& x) { return lps::find_free_variables< lps::specification >(x); }
std::set<data::variable> find_free_variables(const lps::deadlock& x) { return lps::find_free_variables< lps::deadlock >(x); }
std::set<data::variable> find_free_variables(const lps::multi_action& x) { return lps::find_free_variables< lps::multi_action >(x); }
std::set<data::variable> find_free_variables(const lps::process_initializer& x) { return lps::find_free_variables< lps::process_initializer >(x); }
std::set<data::function_symbol> find_function_symbols(const lps::specification& x) { return lps::find_function_symbols< lps::specification >(x); }
std::set<core::identifier_string> find_identifiers(const lps::specification& x) { return lps::find_identifiers< lps::specification >(x); }
//--- end generated lps overloads ---//

std::string pp_with_summand_numbers(const specification& x)
{
  std::ostringstream out;
  core::detail::apply_printer<lps::detail::printer> printer(out);
  printer.print_summand_numbers() = true;
  printer(x);
  return out.str();
}

// TODO: These should be removed when the aterm code has been replaced.
std::string pp(const atermpp::aterm& x) { return to_string(x); }
std::string pp(const atermpp::aterm_appl& x) { return to_string(x); }

} // namespace lps

namespace action_formulas
{

//--- start generated action_formulas overloads ---//
std::string pp(const action_formulas::action_formula& x) { return action_formulas::pp< action_formulas::action_formula >(x); }
std::set<data::variable> find_variables(const action_formulas::action_formula& x) { return action_formulas::find_variables< action_formulas::action_formula >(x); }
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
std::set<data::variable> find_variables(const state_formulas::state_formula& x) { return state_formulas::find_variables< state_formulas::state_formula >(x); }
std::set<data::variable> find_free_variables(const state_formulas::state_formula& x) { return state_formulas::find_free_variables< state_formulas::state_formula >(x); }
std::set<core::identifier_string> find_identifiers(const state_formulas::state_formula& x) { return state_formulas::find_identifiers< state_formulas::state_formula >(x); }
bool find_nil(const state_formulas::state_formula& x) { return state_formulas::find_nil< state_formulas::state_formula >(x); }
//--- end generated state_formulas overloads ---//

} // namespace state_formulas

} // namespace mcrl2

