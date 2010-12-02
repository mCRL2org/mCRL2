// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/sort_expression_builder.inc.h
/// \brief The content of this file is included in other header
/// files, to prevent duplication.

//--- start generated code ---//
sort_expression operator()(const basic_sort& x)
{
  static_cast<Derived&>(*this).enter(x);
  sort_expression result = basic_sort(x.name());
  static_cast<Derived&>(*this).leave(x);
  return result;
}

sort_expression operator()(const container_sort& x)
{
  static_cast<Derived&>(*this).enter(x);
  sort_expression result = container_sort(x.container_name(), static_cast<Derived&>(*this)(x.element_sort()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

sort_expression operator()(const structured_sort& x)
{
  static_cast<Derived&>(*this).enter(x);
  sort_expression result = structured_sort(x.constructors());
  static_cast<Derived&>(*this).leave(x);
  return result;
}

sort_expression operator()(const function_sort& x)
{
  static_cast<Derived&>(*this).enter(x);
  sort_expression result = function_sort(x.domain(), static_cast<Derived&>(*this)(x.codomain()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

sort_expression operator()(const unknown_sort& x)
{
  static_cast<Derived&>(*this).enter(x);
  sort_expression result = unknown_sort();
  static_cast<Derived&>(*this).leave(x);
  return result;
}

sort_expression operator()(const multiple_possible_sorts& x)
{
  static_cast<Derived&>(*this).enter(x);
  sort_expression result = multiple_possible_sorts(x.sorts());
  static_cast<Derived&>(*this).leave(x);
  return result;
}

sort_expression operator()(const sort_expression& x)
{
  static_cast<Derived&>(*this).enter(x);
  sort_expression result;
  if (is_basic_sort(x)) { result = static_cast<Derived&>(*this)(basic_sort(atermpp::aterm_appl(x))); }
  else if (is_container_sort(x)) { result = static_cast<Derived&>(*this)(container_sort(atermpp::aterm_appl(x))); }
  else if (is_structured_sort(x)) { result = static_cast<Derived&>(*this)(structured_sort(atermpp::aterm_appl(x))); }
  else if (is_function_sort(x)) { result = static_cast<Derived&>(*this)(function_sort(atermpp::aterm_appl(x))); }
  else if (is_unknown_sort(x)) { result = static_cast<Derived&>(*this)(unknown_sort(atermpp::aterm_appl(x))); }
  else if (is_multiple_possible_sorts(x)) { result = static_cast<Derived&>(*this)(multiple_possible_sorts(atermpp::aterm_appl(x))); }
  static_cast<Derived&>(*this).leave(x);
  return result;
}
//--- end generated code ---//

//--- data expression classes that have sort attributes ---//

data_expression operator()(const variable& x)
{
  static_cast<Derived&>(*this).enter(x);
  data_expression result = variable(x.name(), static_cast<Derived&>(*this)(x.sort()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

data_expression operator()(const function_symbol& x)
{
  static_cast<Derived&>(*this).enter(x);
  data_expression result = function_symbol(x.name(), static_cast<Derived&>(*this)(x.sort()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

data_expression operator()(const forall& x)
{
  static_cast<Derived&>(*this).enter(x);
  data_expression result = forall(static_cast<Derived&>(*this)(x.variables()), static_cast<Derived&>(*this)(x.body()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

data_expression operator()(const exists& x)
{
  static_cast<Derived&>(*this).enter(x);
  data_expression result = exists(static_cast<Derived&>(*this)(x.variables()), static_cast<Derived&>(*this)(x.body()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

data_expression operator()(const lambda& x)
{
  static_cast<Derived&>(*this).enter(x);
  data_expression result = lambda(static_cast<Derived&>(*this)(x.variables()), static_cast<Derived&>(*this)(x.body()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

//--- other data library classes that have sort attributes ---//
// None!
