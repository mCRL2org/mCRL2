// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/detail/action_formula_builder.inc
/// \brief The content of this file is included in other header
/// files, to prevent duplication.

//--- start generated code ---//
action_formula operator()(const true_& x)
{
  static_cast<Derived&>(*this).enter(x);
  action_formula result = true_();
  static_cast<Derived&>(*this).leave(x);
  return result;
}

action_formula operator()(const false_& x)
{
  static_cast<Derived&>(*this).enter(x);
  action_formula result = false_();
  static_cast<Derived&>(*this).leave(x);
  return result;
}

action_formula operator()(const not_& x)
{
  static_cast<Derived&>(*this).enter(x);
  action_formula result = not_(static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

action_formula operator()(const and_& x)
{
  static_cast<Derived&>(*this).enter(x);
  action_formula result = and_(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

action_formula operator()(const or_& x)
{
  static_cast<Derived&>(*this).enter(x);
  action_formula result = or_(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

action_formula operator()(const imp& x)
{
  static_cast<Derived&>(*this).enter(x);
  action_formula result = imp(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

action_formula operator()(const forall& x)
{
  static_cast<Derived&>(*this).enter(x);
  action_formula result = forall(x.variables(), static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

action_formula operator()(const exists& x)
{
  static_cast<Derived&>(*this).enter(x);
  action_formula result = exists(x.variables(), static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

action_formula operator()(const at& x)
{
  static_cast<Derived&>(*this).enter(x);
  action_formula result = at(static_cast<Derived&>(*this)(x.operand()), x.time_stamp());
  static_cast<Derived&>(*this).leave(x);
  return result;
}

action_formula operator()(const action_formula& x)
{
  static_cast<Derived&>(*this).enter(x);
  action_formula result;
  if (is_true(x)) { result = static_cast<Derived&>(*this)(true_(atermpp::aterm_appl(x))); }
  else if (is_false(x)) { result = static_cast<Derived&>(*this)(false_(atermpp::aterm_appl(x))); }
  else if (is_not(x)) { result = static_cast<Derived&>(*this)(not_(atermpp::aterm_appl(x))); }
  else if (is_and(x)) { result = static_cast<Derived&>(*this)(and_(atermpp::aterm_appl(x))); }
  else if (is_or(x)) { result = static_cast<Derived&>(*this)(or_(atermpp::aterm_appl(x))); }
  else if (is_imp(x)) { result = static_cast<Derived&>(*this)(imp(atermpp::aterm_appl(x))); }
  else if (is_forall(x)) { result = static_cast<Derived&>(*this)(forall(atermpp::aterm_appl(x))); }
  else if (is_exists(x)) { result = static_cast<Derived&>(*this)(exists(atermpp::aterm_appl(x))); }
  else if (is_at(x)) { result = static_cast<Derived&>(*this)(at(atermpp::aterm_appl(x))); }
  static_cast<Derived&>(*this).leave(x);
  return result;
}
//--- end generated code ---//
