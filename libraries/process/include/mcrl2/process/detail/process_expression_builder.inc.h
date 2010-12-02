// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/detail/process_expression_builder.inc.h
/// \brief The content of this file is included in other header
/// files, to prevent duplication.

//--- start generated code ---//
process_expression operator()(const process_instance& x)
{
  static_cast<Derived&>(*this).enter(x);
  process_expression result = process_instance(x.identifier(), x.actual_parameters());
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process_expression operator()(const process_instance_assignment& x)
{
  static_cast<Derived&>(*this).enter(x);
  process_expression result = process_instance_assignment(x.identifier(), x.assignments());
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process_expression operator()(const delta& x)
{
  static_cast<Derived&>(*this).enter(x);
  process_expression result = delta();
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process_expression operator()(const tau& x)
{
  static_cast<Derived&>(*this).enter(x);
  process_expression result = tau();
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process_expression operator()(const sum& x)
{
  static_cast<Derived&>(*this).enter(x);
  process_expression result = sum(x.bound_variables(), static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process_expression operator()(const block& x)
{
  static_cast<Derived&>(*this).enter(x);
  process_expression result = block(x.block_set(), static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process_expression operator()(const hide& x)
{
  static_cast<Derived&>(*this).enter(x);
  process_expression result = hide(x.hide_set(), static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process_expression operator()(const rename& x)
{
  static_cast<Derived&>(*this).enter(x);
  process_expression result = rename(x.rename_set(), static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process_expression operator()(const comm& x)
{
  static_cast<Derived&>(*this).enter(x);
  process_expression result = comm(x.comm_set(), static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process_expression operator()(const allow& x)
{
  static_cast<Derived&>(*this).enter(x);
  process_expression result = allow(x.allow_set(), static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process_expression operator()(const sync& x)
{
  static_cast<Derived&>(*this).enter(x);
  process_expression result = sync(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process_expression operator()(const at& x)
{
  static_cast<Derived&>(*this).enter(x);
  process_expression result = at(static_cast<Derived&>(*this)(x.operand()), x.time_stamp());
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process_expression operator()(const seq& x)
{
  static_cast<Derived&>(*this).enter(x);
  process_expression result = seq(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process_expression operator()(const if_then& x)
{
  static_cast<Derived&>(*this).enter(x);
  process_expression result = if_then(x.condition(), static_cast<Derived&>(*this)(x.then_case()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process_expression operator()(const if_then_else& x)
{
  static_cast<Derived&>(*this).enter(x);
  process_expression result = if_then_else(x.condition(), static_cast<Derived&>(*this)(x.then_case()), static_cast<Derived&>(*this)(x.else_case()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process_expression operator()(const bounded_init& x)
{
  static_cast<Derived&>(*this).enter(x);
  process_expression result = bounded_init(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process_expression operator()(const merge& x)
{
  static_cast<Derived&>(*this).enter(x);
  process_expression result = merge(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process_expression operator()(const left_merge& x)
{
  static_cast<Derived&>(*this).enter(x);
  process_expression result = left_merge(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process_expression operator()(const choice& x)
{
  static_cast<Derived&>(*this).enter(x);
  process_expression result = choice(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

process_expression operator()(const process_expression& x)
{
  static_cast<Derived&>(*this).enter(x);
  process_expression result;
  if (lps::is_action(x)) { result = static_cast<Derived&>(*this)(lps::action(atermpp::aterm_appl(x))); }
  else if (is_process_instance(x)) { result = static_cast<Derived&>(*this)(process_instance(atermpp::aterm_appl(x))); }
  else if (is_process_instance_assignment(x)) { result = static_cast<Derived&>(*this)(process_instance_assignment(atermpp::aterm_appl(x))); }
  else if (is_delta(x)) { result = static_cast<Derived&>(*this)(delta(atermpp::aterm_appl(x))); }
  else if (is_tau(x)) { result = static_cast<Derived&>(*this)(tau(atermpp::aterm_appl(x))); }
  else if (is_sum(x)) { result = static_cast<Derived&>(*this)(sum(atermpp::aterm_appl(x))); }
  else if (is_block(x)) { result = static_cast<Derived&>(*this)(block(atermpp::aterm_appl(x))); }
  else if (is_hide(x)) { result = static_cast<Derived&>(*this)(hide(atermpp::aterm_appl(x))); }
  else if (is_rename(x)) { result = static_cast<Derived&>(*this)(rename(atermpp::aterm_appl(x))); }
  else if (is_comm(x)) { result = static_cast<Derived&>(*this)(comm(atermpp::aterm_appl(x))); }
  else if (is_allow(x)) { result = static_cast<Derived&>(*this)(allow(atermpp::aterm_appl(x))); }
  else if (is_sync(x)) { result = static_cast<Derived&>(*this)(sync(atermpp::aterm_appl(x))); }
  else if (is_at(x)) { result = static_cast<Derived&>(*this)(at(atermpp::aterm_appl(x))); }
  else if (is_seq(x)) { result = static_cast<Derived&>(*this)(seq(atermpp::aterm_appl(x))); }
  else if (is_if_then(x)) { result = static_cast<Derived&>(*this)(if_then(atermpp::aterm_appl(x))); }
  else if (is_if_then_else(x)) { result = static_cast<Derived&>(*this)(if_then_else(atermpp::aterm_appl(x))); }
  else if (is_bounded_init(x)) { result = static_cast<Derived&>(*this)(bounded_init(atermpp::aterm_appl(x))); }
  else if (is_merge(x)) { result = static_cast<Derived&>(*this)(merge(atermpp::aterm_appl(x))); }
  else if (is_left_merge(x)) { result = static_cast<Derived&>(*this)(left_merge(atermpp::aterm_appl(x))); }
  else if (is_choice(x)) { result = static_cast<Derived&>(*this)(choice(atermpp::aterm_appl(x))); }
  static_cast<Derived&>(*this).leave(x);
  return result;
}
//--- end generated code ---//

void operator()(process_equation& x)
{
  x = process_equation(x.identifier(),
                       x.formal_parameters(),
                       static_cast<Derived&>(*this)(x.expression())
                      );
}

void operator()(process_specification& x)
{
  static_cast<Derived&>(*this)(x.equations());
}     

process_identifier operator()(const process_identifier& x)
{
  return x;
}

process_expression operator()(const lps::action& x)
{
  return x;
}     
