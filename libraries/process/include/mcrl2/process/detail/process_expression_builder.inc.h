// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/include/mcrl2/process/detail/process_expression_builder.inc.h
/// \brief The content of this file is included in other header
/// files, to prevent duplication.

//--- start generated code ---//
void operator()(process::process_specification& x)
{
  static_cast<Derived&>(*this).enter(x);  
  static_cast<Derived&>(*this)(x.global_variables());
  static_cast<Derived&>(*this)(x.equations());
  x.init() = static_cast<Derived&>(*this)(x.init());
  static_cast<Derived&>(*this).leave(x);
}

process::process_equation operator()(const process::process_equation& x)
{
  static_cast<Derived&>(*this).enter(x);  
  process::process_equation result = process::process_equation(x.identifier(), static_cast<Derived&>(*this)(x.formal_parameters()), static_cast<Derived&>(*this)(x.expression()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process::process_expression operator()(const process::process_instance& x)
{
  static_cast<Derived&>(*this).enter(x);  
  process::process_expression result = process::process_instance(x.identifier(), static_cast<Derived&>(*this)(x.actual_parameters()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process::process_expression operator()(const process::process_instance_assignment& x)
{
  static_cast<Derived&>(*this).enter(x);  
  process::process_expression result = process::process_instance_assignment(x.identifier(), static_cast<Derived&>(*this)(x.assignments()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process::process_expression operator()(const process::delta& x)
{
  static_cast<Derived&>(*this).enter(x);  
  // skip
  static_cast<Derived&>(*this).leave(x);
  return x;
}

process::process_expression operator()(const process::tau& x)
{
  static_cast<Derived&>(*this).enter(x);  
  // skip
  static_cast<Derived&>(*this).leave(x);
  return x;
}

process::process_expression operator()(const process::sum& x)
{
  static_cast<Derived&>(*this).enter(x);  
  process::process_expression result = process::sum(static_cast<Derived&>(*this)(x.bound_variables()), static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process::process_expression operator()(const process::block& x)
{
  static_cast<Derived&>(*this).enter(x);  
  process::process_expression result = process::block(x.block_set(), static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process::process_expression operator()(const process::hide& x)
{
  static_cast<Derived&>(*this).enter(x);  
  process::process_expression result = process::hide(x.hide_set(), static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process::process_expression operator()(const process::rename& x)
{
  static_cast<Derived&>(*this).enter(x);  
  process::process_expression result = process::rename(x.rename_set(), static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process::process_expression operator()(const process::comm& x)
{
  static_cast<Derived&>(*this).enter(x);  
  process::process_expression result = process::comm(x.comm_set(), static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process::process_expression operator()(const process::allow& x)
{
  static_cast<Derived&>(*this).enter(x);  
  process::process_expression result = process::allow(x.allow_set(), static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process::process_expression operator()(const process::sync& x)
{
  static_cast<Derived&>(*this).enter(x);  
  process::process_expression result = process::sync(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process::process_expression operator()(const process::at& x)
{
  static_cast<Derived&>(*this).enter(x);  
  process::process_expression result = process::at(static_cast<Derived&>(*this)(x.operand()), static_cast<Derived&>(*this)(x.time_stamp()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process::process_expression operator()(const process::seq& x)
{
  static_cast<Derived&>(*this).enter(x);  
  process::process_expression result = process::seq(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process::process_expression operator()(const process::if_then& x)
{
  static_cast<Derived&>(*this).enter(x);  
  process::process_expression result = process::if_then(static_cast<Derived&>(*this)(x.condition()), static_cast<Derived&>(*this)(x.then_case()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process::process_expression operator()(const process::if_then_else& x)
{
  static_cast<Derived&>(*this).enter(x);  
  process::process_expression result = process::if_then_else(static_cast<Derived&>(*this)(x.condition()), static_cast<Derived&>(*this)(x.then_case()), static_cast<Derived&>(*this)(x.else_case()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process::process_expression operator()(const process::bounded_init& x)
{
  static_cast<Derived&>(*this).enter(x);  
  process::process_expression result = process::bounded_init(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process::process_expression operator()(const process::merge& x)
{
  static_cast<Derived&>(*this).enter(x);  
  process::process_expression result = process::merge(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process::process_expression operator()(const process::left_merge& x)
{
  static_cast<Derived&>(*this).enter(x);  
  process::process_expression result = process::left_merge(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process::process_expression operator()(const process::choice& x)
{
  static_cast<Derived&>(*this).enter(x);  
  process::process_expression result = process::choice(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process::process_expression operator()(const process::process_expression& x)
{
  static_cast<Derived&>(*this).enter(x);  
  process::process_expression result;
  if (process::is_process_instance(x)) { result = static_cast<Derived&>(*this)(process::process_instance(atermpp::aterm_appl(x))); }
  else if (process::is_process_instance_assignment(x)) { result = static_cast<Derived&>(*this)(process::process_instance_assignment(atermpp::aterm_appl(x))); }
  else if (process::is_delta(x)) { result = static_cast<Derived&>(*this)(process::delta(atermpp::aterm_appl(x))); }
  else if (process::is_tau(x)) { result = static_cast<Derived&>(*this)(process::tau(atermpp::aterm_appl(x))); }
  else if (process::is_sum(x)) { result = static_cast<Derived&>(*this)(process::sum(atermpp::aterm_appl(x))); }
  else if (process::is_block(x)) { result = static_cast<Derived&>(*this)(process::block(atermpp::aterm_appl(x))); }
  else if (process::is_hide(x)) { result = static_cast<Derived&>(*this)(process::hide(atermpp::aterm_appl(x))); }
  else if (process::is_rename(x)) { result = static_cast<Derived&>(*this)(process::rename(atermpp::aterm_appl(x))); }
  else if (process::is_comm(x)) { result = static_cast<Derived&>(*this)(process::comm(atermpp::aterm_appl(x))); }
  else if (process::is_allow(x)) { result = static_cast<Derived&>(*this)(process::allow(atermpp::aterm_appl(x))); }
  else if (process::is_sync(x)) { result = static_cast<Derived&>(*this)(process::sync(atermpp::aterm_appl(x))); }
  else if (process::is_at(x)) { result = static_cast<Derived&>(*this)(process::at(atermpp::aterm_appl(x))); }
  else if (process::is_seq(x)) { result = static_cast<Derived&>(*this)(process::seq(atermpp::aterm_appl(x))); }
  else if (process::is_if_then(x)) { result = static_cast<Derived&>(*this)(process::if_then(atermpp::aterm_appl(x))); }
  else if (process::is_if_then_else(x)) { result = static_cast<Derived&>(*this)(process::if_then_else(atermpp::aterm_appl(x))); }
  else if (process::is_bounded_init(x)) { result = static_cast<Derived&>(*this)(process::bounded_init(atermpp::aterm_appl(x))); }
  else if (process::is_merge(x)) { result = static_cast<Derived&>(*this)(process::merge(atermpp::aterm_appl(x))); }
  else if (process::is_left_merge(x)) { result = static_cast<Derived&>(*this)(process::left_merge(atermpp::aterm_appl(x))); }
  else if (process::is_choice(x)) { result = static_cast<Derived&>(*this)(process::choice(atermpp::aterm_appl(x))); }
  else if (lps::is_action(x)) { result = static_cast<Derived&>(*this)(lps::action(atermpp::aterm_appl(x))); }
  static_cast<Derived&>(*this).leave(x);
  return result;
}
//--- end generated code ---//
