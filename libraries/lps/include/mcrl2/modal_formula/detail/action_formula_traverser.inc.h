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
void operator()(const action_formulas::true_& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const action_formulas::false_& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const action_formulas::not_& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.operand());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const action_formulas::and_& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.left());
  static_cast<Derived&>(*this)(x.right());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const action_formulas::or_& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.left());
  static_cast<Derived&>(*this)(x.right());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const action_formulas::imp& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.left());
  static_cast<Derived&>(*this)(x.right());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const action_formulas::forall& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.variables());
  static_cast<Derived&>(*this)(x.operand());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const action_formulas::exists& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.variables());
  static_cast<Derived&>(*this)(x.operand());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const action_formulas::at& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.operand());
  static_cast<Derived&>(*this)(x.time_stamp());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const action_formulas::action_formula& x)
{
  static_cast<Derived&>(*this).enter(x);
  if (data::is_data_expression(x)) { static_cast<Derived&>(*this)(data::data_expression(atermpp::aterm_appl(x))); }
  else if (lps::is_multi_action(x)) { static_cast<Derived&>(*this)(lps::multi_action(atermpp::aterm_appl(x))); }
  else if (is_true(x)) { static_cast<Derived&>(*this)(action_formulas::true_(atermpp::aterm_appl(x))); }
  else if (is_false(x)) { static_cast<Derived&>(*this)(action_formulas::false_(atermpp::aterm_appl(x))); }
  else if (is_not(x)) { static_cast<Derived&>(*this)(action_formulas::not_(atermpp::aterm_appl(x))); }
  else if (is_and(x)) { static_cast<Derived&>(*this)(action_formulas::and_(atermpp::aterm_appl(x))); }
  else if (is_or(x)) { static_cast<Derived&>(*this)(action_formulas::or_(atermpp::aterm_appl(x))); }
  else if (is_imp(x)) { static_cast<Derived&>(*this)(action_formulas::imp(atermpp::aterm_appl(x))); }
  else if (is_forall(x)) { static_cast<Derived&>(*this)(action_formulas::forall(atermpp::aterm_appl(x))); }
  else if (is_exists(x)) { static_cast<Derived&>(*this)(action_formulas::exists(atermpp::aterm_appl(x))); }
  else if (is_at(x)) { static_cast<Derived&>(*this)(action_formulas::at(atermpp::aterm_appl(x))); }
  static_cast<Derived&>(*this).leave(x);
}
//--- end generated code ---//
