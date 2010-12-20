// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/traverser.inc.h
/// \brief The content of this file is included in other header
/// files, to prevent duplication.

//--- start generated code ---//
void operator()(const data::assignment& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.lhs());
  static_cast<Derived&>(*this)(x.rhs());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::identifier_assignment& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.lhs());
  static_cast<Derived&>(*this)(x.rhs());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::set_or_bag_comprehension_binder& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::set_comprehension_binder& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::bag_comprehension_binder& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::forall_binder& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::exists_binder& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::lambda_binder& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::structured_sort_constructor_argument& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.name());
  static_cast<Derived&>(*this)(x.sort());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::structured_sort_constructor& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.name());
  static_cast<Derived&>(*this)(x.arguments());
  static_cast<Derived&>(*this)(x.recogniser());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::list_container& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::set_container& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::bag_container& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::fset_container& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::fbag_container& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::basic_sort& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.name());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::container_sort& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.container_name());
  static_cast<Derived&>(*this)(x.element_sort());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::structured_sort& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.constructors());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::function_sort& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.domain());
  static_cast<Derived&>(*this)(x.codomain());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::unknown_sort& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::multiple_possible_sorts& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.sorts());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::identifier& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.name());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::variable& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.name());
  static_cast<Derived&>(*this)(x.sort());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::function_symbol& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.name());
  static_cast<Derived&>(*this)(x.sort());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::application& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.head());
  static_cast<Derived&>(*this)(x.arguments());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::where_clause& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.body());
  static_cast<Derived&>(*this)(x.declarations());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::forall& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.variables());
  static_cast<Derived&>(*this)(x.body());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::exists& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.variables());
  static_cast<Derived&>(*this)(x.body());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::lambda& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.variables());
  static_cast<Derived&>(*this)(x.body());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::data_equation& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.variables());
  static_cast<Derived&>(*this)(x.condition());
  static_cast<Derived&>(*this)(x.lhs());
  static_cast<Derived&>(*this)(x.rhs());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::data_specification& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::data_expression& x)
{
  static_cast<Derived&>(*this).enter(x);
  if (data::is_identifier(x)) { static_cast<Derived&>(*this)(data::identifier(atermpp::aterm_appl(x))); }
  else if (data::is_variable(x)) { static_cast<Derived&>(*this)(data::variable(atermpp::aterm_appl(x))); }
  else if (data::is_function_symbol(x)) { static_cast<Derived&>(*this)(data::function_symbol(atermpp::aterm_appl(x))); }
  else if (data::is_application(x)) { static_cast<Derived&>(*this)(data::application(atermpp::aterm_appl(x))); }
  else if (data::is_where_clause(x)) { static_cast<Derived&>(*this)(data::where_clause(atermpp::aterm_appl(x))); }
  else if (data::is_abstraction(x)) { static_cast<Derived&>(*this)(data::abstraction(atermpp::aterm_appl(x))); }
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::assignment_expression& x)
{
  static_cast<Derived&>(*this).enter(x);
  if (data::is_assignment(x)) { static_cast<Derived&>(*this)(data::assignment(atermpp::aterm_appl(x))); }
  else if (data::is_identifier_assignment(x)) { static_cast<Derived&>(*this)(data::identifier_assignment(atermpp::aterm_appl(x))); }
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::sort_expression& x)
{
  static_cast<Derived&>(*this).enter(x);
  if (data::is_basic_sort(x)) { static_cast<Derived&>(*this)(data::basic_sort(atermpp::aterm_appl(x))); }
  else if (data::is_container_sort(x)) { static_cast<Derived&>(*this)(data::container_sort(atermpp::aterm_appl(x))); }
  else if (data::is_structured_sort(x)) { static_cast<Derived&>(*this)(data::structured_sort(atermpp::aterm_appl(x))); }
  else if (data::is_function_sort(x)) { static_cast<Derived&>(*this)(data::function_sort(atermpp::aterm_appl(x))); }
  else if (data::is_unknown_sort(x)) { static_cast<Derived&>(*this)(data::unknown_sort(atermpp::aterm_appl(x))); }
  else if (data::is_multiple_possible_sorts(x)) { static_cast<Derived&>(*this)(data::multiple_possible_sorts(atermpp::aterm_appl(x))); }
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::container_type& x)
{
  static_cast<Derived&>(*this).enter(x);
  if (data::is_list_container(x)) { static_cast<Derived&>(*this)(data::list_container(atermpp::aterm_appl(x))); }
  else if (data::is_set_container(x)) { static_cast<Derived&>(*this)(data::set_container(atermpp::aterm_appl(x))); }
  else if (data::is_bag_container(x)) { static_cast<Derived&>(*this)(data::bag_container(atermpp::aterm_appl(x))); }
  else if (data::is_fset_container(x)) { static_cast<Derived&>(*this)(data::fset_container(atermpp::aterm_appl(x))); }
  else if (data::is_fbag_container(x)) { static_cast<Derived&>(*this)(data::fbag_container(atermpp::aterm_appl(x))); }
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::binder_type& x)
{
  static_cast<Derived&>(*this).enter(x);
  if (data::is_set_or_bag_comprehension_binder(x)) { static_cast<Derived&>(*this)(data::set_or_bag_comprehension_binder(atermpp::aterm_appl(x))); }
  else if (data::is_set_comprehension_binder(x)) { static_cast<Derived&>(*this)(data::set_comprehension_binder(atermpp::aterm_appl(x))); }
  else if (data::is_bag_comprehension_binder(x)) { static_cast<Derived&>(*this)(data::bag_comprehension_binder(atermpp::aterm_appl(x))); }
  else if (data::is_forall_binder(x)) { static_cast<Derived&>(*this)(data::forall_binder(atermpp::aterm_appl(x))); }
  else if (data::is_exists_binder(x)) { static_cast<Derived&>(*this)(data::exists_binder(atermpp::aterm_appl(x))); }
  else if (data::is_lambda_binder(x)) { static_cast<Derived&>(*this)(data::lambda_binder(atermpp::aterm_appl(x))); }
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data::abstraction& x)
{
  static_cast<Derived&>(*this).enter(x);
  if (data::is_forall(x)) { static_cast<Derived&>(*this)(data::forall(atermpp::aterm_appl(x))); }
  else if (data::is_exists(x)) { static_cast<Derived&>(*this)(data::exists(atermpp::aterm_appl(x))); }
  else if (data::is_lambda(x)) { static_cast<Derived&>(*this)(data::lambda(atermpp::aterm_appl(x))); }
  static_cast<Derived&>(*this).leave(x);
}
//--- end generated code ---//
