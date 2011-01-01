// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/include/mcrl2/bes/detail/boolean_expression_builder.inc.h
/// \brief The content of this file is included in other header
/// files, to prevent duplication.

//--- start generated code ---//
bes::boolean_equation operator()(const bes::boolean_equation& x)
{
  static_cast<Derived&>(*this).enter(x);  
  bes::boolean_equation result = bes::boolean_equation(x.symbol(), static_cast<Derived&>(*this)(x.variable()), static_cast<Derived&>(*this)(x.formula()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

template <typename Container>
void operator()(bes::boolean_equation_system<Container>& x)
{
  static_cast<Derived&>(*this).enter(x);  
  static_cast<Derived&>(*this)(x.equations());
  x.initial_state() = static_cast<Derived&>(*this)(x.initial_state());
  static_cast<Derived&>(*this).leave(x);
}

bes::boolean_expression operator()(const bes::true_& x)
{
  static_cast<Derived&>(*this).enter(x);  
  // skip
  static_cast<Derived&>(*this).leave(x);
  return x;
}

bes::boolean_expression operator()(const bes::false_& x)
{
  static_cast<Derived&>(*this).enter(x);  
  // skip
  static_cast<Derived&>(*this).leave(x);
  return x;
}

bes::boolean_expression operator()(const bes::not_& x)
{
  static_cast<Derived&>(*this).enter(x);  
  bes::boolean_expression result = bes::not_(static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

bes::boolean_expression operator()(const bes::and_& x)
{
  static_cast<Derived&>(*this).enter(x);  
  bes::boolean_expression result = bes::and_(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

bes::boolean_expression operator()(const bes::or_& x)
{
  static_cast<Derived&>(*this).enter(x);  
  bes::boolean_expression result = bes::or_(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

bes::boolean_expression operator()(const bes::imp& x)
{
  static_cast<Derived&>(*this).enter(x);  
  bes::boolean_expression result = bes::imp(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

bes::boolean_expression operator()(const bes::boolean_variable& x)
{
  static_cast<Derived&>(*this).enter(x);  
  // skip
  static_cast<Derived&>(*this).leave(x);
  return x;
}

bes::boolean_expression operator()(const bes::boolean_expression& x)
{
  static_cast<Derived&>(*this).enter(x);  
  bes::boolean_expression result;
  if (bes::is_true(x)) { result = static_cast<Derived&>(*this)(bes::true_(atermpp::aterm_appl(x))); }
  else if (bes::is_false(x)) { result = static_cast<Derived&>(*this)(bes::false_(atermpp::aterm_appl(x))); }
  else if (bes::is_not(x)) { result = static_cast<Derived&>(*this)(bes::not_(atermpp::aterm_appl(x))); }
  else if (bes::is_and(x)) { result = static_cast<Derived&>(*this)(bes::and_(atermpp::aterm_appl(x))); }
  else if (bes::is_or(x)) { result = static_cast<Derived&>(*this)(bes::or_(atermpp::aterm_appl(x))); }
  else if (bes::is_imp(x)) { result = static_cast<Derived&>(*this)(bes::imp(atermpp::aterm_appl(x))); }
  else if (bes::is_boolean_variable(x)) { result = static_cast<Derived&>(*this)(bes::boolean_variable(atermpp::aterm_appl(x))); }
  static_cast<Derived&>(*this).leave(x);
  return result;
}
//--- end generated code ---//
