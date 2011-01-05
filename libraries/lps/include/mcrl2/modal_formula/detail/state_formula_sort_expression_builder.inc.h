// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/include/mcrl2/modal_formula/detail/state_formula_sort_expression_builder.inc.h
/// \brief The content of this file is included in other header
/// files, to prevent duplication.

//--- start generated code ---//
state_formulas::state_formula operator()(const state_formulas::true_& x)
{
  static_cast<Derived&>(*this).enter(x);  
  // skip
  static_cast<Derived&>(*this).leave(x);
  return x;
}

state_formulas::state_formula operator()(const state_formulas::false_& x)
{
  static_cast<Derived&>(*this).enter(x);  
  // skip
  static_cast<Derived&>(*this).leave(x);
  return x;
}

state_formulas::state_formula operator()(const state_formulas::not_& x)
{
  static_cast<Derived&>(*this).enter(x);  
  state_formulas::state_formula result = state_formulas::not_(static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

state_formulas::state_formula operator()(const state_formulas::and_& x)
{
  static_cast<Derived&>(*this).enter(x);  
  state_formulas::state_formula result = state_formulas::and_(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

state_formulas::state_formula operator()(const state_formulas::or_& x)
{
  static_cast<Derived&>(*this).enter(x);  
  state_formulas::state_formula result = state_formulas::or_(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

state_formulas::state_formula operator()(const state_formulas::imp& x)
{
  static_cast<Derived&>(*this).enter(x);  
  state_formulas::state_formula result = state_formulas::imp(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

state_formulas::state_formula operator()(const state_formulas::forall& x)
{
  static_cast<Derived&>(*this).enter(x);  
  state_formulas::state_formula result = state_formulas::forall(static_cast<Derived&>(*this)(x.variables()), static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

state_formulas::state_formula operator()(const state_formulas::exists& x)
{
  static_cast<Derived&>(*this).enter(x);  
  state_formulas::state_formula result = state_formulas::exists(static_cast<Derived&>(*this)(x.variables()), static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

state_formulas::state_formula operator()(const state_formulas::must& x)
{
  static_cast<Derived&>(*this).enter(x);  
  state_formulas::state_formula result = state_formulas::must(static_cast<Derived&>(*this)(x.formula()), static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

state_formulas::state_formula operator()(const state_formulas::may& x)
{
  static_cast<Derived&>(*this).enter(x);  
  state_formulas::state_formula result = state_formulas::may(static_cast<Derived&>(*this)(x.formula()), static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

state_formulas::state_formula operator()(const state_formulas::yaled& x)
{
  static_cast<Derived&>(*this).enter(x);  
  // skip
  static_cast<Derived&>(*this).leave(x);
  return x;
}

state_formulas::state_formula operator()(const state_formulas::yaled_timed& x)
{
  static_cast<Derived&>(*this).enter(x);  
  state_formulas::state_formula result = state_formulas::yaled_timed(static_cast<Derived&>(*this)(x.time_stamp()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

state_formulas::state_formula operator()(const state_formulas::delay& x)
{
  static_cast<Derived&>(*this).enter(x);  
  // skip
  static_cast<Derived&>(*this).leave(x);
  return x;
}

state_formulas::state_formula operator()(const state_formulas::delay_timed& x)
{
  static_cast<Derived&>(*this).enter(x);  
  state_formulas::state_formula result = state_formulas::delay_timed(static_cast<Derived&>(*this)(x.time_stamp()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

state_formulas::state_formula operator()(const state_formulas::variable& x)
{
  static_cast<Derived&>(*this).enter(x);  
  state_formulas::state_formula result = state_formulas::variable(x.name(), static_cast<Derived&>(*this)(x.arguments()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

state_formulas::state_formula operator()(const state_formulas::nu& x)
{
  static_cast<Derived&>(*this).enter(x);  
  state_formulas::state_formula result = state_formulas::nu(x.name(), static_cast<Derived&>(*this)(x.assignments()), static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

state_formulas::state_formula operator()(const state_formulas::mu& x)
{
  static_cast<Derived&>(*this).enter(x);  
  state_formulas::state_formula result = state_formulas::mu(x.name(), static_cast<Derived&>(*this)(x.assignments()), static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

state_formulas::state_formula operator()(const state_formulas::state_formula& x)
{
  static_cast<Derived&>(*this).enter(x);  
  state_formulas::state_formula result;
  if (data::is_data_expression(x)) { result = static_cast<Derived&>(*this)(data::data_expression(atermpp::aterm_appl(x))); }
  else if (state_formulas::is_true(x)) { result = static_cast<Derived&>(*this)(state_formulas::true_(atermpp::aterm_appl(x))); }
  else if (state_formulas::is_false(x)) { result = static_cast<Derived&>(*this)(state_formulas::false_(atermpp::aterm_appl(x))); }
  else if (state_formulas::is_not(x)) { result = static_cast<Derived&>(*this)(state_formulas::not_(atermpp::aterm_appl(x))); }
  else if (state_formulas::is_and(x)) { result = static_cast<Derived&>(*this)(state_formulas::and_(atermpp::aterm_appl(x))); }
  else if (state_formulas::is_or(x)) { result = static_cast<Derived&>(*this)(state_formulas::or_(atermpp::aterm_appl(x))); }
  else if (state_formulas::is_imp(x)) { result = static_cast<Derived&>(*this)(state_formulas::imp(atermpp::aterm_appl(x))); }
  else if (state_formulas::is_forall(x)) { result = static_cast<Derived&>(*this)(state_formulas::forall(atermpp::aterm_appl(x))); }
  else if (state_formulas::is_exists(x)) { result = static_cast<Derived&>(*this)(state_formulas::exists(atermpp::aterm_appl(x))); }
  else if (state_formulas::is_must(x)) { result = static_cast<Derived&>(*this)(state_formulas::must(atermpp::aterm_appl(x))); }
  else if (state_formulas::is_may(x)) { result = static_cast<Derived&>(*this)(state_formulas::may(atermpp::aterm_appl(x))); }
  else if (state_formulas::is_yaled(x)) { result = static_cast<Derived&>(*this)(state_formulas::yaled(atermpp::aterm_appl(x))); }
  else if (state_formulas::is_yaled_timed(x)) { result = static_cast<Derived&>(*this)(state_formulas::yaled_timed(atermpp::aterm_appl(x))); }
  else if (state_formulas::is_delay(x)) { result = static_cast<Derived&>(*this)(state_formulas::delay(atermpp::aterm_appl(x))); }
  else if (state_formulas::is_delay_timed(x)) { result = static_cast<Derived&>(*this)(state_formulas::delay_timed(atermpp::aterm_appl(x))); }
  else if (state_formulas::is_variable(x)) { result = static_cast<Derived&>(*this)(state_formulas::variable(atermpp::aterm_appl(x))); }
  else if (state_formulas::is_nu(x)) { result = static_cast<Derived&>(*this)(state_formulas::nu(atermpp::aterm_appl(x))); }
  else if (state_formulas::is_mu(x)) { result = static_cast<Derived&>(*this)(state_formulas::mu(atermpp::aterm_appl(x))); }
  static_cast<Derived&>(*this).leave(x);
  return result;
}
//--- end generated code ---//
