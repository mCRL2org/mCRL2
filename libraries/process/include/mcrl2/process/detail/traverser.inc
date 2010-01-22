// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/detail/traverser.inc
/// \brief The content of this file is included in other header
/// files, to prevent duplication.

//--- start generated code ---//
void operator()(const action_label& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.name());
  static_cast<Derived&>(*this)(x.sorts());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const action& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.label());
  static_cast<Derived&>(*this)(x.arguments());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const process_instance& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.identifier());
  static_cast<Derived&>(*this)(x.actual_parameters());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const process_instance_assignment& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.identifier());
  static_cast<Derived&>(*this)(x.assignments());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const delta& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const tau& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const sum& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.bound_variables());
  static_cast<Derived&>(*this)(x.operand());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const block& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.block_set());
  static_cast<Derived&>(*this)(x.operand());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const hide& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.hide_set());
  static_cast<Derived&>(*this)(x.operand());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const rename& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.rename_set());
  static_cast<Derived&>(*this)(x.operand());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const comm& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.comm_set());
  static_cast<Derived&>(*this)(x.operand());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const allow& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.allow_set());
  static_cast<Derived&>(*this)(x.operand());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const sync& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.left());
  static_cast<Derived&>(*this)(x.right());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const at& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.operand());
  static_cast<Derived&>(*this)(x.time_stamp());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const seq& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.left());
  static_cast<Derived&>(*this)(x.right());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const if_then& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.condition());
  static_cast<Derived&>(*this)(x.then_case());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const if_then_else& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.condition());
  static_cast<Derived&>(*this)(x.then_case());
  static_cast<Derived&>(*this)(x.else_case());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const bounded_init& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.left());
  static_cast<Derived&>(*this)(x.right());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const merge& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.left());
  static_cast<Derived&>(*this)(x.right());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const left_merge& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.left());
  static_cast<Derived&>(*this)(x.right());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const choice& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.left());
  static_cast<Derived&>(*this)(x.right());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const process_specification& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.action_labels());
  static_cast<Derived&>(*this)(x.equations());
  static_cast<Derived&>(*this)(x.init());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const process_identifier& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.name());
  static_cast<Derived&>(*this)(x.sorts());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const process_equation& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.name());
  static_cast<Derived&>(*this)(x.formal_parameters());
  static_cast<Derived&>(*this)(x.expression());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const rename_expression& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.source());
  static_cast<Derived&>(*this)(x.target());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const communication_expression& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.action_name());
  static_cast<Derived&>(*this)(x.name());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const action_name_multiset& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.names());
  static_cast<Derived&>(*this).leave(x);
}
//--- end generated code ---//
