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
#include "mcrl2/data/substitutions/mutable_map_substitution.h"
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
std::string pp(const data::untyped_data_parameter& x) { return data::pp< data::untyped_data_parameter >(x); }
std::string pp(const data::untyped_identifier& x) { return data::pp< data::untyped_identifier >(x); }
std::string pp(const data::untyped_identifier_assignment& x) { return data::pp< data::untyped_identifier_assignment >(x); }
std::string pp(const data::untyped_possible_sorts& x) { return data::pp< data::untyped_possible_sorts >(x); }
std::string pp(const data::untyped_set_or_bag_comprehension& x) { return data::pp< data::untyped_set_or_bag_comprehension >(x); }
std::string pp(const data::untyped_set_or_bag_comprehension_binder& x) { return data::pp< data::untyped_set_or_bag_comprehension_binder >(x); }
std::string pp(const data::untyped_sort& x) { return data::pp< data::untyped_sort >(x); }
std::string pp(const data::untyped_sort_variable& x) { return data::pp< data::untyped_sort_variable >(x); }
std::string pp(const data::variable& x) { return data::pp< data::variable >(x); }
std::string pp(const data::where_clause& x) { return data::pp< data::where_clause >(x); }
data::data_equation normalize_sorts(const data::data_equation& x, const data::sort_specification& sortspec) { return data::normalize_sorts< data::data_equation >(x, sortspec); }
data::data_equation_list normalize_sorts(const data::data_equation_list& x, const data::sort_specification& sortspec) { return data::normalize_sorts< data::data_equation_list >(x, sortspec); }
void normalize_sorts(data::data_equation_vector& x, const data::sort_specification& sortspec) { data::normalize_sorts< data::data_equation_vector >(x, sortspec); }
data::data_expression normalize_sorts(const data::data_expression& x, const data::sort_specification& sortspec) { return data::normalize_sorts< data::data_expression >(x, sortspec); }
data::sort_expression normalize_sorts(const data::sort_expression& x, const data::sort_specification& sortspec) { return data::normalize_sorts< data::sort_expression >(x, sortspec); }
data::variable_list normalize_sorts(const data::variable_list& x, const data::sort_specification& sortspec) { return data::normalize_sorts< data::variable_list >(x, sortspec); }
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

static bool register_hooks()
{
  register_function_symbol_hooks();
  register_variable_hooks();
  return true;
}

static bool initialised=register_hooks();


sort_expression data_expression::sort() const
{
  using namespace atermpp;
  // This implementation is currently done in this class, because there
  // is no elegant solution of distributing the implementation of the
  // derived classes (as we need to support requesting the sort of a
  // data_expression we do need to provide an implementation here).
  if (is_variable(*this))
  {
    const variable& v = atermpp::down_cast<variable>(*this);
    return v.sort();
  }
  else if (is_function_symbol(*this))
  {
    const function_symbol& f = atermpp::down_cast<function_symbol>(*this);
    return f.sort();
  }
  else if (is_abstraction(*this))
  {
    if (is_forall(*this) || is_exists(*this))
    {
      return sort_bool::bool_();
    }
    else if (is_lambda(*this))
    {
      const atermpp::term_list<aterm_appl> &v_variables = atermpp::down_cast<atermpp::term_list<aterm_appl> >((*this)[1]);
      sort_expression_vector s;
      for (const auto & v_variable : v_variables)
      {
        s.push_back(down_cast<sort_expression>(v_variable[1])); // Push the sort.
      }
      return function_sort(sort_expression_list(s.begin(),s.end()), atermpp::down_cast<data_expression>((*this)[2]).sort());
    }
    else
    {
      assert(is_set_comprehension(*this) || is_bag_comprehension(*this) || is_untyped_set_or_bag_comprehension(*this));
      const atermpp::term_list<aterm_appl> &v_variables  = atermpp::down_cast<atermpp::term_list<aterm_appl> >((*this)[1]);
      assert(v_variables.size() == 1);

      if (is_bag_comprehension(*this))
      {
        return container_sort(bag_container(), atermpp::down_cast<const sort_expression>(v_variables.front()[1]));
      }
      else // If it is not known whether the term is a set or a bag, it returns the type of a set, as there is
           // no setbag type. This can only occur for terms that are not propertly type checked.
      {
        return container_sort(set_container(), atermpp::down_cast<sort_expression>(v_variables.front()[1]));
      }
    }
  }
  else if (is_application(*this))
  {
    const data_expression& head = atermpp::down_cast<const data_expression>((*this)[0]);
    sort_expression s(head.sort());
    if (is_function_sort(s))
    {
      const function_sort& fs = atermpp::down_cast<function_sort>(s);
      assert(fs.domain().size()+1==this->size());
      return (fs.codomain());
    }
    return s;
  }
  else if (is_where_clause(*this))
  {
    return atermpp::down_cast<data_expression>((*this)[0]).sort();
  }
  assert(is_untyped_identifier(*this)); // All cases have been deal with here, except this one.
  return untyped_sort();
}

std::set<data::variable> substitution_variables(const mutable_map_substitution<>& sigma)
{
  std::set<data::variable> result;
  for (const auto & i : sigma)
  {
    data::find_free_variables(i.second, std::inserter(result, result.end()));
  }
  return result;
}

variable_list free_variables(const data_expression& x)
{
  std::set<variable> v = find_free_variables(x);
  return variable_list(v.begin(), v.end());
}

} // namespace data

} // namespace mcrl2

