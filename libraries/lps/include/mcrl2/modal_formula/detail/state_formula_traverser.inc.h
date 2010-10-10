// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/detail/state_formula_traverser.inc
/// \brief The content of this file is included in other header
/// files, to prevent duplication.

//--- start generated code ---//
void operator()(const state_formulas::true_& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const state_formulas::false_& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const state_formulas::not_& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.operand());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const state_formulas::and_& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.left());
  static_cast<Derived&>(*this)(x.right());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const state_formulas::or_& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.left());
  static_cast<Derived&>(*this)(x.right());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const state_formulas::imp& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.left());
  static_cast<Derived&>(*this)(x.right());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const state_formulas::forall& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.variables());
  static_cast<Derived&>(*this)(x.operand());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const state_formulas::exists& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.variables());
  static_cast<Derived&>(*this)(x.operand());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const state_formulas::must& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.formula());
  static_cast<Derived&>(*this)(x.operand());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const state_formulas::may& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.formula());
  static_cast<Derived&>(*this)(x.operand());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const state_formulas::yaled& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const state_formulas::yaled_timed& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.time_stamp());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const state_formulas::delay& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const state_formulas::delay_timed& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.time_stamp());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const state_formulas::variable& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.name());
  static_cast<Derived&>(*this)(x.arguments());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const state_formulas::nu& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.name());
  static_cast<Derived&>(*this)(x.assignments());
  static_cast<Derived&>(*this)(x.operand());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const state_formulas::mu& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.name());
  static_cast<Derived&>(*this)(x.assignments());
  static_cast<Derived&>(*this)(x.operand());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const state_formulas::state_formula& x)
{
  static_cast<Derived&>(*this).enter(x);
  if (data::is_data_expression(x)) { static_cast<Derived&>(*this)(data::data_expression(atermpp::aterm_appl(x))); }
  else if (is_true(x)) { static_cast<Derived&>(*this)(state_formulas::true_(atermpp::aterm_appl(x))); }
  else if (is_false(x)) { static_cast<Derived&>(*this)(state_formulas::false_(atermpp::aterm_appl(x))); }
  else if (is_not(x)) { static_cast<Derived&>(*this)(state_formulas::not_(atermpp::aterm_appl(x))); }
  else if (is_and(x)) { static_cast<Derived&>(*this)(state_formulas::and_(atermpp::aterm_appl(x))); }
  else if (is_or(x)) { static_cast<Derived&>(*this)(state_formulas::or_(atermpp::aterm_appl(x))); }
  else if (is_imp(x)) { static_cast<Derived&>(*this)(state_formulas::imp(atermpp::aterm_appl(x))); }
  else if (is_forall(x)) { static_cast<Derived&>(*this)(state_formulas::forall(atermpp::aterm_appl(x))); }
  else if (is_exists(x)) { static_cast<Derived&>(*this)(state_formulas::exists(atermpp::aterm_appl(x))); }
  else if (is_must(x)) { static_cast<Derived&>(*this)(state_formulas::must(atermpp::aterm_appl(x))); }
  else if (is_may(x)) { static_cast<Derived&>(*this)(state_formulas::may(atermpp::aterm_appl(x))); }
  else if (is_yaled(x)) { static_cast<Derived&>(*this)(state_formulas::yaled(atermpp::aterm_appl(x))); }
  else if (is_yaled_timed(x)) { static_cast<Derived&>(*this)(state_formulas::yaled_timed(atermpp::aterm_appl(x))); }
  else if (is_delay(x)) { static_cast<Derived&>(*this)(state_formulas::delay(atermpp::aterm_appl(x))); }
  else if (is_delay_timed(x)) { static_cast<Derived&>(*this)(state_formulas::delay_timed(atermpp::aterm_appl(x))); }
  else if (is_variable(x)) { static_cast<Derived&>(*this)(state_formulas::variable(atermpp::aterm_appl(x))); }
  else if (is_nu(x)) { static_cast<Derived&>(*this)(state_formulas::nu(atermpp::aterm_appl(x))); }
  else if (is_mu(x)) { static_cast<Derived&>(*this)(state_formulas::mu(atermpp::aterm_appl(x))); }
  static_cast<Derived&>(*this).leave(x);
}
//--- end generated code ---//
