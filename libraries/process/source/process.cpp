// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file process.cpp
/// \brief

#include "mcrl2/process/find.h"
#include "mcrl2/process/index_traits.h"
#include "mcrl2/process/normalize_sorts.h"
#include "mcrl2/process/print.h"
#include "mcrl2/process/replace.h"
#include "mcrl2/process/translate_user_notation.h"

namespace mcrl2
{

namespace process
{

//--- start generated process overloads ---//
std::string pp(const process::action_list& x) { return process::pp< process::action_list >(x); }
std::string pp(const process::action_vector& x) { return process::pp< process::action_vector >(x); }
std::string pp(const process::action_label_list& x) { return process::pp< process::action_label_list >(x); }
std::string pp(const process::action_label_vector& x) { return process::pp< process::action_label_vector >(x); }
std::string pp(const process::process_identifier_list& x) { return process::pp< process::process_identifier_list >(x); }
std::string pp(const process::process_identifier_vector& x) { return process::pp< process::process_identifier_vector >(x); }
std::string pp(const process::process_expression_list& x) { return process::pp< process::process_expression_list >(x); }
std::string pp(const process::process_expression_vector& x) { return process::pp< process::process_expression_vector >(x); }
std::string pp(const process::process_equation_list& x) { return process::pp< process::process_equation_list >(x); }
std::string pp(const process::process_equation_vector& x) { return process::pp< process::process_equation_vector >(x); }
std::string pp(const process::action& x) { return process::pp< process::action >(x); }
std::string pp(const process::action_label& x) { return process::pp< process::action_label >(x); }
std::string pp(const process::action_name_multiset& x) { return process::pp< process::action_name_multiset >(x); }
std::string pp(const process::allow& x) { return process::pp< process::allow >(x); }
std::string pp(const process::at& x) { return process::pp< process::at >(x); }
std::string pp(const process::block& x) { return process::pp< process::block >(x); }
std::string pp(const process::bounded_init& x) { return process::pp< process::bounded_init >(x); }
std::string pp(const process::choice& x) { return process::pp< process::choice >(x); }
std::string pp(const process::comm& x) { return process::pp< process::comm >(x); }
std::string pp(const process::communication_expression& x) { return process::pp< process::communication_expression >(x); }
std::string pp(const process::delta& x) { return process::pp< process::delta >(x); }
std::string pp(const process::hide& x) { return process::pp< process::hide >(x); }
std::string pp(const process::if_then& x) { return process::pp< process::if_then >(x); }
std::string pp(const process::if_then_else& x) { return process::pp< process::if_then_else >(x); }
std::string pp(const process::left_merge& x) { return process::pp< process::left_merge >(x); }
std::string pp(const process::merge& x) { return process::pp< process::merge >(x); }
std::string pp(const process::process_equation& x) { return process::pp< process::process_equation >(x); }
std::string pp(const process::process_expression& x) { return process::pp< process::process_expression >(x); }
std::string pp(const process::process_identifier& x) { return process::pp< process::process_identifier >(x); }
std::string pp(const process::process_instance& x) { return process::pp< process::process_instance >(x); }
std::string pp(const process::process_instance_assignment& x) { return process::pp< process::process_instance_assignment >(x); }
std::string pp(const process::process_specification& x) { return process::pp< process::process_specification >(x); }
std::string pp(const process::rename& x) { return process::pp< process::rename >(x); }
std::string pp(const process::rename_expression& x) { return process::pp< process::rename_expression >(x); }
std::string pp(const process::seq& x) { return process::pp< process::seq >(x); }
std::string pp(const process::sum& x) { return process::pp< process::sum >(x); }
std::string pp(const process::sync& x) { return process::pp< process::sync >(x); }
std::string pp(const process::tau& x) { return process::pp< process::tau >(x); }
std::string pp(const process::untyped_action& x) { return process::pp< process::untyped_action >(x); }
std::string pp(const process::untyped_parameter_identifier& x) { return process::pp< process::untyped_parameter_identifier >(x); }
std::string pp(const process::untyped_process_assignment& x) { return process::pp< process::untyped_process_assignment >(x); }
process::action normalize_sorts(const process::action& x, const data::data_specification& dataspec) { return atermpp::aterm_cast<process::action>(process::normalize_sorts<process::process_expression>(atermpp::aterm_cast<process::process_expression>(x), dataspec)); }
process::action_label_list normalize_sorts(const process::action_label_list& x, const data::data_specification& dataspec) { return process::normalize_sorts< process::action_label_list >(x, dataspec); }
void normalize_sorts(process::process_equation_vector& x, const data::data_specification& dataspec) { process::normalize_sorts< process::process_equation_vector >(x, dataspec); }
void normalize_sorts(process::process_specification& x, const data::data_specification& /* dataspec */) { process::normalize_sorts< process::process_specification >(x, x.data()); }
process::action translate_user_notation(const process::action& x) { return atermpp::aterm_cast<process::action>(process::translate_user_notation<process::process_expression>(atermpp::aterm_cast<process::process_expression>(x))); }
void translate_user_notation(process::process_specification& x) { process::translate_user_notation< process::process_specification >(x); }
std::set<data::sort_expression> find_sort_expressions(const process::action_label_list& x) { return process::find_sort_expressions< process::action_label_list >(x); }
std::set<data::sort_expression> find_sort_expressions(const process::process_equation_vector& x) { return process::find_sort_expressions< process::process_equation_vector >(x); }
std::set<data::sort_expression> find_sort_expressions(const process::process_expression& x) { return process::find_sort_expressions< process::process_expression >(x); }
std::set<data::sort_expression> find_sort_expressions(const process::process_specification& x) { return process::find_sort_expressions< process::process_specification >(x); }
std::set<data::variable> find_all_variables(const process::action& x) { return process::find_all_variables< process::action >(x); }
std::set<data::variable> find_free_variables(const process::action& x) { return process::find_free_variables< process::action >(x); }
std::set<core::identifier_string> find_identifiers(const process::process_specification& x) { return process::find_identifiers< process::process_specification >(x); }
//--- end generated process overloads ---//

static bool register_hooks()
{
  register_process_identifier_hooks();
  return true;
}
static bool mcrl2_register_process(register_hooks());

} // namespace process

} // namespace mcrl2

