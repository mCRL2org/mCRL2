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
#include "mcrl2/lps/translate_user_notation.h"
#include "mcrl2/lps/detail/lps_well_typed_checker.h"

namespace mcrl2
{

namespace lps
{

//--- start generated lps overloads ---//
// std::string pp(const lps::state& x) { return lps::pp< lps::state >(x); }
std::string pp(const lps::action_summand& x) { return lps::pp< lps::action_summand >(x); }
std::string pp(const lps::deadlock& x) { return lps::pp< lps::deadlock >(x); }
std::string pp(const lps::deadlock_summand& x) { return lps::pp< lps::deadlock_summand >(x); }
std::string pp(const lps::linear_process& x) { return lps::pp< lps::linear_process >(x); }
std::string pp(const lps::multi_action& x) { return lps::pp< lps::multi_action >(x); }
std::string pp(const lps::process_initializer& x) { return lps::pp< lps::process_initializer >(x); }
std::string pp(const lps::specification& x) { return lps::pp< lps::specification >(x); }
std::string pp(const lps::untyped_multi_action& x) { return lps::pp< lps::untyped_multi_action >(x); }
void normalize_sorts(lps::multi_action& x, const data::data_specification& dataspec) { lps::normalize_sorts< lps::multi_action >(x, dataspec); }
void translate_user_notation(lps::multi_action& x) { lps::translate_user_notation< lps::multi_action >(x); }
std::set<data::sort_expression> find_sort_expressions(const lps::specification& x) { return lps::find_sort_expressions< lps::specification >(x); }
std::set<data::variable> find_all_variables(const lps::linear_process& x) { return lps::find_all_variables< lps::linear_process >(x); }
std::set<data::variable> find_all_variables(const lps::specification& x) { return lps::find_all_variables< lps::specification >(x); }
std::set<data::variable> find_all_variables(const lps::deadlock& x) { return lps::find_all_variables< lps::deadlock >(x); }
std::set<data::variable> find_all_variables(const lps::multi_action& x) { return lps::find_all_variables< lps::multi_action >(x); }
std::set<data::variable> find_free_variables(const lps::linear_process& x) { return lps::find_free_variables< lps::linear_process >(x); }
std::set<data::variable> find_free_variables(const lps::specification& x) { return lps::find_free_variables< lps::specification >(x); }
std::set<data::variable> find_free_variables(const lps::deadlock& x) { return lps::find_free_variables< lps::deadlock >(x); }
std::set<data::variable> find_free_variables(const lps::multi_action& x) { return lps::find_free_variables< lps::multi_action >(x); }
std::set<data::variable> find_free_variables(const lps::process_initializer& x) { return lps::find_free_variables< lps::process_initializer >(x); }
std::set<data::function_symbol> find_function_symbols(const lps::specification& x) { return lps::find_function_symbols< lps::specification >(x); }
std::set<core::identifier_string> find_identifiers(const lps::specification& x) { return lps::find_identifiers< lps::specification >(x); }
//--- end generated lps overloads ---//

data::data_expression_list action_summand::next_state(const data::variable_list& process_parameters) const
{
  return data::replace_variables(atermpp::aterm_cast<data::data_expression_list>(process_parameters), 
                                 data::assignment_sequence_substitution(assignments()));
}

std::string pp_with_summand_numbers(const specification& x)
{
  std::ostringstream out;
  core::detail::apply_printer<lps::detail::printer> printer(out);
  printer.print_summand_numbers() = true;
  printer(x);
  return out.str();
}

bool is_well_typed(const linear_process& x)
{
  return lps::detail::is_well_typed(x);
}

bool is_well_typed(const specification& x)
{
  return lps::detail::is_well_typed(x);
}

} // namespace lps

} // namespace mcrl2
