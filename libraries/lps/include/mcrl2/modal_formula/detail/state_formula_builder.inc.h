// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/detail/state_formula_builder.inc
/// \brief The content of this file is included in other header
/// files, to prevent duplication.

//--- start generated code ---//
state_formula operator()(const true_& x)
{
  static_cast<Derived&>(*this).enter(x);
  state_formula result = true_();
  static_cast<Derived&>(*this).leave(x);
  return result;
}

state_formula operator()(const false_& x)
{
  static_cast<Derived&>(*this).enter(x);
  state_formula result = false_();
  static_cast<Derived&>(*this).leave(x);
  return result;
}

state_formula operator()(const not_& x)
{
  static_cast<Derived&>(*this).enter(x);
  state_formula result = not_(static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

state_formula operator()(const and_& x)
{
  static_cast<Derived&>(*this).enter(x);
  state_formula result = and_(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

state_formula operator()(const or_& x)
{
  static_cast<Derived&>(*this).enter(x);
  state_formula result = or_(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

state_formula operator()(const imp& x)
{
  static_cast<Derived&>(*this).enter(x);
  state_formula result = imp(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

state_formula operator()(const forall& x)
{
  static_cast<Derived&>(*this).enter(x);
  state_formula result = forall(x.variables(), static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

state_formula operator()(const exists& x)
{
  static_cast<Derived&>(*this).enter(x);
  state_formula result = exists(x.variables(), static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

state_formula operator()(const must& x)
{
  static_cast<Derived&>(*this).enter(x);
  state_formula result = must(x.formula(), static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

state_formula operator()(const may& x)
{
  static_cast<Derived&>(*this).enter(x);
  state_formula result = may(x.formula(), static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

state_formula operator()(const yaled& x)
{
  static_cast<Derived&>(*this).enter(x);
  state_formula result = yaled();
  static_cast<Derived&>(*this).leave(x);
  return result;
}

state_formula operator()(const yaled_timed& x)
{
  static_cast<Derived&>(*this).enter(x);
  state_formula result = yaled_timed(x.time_stamp());
  static_cast<Derived&>(*this).leave(x);
  return result;
}

state_formula operator()(const delay& x)
{
  static_cast<Derived&>(*this).enter(x);
  state_formula result = delay();
  static_cast<Derived&>(*this).leave(x);
  return result;
}

state_formula operator()(const delay_timed& x)
{
  static_cast<Derived&>(*this).enter(x);
  state_formula result = delay_timed(x.time_stamp());
  static_cast<Derived&>(*this).leave(x);
  return result;
}

state_formula operator()(const variable& x)
{
  static_cast<Derived&>(*this).enter(x);
  state_formula result = variable(x.name(), x.arguments());
  static_cast<Derived&>(*this).leave(x);
  return result;
}

state_formula operator()(const nu& x)
{
  static_cast<Derived&>(*this).enter(x);
  state_formula result = nu(x.name(), x.assignments(), static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

state_formula operator()(const mu& x)
{
  static_cast<Derived&>(*this).enter(x);
  state_formula result = mu(x.name(), x.assignments(), static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

state_formula operator()(const state_formula& x)
{
  static_cast<Derived&>(*this).enter(x);
  state_formula result;
  if (data::is_data_expression(x)) { result = static_cast<Derived&>(*this)(data::data_expression(atermpp::aterm_appl(x))); }
  else if (is_true(x)) { result = static_cast<Derived&>(*this)(true_(atermpp::aterm_appl(x))); }
  else if (is_false(x)) { result = static_cast<Derived&>(*this)(false_(atermpp::aterm_appl(x))); }
  else if (is_not(x)) { result = static_cast<Derived&>(*this)(not_(atermpp::aterm_appl(x))); }
  else if (is_and(x)) { result = static_cast<Derived&>(*this)(and_(atermpp::aterm_appl(x))); }
  else if (is_or(x)) { result = static_cast<Derived&>(*this)(or_(atermpp::aterm_appl(x))); }
  else if (is_imp(x)) { result = static_cast<Derived&>(*this)(imp(atermpp::aterm_appl(x))); }
  else if (is_forall(x)) { result = static_cast<Derived&>(*this)(forall(atermpp::aterm_appl(x))); }
  else if (is_exists(x)) { result = static_cast<Derived&>(*this)(exists(atermpp::aterm_appl(x))); }
  else if (is_must(x)) { result = static_cast<Derived&>(*this)(must(atermpp::aterm_appl(x))); }
  else if (is_may(x)) { result = static_cast<Derived&>(*this)(may(atermpp::aterm_appl(x))); }
  else if (is_yaled(x)) { result = static_cast<Derived&>(*this)(yaled(atermpp::aterm_appl(x))); }
  else if (is_yaled_timed(x)) { result = static_cast<Derived&>(*this)(yaled_timed(atermpp::aterm_appl(x))); }
  else if (is_delay(x)) { result = static_cast<Derived&>(*this)(delay(atermpp::aterm_appl(x))); }
  else if (is_delay_timed(x)) { result = static_cast<Derived&>(*this)(delay_timed(atermpp::aterm_appl(x))); }
  else if (is_variable(x)) { result = static_cast<Derived&>(*this)(variable(atermpp::aterm_appl(x))); }
  else if (is_nu(x)) { result = static_cast<Derived&>(*this)(nu(atermpp::aterm_appl(x))); }
  else if (is_mu(x)) { result = static_cast<Derived&>(*this)(mu(atermpp::aterm_appl(x))); }
  static_cast<Derived&>(*this).leave(x);
  return result;
}
//--- end generated code ---//
