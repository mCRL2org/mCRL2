// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file data.cpp
/// \brief

#include "mcrl2/data/find.h"
#include "mcrl2/data/index_traits.h"
#include "mcrl2/data/normalize_sorts.h"
#include "mcrl2/data/print.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/translate_user_notation.h"

namespace mcrl2
{

namespace data
{

//--- start generated data overloads ---//
std::string pp(const data::sort_expression_list& x) { return data::pp< data::sort_expression_list >(x); }
std::string pp(const data::sort_expression_vector& x) { return data::pp< data::sort_expression_vector >(x); }
std::string pp(const data::data_expression_list& x) { return data::pp< data::data_expression_list >(x); }
std::string pp(const data::data_expression_vector& x) { return data::pp< data::data_expression_vector >(x); }
std::string pp(const data::assignment_list& x) { return data::pp< data::assignment_list >(x); }
std::string pp(const data::assignment_vector& x) { return data::pp< data::assignment_vector >(x); }
std::string pp(const data::variable_list& x) { return data::pp< data::variable_list >(x); }
std::string pp(const data::variable_vector& x) { return data::pp< data::variable_vector >(x); }
std::string pp(const data::function_symbol_list& x) { return data::pp< data::function_symbol_list >(x); }
std::string pp(const data::function_symbol_vector& x) { return data::pp< data::function_symbol_vector >(x); }
std::string pp(const data::structured_sort_constructor_list& x) { return data::pp< data::structured_sort_constructor_list >(x); }
std::string pp(const data::structured_sort_constructor_vector& x) { return data::pp< data::structured_sort_constructor_vector >(x); }
std::string pp(const data::data_equation_list& x) { return data::pp< data::data_equation_list >(x); }
std::string pp(const data::data_equation_vector& x) { return data::pp< data::data_equation_vector >(x); }
std::string pp(const data::abstraction& x) { return data::pp< data::abstraction >(x); }
std::string pp(const data::alias& x) { return data::pp< data::alias >(x); }
std::string pp(const data::application& x) { return data::pp< data::application >(x); }
std::string pp(const data::assignment& x) { return data::pp< data::assignment >(x); }
std::string pp(const data::assignment_expression& x) { return data::pp< data::assignment_expression >(x); }
std::string pp(const data::bag_comprehension& x) { return data::pp< data::bag_comprehension >(x); }
std::string pp(const data::bag_comprehension_binder& x) { return data::pp< data::bag_comprehension_binder >(x); }
std::string pp(const data::bag_container& x) { return data::pp< data::bag_container >(x); }
std::string pp(const data::basic_sort& x) { return data::pp< data::basic_sort >(x); }
std::string pp(const data::binder_type& x) { return data::pp< data::binder_type >(x); }
std::string pp(const data::container_sort& x) { return data::pp< data::container_sort >(x); }
std::string pp(const data::container_type& x) { return data::pp< data::container_type >(x); }
std::string pp(const data::data_equation& x) { return data::pp< data::data_equation >(x); }
std::string pp(const data::data_expression& x) { return data::pp< data::data_expression >(x); }
std::string pp(const data::data_specification& x) { return data::pp< data::data_specification >(x); }
std::string pp(const data::exists& x) { return data::pp< data::exists >(x); }
std::string pp(const data::exists_binder& x) { return data::pp< data::exists_binder >(x); }
std::string pp(const data::fbag_container& x) { return data::pp< data::fbag_container >(x); }
std::string pp(const data::forall& x) { return data::pp< data::forall >(x); }
std::string pp(const data::forall_binder& x) { return data::pp< data::forall_binder >(x); }
std::string pp(const data::fset_container& x) { return data::pp< data::fset_container >(x); }
std::string pp(const data::function_sort& x) { return data::pp< data::function_sort >(x); }
std::string pp(const data::function_symbol& x) { return data::pp< data::function_symbol >(x); }
std::string pp(const data::lambda& x) { return data::pp< data::lambda >(x); }
std::string pp(const data::lambda_binder& x) { return data::pp< data::lambda_binder >(x); }
std::string pp(const data::list_container& x) { return data::pp< data::list_container >(x); }
std::string pp(const data::set_comprehension& x) { return data::pp< data::set_comprehension >(x); }
std::string pp(const data::set_comprehension_binder& x) { return data::pp< data::set_comprehension_binder >(x); }
std::string pp(const data::set_container& x) { return data::pp< data::set_container >(x); }
std::string pp(const data::sort_expression& x) { return data::pp< data::sort_expression >(x); }
std::string pp(const data::structured_sort& x) { return data::pp< data::structured_sort >(x); }
std::string pp(const data::structured_sort_constructor& x) { return data::pp< data::structured_sort_constructor >(x); }
std::string pp(const data::structured_sort_constructor_argument& x) { return data::pp< data::structured_sort_constructor_argument >(x); }
std::string pp(const data::untyped_identifier& x) { return data::pp< data::untyped_identifier >(x); }
std::string pp(const data::untyped_identifier_assignment& x) { return data::pp< data::untyped_identifier_assignment >(x); }
std::string pp(const data::untyped_possible_sorts& x) { return data::pp< data::untyped_possible_sorts >(x); }
std::string pp(const data::untyped_set_or_bag_comprehension& x) { return data::pp< data::untyped_set_or_bag_comprehension >(x); }
std::string pp(const data::untyped_set_or_bag_comprehension_binder& x) { return data::pp< data::untyped_set_or_bag_comprehension_binder >(x); }
std::string pp(const data::untyped_sort& x) { return data::pp< data::untyped_sort >(x); }
std::string pp(const data::variable& x) { return data::pp< data::variable >(x); }
std::string pp(const data::where_clause& x) { return data::pp< data::where_clause >(x); }
data::data_equation normalize_sorts(const data::data_equation& x, const data::data_specification& dataspec) { return data::normalize_sorts< data::data_equation >(x, dataspec); }
data::data_equation_list normalize_sorts(const data::data_equation_list& x, const data::data_specification& dataspec) { return data::normalize_sorts< data::data_equation_list >(x, dataspec); }
void normalize_sorts(data::data_equation_vector& x, const data::data_specification& dataspec) { data::normalize_sorts< data::data_equation_vector >(x, dataspec); }
data::data_expression normalize_sorts(const data::data_expression& x, const data::data_specification& dataspec) { return data::normalize_sorts< data::data_expression >(x, dataspec); }
data::sort_expression normalize_sorts(const data::sort_expression& x, const data::data_specification& dataspec) { return data::normalize_sorts< data::sort_expression >(x, dataspec); }
data::variable_list normalize_sorts(const data::variable_list& x, const data::data_specification& dataspec) { return data::normalize_sorts< data::variable_list >(x, dataspec); }
data::data_expression translate_user_notation(const data::data_expression& x) { return data::translate_user_notation< data::data_expression >(x); }
data::data_equation translate_user_notation(const data::data_equation& x) { return data::translate_user_notation< data::data_equation >(x); }
std::set<data::sort_expression> find_sort_expressions(const data::data_equation& x) { return data::find_sort_expressions< data::data_equation >(x); }
std::set<data::sort_expression> find_sort_expressions(const data::data_expression& x) { return data::find_sort_expressions< data::data_expression >(x); }
std::set<data::sort_expression> find_sort_expressions(const data::sort_expression& x) { return data::find_sort_expressions< data::sort_expression >(x); }
std::set<data::variable> find_all_variables(const data::data_expression& x) { return data::find_all_variables< data::data_expression >(x); }
std::set<data::variable> find_all_variables(const data::data_expression_list& x) { return data::find_all_variables< data::data_expression_list >(x); }
std::set<data::variable> find_all_variables(const data::function_symbol& x) { return data::find_all_variables< data::function_symbol >(x); }
std::set<data::variable> find_all_variables(const data::variable& x) { return data::find_all_variables< data::variable >(x); }
std::set<data::variable> find_all_variables(const data::variable_list& x) { return data::find_all_variables< data::variable_list >(x); }
std::set<data::variable> find_free_variables(const data::data_expression& x) { return data::find_free_variables< data::data_expression >(x); }
std::set<data::variable> find_free_variables(const data::data_expression_list& x) { return data::find_free_variables< data::data_expression_list >(x); }
std::set<data::function_symbol> find_function_symbols(const data::data_equation& x) { return data::find_function_symbols< data::data_equation >(x); }
std::set<core::identifier_string> find_identifiers(const data::variable_list& x) { return data::find_identifiers< data::variable_list >(x); }
bool search_variable(const data::data_expression& x, const data::variable& v) { return data::search_variable< data::data_expression >(x, v); }
//--- end generated data overloads ---//

std::string pp(const std::set<variable>& x) { return data::pp< std::set<variable> >(x); }

// TODO: These should be removed when the aterm code has been replaced.
std::string pp(const atermpp::aterm& x) { return to_string(x); }
std::string pp(const atermpp::aterm_appl& x) { return to_string(x); }

static bool register_hooks()
{
  register_function_symbol_hooks();
  register_variable_hooks();
  return true;
}
static bool mcrl2_register_data(register_hooks());

} // namespace data

} // namespace mcrl2

