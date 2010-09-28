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
void operator()(const assignment& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.lhs());
  static_cast<Derived&>(*this)(x.rhs());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const identifier_assignment& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.lhs());
  static_cast<Derived&>(*this)(x.rhs());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const set_or_bag_comprehension_binder& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const set_comprehension_binder& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const bag_comprehension_binder& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const forall_binder& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const exists_binder& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const lambda_binder& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const structured_sort_constructor_argument& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.name());
  static_cast<Derived&>(*this)(x.sort());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const structured_sort_constructor& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.name());
  static_cast<Derived&>(*this)(x.arguments());
  static_cast<Derived&>(*this)(x.recogniser());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const list_container& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const set_container& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const bag_container& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const fset_container& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const fbag_container& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const basic_sort& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.name());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const container_sort& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.container_name());
  static_cast<Derived&>(*this)(x.element_sort());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const structured_sort& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.constructors());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const function_sort& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.domain());
  static_cast<Derived&>(*this)(x.codomain());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const unknown_sort& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const multiple_possible_sorts& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.sorts());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const identifier& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.name());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const variable& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.name());
  static_cast<Derived&>(*this)(x.sort());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const function_symbol& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.name());
  static_cast<Derived&>(*this)(x.sort());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const application& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.head());
  static_cast<Derived&>(*this)(x.arguments());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const abstraction& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.binding_operator());
  static_cast<Derived&>(*this)(x.variables());
  static_cast<Derived&>(*this)(x.body());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const where_clause& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.body());
  static_cast<Derived&>(*this)(x.declarations());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data_equation& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.variables());
  static_cast<Derived&>(*this)(x.condition());
  static_cast<Derived&>(*this)(x.lhs());
  static_cast<Derived&>(*this)(x.rhs());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const data_expression& x)
{
  static_cast<Derived&>(*this).enter(x);
  if (is_identifier(x)) { static_cast<Derived&>(*this)(identifier(x)); }
  else if (is_variable(x)) { static_cast<Derived&>(*this)(variable(x)); }
  else if (is_function_symbol(x)) { static_cast<Derived&>(*this)(function_symbol(x)); }
  else if (is_application(x)) { static_cast<Derived&>(*this)(application(x)); }
  else if (is_abstraction(x)) { static_cast<Derived&>(*this)(abstraction(x)); }
  else if (is_where_clause(x)) { static_cast<Derived&>(*this)(where_clause(x)); }
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const assignment_expression& x)
{
  static_cast<Derived&>(*this).enter(x);
  if (is_assignment(x)) { static_cast<Derived&>(*this)(assignment(x)); }
  else if (is_identifier_assignment(x)) { static_cast<Derived&>(*this)(identifier_assignment(x)); }
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const sort_expression& x)
{
  static_cast<Derived&>(*this).enter(x);
  if (is_basic_sort(x)) { static_cast<Derived&>(*this)(basic_sort(x)); }
  else if (is_container_sort(x)) { static_cast<Derived&>(*this)(container_sort(x)); }
  else if (is_structured_sort(x)) { static_cast<Derived&>(*this)(structured_sort(x)); }
  else if (is_function_sort(x)) { static_cast<Derived&>(*this)(function_sort(x)); }
  else if (is_unknown_sort(x)) { static_cast<Derived&>(*this)(unknown_sort(x)); }
  else if (is_multiple_possible_sorts(x)) { static_cast<Derived&>(*this)(multiple_possible_sorts(x)); }
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const container_type& x)
{
  static_cast<Derived&>(*this).enter(x);
  if (is_list_container(x)) { static_cast<Derived&>(*this)(list_container(x)); }
  else if (is_set_container(x)) { static_cast<Derived&>(*this)(set_container(x)); }
  else if (is_bag_container(x)) { static_cast<Derived&>(*this)(bag_container(x)); }
  else if (is_fset_container(x)) { static_cast<Derived&>(*this)(fset_container(x)); }
  else if (is_fbag_container(x)) { static_cast<Derived&>(*this)(fbag_container(x)); }
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const binder_type& x)
{
  static_cast<Derived&>(*this).enter(x);
  if (is_set_or_bag_comprehension_binder(x)) { static_cast<Derived&>(*this)(set_or_bag_comprehension_binder(x)); }
  else if (is_set_comprehension_binder(x)) { static_cast<Derived&>(*this)(set_comprehension_binder(x)); }
  else if (is_bag_comprehension_binder(x)) { static_cast<Derived&>(*this)(bag_comprehension_binder(x)); }
  else if (is_forall_binder(x)) { static_cast<Derived&>(*this)(forall_binder(x)); }
  else if (is_exists_binder(x)) { static_cast<Derived&>(*this)(exists_binder(x)); }
  else if (is_lambda_binder(x)) { static_cast<Derived&>(*this)(lambda_binder(x)); }
  static_cast<Derived&>(*this).leave(x);
}
//--- end generated code ---//
