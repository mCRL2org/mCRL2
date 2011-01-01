// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/include/mcrl2/modal_formula/detail/action_formula_builder.inc.h
/// \brief The content of this file is included in other header
/// files, to prevent duplication.

//--- start generated code ---//
action_formulas::action_formula operator()(const action_formulas::true_& x)
{
  static_cast<Derived&>(*this).enter(x);  
  // skip
  static_cast<Derived&>(*this).leave(x);
  return x;
}

action_formulas::action_formula operator()(const action_formulas::false_& x)
{
  static_cast<Derived&>(*this).enter(x);  
  // skip
  static_cast<Derived&>(*this).leave(x);
  return x;
}

action_formulas::action_formula operator()(const action_formulas::not_& x)
{
  static_cast<Derived&>(*this).enter(x);  
  action_formulas::action_formula result = action_formulas::not_(static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

action_formulas::action_formula operator()(const action_formulas::and_& x)
{
  static_cast<Derived&>(*this).enter(x);  
  action_formulas::action_formula result = action_formulas::and_(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

action_formulas::action_formula operator()(const action_formulas::or_& x)
{
  static_cast<Derived&>(*this).enter(x);  
  action_formulas::action_formula result = action_formulas::or_(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

action_formulas::action_formula operator()(const action_formulas::imp& x)
{
  static_cast<Derived&>(*this).enter(x);  
  action_formulas::action_formula result = action_formulas::imp(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

action_formulas::action_formula operator()(const action_formulas::forall& x)
{
  static_cast<Derived&>(*this).enter(x);  
  action_formulas::action_formula result = action_formulas::forall(static_cast<Derived&>(*this)(x.variables()), static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

action_formulas::action_formula operator()(const action_formulas::exists& x)
{
  static_cast<Derived&>(*this).enter(x);  
  action_formulas::action_formula result = action_formulas::exists(static_cast<Derived&>(*this)(x.variables()), static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

action_formulas::action_formula operator()(const action_formulas::at& x)
{
  static_cast<Derived&>(*this).enter(x);  
  action_formulas::action_formula result = action_formulas::at(static_cast<Derived&>(*this)(x.operand()), static_cast<Derived&>(*this)(x.time_stamp()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

action_formulas::action_formula operator()(const action_formulas::action_formula& x)
{
  static_cast<Derived&>(*this).enter(x);  
  action_formulas::action_formula result;
  if (data::is_data_expression(x)) { result = static_cast<Derived&>(*this)(data::data_expression(atermpp::aterm_appl(x))); }
  else if (action_formulas::is_true(x)) { result = static_cast<Derived&>(*this)(action_formulas::true_(atermpp::aterm_appl(x))); }
  else if (action_formulas::is_false(x)) { result = static_cast<Derived&>(*this)(action_formulas::false_(atermpp::aterm_appl(x))); }
  else if (action_formulas::is_not(x)) { result = static_cast<Derived&>(*this)(action_formulas::not_(atermpp::aterm_appl(x))); }
  else if (action_formulas::is_and(x)) { result = static_cast<Derived&>(*this)(action_formulas::and_(atermpp::aterm_appl(x))); }
  else if (action_formulas::is_or(x)) { result = static_cast<Derived&>(*this)(action_formulas::or_(atermpp::aterm_appl(x))); }
  else if (action_formulas::is_imp(x)) { result = static_cast<Derived&>(*this)(action_formulas::imp(atermpp::aterm_appl(x))); }
  else if (action_formulas::is_forall(x)) { result = static_cast<Derived&>(*this)(action_formulas::forall(atermpp::aterm_appl(x))); }
  else if (action_formulas::is_exists(x)) { result = static_cast<Derived&>(*this)(action_formulas::exists(atermpp::aterm_appl(x))); }
  else if (action_formulas::is_at(x)) { result = static_cast<Derived&>(*this)(action_formulas::at(atermpp::aterm_appl(x))); }
  else if (lps::is_multi_action(x)) { result = static_cast<Derived&>(*this)(lps::multi_action(atermpp::aterm_appl(x))); }
  static_cast<Derived&>(*this).leave(x);
  return result;
}
//--- end generated code ---//
