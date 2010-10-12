// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/detail/builder.inc.h
/// \brief The content of this file is included in other header
/// files, to prevent duplication.

//--- start generated code ---//
boolean_expression operator()(const true_& x)
{
  static_cast<Derived&>(*this).enter(x);
  boolean_expression result = true_();
  static_cast<Derived&>(*this).leave(x);
  return result;
}

boolean_expression operator()(const false_& x)
{
  static_cast<Derived&>(*this).enter(x);
  boolean_expression result = false_();
  static_cast<Derived&>(*this).leave(x);
  return result;
}

boolean_expression operator()(const not_& x)
{
  static_cast<Derived&>(*this).enter(x);
  boolean_expression result = not_(static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

boolean_expression operator()(const and_& x)
{
  static_cast<Derived&>(*this).enter(x);
  boolean_expression result = and_(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

boolean_expression operator()(const or_& x)
{
  static_cast<Derived&>(*this).enter(x);
  boolean_expression result = or_(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

boolean_expression operator()(const imp& x)
{
  static_cast<Derived&>(*this).enter(x);
  boolean_expression result = imp(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

boolean_expression operator()(const boolean_variable& x)
{
  static_cast<Derived&>(*this).enter(x);
  boolean_expression result = boolean_variable(x.name());
  static_cast<Derived&>(*this).leave(x);
  return result;
}

boolean_expression operator()(const boolean_expression& x)
{
  static_cast<Derived&>(*this).enter(x);
  boolean_expression result;
  if (is_true(x)) { result = static_cast<Derived&>(*this)(true_(atermpp::aterm_appl(x))); }
  else if (is_false(x)) { result = static_cast<Derived&>(*this)(false_(atermpp::aterm_appl(x))); }
  else if (is_not(x)) { result = static_cast<Derived&>(*this)(not_(atermpp::aterm_appl(x))); }
  else if (is_and(x)) { result = static_cast<Derived&>(*this)(and_(atermpp::aterm_appl(x))); }
  else if (is_or(x)) { result = static_cast<Derived&>(*this)(or_(atermpp::aterm_appl(x))); }
  else if (is_imp(x)) { result = static_cast<Derived&>(*this)(imp(atermpp::aterm_appl(x))); }
  else if (is_boolean_variable(x)) { result = static_cast<Derived&>(*this)(boolean_variable(atermpp::aterm_appl(x))); }
  static_cast<Derived&>(*this).leave(x);
  return result;
}
//--- end generated code ---//
