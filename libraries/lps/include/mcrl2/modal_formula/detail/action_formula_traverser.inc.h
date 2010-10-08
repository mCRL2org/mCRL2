// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/detail/action_formula_traverser.inc.h
/// \brief add your file description here.

//--- start generated code ---//
void operator()(const true_& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const false_& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const not_& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.operand());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const and_& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.left());
  static_cast<Derived&>(*this)(x.right());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const or_& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.left());
  static_cast<Derived&>(*this)(x.right());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const imp& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.left());
  static_cast<Derived&>(*this)(x.right());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const forall& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.variables());
  static_cast<Derived&>(*this)(x.operand());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const exists& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.variables());
  static_cast<Derived&>(*this)(x.operand());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const at& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.operand());
  static_cast<Derived&>(*this)(x.time_stamp());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const action_formula& x)
{
  static_cast<Derived&>(*this).enter(x);
  if (is_true(x)) { static_cast<Derived&>(*this)(true_(atermpp::aterm_appl(x))); }
  else if (is_false(x)) { static_cast<Derived&>(*this)(false_(atermpp::aterm_appl(x))); }
  else if (is_not(x)) { static_cast<Derived&>(*this)(not_(atermpp::aterm_appl(x))); }
  else if (is_and(x)) { static_cast<Derived&>(*this)(and_(atermpp::aterm_appl(x))); }
  else if (is_or(x)) { static_cast<Derived&>(*this)(or_(atermpp::aterm_appl(x))); }
  else if (is_imp(x)) { static_cast<Derived&>(*this)(imp(atermpp::aterm_appl(x))); }
  else if (is_forall(x)) { static_cast<Derived&>(*this)(forall(atermpp::aterm_appl(x))); }
  else if (is_exists(x)) { static_cast<Derived&>(*this)(exists(atermpp::aterm_appl(x))); }
  else if (is_at(x)) { static_cast<Derived&>(*this)(at(atermpp::aterm_appl(x))); }
  static_cast<Derived&>(*this).leave(x);
}
//--- end generated code ---//
