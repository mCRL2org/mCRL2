// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/data_expression_builder.inc.h
/// \brief The content of this file is included in other header
/// files, to prevent duplication.

//--- start generated code ---//
data_expression operator()(const identifier& x)
{
  static_cast<Derived&>(*this).enter(x);
  data_expression result = identifier(x.name());
  static_cast<Derived&>(*this).leave(x);
  return result;
}

data_expression operator()(const variable& x)
{
  static_cast<Derived&>(*this).enter(x);
  data_expression result = variable(x.name(), x.sort());
  static_cast<Derived&>(*this).leave(x);
  return result;
}

data_expression operator()(const function_symbol& x)
{
  static_cast<Derived&>(*this).enter(x);
  data_expression result = function_symbol(x.name(), x.sort());
  static_cast<Derived&>(*this).leave(x);
  return result;
}

data_expression operator()(const application& x)
{
  static_cast<Derived&>(*this).enter(x);
  data_expression result = application(static_cast<Derived&>(*this)(x.head()), static_cast<Derived&>(*this)(x.arguments()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

data_expression operator()(const where_clause& x)
{
  static_cast<Derived&>(*this).enter(x);
  data_expression result = where_clause(static_cast<Derived&>(*this)(x.body()), static_cast<Derived&>(*this)(x.declarations()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

data_expression operator()(const forall& x)
{
  static_cast<Derived&>(*this).enter(x);
  data_expression result = forall(x.variables(), static_cast<Derived&>(*this)(x.body()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

data_expression operator()(const exists& x)
{
  static_cast<Derived&>(*this).enter(x);
  data_expression result = exists(x.variables(), static_cast<Derived&>(*this)(x.body()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

data_expression operator()(const lambda& x)
{
  static_cast<Derived&>(*this).enter(x);
  data_expression result = lambda(x.variables(), static_cast<Derived&>(*this)(x.body()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

data_expression operator()(const data_expression& x)
{
  static_cast<Derived&>(*this).enter(x);
  data_expression result;
  if (is_identifier(x)) { result = static_cast<Derived&>(*this)(identifier(atermpp::aterm_appl(x))); }
  else if (is_variable(x)) { result = static_cast<Derived&>(*this)(variable(atermpp::aterm_appl(x))); }
  else if (is_function_symbol(x)) { result = static_cast<Derived&>(*this)(function_symbol(atermpp::aterm_appl(x))); }
  else if (is_application(x)) { result = static_cast<Derived&>(*this)(application(atermpp::aterm_appl(x))); }
  else if (is_where_clause(x)) { result = static_cast<Derived&>(*this)(where_clause(atermpp::aterm_appl(x))); }
  else if (is_forall(x)) { result = static_cast<Derived&>(*this)(forall(atermpp::aterm_appl(x))); }
  else if (is_exists(x)) { result = static_cast<Derived&>(*this)(exists(atermpp::aterm_appl(x))); }
  else if (is_lambda(x)) { result = static_cast<Derived&>(*this)(lambda(atermpp::aterm_appl(x))); }
  static_cast<Derived&>(*this).leave(x);
  return result;
}
//--- end generated code ---//

assignment operator()(const assignment& x)
{
  return assignment(static_cast<Derived&>(*this)(x.lhs()), static_cast<Derived&>(*this)(x.rhs()));
}

identifier_assignment operator()(const identifier_assignment& x)
{
  return identifier_assignment(static_cast<Derived&>(*this)(x.lhs()), static_cast<Derived&>(*this)(x.rhs()));
}
            
data_equation operator()(const data_equation& x)
{
  return data_equation(static_cast<Derived&>(*this)(x.variables()), static_cast<Derived&>(*this)(x.condition()), static_cast<Derived&>(*this)(x.lhs()), static_cast<Derived&>(*this)(x.rhs()));
}

assignment_expression operator()(const assignment_expression& x)
{
  return assignment_expression(static_cast<Derived&>(*this)(x.lhs()), static_cast<Derived&>(*this)(x.rhs()));
}     
